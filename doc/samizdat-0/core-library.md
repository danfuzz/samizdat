Samizdat Layer 0
================

Core Library
------------

The core library is split into two layers, primitive and in-language.
From the perspective of "client" code written in the language, there
is no distinction between the two layers, but from the perspective of
implementation, there is. In particular, an implementation of
*Samizdat Layer 0* must provide the primitive library, but it can rely
on the canonical in-language library source for the remainder.

Each function binding here is listed in a short-hand form suggestive
of how it would be called.

* The name of the function is first on the line.

* Next come the names of arguments in order, separated with spaces but
  no other punctuation.

* If the function has no arguments, the marker `()` is appended to
  the name.

* If an argument is optional, it is followed by a `?` (question mark).

* A "rest" argument at the end (capturing zero or more additional
  arguments) is indicated by following it by a `*` (star / asterisk).

* Next the return type is indicated with a `<>` (a "diamond") and
  then one of: the specific value returned, name of the type
  returned, or one of `.` `~.` `. | ~.` as described above in the
  section about data types.


<br><br>
### Primitive Library: Constants

#### `false`

The boolean value false.

**Note:** Technically, this value could be defined in-language as
`false = [:@boolean @0:]`. However, as a practical matter the
lowest layer of implementation needs to refer to this value, so
it makes sense to allow it to be exported as a primitive.

#### `true`

The boolean value true.

**Note:** Technically, this value could be defined in-language as
`true = [:@boolean @1:]`. However, as a practical matter the
lowest layer of implementation needs to refer to this value, so
it makes sense to allow it to be exported as a primitive.

<br><br>
### Primitive Library: Ultraprimitive Functions

This set of primitive functions is required in order to execute
parse trees as produced by `sam0Tree` (see which).

#### `makeHighlet type value? <> highlet`

Returns a highlet with the given type tag (an arbitrary value)
and optional data payload value (also an arbitrary value). These
equivalences hold for *Samizdat Layer 0* source code:

```
v = [:key:];         is equivalent to   v = makeHighlet key;
v = [:key value:];   is equivalent to   v = makeHighlet key value;
```

#### `makeListlet rest* <> listlet`

Returns a listlet with the given elements (in argument order).
These equivalences hold for *Samizdat Layer 0* source code:

```
v = @[v1];      is equivalent to   v = makeListlet v1;
v = @[v1 v2];   is equivalent to   v = makeListlet v1 v2;
[etc.]
```

**Note:** The equivalence requires at least one argument, even though
the function is happy to operate given zero arguments.

**Note:** Technically, this function could be defined in-language as the
following, but for practical reasons &mdash; e.g. and in particular,
expected ordering of human operations during the course of
bootstrapping an implementation, as well as efficiency of
implementation (without sacrificing clarity) &mdash; it makes sense to
keep this defined as an "ultraprimitive":

```
makeListlet = { rest* :: <> rest; };
```

#### `makeMaplet rest* <> maplet`

Returns a maplet with the given key-value bindings (in argument
order), with each key-value pair represented as two consecutive
arguments. The number of arguments passed to this function must be
even. It is valid to repeat keys in the arguments to this function, in
which case the *final* value binding for any given key in the argument
list (in argument order) is the one that ends up in the result. These
equivalences hold for *Samizdat Layer 0* source code:

```
v = @[k1=v1];         is equivalent to   v = makeMaplet k1 v1;
v = @[k1=v1 k2=v2];   is equivalent to   v = makeMaplet k1 v1 k2 v2;
[etc.]
```

**Note:** The equivalence requires at least two arguments, even though
the function is happy to operate given zero arguments.

**Note:** Technically, this function could be defined in-language as the
following. (See `makeListlet` for discussion.):

```
makeMaplet = { rest* ::
    makeStep = { key value rest* ::
        restMap = apply makeMaplet rest;
        <> ifValue { <> mapletGet restMap key; }
            { <> restMap; }
            { <> mapletPut restMap key value; };
    };

    <> ifTrue { <> eq rest @[] }
        { <> @[=]; }
        { <> apply makeStep rest; };
};
```

#### `makeUniqlet() <> uniqlet`

Returns a uniqlet that has never before been returned from this
function (nor from any other uniqlet-producing source, should such a
source exist). This equivalence holds for *Samizdat Layer 0* source
code:

```
v = @@;   is equivalent to   v = makeUniqlet();
```


<br><br>
### Primitive Library: Conditionals and Iteration

#### `argsMap function rest* <> listlet`

Primitive mapping iterator. This calls the given function on each of
the rest of the arguments in sequence, collecting all the non-void
results into a listlet, which is returned. The function is called with
exactly one argument, namely the item to process.

