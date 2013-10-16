Samizdat Layer 0: Core Library
==============================

core::ParseForwarder
--------------------

This module provides a class that knows how to forward `parse` calls.


<br><br>
### Function Definitions

#### `make() <> parserBox`

Simple parser forward declaration utility. The result of a call to this
function is a parser, which forwards `parse` calls to an initially un-set
box. In addition to being a callable parser, the result of this function
is also a yield box.

This function is like `makeFunctionForwarder`, except for parsers not
functions. See that function for more details.
