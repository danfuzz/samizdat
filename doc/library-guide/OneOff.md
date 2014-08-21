Samizdat Layer 0: Core Library
==============================

One-Off Methods
---------------

Some methods are effectivly protocols in and of themselves,
in that binding to them doesn't particularly imply that a value binds
to any others. This section describes those methods.

Note that some of these are also used as part of larger protocols, e.g.
`get`. These are noted in the documentation for those protocols.


<br><br>
### Method Definitions: One-Offs

#### `.cat(more*) -> value`

Returns the concatenation of all of the given values. The `more*` values
must be "compatible" with `this` (as defined by the class of `this`),
and the result is expected to be of the same class as `this`. It is an
error (terminating the runtime) if one of the arguments is incompatible.
This function is intended for "collection-like" values which have constituent
parts of some sort.

To the extent that a value is unconstrained in terms of its constituent
elements and their arrangement, the result of concatenation consists
of the elements of all the original values, in order, in the order of the
arguments.

For values that have element constraints, a concatenation will not
necessarily contain all the original constituent elements, and the order might
be different. See individual implementation docs for details.

**Note:** To account for the possibility of passing *no* arguments to
concatenate (e.g. when handling a list of arguments generically), include
a first argument of the empty value of the desired class, e.g.
`""` to ensure string concatenation.

#### `.get(key) -> . | void`

Returns the constituent element of the value that corresponds to the given
`key`. `key` is an arbitrary value. Returns void if there is no unique
corresponding value for the given `key` (including if `key` is not
bound in `this` at all).

This function is intended for "collection-like" values which have constituent
parts of some sort.

#### `.get_key() -> .`

Some values have an associated "key" or "key-like" value.
This method provides access to that key. This method often goes
hand-in-hand with `get_value`.

It is discouraged to bind this to a function that sometimes returns void;
it is preferable to cause an error if it is bound but inapplicable.

#### `.get_size() -> int`

Returns the number of elements in the given value. This function is intended
for "collection-like" values which have constituent parts of some sort.

**Syntax Note:** This is the function underlying the `#value` syntactic
form (prefix `#` operator).

#### `.get_value() -> .`

Some values have an associated "sub-value" or "inner value."
This method provides access to that value. This method often goes
hand-in-hand with `get_key`.

It is discouraged to bind this to a function that sometimes returns void;
it is preferable to cause an error if it is bound but inapplicable.

#### `.nth(n) -> . | void`

Returns the nth (zero-based) element of the given value.
Returns void if `n < 0` or `n >= #value`. It is an error
(terminating the runtime) if `n` is not an `Int`.

This function is intended for "sequence-like" values which have
strongly-ordered constituent parts of some sort.

#### `.toInt() -> int`

Some values have an obvious correspondence to an integer quantity. If
so, then this method can be used to access it.

It is discouraged to bind this to a function that sometimes returns void;
it is preferable to cause an error.

It is also discouraged to use this method for lossy conversion.

#### `.toNumber() -> number`

Some values have an obvious correspondence to a numeric quantity. If
so, then this method can be used to access it. It is discouraged to
bind this to a function that sometimes returns void; it is preferable
to cause an error.

#### `.toString() -> string`

Some values have an obvious correspondence to a string value. If
so, then this method can be used to access it.

It is discouraged to bind this to a function that sometimes returns void;
it is preferable to cause an error.

It is discouraged to use this method for textual conversion of a value
to a string (e.g. turning the int value 242 into the string `"242"`).
