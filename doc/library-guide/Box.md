Samizdat Layer 0: Core Library
==============================

core.Box / Box Protocol
-----------------------

A `Box` is a container for a single other value or for void.
In terms of value comparison, all boxes should compare by identity,
and not by "happenstance content." That is, two boxes should only be
considered "equal" if they are indistinguishable, even in the face of
calling mutating operations.

The `Box` module provides the methods of the `Box` protocol,
as well as related constructors and constants.

The `Box` protocol is defined in a `proto.` module as one
would expect. However, as a special case, the `Box` methods and the
special value `nullBox` are also exported to the global variable environment.
This is because they are so commonly used (sometimes overtly, and sometimes
"behind the scenes").

As a protocol, `Box` consists of the `Generator` protocol with one additional
function. As a generator, a box will generate either its sole stored value,
or void if it has no stored value.


<br><br>
### Method Definitions: `Value` protocol

#### `.perEq(other) -> box | void`

Default implementation.

#### `.perOrder(other) -> int | void`

Default implementation.

#### `.totalEq(other) -> box | void`

Performs an identity comparison. No two boxes are alike.

#### `.totalOrder(other) -> int | void`

Performs an identity comparison. No two boxes are alike, and two
different boxes have no defined order.


<br><br>
### Method Definitions: `Box` protocol

#### `.collect(optFilterFunction?) -> list`

Refinement of the `Generator` protocol. This is equivalent to getting the
contents of the box as a list (of zero or one element), and calling
`collect` on that list with the same arguments.

#### `.fetch() -> . | void`

Refinement of the `Generator` protocol. Gets the value inside a box, if any.
If the box either is unset or has been set to void, this returns void.
Unlike the general `Generator` protocol, it is never a fatal error to call
this function.

#### `.nextValue(outBox) -> [] | void`

Refinement of the `Generator` protocol. If the box has a stored value, this
stores it to the given `outBox` and returns `[]` (the empty list). If the
box has no stored value, this performs no action and returns void.

#### `.store(value?) -> . | void`

Sets the value of a box to the given value, or to void if `value` is
not supplied. This function always returns `value` (or void if `value` is
not supplied).

It is an error (terminating the runtime) for `this` to be a yield box on
which `store` has already been called.


<br><br>
### Primitive Definitions

#### `makeCell(optValue?) -> cell`

Creates a cell (a mutable box), with optional pre-set value. The result of
a call to this function is a cell which can be set any number of times using
`store`. The contents of the cell are accessible by calling `fetch`.

The initial cell content value is the `optValue` given to this function. This
is what is returned from `fetch` until `store` is called to replace it.
If `optValue` is not supplied, `fetch` returns void until `store` is called.

This function is meant to be the primary way to define (what amount to)
mutable variables, in that Samizdat Layer 0 only provides immutably-bound
variables. It is hoped that this facility will be used as minimally as
possible, so as to not preclude the system from performing functional-style
optimizations.

#### `makePromise() -> promise`

Creates a promise (set-at-most-once box). The result of a call to this
function is a box which can be stored to at most once, using `store`.
Subsequent attempts to call `store` will fail (terminating the runtime). The
contents of the promise are accessible by calling `fetch`. `fetch` returns
void until and unless `store` is called with a second argument.

This function is meant to be the primary way to capture the yielded values
from functions (such as object service functions and parser functions) which
expect to yield values by calling a function.

#### `makeResult(optValue?) -> promise`

Creates a permanently-set or permanently valueless box. The result of a call
to this function is a box which cannot be stored to, and which will always
respond to `fetch` with the given `optValue`. If `optValue` is not supplied,
`fetch` on the result will always return void.


<br><br>
### In-Language Definitions

#### Constant: `nullBox`

A value that represents a permanently empty (un-set, un-stored) box.
`nullBox.store(value)` is effectively a no-op. This arrangement is done in
order to make it easy to pass a box into functions that require one, but where
the box value is never needed.
