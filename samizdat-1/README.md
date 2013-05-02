Samizdat Layer 1
================

This is an implementation of the *Samizdat Layer 0* language in
*Samizdat Layer 0*.

- - - - -

The code consists mostly of a tokenizer and a tre parser, with the result
of tokenization feeding into the tree parser, as one might expect.

Part of the point of this program is to validate and cross-verify
the consistency of implementations of the languages, and as such,
any program parsed by this code is automatically parsed by the
`samizdat-0` parser as well, with the two results compared. If they
differ, this program will note that fact to the console, perform a
structural diff between the two trees, and exit.
