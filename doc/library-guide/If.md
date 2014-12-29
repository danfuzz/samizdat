Samizdat Layer 0: Core Library
==============================

If
--

`If` is a utility class (an uninstantiable repository for class methods),
which implements all the standard conditional methods.


<br><br>
### Class Method Definitions

#### `class.and(functions*) -> . | void`

Primitive logic conditional. This calls each of the given `functions`
in order, as long as each returns a value (not void). The previous call
results are passed as arguments to each subsequent test function. That is,
the first call gets passed no arguments, the second gets passed the first
result, the third gets passed the first and second result, and so on.

Should all of the `functions` return a value, then this function returns
the result of the *last* of `functions`. Should any of the calls return
void, or if no functions are passed at all, then this function immediately
returns void.

**Syntax Note:** Used in the translation of some `&` forms and
multiple-binding `if` forms.

#### `class.andThenElse(functions*, thenFunction, elseFunction) -> . | void`

Primitive logic conditional. This is like `.and()`, except that the
function takes two additional arguments at the end of the argument list,
which are treated specially.

In particular, if all of the initial `functions` return a value, then this
calls the `thenFunction` (passing it all the results of the `functions`,
in order) and returns its value. Otherwise &mdash; that is, if any of the
`functions` returns void &mdash; then this calls the `elseFunction` and
returns its value.

If no `functions` are passed, then this executes the `thenFunction` and
not the `elseFunction`: The condition for "then" is that all of the
`functions` succeed, which is trivially true in this case.

Note that the argument list is "stretchy" in front, which isn't
representable in real Samizdat syntax.

**Syntax Note:** Used in the translation of some multiple-binding `if` forms.

#### `class.cases(testFunction, valueFunctions, optDefaultFunction?) -> . | void`

Case-switched conditional. This calls the given `testFunction` with
no arguments, taking note of its return value (hereafter, the "test result").
`testFunction` must return a value; if not, it is a fatal error (terminating
the runtime).

The test result is looked up as a key in the given `valueFunctions`, which
must be a map which binds to functions as its values. If a binding is found,
then that function is called, passing it the test result as the sole argument.
If a binding is *not* found, then the `optDefaultFunction*` (if passed) is
called, again with the test result as the sole argument.

The return value of this function is the same as the result of whatever
consequent function was called, if any. If no consequent function was called,
this function returns void.

**Syntax Note:** Used in the translation of `switch` forms.

#### `class.is(testFunction, isFunction, notFunction?) -> . | void`

Primitive logic conditional. This calls the given `testFunction` with no
arguments, taking note of its return value or lack thereof.

If the function returns a value, then the `isFunction` is called
with no arguments. If the predicate returns void, then the
`notFunction` (if any) is called with no arguments.

The return value from this function is whatever was returned by the
consequent function that was called (including void). If no consequent
was called, this returns void.

This function is identical to `.value()`, except that in the value case,
this function calls the consequent function with no arguments, whereas
`.value()` calls it with an argument.

**Syntax Note:** Used in the translation of `if` and `expression & expression`
forms.

#### `class.loop(function) -> .`

Primitive unconditional loop construct. This repeatedly calls the given
`function` with no arguments.

In order for the loop to terminate, the `function` must use a nonlocal exit.

This is, of course, not a conditional of any sort. Nonetheless, it is defined
here to go with `.loopUntil()`.

**Syntax Note:** Used in the translation of `do` and `while` forms.

#### `class.loopUntil(function) -> .`

Primitive conditional loop. This calls `function` with no arguments. If
it returns a value (not void), then this method returns that value. Otherwise,
it repeats.

#### `class.maybeValue(function) -> isa List`

Function call helper, to deal with value-or-void situations. This calls
`function` with no arguments, wrapping its return value in a list and in
turn returning that list. That is, if `function` returns `value`, then this
function returns `[value]` (a single-element list), and if `function` returns
void, then this function returns `[]` (the empty list).

This method could be implemented as:

```
class.maybeValue(function) {
    return (def v = function()) & [v] | []
}
```

or more primitively as:

```
class.maybeValue(function) {
    return If.value(function, { v -> [v] }, { -> [] })
}
```

**Syntax Note:** Used in the translation of string interpolation and
`expression?` forms.

#### `class.not(testFunction, notFunction) -> . | void`

This is identical to `.is()`, except that the `isFunction` argument is
omitted and the `notFunction` argument is required (not optional).

**Syntax Note:** Used in the translation of `do` and `!expression` forms.

#### `class.or(functions*) -> . | void`

Primitive logic conditional. This calls each of the given `functions` in
order with no arguments, until one of them returns non-void. When a non-void
result is obtained, this function returns that value. Otherwise (that is,
if all the `functions` returned void) this function returns void.

This function is meant as the primitive that higher-layer logical-or
expressions bottom out into, hence the name.

**Syntax Note:** Used in the translation of `expression | expression` forms.

#### `class.value(testFunction, valueFunction, voidFunction?) -> . | void`

Primitive logic conditional. This calls the given `testFunction` with no
arguments, taking note of its return value or lack thereof.

If the function returns a value, then the `valueFunction` is called
with one argument, namely the value returned from the original
function. If the function returns void, then the `voidFunction` (if
any) is called with no arguments.

The return value from this function is whatever was returned by the
consequent function that was called (including void). If no consequent
was called, this returns void.

This function is identical to `.is()`, except that in the value case,
this function calls the consequent function with an argument, whereas
`.is()` calls it with no arguments.

**Syntax Note:** Used in the translation of `if`, `switch`, `while`, and
`expression & expression` forms.
