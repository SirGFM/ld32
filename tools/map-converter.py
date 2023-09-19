import argparse
import pathlib
import pyexpat
import xml.etree.ElementTree as ET

# Ensure that a recent version of expat is being used.
# https://docs.python.org/3/library/xml.html#xml-vulnerabilities
if pyexpat.version_info < (2, 4, 1):
	raise Exception('pyexpat must be on version 2.4.1 or newer!')


tile_info = tuple[int, str]
"""Associates a tile's index to the name of its type."""

tileset_info = list[tile_info]
"""A simple list of tile_info."""


def main() -> None:
	"""map-converter's entrypoint.

	Parses all arguments, then converts the input file.
	"""

	parser = argparse.ArgumentParser(
		prog='map-converter',
		description='Converts a tiled map into multiple map files for kitten-v2',
	)
	parser.add_argument(
		'tmx_file',
		help='The tiled input file',
		type=str,
	)
	parser.add_argument(
		'dest_dir',
		help='The output directory where maps are created',
		type=str,
	)
	parser.add_argument(
		'--rm',
		help='Remove the previous destination directory, if it exists',
		action='store_true',
		default=False,
	)
	parser.add_argument(
		'-t',
		'--tileset',
		help='Name of the tileset with type information',
		action='store',
		type=str,
	)
	args = parser.parse_args()

	# Check that the dest_dir is within the project's directory.
	project_dir = (pathlib.Path(__file__) / '../..').resolve(strict=True)
	dest_dir = pathlib.Path(args.dest_dir).resolve()
	assert_path_within(project_dir, dest_dir)

	# Clean up the destination directory, if requested.
	if args.rm and dest_dir.exists():
		remove_dir(dest_dir)
	dest_dir.mkdir(parents=True, exist_ok=True)

	# Convert the tiled file.
	convert_world(args.tmx_file, dest_dir, args.tileset)


def assert_path_within(container: pathlib.Path, path: pathlib.Path) -> None:
	"""Checks that a given path is within another one.

	This function raises a simple exception with a textual description of the issue.

	:param pathlib.Path container: The path that must contain 'path'.
	:param pathlib.Path path: The path that must be contained within container.
	:raises: Exception
	"""

	container = container.resolve()
	path = path.resolve()

	if len(container.parts) > len(path.parts):
		raise Exception(
			f'"{path}" must be within "{container}"',
		)
	elif len(container.parts) == len(path.parts):
		raise Exception(
			f'"{path}" must be a sub-directory within the project "{container}"',
		)
	for i in range(len(container.parts)):
		if container.parts[i] != path.parts[i]:
			raise Exception(
				f'Destination directory "{path}" must be within the project "{container}"',
			)


def remove_dir(path: pathlib.Path) -> None:
	"""Deletes the directory and all of its contents.

	:param pathlib.Path path: The path to be deleted.
	"""

	if not path.is_dir():
		path.unlink(missing_ok=True)
	else:
		for child in path.iterdir():
			remove_dir(child)
		path.rmdir()


def convert_world(in_file: str, dest_dir: pathlib.Path, tileset_name: str | None = None) -> None:
	"""Converts a world, defined in a single Tiled file.

	:param in_file:
	:param dest_dir:
	"""

	tree = ET.parse(in_file)
	root = tree.getroot()

	# Extract the tileset information/types.
	tm: tileset_info = []
	if tileset_name:
		tm = get_tile_types(root, tileset_name)

	for child in root:
		if child.tag == 'group':
			sub_dir = dest_dir / child.attrib['name']
			convert_map(child, sub_dir, tm)
		elif child.tag == 'layer':
			convert_layer(child, dest_dir, tm)
		elif child.tag == 'objectgroup':
			convert_objects(child, dest_dir)


def convert_map(root: ET.Element, dest_dir: pathlib.Path, tm: tileset_info) -> None:
	"""Converts an entire map, which may be composed of various layers.

	:param root:
	:param dest_dir:
	"""

	dest_dir.mkdir(parents=True, exist_ok=True)

	for child in root:
		if child.tag == 'layer':
			convert_layer(child, dest_dir, tm)
		elif child.tag == 'objectgroup':
			convert_objects(child, dest_dir)
		else:
			print(f'ignoring layer {child.tag} in {dest_dir}')