**Note:** Unlike most of the map/reduce functions, this one takes its
function as the first argument. This is done specifically so that it is
convenient to `apply` it.

#### `argsReduce function base rest* <> . | ~.`

Primitive reducing iterator. This calls the given function on each of the
rest of the arguments in sequence, yielding a final reduction result.
(That is, this is a left-reduce operation.) The function is called with
exactly two arguments, first the previous non-void reduction result (or
the `base` for the first item in `rest`), and second the item to process.
The return value of this call is what would have been passed as the
reduction result to a would-be next function call.

**Note:** Unlike most of the map/reduce functions, this one takes its
function as the first argument. This is done specifically so that it is
convenient to `apply` it.

#### `ifTrue predicate trueFunction falseFunction? <> . | ~.`

Primitive boolean conditional. This calls the given predicate with no
arguments, expecting it to return a boolean.

If the predicate returns `true`, then the `trueFunction` is called
with no arguments. If the predicate returns `false`, then the
`falseFunction` (if any) is called with no arguments.

The return value from this function is whatever was returned by the
consequent function that was called (including void). If no consequent
was called, this returns void.

#### `ifValue function valueFunction voidFunction? <> . | ~.`

Primitive value conditional. This calls the given function with no
arguments, taking note of its return value or lack thereof.

If the function returns a value, then the `valueFunction` is called
with one argument, namely the value returned from the original
function. If the function returns void, then the `voidFunction` (if
any) is called with no arguments.

The return value from this function is whatever was returned by the
consequent function that was called (including void). If no consequent
was called, this returns void.

#### `loop function <> ~.`

Primitive unconditional loop construct. This repeatedly calls the given
function with no arguments.

In order for the loop to terminate, the function must use a nonlocal exit.

#### `loopReduce base function <> ~.`

Primitive unconditional loop construct, with reduce semantics. This repeatedly
calls the given function with a single argument. The argument is the
most recent non-void result of calling the function, or the `base` value
if the function has yet to return non-void (including notably to the first
call to the function).

In order for the loop to terminate, the function must use a nonlocal exit.


<br><br>
### Primitive Library: General Low-Order Values

#### `lowOrder value1 value2 <> intlet`

Returns the order of the two given values in the total order of
Samizdat low-layer values. This returns one of `@-1 @0 @1` indicating
how the two values sort with each other, using the reasonably
standard meaning of those values:

* `-1` &mdash; The first value orders before the second value.

* `0` &mdash; The two values are identical.

* `1` &mdash; The first value orders after the second value.

Ordering is calculated as follows:

The major order is by type &mdash; `intlet < stringlet < listlet <
maplet < uniqlet < highlet` &mdash; and minor order is type-dependant.

* Intlets order by integer value.

* Listlets and stringlets order by pairwise corresponding-element
  comparison, with a strict prefix always ordering before its
  longer brethren.

* Maplets order by first comparing corresponding ordered lists
  of keys with the same rules as listlet comparison. If the key
  lists are identical, then the result is the comparison of
  corresponding lists of values, in key order.

* Any given uniqlet never compares as identical to anything but
  itself. Any two uniqlets have a consistent, transitive, and
  commutative &mdash; but otherwise arbitrary &mdash; ordering.

* Highlets compare by type as primary, and value as secondary.
  With types equal, highlets without a value order earlier than
  ones with a value.

#### `lowOrderIs value1 value2 check1 check2? <> boolean`

The two values are compared as with `lowOrder`. The intlet
result of that comparison are checked for equality with
the one or two check values. If the comparison result is equal
to either check value, this function returns `true`. Otherwise
it returns `false`.

**Note:** This function exists in order to provide a primitive
comparison function that returns a boolean. Without it (or something
like it), there would be no way to define boolean-returning
comparators in-language.

#### `lowSize value <> intlet`

Returns the "size" of the given value. Every low-layer value has
a size, defined as follows:

* `intlet` &mdash; the number of significant bits (not bytes) in
  the value when represented in twos-complement form, including a
  high-order sign bit. The minimum size of an intlet is 1, which
  is the size of both `@0` and `@-1`.

* `stringlet` &mdash; the number of characters.

* `listlet` &mdash; the number of elements.

* `maplet` &mdash; the number of mappings (bindings).

* `uniqlet` &mdash; always `@0`.

* `highlet` &mdash; `@0` for a valueless highlet, or `@1` for a
  valued highlet.

#### `lowType value <> stringlet`

Returns the type name of the low-layer type of the given value. The
result will be one of: `@intlet` `@stringlet` `@listlet` `@maplet`
`@uniqlet` `@highlet`


<br><br>
### Primitive Library: Intlets

#### `iadd intlet1 intlet2 <> intlet`

