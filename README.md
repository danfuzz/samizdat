Samizdat
========

Watch this space!

Plan of attack
--------------

* `libsamidumb` &mdash; data structure C library, "dumb edition".

  Has functions to (a) build up low-layer data values piece by piece,
  and (b) dump C code based on such data values to recreate them using
  the same library.

* `datacomp` &mdash; tool to compile data structure assignments

  Accepts input in the form of a series of assignment statements
  `varName := dataStruct` where `dataStruct` is low-layer Samizdat
  data value syntax, outputs C code that recreates those data values.

  Built using `libsamidumb`.

* ?

* Profit!