def convert_layer(el: ET.Element, dest_dir: pathlib.Path, tm: tileset_info) -> None:
	"""Converts a single layer.

	A layer is comprised of:

	- A few type descriptions (a name followed by its tile index)
	- Tile animations (TODO)
	- Collider areas (TODO)
	- The map itself (the descriptor 'map' followed by the map's width, height and tiles)

	For example, a 5x3 tilemap with tiles 1 and 2 having the 'floor' type:

	floor 1
	floor 2
	map 5 3
	  -1 -1 -1 -1 -1
	  -1  2  3  2 -1
	  -1 -1 -1 -1 -1

	:param ET.Element el: The layer element.
	:param pathlib.Path dest_dir: The destination directory for this layer.
	"""

	dest_file = dest_dir / el.attrib['name']
	with dest_file.open('wb') as out:
		for tile in tm:
			out.write(f'type {tile[1]} {tile[0]}\n'.encode('utf-8'))

		w = int(el.attrib["width"])
		h = int(el.attrib["height"])

		out.write(f'map {w} {h}\n'.encode('utf-8'))

		data = el.find('data').text.strip().split(',')
		for i in range(h):
			# Tiled has 1-indexed tiles.
			# Convert it to 0-indexed, using -1 as the 'empty' tile.
			line = [x.strip() for x in data[i*w:(i+1)*w]]
			line = [str(int(x) - 1) for x in line]

			out.write(f'  {" ".join(line)}\n'.encode('utf-8'))


def convert_objects(root: ET.Element, dest_dir: pathlib.Path) -> None:
	"""Converts an object group into a single file.

	The file is named after the root element's name attribute.

	An object group is comprised of various 'object' tags.
	Most properties are defined as tag attributes,
	but a few may also be defined in a 'property' tag.

	Before any object,
	a single line is added with the number of objects in this file.

	Each generated object shall have a name,
	which doesn't need to be unique and is simply used to identify the object.
	The name must be placed at the start of a line!
	After that, each property must be defined in a new line,
	preceded by a single tab character
	and with an equals sign separating the property name from its value.

	E.g.:

	count=2

	object
		type=some type
		x=123
		y=456
		w=24
		h=16

	another object
		type=some type
		x=123
		y=456
		w=24
		h=16
		custom=foo

	:param ET.Element root: The object group being converted.
	:param pathlib.Path dest_dir: The destination directory for this object group.
	"""

	dest_file = dest_dir / root.attrib['name']
	with dest_file.open('wb') as out:
		out.write(f'count={len(root)}\n\n'.encode('utf-8'))

		for child in root:
			if child.tag != 'object':
				raise Exception(f'Invalid tag "{child.tag}" in object')

			out.write(f'{child.attrib["name"]}\n'.encode('utf-8'))
			out.write(f'\ttype={child.attrib["type"]}\n'.encode('utf-8'))
			out.write(f'\tx={child.attrib["x"]}\n'.encode('utf-8'))
			out.write(f'\ty={child.attrib["y"]}\n'.encode('utf-8'))
			out.write(f'\tw={child.attrib["width"]}\n'.encode('utf-8'))
			out.write(f'\th={child.attrib["height"]}\n'.encode('utf-8'))
			for prop in child.findall('property'):
				name = child.attrib['name']
				value = child.attrib['value']
				out.write(f'\t{name}={value}\n'.encode('utf-8'))
			out.write(f'\n'.encode('utf-8'))


def get_tile_types(root: ET.Element, tileset_name: str) -> tileset_info:
	"""Extracts the type information from the tileset.

	:param ET.Element root: The object group being converted.
	:param str tileset_name: The name of the tileset with type information.
	:return tileset_info: The list of tile_info found in this tileset.
	"""

	for tm in root.findall('tileset'):
		name = tm.attrib['name']
		if name != tileset_name:
			continue

		# This assumes there's only a single terrain in the tileset!
		tm = tm.find('wangsets')
		tm = tm.find('wangset')

		types: list[str] = []
		for child in tm.findall('wangcolor'):
			types.append(child.attrib['name'])

		# Associate every type to its list of tiles in the tileset.
		unsorted_tiles: dict[str, list[int]] = {}
		for child in tm.findall('wangtile'):
			typ = ''

			# This has the info for every corner/direction/position.
			# For simplicity, assume every tile is fully filled,
			# and that the first non-zero value is the type's index.
			tiled_terrain = child.attrib['wangid'].split(',')
			for value in [int(x) for x in tiled_terrain]:
				if value != 0:
					typ = types[value - 1]
					break
			if typ == '':
				raise Exception(f'could not find a type for tileid {idx}')

			idx = child.attrib['tileid']
			if typ in unsorted_tiles:
				unsorted_tiles[typ].append(idx)
			else:
				unsorted_tiles[typ] = [idx]

		# Finally, create the tileset info.
		tiles: tileset_info = []
		for typ in unsorted_tiles:
			for idx in unsorted_tiles[typ]:
				tile: tile_info = (idx, typ)
				tiles.append(tile)

		return tiles

	raise Exception(f'could not find the tileset {tileset_name}')


if __name__ == '__main__':
	main()