Returns the sum of the given values.

#### `iand intlet1 intlet2 <> intlet`

Returns the binary-and (intersection of all one-bits) of the given values.

#### `ibit intlet n <> intlet`

Returns as an intlet (`@0` or `@1`) the bit value in the first
argument at the bit position (zero-based) indicated by the second
argument. It is an error (terminating the runtime) if the second
argument is negative.

#### `idiv intlet1 intlet2 <> intlet`

Returns the quotient of the given values (first over second),
truncated (rounded toward zero) to yield an intlet result. It is an
error (terminating the runtime) if the second argument is `@0`.

#### `imod intlet1 intlet2 <> intlet`

Returns the division modulus of the given values (first over
second). The sign of the result will always match the sign of the
second argument. It is an error (terminating the runtime) if the
second argument is `@0`.

`imod x y` can be thought of as the smallest magnitude value `m` with
the same sign as `y` such that `isub x m` is a multiple of `y`.

`imod x y` can be defined as `irem (iadd (irem x y) y) y`.

**Note:** This differs from the `irem` in the treatment of negative
numbers.

#### `imul intlet1 intlet2 <> intlet`

Returns the product of the given values.

#### `ineg intlet <> intlet`

Returns the negation (same magnitude, opposite sign) of the given
value.

#### `inot intlet <> intlet`

Returns the binary complement (all bits opposite) of the given value.

#### `ior intlet1 intlet2 <> intlet`

Returns the binary-or (union of all one-bits) of the given values.

#### `irem intlet1 intlet2 <> intlet`

Returns the truncated-division remainder of the given values (first
over second). The sign of the result will always match the sign of the
first argument. It is an error (terminating the runtime) if the second
argument is `@0`.

`irem x y` can be defined as `isub x (imul (idiv x y) y)`.

**Note:** This differs from the `imod` in the treatment of negative
numbers.

#### `ishl intlet shift <> intlet`

Returns the first argument (an intlet) bit-shifted an amount indicated
by the second argument (also an intlet). If `shift` is positive, this
is a left-shift operation. If `shift` is negative, this is a right-shift
operation. If `shift` is `@0`, this is a no-op, returning the first
argument unchanged.

**Note:** The `shift` argument is not limited in any particular way (not
masked, etc.).

#### `ishr intlet shift <> intlet`

Returns the first argument bit-shifted by an amount indicated by the
second argument. This is identical to `ishl`, except that the sense of
positive and negative `shift` is reversed.

**Note:** Unlike some other languages, there is no unsigned right-shift
operation in *Samizdat Layer 0*. This is because intlets are unlimited
in bit width, and so there is no way to define such an operation. If
you need "unsigned" operations, just operate consistently on
non-negative intlets.

#### `isub intlet1 intlet2 <> intlet`

Returns the difference of the given values (first minus second).

#### `ixor intlet1 intlet2 <> intlet`

Returns the binary-xor (bitwise not-equal) of the given values.


<br><br>
### Primitive Library: Stringlets

#### `intletFromStringlet stringlet <> intlet`

Given a single-character stringlet, returns the character code
of its sole character, as an intlet. It is an error (terminating
the runtime) if `stringlet` is not a stringlet of size 1.

#### `stringletAdd stringlet1 stringlet2 <> stringlet`

Returns a stringlet consisting of the concatenation of the contents
of the two argument stringlets, in argument order.

#### `stringletFromIntlet intlet <> stringlet`

Returns a single-character stringlet that consists of the character
code indicated by the given intlet argument, which must be in the
range for representation as an unsigned 32-bit quantity.

#### `stringletNth stringlet n notFound? <> . | ~.`

Returns the `n`th (zero-based) element of the given stringlet, as a
single-element stringlet, if `n` is a valid intlet index into the given
stringlet. If `n` is not valid (not an intlet, or out of range),
then this returns the `notFound` value (an arbitrary value) if supplied,
or returns void if `notFound` was not supplied.


<br><br>
### Primitive Library: Listlets

#### `listletAdd listlet1 listlet2 <> listlet`

Returns a listlet consisting of the concatenation of the elements
of the two argument listlets, in argument order.

#### `listletDelNth listlet n <> listlet`

Returns a listlet just like the given one, except that the `n`th
(zero-based) element is deleted, if `n` is a valid intlet index into
the given listlet. If `n` is not a valid index (not an intlet, or with
a value out of range), then this returns the original listlet as the
result.

#### `listletInsNth listlet n value <> listlet`

Returns a listlet just like the given one, except that the `n`th
(zero-based) element is set to be the given value (an arbitrary
value), and all elements at or beyond index `n` in the original
are shifted up by one index.

