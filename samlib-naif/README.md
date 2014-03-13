Samizdat Core Library
=====================

This is an implementation of the non-primitive portion of the
Layers 0&ndash;2 core libraries, written in Layers 0&ndash;2, in terms of
the specified lower-layer libraries.

The files in this directory are organized by module, all of which are
defined in the library specification (see which). The main entry point
to the library is the `main` file, which is responsible for loading up all
the other files in a dependency-appropriate order.

To figure out what layer of the language a particular module is written in,
look in the `module.sam` file for a `language` tag. The files that aren't
in a module at all (e.g. `Module0.sam`) are all written in the Layer 0
language.

**Note:** Language layers 0 and 1 are particularly light on syntax and
so are (a) a bit hard to read, and (b) not representative of the style
of the final language layer.

**Note:** If files or directories are added or removed here, then
corresponding lines need to be added to or removed from the file
`manifest.sam` in this directory.
