Samizdat Layer 0: Core Library
==============================

One-Off Generics
----------------

Some generic functions are effectivly protocols in and of themselves,
in that binding to them doesn't particularly imply that a value binds
to any others. This section describes those generics.

All of these generics are available in the standard global environment.

Note that some of these are also used as part of larger protocols, e.g.
`get`. These are noted in the documentation for those protocols.


<br><br>
### Generic Function Definitions: One-Offs

#### `cat(value, more*) <> value`

Returns the concatenation of all of the given values. The values
must all be of the same type, and the result is expected to be of the same
type as the given values. It is an error (terminating the runtime) if one of
the arguments is of a different type. This function is intended for
"collection-like" values which have constituent parts of some sort.

To the extent that a value is unconstrained in terms of its constituent
elements and their arrangement, the result of concatenation consists
of the elements of all the original values, in order, in the order of the
arguments.

For values that have element constraints, a concatenation will not
necessarily contain all the original constituent elements, and the order may
be different. See individual implementation docs for details.

**Note:** To account for the possibility of passing *no* arguments to
concatenate (e.g. when handling a list of arguments generically), include
a first argument of the empty value of the desired type, e.g.
`""` to ensure string concatenation.

**Note:** The argument type restriction is enforced by the generic function
itself, which is a "unitype" generic.

#### `get(value, key) <> . | void`

Returns the constituent element of the value that corresponds to the given
`key`. `key` is an arbitrary value. Returns void if there is no unique
corresponding value for the given `key` (including if `key` is not
bound in `value` at all).

This function is intended for "collection-like" values which have constituent
parts of some sort.

#### `keyOf(value) <> .`

Some values have an associated "key" or "key-like" value.
This generic provides access to that key. This generic often goes
hand-in-hand with `valueOf`.

It is discouraged to bind this to a function that sometimes returns void;
it is preferable to cause an error if it is bound but inapplicable.

#### `nameOf(value) <> . | void`

Some values have an associated name, or an optional associated name.
This generic provides access to that name. There is no restriction
on the composition (type, etc.) of a name.

#### `toInt(value) <> int`

Some values have an obvious correspondence to an integer quantity. If
so, then this generic can be used to access it.

It is discouraged to bind this to a function that sometimes returns void;
it is preferable to cause an error.

It is also discouraged to use this generic for lossy conversion.

#### `toNumber(value) <> number`

Some values have an obvious correspondence to a numeric quantity. If
so, then this generic can be used to access it. It is discouraged to
bind this to a function that sometimes returns void; it is preferable
to cause an error.

#### `toString(value) <> string`

Some values have an obvious correspondence to a string value. If
so, then this generic can be used to access it.

It is discouraged to bind this to a function that sometimes returns void;
it is preferable to cause an error.

It is discouraged to use this generic for textual conversion of a value
to a string (e.g. turning the int value 242 into the string `"242"`).

#### `valueOf(value) <> .`

Some values have an associated "sub-value" or "inner value."
This generic provides access to that value. This generic often goes
hand-in-hand with `keyOf`.

It is discouraged to bind this to a function that sometimes returns void;
it is preferable to cause an error if it is bound but inapplicable.
