Samizdat Layer 0 Core Library
=============================

This is an implementation of the non-primitive portion of the
*Layers 0&ndash;2* core libraries, written in *Layers 0&ndash;2*, in terms of
the specified lower-layer libraries.

The files in this directory are organized by module, all of which are
defined in the library specification (see which). The main entry point
to the library is the `main` file, which is responsible for loading up all
the other files in a dependency-appropriate order.

**Note:** If files are added or removed here, then a corresponding line
needs to be added to or removed from the file `manifest.sam` in this
directory.
