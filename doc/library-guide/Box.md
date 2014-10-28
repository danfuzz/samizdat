Samizdat Layer 0: Core Library
==============================

Box and subclasses
------------------

A `Box` is a container for a single other value or for void.
In terms of value comparison, all boxes should compare by identity,
and not by "happenstance content." That is, two boxes should only be
considered "equal" if they are indistinguishable, even in the face of
calling mutating operations.

The system provides a `Box` abstract class as well as four concrete
classes that implement the `Box` protocol:

* `Cell` &mdash; A box whose value can be changed an arbitrary number
  of times.
* `NullBox` &mdash; A box which always indicates it holds void, and which can
  be stored to any number of times without effect. The global value `nullBox`
  is an instance of this class.
* `Promise` &mdash; A box whose value (or voidness) can be set only once.
* `Result` &mdash; A box whose value (or voidness) is set upon construction
  and cannot be altered.

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

Performs an identity comparison. No two different boxes are ever considered
equal.

#### `.totalOrder(other) -> int | void`

Performs an identity comparison. No two different boxes are ever considered
equal, and two different boxes have no defined order.


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

Concrete subclasses have differing behavior in response to this method.


<br><br>
### Class Method Definitions: `Cell` class

#### `.new(value?) -> cell`

Constructs a new cell. If `value` is specified, that is the value stored
in the cell. If `value` is not specified, the cell initially stores void.


<br><br>
### Method Definitions: `Cell` class

`Cell` inherits all its behavior from `Box`, except:

#### `.store(value?) -> . | void`

`Cell` implements the behavior as specified by `Box`, with no additions.


<br><br>
### Method Definitions: `NullBox` class

`NullBox` inherits all its behavior from `Box`, except:

#### `.store(value?) -> . | void`

Calling this method always succeeds, but never causes the `this` box to
refer to the so-specified value.


<br><br>
### Class Method Definitions: `Promise` class

#### `.new() -> promise`

Constructs a new promise. It initially stores void, and may be `.store()`d
to no more than once.


<br><br>
### Method Definitions: `Promise` class

`Promise` inherits all its behavior from `Box`, except:

#### `.store(value?) -> . | void`

`Promise` implements the behavior as specified by `Box`, except that
it is invalid to call this method twice on the same promise.


<br><br>
### Class Method Definitions: `Result` class

#### `.new(value?) -> cell`

Constructs a new result. If `value` is specified, that is the value stored
in the result. If `value` is not specified, the result stores void. Once
constructed, a `Result` is immutable and rejects attempts to `.store()` to it.


<br><br>
### Method Definitions: `Result` class

`Result` inherits all its behavior from `Box`, except:

#### `.store(value?) -> . | void`

Calling this method always results in the runtime terminating with an error.


<br><br>
### Constants

#### `nullBox`

A value that represents a permanently empty (un-set, un-stored) box.
`nullBox.store(value)` is effectively a no-op. This arrangement is done in
order to make it easy to pass a box into functions that require one, but where
the box value is never needed.

This is an instance of `NullBox`.
