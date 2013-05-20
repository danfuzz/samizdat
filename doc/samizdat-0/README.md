Samizdat Layer 0
================

This is the specification for the language known as *Samizdat Layer 0*.
The language is meant to provide a syntactically and semantically
minimal way to build up and manipulate low-layer Samizdat data.

*Samizdat Layer 0* is run by translating it into low-layer Samizdat
data, as specified here, and running it with the specified core
library bindings.

The language it is meant to be a "parti" of the final *Samizdat* language
layer, that is, it embodies *just* the main thrusts of the
language with very little embellishment.

The core library isn't as minimal as the language itself, but it is still
intended to be more on the minimal side of things. Beyond the true
essentials, the library contains bindings that have proven to be useful
in practice for building programs in the language. A subset of the library
is defined to be "primitive", that is written into the lowest layer of
language implementation as primitive functions. The remainder of the library
is implemented in-language in terms of the primitive functions and the
language syntax.

This specification includes an executable parse tree format. This format,
along with API defined by the primitively-defined part of the core library,
are the analog to a bytecode specification in systems that are
bytecode-based.

Contents:

* [Data Syntax And Semantics In A Nutshell](data-syntax-semantics.md)
* [Token Syntax](token-syntax.md)
* [Tree Syntax](tree-syntax.md)
* [Tree Semantics](tree-semantics.md)
* [Core Library](library.md)
