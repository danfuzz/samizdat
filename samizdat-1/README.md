Samizdat Layer 1
================

This is an implementation of the *Samizdat Layer 1* language in
*Samizdat Layer 0*.

The goals of this implementation, in priority order, are:

* Correctness.
* Debugability.
* Readability.
* Smallness.
* Efficiency.

Part of the point of this program is to validate and cross-verify
the consistency of implementations of the languages, and as such,
any *Samizdat Layer 0* program parsed by this code is automatically parsed
by the `samizdat-0` (C implementation) parser as well, with the two results
compared. If they differ, this program will note that fact to the console,
perform a structural diff between the two trees, and exit.

This program is itself written in *Samizdat Layer 0*, in a style
which mostly "absorbs" the core data library and entirely "reifies"
the token and tree parsers. In particular, this program translates
input program text into the identical executable tree form as specified
in the *Samizdat Layer 0* specification.


Structure
---------

The code consists mostly of a tokenizer and a tree parser, with the result
of tokenization feeding into the tree parser, as one might expect.
