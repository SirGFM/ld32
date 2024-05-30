import argparse
import csv
import importlib
import pathlib

file_manip = importlib.import_module('file-manip')


def main() -> None:
	"""collision-generator's entrypoint.

	Parses all arguments, then converts the input file.
	"""

	parser = argparse.ArgumentParser(
		prog='collision-generator',
		description='Converts a CSV collision map into two collision list files for kitten-v2',
	)
	parser.add_argument(
		'csv_file',
		help='The input CSV file',
		type=str,
	)
	parser.add_argument(
		'dest_dir',
		help='The output directory where the generated C files are stored',
		type=str,
	)
	parser.add_argument(
		'--rm',
		help='Remove the previous destination directory, if it exists',
		action='store_true',
		default=False,
	)
	args = parser.parse_args()

	# Check that the dest_dir is within the project's directory,
	# and then create it.
	project_dir = (pathlib.Path(__file__) / '../..').resolve(strict=True)
	dest_dir = pathlib.Path(args.dest_dir).resolve()
	file_manip.assert_path_within(project_dir, dest_dir)

	# Clean up the destination directory, if requested.
	if args.rm and dest_dir.exists():
		file_manip.remove_dir(dest_dir)
	dest_dir.mkdir(parents=True, exist_ok=True)

	# Convert the CSV collision file.
	convert_collision(args.csv_file, dest_dir)


def convert_collision(csv_file: str, dest_dir: pathlib.Path) -> None:
	"""Converts the collision map in the CSV file to a C file.

	:param str csv_file: Path to the input CSV file.
	:param pathlib.Path dest_dir: Directory where the output file(s) shall be written.
	"""

	collision_map = {}

	# Parse the CSV into collision_map.
	with open(csv_file, 'rt') as file:
		reader = csv.reader(file)
		for row in reader:
			if len(collision_map) == 0:
				collision_map = parse_types(row)
			else:
				add_collisions(collision_map, row)

	# Invert the map into handler -> type tuple.
	handler_map = gen_by_handler(collision_map)

	# Generate the C code.
	gen_code(dest_dir / 'auto_collision.c', handler_map, csv_file)


def parse_types(csv_row: list[str]) -> dict[str, any]:
	"""Extract the types from the CSV's header.

	Extract the types from the CSV's header,
	returning a dictionary with these types as the keys.
	The CSV's index 0 is skipped as that shouldn't have anything!

	Additionally, a special key '_names_' mapping each index to its type
	is also returned.
	However, note that this list maps the CSV's index 1 as the list's index 0!

	:param list[str] csv_row: The CSV header.
	:return dict[str, any]: The map of types in the CSV header.
	"""

	collision_map = {}
	names = []

	for name in csv_row[1:]:
		name = name.strip()

		collision_map[name] = {}
		names.append(name)

	collision_map['_names_'] = names

	return collision_map


def add_collisions(collision_map: dict[str, any], csv_row: list[str]) -> None:
	"""Adds the CSV row to the collision map.

	In each row (after the header),
	the first column is the colliding type
	and the following columns are the handlers for each colliding type,
	whose name must be retrieved from the '_names_' entry in collision_map.

	Each handler is added as a new dictionary in the colliding type,
	structured as:

	{
		'handler': '<the value from the column>',
		'invert': False,
		'handled': True,
	}

	However, the following names have a special meaning:

	- '_noop_': A dictionary with 'handled' set to False;
	- '_dupl_': The same as the handler for the colliding type,
	  but with 'invert' set to True;

	:param dict[str, any] collision_map: The collision map, modified in place.
	:param list[str] csv_row: The current CSV row.
	"""

	cur_name = csv_row[0].strip()
	entry = collision_map[cur_name]

	for idx, handler in enumerate(csv_row[1:]):
		handler = handler.strip()
		name = collision_map['_names_'][idx]

		if handler == '_noop_':
			entry[name] = {
				'handled': False,
				'handler': '_noop_',
			}
		elif handler == '_dupl_':
			other_handler = collision_map[name][cur_name]
			entry[name] = {
				'handler': other_handler['handler'],
				'invert': True,
				'handled': other_handler['handled'],
			}
		else:
			entry[name] = {
				'handler': handler,
				'invert': False,
				'handled': True,
			}


