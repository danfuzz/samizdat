Samizdat Layer 0 Core Library
=============================

This is an implementation of the specified *Samizdat Layer 0* core
library in terms of the specified primitive library. The result
(return value) of running the code here is a maplet of all of the core
library bindings (including primitives and what is defined in-language
here).

The files in this directory are organized by library definition sub-section
in the *Samizdat Layer 0* specification (see which). The main entry point
to the library is the `main` file, which is responsible for loading up all
the other files in a dependency-appropriate order.
