Samizdat Layer 0 Core Library
=============================

This is an implementation of the non-primitive portion of the *Samizdat
Layer 0* core library, written in *Samizdat Layer 0*, in terms of the
specified primitive library.

The result (return value) of running the code here is a map of all of the
core library bindings (including primitives and what is defined in-language
here).

The files in this directory are organized by library definition sub-section
in the *Samizdat Layer 0* specification (see which). The main entry point
to the library is the `main` file, which is responsible for loading up all
the other files in a dependency-appropriate order.

**Note:** If files are added or removed here, then a corresponding line
needs to be added to or removed from the file `manifest.sam0` in this
directory.