`n` must be non-negative and less than or equal to the size of the
listlet. If not, it is an error (terminating the runtime).

#### `listletNth listlet n notFound? <> . | ~.`

Returns the `n`th (zero-based) element of the given listlet, if `n` is
a valid intlet index into the listlet. If `n` is not a valid index
(either an out-of-range intlet, or some other value), then this
returns the `notFound` value (an arbitrary value) if supplied, or
returns void if `notFound` was not supplied.

#### `listletPutNth listlet n value <> listlet`

Returns a listlet just like the given one, except that the `n`th
(zero-based) element is set to be the given value (an arbitrary
value), replacing whatever was at that index in the original. If
`n` is the size of the original listlet, then this call behaves
exactly like `listletAppend`.

`n` must be non-negative and less than or equal to the size of the
listlet. If not, it is an error (terminating the runtime).


<br><br>
### Primitive Library: Maplets

#### `mapletAdd maplet1 maplet2 <> maplet`

Returns a maplet consisting of the combination of the mappings of the
two argument maplets. For any keys in common between the two maplets,
the second argument's value is the one that ends up in the result.

#### `mapletDel maplet key <> maplet`

Returns a maplet just like the one given as an argument, except that
the result does not have a binding for the key `key`. If the given
maplet does not have `key` as a key, then this returns the given
maplet as the result.

#### `mapletGet maplet key notFound? <> . | ~.`

Returns the value mapped to the given key (an arbitrary value) in
the given maplet. If there is no such mapping, then this
returns the `notFound` value (an arbitrary value) if supplied,
or returns void if `notFound` was not supplied.

#### `mapletNth maplet n notFound? <> . | ~.`

Returns the `n`th (zero-based) mapping of the given maplet, if `n` is
a valid intlet index into the maplet. If `n` is not a valid index
(either an out-of-range intlet, or some other value), then this
returns the `notFound` value (an arbitrary value) if supplied, or
returns void if `notFound` was not supplied.

The ordering of the mappings is by sort order of the keys.

#### `mapletNthKey maplet n notFound? <> . | ~.`

Returns the key of the `n`th (zero-based) mapping of the given maplet,
if `n` is a valid intlet index into the maplet. If `n` is not a valid index
(either an out-of-range intlet, or some other value), then this
returns the `notFound` value (an arbitrary value) if supplied, or
returns void if `notFound` was not supplied.

The ordering of the mappings is by sort order of the keys.

#### `mapletNthValue maplet n notFound? <> . | ~.`

Returns the value of the `n`th (zero-based) mapping of the given maplet,
if `n` is a valid intlet index into the maplet. If `n` is not a valid index
(either an out-of-range intlet, or some other value), then this
returns the `notFound` value (an arbitrary value) if supplied, or
returns void if `notFound` was not supplied.

The ordering of the mappings is by sort order of the keys.

#### `mapletPut maplet key value <> maplet`

Returns a maplet just like the given one, except with a new binding
for `key` to `value`. The result has a replacement for the existing
binding for `key` in `maplet` if such a one existed, or has an
additional binding in cases where `maplet` didn't already bind `key`.
These two scenarios can be easily differentiated by either noting a
change in size (or not) between original and result, or by explicitly
checking for the existence of `key` in the original.


<br><br>
### Primitive Library: Highlets

#### `highletHasValue highlet <> boolean`

Returns an indication of whether or not the given highlet has
a payload data value.

#### `highletType highlet <> .`

Returns the type tag value (an arbitrary value) of the given highlet.

#### `highletValue highlet notFound? <> . | ~.`

Returns the payload data value (an arbitrary value) of the given
highlet, if any. If the highlet is valueless, returns the `notFound`
value if supplied, or returns void if not.

<br><br>
### Primitive Library: Functions and Code

#### `apply function (value* listlet)? <> . | ~.`

Calls the given function with the given arguments.

It is valid to pass no arguments other than `function`, in which case the
function is called with no arguments.

If any other arguments are supplied, then each of the initial `value`
arguments is taken to be an arbitrary value, and the final `listlet`
argument must be a listlet, whose contents are "flattened" into the
final list of arguments to pass to the function. For example, this is
a five-argument call: `apply fizmoFunc @foo @bar @baz @[@frob @twiddle]`

This function returns whatever the called function returned (including
void).

#### `object implementation state <> function`

Constructs and returns an "object". In *Samizdat Layer 0* an object is
merely the combination of a mutable `state` value (an arbitarary
value) with an `implementation` function (an arbitrary function).
The return value from the call to `object` is another function,
referred to as the "object interface function".

When the object interface function is called, it in turn calls the
`implementation` function, passing it the same arguments as it was
called with, along with two additional arguments at the head of the
argument list:

* The first argument is a `yield` function. This function must be
  called exactly once during the course of a call to `implementation`.
  This is used to indicate the return value from the object interface
  function. If `yield` is called with no argument, then the object
  interface function returns void. If `yield` is called with one argument,
  then the object interface function returns that value.

  When the `yield` function is called, it does *not* cause the
  implementation function to return; it merely causes the eventual
  result value (or void) to be cached until the function does finally
  return.

* The second argument is a `state` value. This is the latest state
  for the object, as defined immediately below.

The implementation function can return either a value or void. If
it returns a value, then the value becomes the new `state` for
the object, replacing either the `state` originally passed to
`object`, or whatever other state had been returned in the mean time.

To avoid confusion as well as hew closely to the actor model,
it is invalid (terminating the runtime) for an object implementation
function to call its own interface function, either directly or
indirectly. To implement a recursive operation, it is necessary to do
so without going through the interface.

#### `sam0Eval context expressionNode <> . | ~.`

Returns the evaluation result of executing the given expression node,
which is a parse tree as specified in this document. It is valid for
the expression to yield void, in which case this function returns
void. Evaluation is performed in an execution context that includes
all of the variable bindings indicated by `context`, which must be a
maplet.

Very notably, the result of calling `sam0Tree` is valid as the
`expressionNode` argument here.

It is recommended (but not required) that the given `context` include
bindings for all of the library functions specified by this document.

#### `sam0Tree stringlet <> functionNode`

Parses the given stringlet as a program written in *Samizdat Layer 0*.
Returns a `function` node (as defined by the parse tree semantics)
that represents the function.


<br><br>
### Primitive Library: I/O

#### `io0Die stringlet? <> ~. (exits)`

Prints the given stringlet to the system console (as if with `io0Note`)
if supplied, and terminates the runtime with a failure status code (`1`).

#### `io0Note stringlet <> ~.`

Writes out a newline-terminated note to the system console or equivalent.
This is intended for debugging, and as such this will generally end up
emitting to the standard-error stream.

#### `io0PathFromStringlet stringlet <> pathListlet`

Converts the given path stringlet to an absolute form, in the "form factor"
that is used internally. The input `stringlet` is expected to be a
"Posix-style" path:

* Path components are separated by slashes (`"/"`).

* A path-initial slash indicates an absolute path.

* If there is no path-initial slash, then the result has the system's
  "current working directory" prepended.

* Empty path components (that is, if there are two slashes in a row)
  are ignored.

* Path components of value `@"."` (canonically representing "this directory")
  are ignored.

* Path components of value `@".."` cause the previous non-`..` path component
  to be discarded. It is invalid (terminating the runtime) for such a
  component to "back up" beyond the filesystem root.

The result is a listlet of path components, representing the absolute path.
None of the components will be the empty stringlet (`@""`), except possibly
the last. If the last component is empty, that is an indication that the
original path ended with a trailing slash.

#### `io0ReadFileUtf8 pathListlet <> stringlet`

Reads the named file, using the underlying OS's functionality,
interpreting the contents as UTF-8 encoded text. Returns a stringlet
of the read and decoded text.

`pathListlet` must be a listlet of the form described by `io0PathFromStringlet`
(see which). It is invalid (terminating the runtime) for a component to
be any of `@""` `@"."` `@".."` or to contain a slash (`/`).

#### `io0ReadLink pathListlet <> pathListlet | ~.`

Checks the filesystem to see if the given path refers to a symbolic
link. If it does, then this returns the path which represents the
direct resolution of that link. It does not try to re-resolve
the result iteratively, so the result may not actually refer to a
real file (for example).

If the path does not refer to a symbolic link, then this function returns
void.

`pathListlet` must be a listlet of the form described by `io0PathFromStringlet`
(see which). See `io0ReadFileUtf8` for further discussion.

#### `io0WriteFileUtf8 pathListlet text <> ~.`

Writes out the given text to the named file, using the underlying OS's
functionality, and encoding the text (a stringlet) as a stream of UTF-8 bytes.

`pathListlet` must be a listlet of the form described by `io0PathFromStringlet`
(see which). See `io0ReadFileUtf8` for further discussion.


### Primitive Library: Meta-Library

#### `LIBRARY`

