# Umbra's Rainbow Quest

## Quick guide

The easiest way to compile this is by using a docker image with all the dependencies and tools:

```
docker run --rm -v `pwd`:/app -w /app sirgfm/gframe-dev:3.1.0-rc <target>
```

Where `<target>` may be one of:

- `linux-32`: Build the game for Linux (32 bits)
- `linux-64`: Build the game for Linux (64 bits)
- `linux-debug-32 `: Build the game for Linux (32 bits) with debug symbols
- `linux-debug-64`: Build the game for Linux (64 bits) with debug symbols
- `win-32`: Build the game for Window (32 bits)
- `win-64`: Build the game for Window (64 bits)
- `win-debug-32`: Build the game for Window (32 bits) with debug symbols
- `win-debug-64`: Build the game for Window (64 bits) with debug symbols
- `web`: Build the game for web, using WebAssembly

## Updating maps

1. Update the tiled XML file.
2. Run `make maps`
3. Run `make preloader`
3. Run `make linux-debug`

## Configuring collision

### Tiled

1. In the tileset tab, click on `edit tileset` (at the bottom)
2. Create a new terrain
3. Paint the tiles that should be of that type

### Engine

1. Open `include/core/types.h`
2. Add the new type to `TYPES`, named identically to the terrain in Tiled
3. Implement the methods that will handle this type's collision in `include/collision.h`/`src/collision.c`
	- These methods must receive two `struct collision_node*` and return an `int`

### Collision matrix

1. Open `assets/raw/collision-map.col`
2. Add a new column and row for the new type
3. Add the methods that handle interaction between types
	- The method must have been declared in `include/collision.h`
	- The column is passed as the first parameter and the row as the second
	- Use `_noop_` if two types shouldn't interact
	- Use `_dupl_` to indicate a previously declared method, but inverting the parameters order
		- `_dupl_` should basically fill the entier bottom-left half of the table
4. Run `make collision`
