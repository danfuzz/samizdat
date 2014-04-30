Samizdat Core Library
=====================

This is an implementation of the non-primitive portion of the
Layers 0&ndash;2 core libraries, written in Layers 0&ndash;2, in terms of
the specified lower-layer libraries.

The files in this directory are organized by module, all of which are
defined in the library specification (see which). The main entry point
to the library is the `main` file, which is responsible for loading up all
the other files in a dependency-appropriate order.

To figure out what layer of the language a particular file is written in,
look toward the top for a `#= language` directive, which names the language
module to be used to parse the file. If there's no directive, the file is
written in the top language layer.

**Note:** Language layers 0 and 1 are particularly light on syntax and
so are (a) a bit hard to read, and (b) not representative of the style
of the top language layer.
