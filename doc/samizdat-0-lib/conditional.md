Samizdat Layer 0: Core Library
==============================

Conditionals And Iteration
--------------------------

<br><br>
### Primitive Definitions

#### `ifIs(testFunction, isFunction, notFunction?) <> . | void`

Primitive logic conditional. This calls the given `testFunction` with no
arguments, taking note of its return value or lack thereof.

If the function returns a value, then the `isFunction` is called
with no arguments. If the predicate returns void, then the
`notFunction` (if any) is called with no arguments.

The return value from this function is whatever was returned by the
consequent function that was called (including void). If no consequent
was called, this returns void.

This function is identical to `ifValue`, except that in the value case,
this function calls the consequent function with no arguments, whereas
`ifValue` calls it with an argument.

**Syntax Note:** Used in the translation of `if`, `expression??`,
and `expression & expression` forms.


#### `ifNot(testFunction, notFunction) <> . | void`

This is identical to `ifIs`, except that the `isFunction` argument is omitted.

**Syntax Note:** Used in the translation of `do` and `!expression` forms.

#### `ifValue(testFunction, valueFunction, voidFunction?) <> . | void`

Primitive logic conditional. This calls the given `testFunction` with no
arguments, taking note of its return value or lack thereof.

If the function returns a value, then the `valueFunction` is called
with one argument, namely the value returned from the original
function. If the function returns void, then the `voidFunction` (if
any) is called with no arguments.

The return value from this function is whatever was returned by the
consequent function that was called (including void). If no consequent
was called, this returns void.

This function is identical to `ifIs`, except that in the value case,
this function calls the consequent function with an argument, whereas
`ifIs` calls it with no arguments.

**Syntax Note:** Used in the translation of `if`, `switch`, `while`, and
`expression & expression` forms.

#### `ifValueOr(testFunction, voidFunction) <> . | void`

This is identical to `ifValue`, except that the `valueFunction` is
omitted and taken to be the identity function, and the `voidFunction`
is required (not an optional argument). That is, `ifValueOr(x, y)` is the
same as `ifValue(x, { value <> value }, y)`.

The reason `voidFunction` is required is because it is pointless to omit it,
in that `ifValueOr(x)` would mean the same thing as `x()`, and
`ifValueOr({ <> x })` would mean the same thing as just `x`.

This function is meant as the primitive that higher-layer logical-or
expressions bottom out into, hence the name.

**Syntax Note:** Used in the translation of `expression | expression` forms.

#### `ifValues([testFunctions*], valueFunction, voidFunction?) <> . | void`

Primitive logic conditional. This calls each of the given `testFunctions`
in order, as long as each returns a value (not void). The list of previous
results are passed to each subsequent test function. Should all of the
`testFunctions` return a value, this then calls the `valueFunction`
passing it a full list of test results. Should any of the `testFunctions`
return void, this then calls the `voidFunction` (if any) with no arguments.

The return value from this function is whatever was returned by the
consequent function that was called (including void). If no consequent
was called, this returns void.

**Syntax Note:** Used in the translation of multiple-binding `if` forms.

#### `loop(function) <> void`

Primitive unconditional loop construct. This repeatedly calls the given
function with no arguments.

In order for the loop to terminate, the function must use a nonlocal exit.

**Syntax Note:** Used in the translation of `do` and `while` forms.

#### `loopReduce(function, baseValues*) <> void`

Primitive unconditional loop construct, with reduce semantics. This repeatedly
calls the given function with a list of arguments. The arguments are the
most recent non-void result of calling the function (which must be a list),
or the original `baseValues` list if the function has yet to return non-void
(including notably to the first call to the function).

In order for the loop to terminate, the function must use a nonlocal exit.

#### `optValue(function) <> list`

Function call helper, to deal with value-or-void situations. This calls
`function` with no arguments, wrapping its return value in a list and in
turn returning that list. That is, if `function` returns `value`, then this
function returns `[value]`, and if `function` returns void, then this
function returns `[]`.

This function could be implemented as:

```
fn optValue(function) {
    <> [&function()] | []
}
```

or more primitively as:

```
fn optValue(function) {
    <> ifValue(function, { v <> [v] }, { <> [] })
}
```

**Syntax Note:** Used in the translation of string interpolation,
`expression?`, and `break` and other nonlocal exit forms.


<br><br>
### In-Language Definitions

(none)
