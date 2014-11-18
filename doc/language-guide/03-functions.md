Samizdat Language Guide
=======================

Functions and Blocks
--------------------

Samizdat is a closure-forward language, in that almost any nontrivial
piece of code will consist of multiple nested closures. Closures in the
language are the typical sort, namely a function which captures elements
from its execution environment and which continues to be valid after its
enclosing execution environment has exited.

There are two kinds of closure in the language, "functions" and "blocks,"
which correspond to two different common use cases. Both forms have in
common that the body of the closure (containing its statements) are
enclosed within matching curly braces (`{...}`). The forms differ in
how their declarations look, and there are a couple of semantic differences
as well, as detailed below.

With the exception of the top level of a program, every closure has
curly brace delimiters  (`{...}`). Note that curly braces are *also*
used to delimit map literals, which leads to a couple ambiguities; in
general, ambiguities are resolved in favor of treating a construct as
a map literal, not a closure.

Closures can be declared to take arguments, and closures can be defined
either to yield (return) a value, or yield no value, sometimes determined
at runtime. Finally, all closures can have a name; in some cases, the
name is used for variable binding, while in others, only as debugging
information.


### Argument Declarations

Every closure can declare itself to take some number of arguments (including
none). Functions and blocks differ in where arguments are declared,
syntactically, but within an argument declaration clause the two forms are
identical.

An argument declaration clause consists of a comma-separated list of
individual argument declarations. Each argument declaration consists
of a name and, optionally, a repetition specifier.

