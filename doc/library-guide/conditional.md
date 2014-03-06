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

**Syntax Note:** Used in the translation of some `&` forms and
multiple-binding `if` forms.

#### `loop(function) <> void`

Primitive unconditional loop construct. This repeatedly calls the given
function with no arguments.

In order for the loop to terminate, the function must use a nonlocal exit.

**Syntax Note:** Used in the translation of `do` and `while` forms.


<br><br>
### In-Language Definitions

(none)
