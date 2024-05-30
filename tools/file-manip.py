import pathlib


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