def gen_by_handler(collision_map: dict[str, any]) -> dict[str, any]:
	"""Regenerates the collision map from the handler's perspective.

	Instead of mapping from "left type" to "right rype with handler",
	map the collisions as handler to colliding types.

	A the handler takes two parameters, which must be passed in order.
	Thus, to account for collisions marked as inverted,
	each handler has two array:

	- 'lr' (left-to-right) for the regular order;
	- 'rl' (right-to-left) for the inverted order.

	Each pair of types is added as a tuple to one of these arrays.

	Lastly, the special key '_noop_' maps every known tuple that doesn't have a handler.

	:param dict[str, any] collision_map: The collision map.
	:return dict[str, any]: The map of collision handler to type tuples.
	"""

	handler_map = {
		'_noop_': [],
	}

	for name_1, entries in collision_map.items():
		if name_1 == '_names_':
			continue

		for name_2, data in entries.items():
			value = (name_1, name_2)

			if not data['handled']:
				handler_map['_noop_'].append(value)
				continue

			handler = data['handler']

			if not handler in handler_map:
				handler_map[handler] = {
					'lr': [],
					'rl': [],
				}

			if not data['invert']:
				handler_map[handler]['lr'].append(value)
			else:
				handler_map[handler]['rl'].append(value)

	return handler_map


def gen_code(out_file: pathlib.Path, handler_map: dict[str, any], csv_file: str) -> None:
	"""Generates the output code.

	:param pathlib.Path out_file: The output file to be generated.
	:param dict[str, any] handler_map: The map of collision handler to type tuples.
	:param str csv_file: The name of the input CSV file
	"""

	with out_file.open('wb') as file:
		file.write(f"""/**
 * File generated by 'tools/collision-generator.py' and '{csv_file}' to simplify handling collisions.
 *
 * DO NOT EDIT MANUALLY
 */

#include <core/types.h>
#include <collision.h>
#include <error.h>

#include <GFraMe/gfmObject.h>
#include <GFraMe/gfmQuadtree.h>
#include <GFraMe/gfmSprite.h>

#if defined(DEBUG) && !(defined(__WIN32) || defined(__WIN32__))
#include <stdlib.h>
#include <signal.h>
#endif /* defined(DEBUG) && !(defined(__WIN32) || defined(__WIN32__)) */


/**
 * Retrieve the type and all the children for a given object.
 *
 * @param  [ in]node The object (with a valid gfmObject)
 */
static void _collision_getSubtype(struct collision_node *node) {{
	gfmObject_getChild(&node->child, &node->type, node->object);

	if (node->type == gfmType_sprite && node->child) {{
		node->sprite = (gfmSprite*)node->child;
		gfmSprite_getChild(&node->child, &node->type, node->sprite);
	}}
}}


int collision_handle(gfmQuadtreeRoot *qt) {{
	struct collision_node node1 = {{0}};
	struct collision_node node2 = {{0}};
	gfmRV grv = GFMRV_OK;
	int rv = 0;

	ASSERT_OK(
		rv = gfmQuadtree_getOverlaping(&node1.object, &node2.object, qt),
		__ret
	);

	/* Filter out self-collision. */
	if (node1.object == node2.object) {{
		goto __ret;
	}}

	_collision_getSubtype(&node1);
	_collision_getSubtype(&node2);

	switch(MERGE_TYPES(node1.type, node2.type)) {{
""".encode('utf-8'))

		# Output a case for each collision pair witha  valid function.
		for func, data in handler_map.items():
			if func == '_noop_':
				continue

			file.write(f'		/* ===== {func} ===== */\n'.encode('utf-8'))
			for types in data['lr']:
				file.write(f'		case MERGE_TYPES({types[0]}, {types[1]}):\n'.encode('utf-8'))

			file.write(f'			ASSERT_OK(rv = {func}(&node1, &node2), __ret);\n'.encode('utf-8'))
			file.write('			break;\n'.encode('utf-8'))

			for types in data['rl']:
				file.write(f'		case MERGE_TYPES({types[0]}, {types[1]}):\n'.encode('utf-8'))

			file.write(f'			ASSERT_OK(rv = {func}(&node2, &node1), __ret);\n'.encode('utf-8'))
			file.write('			break;\n\n'.encode('utf-8'))

		# List every known type whose collisions are ignored.
		if len(handler_map['_noop_']) > 0:
			file.write('		/* ===== Ignore collisions ===== */\n'.encode('utf-8'))
			for types in handler_map['_noop_']:
				file.write(f'		case MERGE_TYPES({types[0]}, {types[1]}):\n'.encode('utf-8'))

			file.write('			{/* Do nothing */}\n'.encode('utf-8'))
			file.write('			break;\n'.encode('utf-8'))

		# Lastly, report any remaining and unexpected collisions.
		file.write("""		default: {
			/* Unfiltered collision, do something about it */
#if defined(DEBUG) && !(defined(__WIN32) || defined(__WIN32__))
			raise(SIGINT);
#endif /* defined(DEBUG) && !(defined(__WIN32) || defined(__WIN32__)) */
		} break;
	}

__ret:
	return grv || rv;
}
""".encode('utf-8'))


if __name__ == '__main__':
	main()
