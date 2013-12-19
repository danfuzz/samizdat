samex
=====

This provides commands and utilities that work with multiple versions
of Samizdat runtime.

* `samex` &mdash; This is a simple front-end which knows how to dispatch to
  the most "mature" available version of the runtime.

* `compile-samex-addon` &mdash; This is a wrapper for the C compiler, which
  knows how to invoke it to produce "addon" libraries for the runtime.

* `find-samex` &mdash; Internal tool to identify the most "mature" available
  version of the runtime.