The argument name is *either* a variable name, using the same syntax
as variable names elsewhere (see which), or the special name `.` (that is,
a dot / period) to indicate that the argument is being declared but will
not be accessed in the body of the closure. (The `.` form is useful when
defining closures whose context requires they take certain arguments, even
though they don't need to use those arguments.)

The repetition specifier is a postfix on the argument name, and can be
one of the following:

* `?` &mdash; An optional argument.

* `*` &mdash; Any number of optional arguments (including none).

* `+` &mdash; Any positive number of optional arguments (that is,
  at least one).

If a repetition specifier is present on an argument, then in the environment
of the closure, the variable associated with the argument will always be a
list, consisting of the actual values bound to the argument. For example, an
optional argument will always be a list of zero or one element.

When bound, repetition specifiers consistently cause "maximum greed" and
no backtracking. As such, it does not make sense to list an unmarked argument
after one that is marked (since the unmarked argument could never be
successfully bound). It also only makes sense to only ever use one of
`+` or `*`, and only on the final argument.


### Body

The main body of a closure is a sequence of statements. The last
statement of a closure can optionally be one of several possible
yield (return) statements. See below for details.

Statements are separated from each other with semicolons (`;`). In addition
(TODO), there is implicit statement separation based on the indentation
level of adjacent lines.

Aside from yields, a statement can be either an arbitrary expression
(as documented elsewhere in this guide) or one of several special
definition statements. The `fn` statement is described in this section.
Other statements are described elsewhere.


### Yield / return

Every closure has the potential to yield (return) a value to its caller. There
are several ways to represent this in code, with various meanings.

#### Yield Definitions

Every closure can optionally define an explicit name to use within its
body to yield from the closure. This allows code to cause an exit out of a
closure other than the one that directly contains the yield in question.

A yield definition consists of a slash (`/`) followed by a variable name.
When present, a yield definition occurs just before the first statement
of a closure.

#### Yield statements

The final statement of a closure can optionally be a yield statement
of some form. There are several kinds of yield statement. Most of them
consist of one of the yield operators, optionally followed by an arbitrary
expression. If an expression is not supplied, this indicates that the
yield value is void.

Two kinds of yield are applicable in all contexts:

* `yield` &mdash; Direct yield from closure. This yields a
  value from the closure that this statement appears directly in.

* `yield /name` &mdash; Yield from named yield point. The `name` must match
  the yield definition name of an enclosing closure. (See "Yield Definitions,"
  above.) In addition, the so-named enclosing closure must still be
  in the middle of executing; that is, the enclosing closure must not
  have yet yielded (including yielding void). It is a fatal error to try to
  yield from a closure that has already yielded.

  If there is more than one closure in the lexical environment that has the
  same yield definition name, this form binds to the closest enclosing one.

One kind of yield is applicable in the context of a function definition
(as defined in this section):

* `return` &mdash; Yield from closest enclosing function definition. As
  with named yield (above), it is a fatal error to try to yield from a
  function invocation which has already yielded.

The two additional forms of yield, `break` and `continue`, are associated
with looping constructs and are described along with those constructs.

#### Maybe yield

The base yield statement syntax is always explicit about whether a value
or void is being yielded; if a value expression is present, then it must
evaluate to a value (not void).

In contexts where it is not the case that the yield expression necessarily
evaluates to a value, then the base yield operator can be followed by a
question mark (`?`). This indicates that the expression is allowed to
evaluate to void, and if it does, for the closure itself to yield void.

#### Default yield

In many (but not all) cases, an unmarked expression at the end of a
closure is treated as if it were preceded by `yield?`, that is, it is
treated as a maybe-yield. This is done as a way to strike a balance
between total safety and practical convenience, in cases where an
unintentionally "leaked" value is unlikely to cause harm.

In particular, this is only the case when the closure in question does *not*
bind a named (`/name`) or implicit (e.g. `return` binding) nonlocal
exit. This includes named yields from control constructs (e.g.,
`if /name ...`). In all of these contexts, if you want to yield void, you
have to do so explicitly.


### Function statements

Functions are closures that are meant to "stand alone." These
correspond to functions as used by most languages in the C tradition.
Functions have a name and get bound to a variable (sometimes a local
variable, and sometimes a module variable) whose name matches the
function name.

When a function statement is used, it is as if the variable naming the
function is declared at the top of the block in which the statement
appears, though it only becomes bound to the function when the statement
is executed in the usual order. The upshot of this is that functions
can generally be called from within other functions defined in the same
block, and from within the function itself. (That is, both self- and
mutual-recursion are possible and reasonably convenient.)

Functions definitions are introduced with the `fn` keyword. The keyword
is followed by declarations, and then followed by the main code body,
inside curly braces.

The declarations to a function consist of the following, in order:

* The name. This provides a name for the function, which serves three
  purposes:

  * It provides a name that can be used when calling the function.
  * It provides a name that is visible in some debugging contexts
    (such as when generating stack traces).
  * It provides the name of the variable to bind the function to.

* The formal arguments. These consist of a comma-separated list of
  individual argument declarations, all enclosed within parentheses
  (`(...)`). If there are no arguments to declare, the parentheses are
  still required. See "Argument Declarations" above.

Aside from the name binding, the one semantic difference between functions
and blocks is that functions define the closure that a `return` statement
will yield from. It is also valid to define an explicit yield variable,
as with non-function closures. See "Yield Defintion" above.

Similar to immutable variables, functions can be exported from a module,
by prefixing the definition with `export`.

Examples:

```
## This is a function, which defines the function named `blort`, binding it
## to a variable with the same name. The function can take one or two
## arguments. `...` would be replaced with a full function body.
fn blort(a, b?) { ... }

## This is a function expression, which defines the function named `fizmo`.
## The function takes one or more arguments and returns the list of them
## to its caller.
fn fizmo(args+) { return args }

## This is a function that includes a yield definition. Within the body,
## `yield /out` indicates a yield from `igram`. The function takes any number
## of arguments (including zero).
fn igram(a*) { /out -> ... yield /out ... }

## The same as the previous, exported from its module.
export fn igram(a*) { /out -> ... yield /out ... }
```

### Blocks

Blocks are closures that generally serve as arguments to function calls
or as elements of complex expressions. These serve the same purpose as
compound statements in most languages in the C tradition.

Syntactically, blocks start with an open curly brace (`{`) and end with a
close curly brace (`}`). Within the braces, blocks start with an optional
declaration section and are followed by the main code body.

The declaration section consists of the following, in order:

* The name (optional), in the form of an identifier in the language.
  This name is only used for debugging purposes. That is, using a name
  here does not cause any variable binding to take place, and particularly
  *does not* provide a name for self-recursive calls.

* The formal arguments. These consist of a comma-separated list of
  individual argument declarations, enclosed in parentheses. If the
  closure has no name, then the parentheses are optional. See "Argument
  Declarations" above for more details.

* The yield definition (optional). See "Yield Definition" above.

* The special token right-arrow (`->`), to indicate the end of
  the declarations. If there are no declarations, then the right-arrow can
  be omitted.

Examples:

```
## This is a block that takes no arguments.
def borch = { ... }

## This is a block that takes no arguments but does define a yield point.
def frotz = { /leave -> ... yield /leave ... }

## This is equivalent to the second example in the "Function Statements"
## section, above.
def fizmo = { args+ /out -> yield /out args }

## This is one with everything. Note that the variable name (`ignatz` in this
## case) does not have to match the closure name (`krazy` in this case).
def ignatz = { krazy(x, y?, z*) /out -> ... yield /out ... }

## Since the main body is just a yield, no right-arrow is required.
def krazy = { x, y -> x + y }
```

#### The empty block

In order to disambiguate with the empty map, an otherwise empty block
must contain a right-arrow (`->`), e.g. `{ -> }`. This is only required
in contexts where both blocks and maps are valid.


### Special function shapes

There are a few different "shapes" of function &mdash; what kinds and
how many arguments they take, and what sort of things they return
&mdash; that have particular uses in the language.

#### Logic functions

A logic function is one which is meant to be used, at least some of the
time, as a logical predicate of some sort. In Samizdat, logical true
is represented by a return value of any value at all, and logical false
is represented by a void return. Logic functions, in general, can take
any number of arguments (including none).

See the introductory section "Logic operations" for more details.


### Symbols and methods

Classes in Samizdat bind symbols (method identifiers) to functions,
for later invocation. See the "Symbol" subsection under "Data" for
information about symbol syntax and semantics.

### Class definition

**Note:** This section reflects Samizdat syntax in transition.

Classes in Samizdat bind symbols (method identifiers) to functions,
for later invocation.

Classes are defined using a `class` statement, which has the general form:

```
class NameOfClass
        access: ACCESS,
        new: NEW {
    .instanceMethodName(arg) {
        ...
    };
    class.classMethodName(arg) {
        ...
    };
    ...
};
```

Within a `class` definition, instance methods are defined using a syntax
identical to the regular function statement syntax (as described above),
except with `.` instead of `fn`. Class methods are defined the same way as
instance methods, except prefixed with `class.` instead of just `.`. In both
cases, the variable `this` is bound in the body of the method to the instance
or class that the method was called on.

The `access...` and `new...` lines are used to define the (effectively
private) methods used to access instance state and construct an instance
(respectively). The values they bind to (in the example `ACCESS` and `NEW`)
must be symbols and are usually unlisted symbols, which can be created
along the lines of:

```
def ACCESS = @ACCESS.toUnlisted();
def NEW = @NEW.toUnlisted();
```

Neither of these two attributes is required.

When provided, the `new` symbol is bound as both a class and instance method
which constructs new instances, taking an optional data payload argument.

When provided, the `access` symbol is bound as an instance method which
takes either zero or one argument. If given no arguments, it returns the data
payload of the instance. If given one argument, it is taken to be a symbol
and returns the so-named binding from the data payload, or void if the name
isn't bound.
