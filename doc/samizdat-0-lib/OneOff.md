Samizdat Layer 0: Core Library
==============================

One-Off Generics
----------------

Some generic functions are effectivly protocols in and of themselves,
in that binding to them doesn't particularly imply that a value binds
to any others. This section describes those generics.

<br><br>
### Generic Function Definitions: One-Offs

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

Some values have an associated "sub-value" or "inner value".
This generic provides access to that value. This generic often goes
hand-in-hand with `keyOf`.

It is discouraged to bind this to a function that sometimes returns void;
it is preferable to cause an error if it is bound but inapplicable.