A maplet of bindings of the entire *Samizdat Layer 0*
primitive library, except for the binding of `LIBRARY` itself (which can't
be done in that the data model doesn't allow self-reference).

**Note:** This is a constant maplet value, not a function.

**Note:** This binding as the *primitive* library is only available when
the in-language core library is first being loaded. When non-library code
is loaded, its `LIBRARY` binding is the full core library, including both
primitives and in-language definitions.


<br><br>
### In-Language Library: Constants

#### `null`

A value used when no other value is suitable, but when a value is
nonetheless required. It can also be written as `[:@null:]`.

**Note:** This is a constant value, not a function.


<br><br>
### In-Language Library: Comparisons / Booleans

#### `eq value1 value2 <> boolean`

Checks for equality. Returns `true` iff the two given values are
identical.

#### `le value1 value2 <> boolean`

Checks for a less-than-or-equal relationship. Returns `true` iff the
first value orders before the second or is identical to it.

#### `lt value1 value2 <> boolean`

Checks for a less-than relationship. Returns `true` iff the first value
orders before the second.

#### `ge value1 value2 <> boolean`

Checks for a greater-than-or-equal relationship. Returns `true` iff the
first value orders after the second or is identical to it.

#### `gt value1 value2 <> boolean`

Checks for a greater-than relationship. Returns `true` iff the first value
orders after the second.

#### `ne value1 value2 <> boolean`

Checks for inequality. Returns `true` iff the two given values are not
identical.

#### `not boolean <> boolean`

Returns the opposite boolean to the one given.

**Note:** Only accepts boolean arguments.


<br><br>
### In-Language Library: Types

#### `isHighlet value <> boolean`

Returns `true` iff the given value is a highlet.

#### `isIntlet value <> boolean`

Returns `true` iff the given value is an intlet.

#### `isListlet value <> boolean`

Returns `true` iff the given value is a listlet.

#### `isMaplet value <> boolean`

Returns `true` iff the given value is a maplet.

#### `isStringlet value <> boolean`

Returns `true` iff the given value is a stringlet.

#### `isUniqlet value <> boolean`

Returns `true` iff the given value is a uniqlet.


<br><br>
### In-Language Library: Conditionals

#### `and predicates* <> boolean`

Short-circuit conjunction. Takes an arbitrary number of predicates,
each a no-argument function. Calls each of them in turn until one of
them returns `false`, in which case this function also returns
`false`. If no predicate returns `false`, this function returns
`true`.

#### `ifFalse predicate falseFunction trueFunction? <> . | ~.`

This is identical to `ifTrue` except that the order of the second
and third arguments is reversed.

#### `ifVoid function voidFunction valueFunction? <> . | ~.`

This is identical to `ifValue` except that the order of the second
and third arguments is reversed.

#### `or predicates* <> boolean`

Short-circuit disjunction. Takes an arbitrary number of predicates,
each a no-argument function. Calls each of them in turn until one of
them returns `true`, in which case this function also returns
`true`. If no predicate returns `true`, this function returns `false`.


<br><br>
### In-Language Library: Intlets

#### `intletSign intlet <> intlet`

Returns the sign of the given value: `@-1` for negative values,
`@1` for positive values, or `@0` for `@0`.


<br><br>
### In-Language Library: Stringlets

#### `stringletCat stringlet rest* <> stringlet`

Concatenates one or more stringlets together into a single resulting
stringlet.

#### `stringletForEach stringlet function <> ~.`

Calls the given function on each element (character) of the given stringlet.
The given function is called with two arguments, namely the element (as a
single-character stringlet) and its index number (zero-based). Always returns
void.

#### `stringletMap stringlet function <> listlet`

Maps each element of a stringlet using a mapping function, collecting
the results into a listlet (note, not into a stringlet). The given
function is called on each element (character), with two arguments,
namely the element (as a single-character stringlet) and its index
number (zero-based).

Similar to `argsMap`, if the function returns void, then no item is
added for the corresponding element. This means the size of the
result may be smaller than the size of the argument.

**Note:** Unlike many other languages with similar functions, the
function argument is the *last* one and not the *first* one. This is
specifically done to make it natural to write a multi-line function
without losing track of the other two arguments.

#### `stringletReduce base stringlet function <> . | ~.`

Reduces a stringlet to a single value, given a base value and a
reducer function, operating in low-to-high index order (that is, this
is a left-reduce operation). The given function is called once per
stringlet element (character), with three arguments: the last (or base)
reduction result, the element (as a single-character stringlet), and its
index number (zero-based). The function result becomes the reduction
result, which is passed to the next call of `function` or becomes the
return value of the call to this function if it was the call for the
final element.

Similar to `argsReduce`, if the function returns void, then the
previously-returned non-void value (or `base` value if there has
yet to be a non-void function return) is what is passed to the
subsequent iteration and returned at the end of the call.

See note on `stringletMap` about choice of argument order.


<br><br>
### In-Language Library: Listlets

#### `listletAppend listlet value <> listlet`

Returns a listlet consisting of the elements of the given
listlet argument followed by the given additional value.

#### `listletCat listlet rest* <> listlet`

Concatenates one or more listlets together into a single resulting
listlet.

#### `listletForEach listlet function <> ~.`

Calls the given function on each element of the given listlet.
The given function is called with two arguments, namely the element
and its index number (zero-based). Always returns void.

#### `listletMap listlet function <> listlet`

Maps each element of a listlet using a mapping function, collecting
the results into a new listlet. The given function is called on each
listlet element, with two arguments, namely the element and its index
number (zero-based).

Similar to `argsMap`, if the function returns void, then no item is
added for the corresponding element. This means the size of the
result may be smaller than the size of the argument.

See note on `stringletMap` about choice of argument order.

#### `listletPrepend value listlet <> listlet`

Returns a listlet consisting of the given first value followed by the
elements of the given listlet argument.

**Note:** The arguments are given in an order meant to reflect the
result (and not listlet-first).

#### `listletReduce base listlet function <> . | ~.`

Reduces a listlet to a single value, given a base value and a reducer
function, operating in low-to-high index order (that is, this is a
left-reduce operation). The given function is called on each listlet
element, with three arguments: the last (or base) reduction result,
the element, and its index number (zero-based). The function result
becomes the reduction result, which is passed to the next call of
`function` or becomes the return value of the call to this function if
it was the call for the final element.

Similar to `argsReduce`, if the function returns void, then the
previously-returned non-void value (or `base` value if there has
yet to be a non-void function return) is what is passed to the
subsequent iteration and returned at the end of the call.

See note on `stringletMap` about choice of argument order.


<br><br>
### In-Language Library: Maplets

#### `mapletCat maplet rest* <> maplet`

Concatenates one or more maplets together into a single resulting
maplet. If there are any duplicate keys among the arguments, then
value associated with the last argument in which that key appears
is the value that "wins" in the final result.

#### `mapletForEach maplet function <> ~.`

Calls the given function for each mapping in the given maplet. The
function is called with two arguments, a value from the maplet and
its corresponding key (in that order). Always returns void.

#### `mapletMap maplet function <> maplet`

Maps the values of a maplet using the given mapping function,
resulting in a maplet whose keys are the same as the given maplet but
whose values may differ. In key order, the function is called with
two arguments representing the binding, a value and a key (in that
order, because it is common enough to want to ignore the key). The
return value of the function becomes the bound value for the given
key in the final result.

Similar to `argsMap`, if the function returns void, then no item is
added for the corresponding element. This means the size of the
result may be smaller than the size of the argument.

See note on `stringletMap` about choice of argument order.

#### `mapletReduce base maplet function <> . | ~.`

Reduces a maplet to a single value, given a base value and a reducer
function, operating in key order. The given function is called on each
maplet binding, with three arguments: the last (or base) reduction
result, the value associated with the binding, and its key. The
function result becomes the reduction result, which is passed to the
next call of `function` or becomes the return value of the call to
this function if it was the call for the final binding.

Similar to `argsReduce`, if the function returns void, then the
previously-returned non-void value (or `base` value if there has
yet to be a non-void function return) is what is passed to the
subsequent iteration and returned at the end of the call.

See note on `stringletMap` about choice of argument order.


<br><br>
### In-Language Library: I/O

#### `io0SandboxedReader directory <> function`

Returns a file reader function which is limited to *only* reading
files from underneath the named directory (a path-listlet as
described in `io0PathFromStringlet`). The return value from this call
behaves like `ioReadFileUtf8`, as if the given directory is both the
root of the filesystem and is the current working directory. Symbolic
links are respected, but only if the link target is under the named
directory.

This function is meant to help enable a "supervisor" to build a sandbox
from which untrusted code can read its own files.


<br><br>
### In-Language Library: String Formatting

#### `format formatStringlet rest* <> stringlet`

Using the given `formatStringlet`, format the remaining arguments, returning
a stringlet. Formatting syntax is in the style of C's `printf()`. Specifically,
all characters from the `formatStringlet` are passed through to the result
as-is, except that percent (`%`) is used to introduce format conversions
of each of the `rest` arguments. Each time a percent is encountered
(with one exception noted below), another one of the `rest` arguments is
"consumed" and formatted.

The following are the format codes and their meanings:

* `%%` &mdash; Outputs a literal percent. This does not consume an argument.

* `%s` &mdash; Assumes the argument is a stringlet, and includes it in
  the output without additional formatting.

* `%q` &mdash; "Quotes" the argument by passing it through `sourceStringlet`
  (see which).

* `%Q` &mdash; "Quotes" the argument without top-level adornment through
  `sourceStringletUnadorned` (see which).

#### `sourceStringlet value <> stringlet`

Converts an arbitrary value into a stringlet representation form
that is meant to mimic the Samizdat source syntax.

**Note:** The output differs from *Samizdat Layer 0* syntax in that
stringlet forms can include two escape forms not defined in the
language:

* `\0` &mdash; This represents the value 0.

* `\xHEX;` where `HEX` is a sequence of one or more hexadecimal digits
  (using lowercase letters) &mdash; These represent the so-numbered
  Unicode code points, and this form is used to represent all
  non-printing characters other than newline that are in the Latin-1
  code point range.

#### `sourceStringletUnadorned value <> stringlet`

This is just like `sourceStringlet`, except that top-level adornment
(quotes, etc.) are not produced.


<br><br>
### In-Language Library: Meta-Library

#### `LIBRARY`

A maplet of bindings of the entire *Samizdat Layer 0*
library, except for the binding of `LIBRARY` itself (which can't
be done in that the data model doesn't allow self-reference).

**Note:** This is a constant maplet value, not a function.

#### `makeLibrary maplet <> maplet`

Takes a library binding maplet and returns one that is just like the
one given, except that the key `@LIBRARY` is bound to the given
maplet. This makes a `LIBRARY` binding into a form suitable for
passing as the library / global context argument to evaluation
functions (such as `sam0Eval`), in that callees can rightfully expect
there to be a binding for `LIBRARY`.


<br><br>
### In-Language Library: Combinators

#### `yCombinator function <> function`

The Y combinator, in a form suitable for use in Samizdat Layer 0 when
defining self-recursive functions.

**Summary**

If you want to make a self-recursive function in *Samizdat Layer 0*,
you can write it like this:

```
myRecursiveFunction = yCombinator { selfCall ::
    # Inner function.
    <> { myArg1 myArg2 ... ::
        ... my code ...
        selfCall args ...
        ... my code ...
    }
};
```

In the example above, a call to `selfCall` will end up recursively
calling into the (so-labeled) inner function.

**Detailed Explanation**

This function takes another function as its argument, called the
"wrapper" function. That function must take a single argument, which itself
is a function, called the "recurser" function. The "wrapper" function must
return yet another function, called the "inner" function. The return value
of this function, called the "result" function, is a function which, when
called, ends up calling the wrapper function and then calling the inner
function that the wrapper function returned.

The inner function is an arbitrary function, taking arbitrary arguments,
returning anything including void, and performing any arbitrary
actions in its body. In particular, it can be written to call the
"recurser" argument passed to its wrapper. If it does so, that will in
turn result in a recursive call to itself.

This function is used to write recursive functions without relying
on use-before-def variable binding.

See Wikipedia [Fixed-point
combinator](http://en.wikipedia.org/wiki/Fixed-point_combinator) for
more details about some of the theoretical basis for this stuff.
This function is in the "Y combinator family" but is not exactly any
of the ones described on that page. It is most similar to the Z
combinator, which is also known as the "call-by-value Y combinator"
and the "applicative-order Y combinator", but this function is not
*exactly* the Z combinator. In particular, this version is written
such that the wrapper function always gets called directly with the
result of a U combinator call. It is unclear whether this results
in any meaningful difference in behavior. See also [this question on
StackOverflow](http://stackoverflow.com/questions/16258308).

In traditional notation (and with
the caveat that `a` represents an arbitrary number of arguments here),
this function would be written as:

```
U = λx.x x
Y_sam = λf . U (λs . (λa . (f (U s)) a))
```

#### `yStarCombinator functions* <> @[functions*]`

The Y* combinator, in a form suitable for use in Samizdat Layer 0 when
defining sets of mutually-recursive functions.

This is like `yCombinator`, except that it can take any number of
functions as arguments, resulting in a listlet of mutual-recursion-enabled
result functions.

If you want to make a set of N mututally-recursive functions in
*Samizdat Layer 0*, you can write it like this:

```
myRecursiveFunctions = xCombinator
    { selfCall1 selfCall2 ... ::
        # Inner function.
        <> { myArg1 myArg2 ... ::
            ... my code ...
            selfCall1 args ... # Call this function self-recursively.
            selfCall2 args ... # Call the other function.
            ... my code ...
        }
    }
    { selfCall1 selfCall2 ... ::
        # Inner function.
        <> { myArg1 myArg2 ... ::
            ... my code ...
            selfCall1 args ... # Call the other function.
            selfCall2 args ... # Call this function self-recursively.
            ... my code ...
        }
    };
```

This results in an array of functions corresponding to the original argument
functions. Each of those functions can recurse by calling any of the other
functions, via the arguments passed into the wrapper functions (arguments
prefixed `selfCall` in the example here).

See `yCombinator` for more detailed discussion and explanation.
