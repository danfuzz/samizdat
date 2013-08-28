Samizdat Language Guide
=======================

Functions and Blocks
--------------------

*Samizdat* is a closure-forward language, in that almost any nontrivial
piece of code will consist of multiple nested closures. Closures in the
language are the typical sort, namely a function which captures elements
from its execution context and which continues to be valid after its
enclosing execution context has exited.

There are two kinds of closure in the language, "functions" and blocks",
which correspond to two different common use cases. Both forms have in
common that the body of the closure (containing its statements) are
enclosed within matching curly braces (`{...}`). The forms differ in
how their declarations look, and there are a couple of semantic differences
as well, as detailed below.

With the exception of the top level of a program, every closure has
curly brace delimiters  (`{...}`), and with few exceptions curly braces
are only used to indicate a closure of some form or another.

Every closure can be declared to take arguments, and every closure can
yield (return) a value. With only one exception, the curly braces are *only*
used in the definition of closures. (This is a departure from most
languages in the C family, however it is similar to Smalltalk.)


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

If a repetition specifier is present on an argument, then in the context
of the closure, the variable name associated with the argument will
always be a list, consisting of the actual values bound to the argument.
For example, an optional argument will always be a list of zero or one
element.

When bound, repetition specifiers consistently cause "maximum greed" and
no backtracking. As such, it does not make sense to list an unmarked argument
after one that is marked (since the unmarked argument could never be
successfully bound). It also only makes sense to only ever use one of
`+` or `*`, and only on the final argument.


### Yield Definitions

Every closure can define a name to use within its body to yield (return)
from the closure. This allows code to cause an exit out of a closure other
than the one that directly contains the yield in question. As with
argument declarations, functions and blocks differ in where the yield
definition is placed syntactically.

A yield definition consists of a variable name enclosed in angle
brackets (`<...>`).

### Body

The main body of a closure is a sequence of statements. The last
statement of a closure may optionally be one of several possible
yield (return) statements.

Statements are separated from each other with semicolons `;`. In addition
(TODO), there is implicit statement separation based on the indentation
level of adjacent lines.

Aside from yields, a statement can be either an arbitrary expression
(as documented elsewhere in this guide) or one of several special
definition statements. The `fn` statement is described in this section.
Other statements are described elsewhere.

There are several kinds of yield statement. All of them consist of
a yield indicator of some form, optionally followed by an arbitrary
expression. If an expression is not supplied, this indicates that the
yield value is void.

Two kinds of yield are applicable in all contexts:

* `<>` &mdash; Direct yield from closure. This yields a
  value from the closure that this statement appears directly in.
  Note that yield-of-void is the default if no yield statement is
  present at all, so the only reason to use `<>` without an
  expression is in order to be stylisticly unambiguous.

* `<name>` &mdash; Yield from named closure. The `name` must match
  the yield definition name of an enclosing closure. (See "Yield Definitions",
  above.) In addition, the so-named enclosing closure must still be
  in the middle of executing; that is, the enclosing closure must not
  have yet yielded (including yielding void). It is a fatal error to try to
  yield from a closure that has already yielded.

  If there is more than one closure in the lexical context that has the same
  name, this form binds to the closest enclosing one.

One kind of yield is applicable in the context of a function definition
(as defined in this section):

* `return` &mdash; Yield from closest enclosing function definition. As
  with named yield (above), it is a fatal error to try to yield from a
  function invocation which has already yielded.

The remaining forms of yield are associated with looping constructs
and are described along with those constructs.


### Functions

Functions are closures that are meant to "stand alone" in some way. These
correspond to functions as used by most languages in the C tradition.
Most functions have a name and are bound to a variable (sometimes a local
variable, and sometimes a module variable) whose name matches the
function name.

Most typically functions are defined and bound to a variable within
the same construct; these are known as "function statements". It is
also possible to define a function as a component of an enclosing
expression; these are known as "function expressions". There is no
syntactic difference between these two forms, except that the statement
form requires the function to have a name. There is no semantic
difference between these two forms, except that the statement form
binds the function to a variable outside the context of the execution
of the function itself.

Functions definitions are introduced with the `fn` keyword. The keyword
is followed by declarations, and then followed by the main code body,
inside curly braces.

The declarations to a function consist of the following, in order:

* The yield definition (optional). See "Yield Definition" above.

* The name (optional, except if the function is being bound directly to a
  variable). This provides a name for the function, which serves three
  purposes:

  * It provides a name that can be used when calling the function
    self-recursively.
  * It provides a name that is visible in some debugging contexts
    (such as when generating stack traces).
  * In a function *statement*, it provides the name of the variable to
    bind the function to.

* The formal arguments. These consist of a comma-separated list of
  individual argument declarations, all enclosed within parentheses
  (`(...)`). If there are no arguments to declare, the parentheses are
  still required. See "Argument Declarations" above.

Aside from the name binding, the one semantic difference between functions
and blocks is that functions define the closure that a `return` statement
will yield from. See "Yield Defintion" above.

Examples:

```
# This is a function statement, which defines the function named `blort`,
# binding it to a variable with the same name. The function can take one
# or two arguments. `...` would be replaced with a full function body.
fn blort(a, b?) { ... }

# This is a function expression, of an anonymous (unnamed) function.
# Note that this is a contrived example. The function takes one or more
# arguments.
def fizmo = (fn (a+) { ... })

# This is a function statement that includes a yield definition. Within
# the body, `<out>` indicates a yield from `igram`. The function takes any
# number of arguments (including zero).
fn <out> igram(a*) { ... <out> ... }
```


### Blocks

Blocks are closures that generally serve as arguments to function calls
or as elements of complex expressions. These serve the same purpose as
compound statements in most languages in the C tradition.

Syntactically, blocks start with an open curly brace (`{`) and end with a
close curly brace (`}`). Within the braces, blocks start with an optional
declaration section and are followed by the main code body.

The declaration section consists of the following, in order:

* The yield definition (optional). See "Yield Definition" above.

* The formal arguments. These consist of a comma-separated list of
  individual argument declarations (without any enclosing parentheses).
  See "Argument Declarations" above.

* The special token double-colon (`::`), to indicate the end of
  the declarations. If the entire body of the block is just a direct
  yield (`<> ...`), then the double-colon may be omitted.

Examples:

```
# This is equivalent to the second example in the "Functions" section, above.
def fizmo = { a+ :: ... }

# This is a block that takes no arguments.
def borch = { ... }

# This is a block that takes no arguments but does define a yield point.
def frotz = { <leave> :: ... <leave> ... }

# This is one with everything.
def ignatz = { <out> x, y?, z* :: ... <out> ... }

# Since the main body is just a yield, no double-colon is required.
def krazy = { x, y <> x + y }
```

### Special function shapes

There are a few different "shapes" of function &mdash; what kinds and
how many arguments they take, and what sort of things they return
&mdash; that have particular uses in the language.

#### Logic functions

A logic function is one which is meant to be used, at least some of the
time, as a logical predicate of some sort. In *Samizdat*, logical true
is represented by a return value of any value at all, and logical false
is represented by a void return. Logic functions, in general, may take
any number of arguments (including none).

See the introductory section "Logic operations" for more details.

#### Parser functions

Parser functions are used (unsurprisingly) to perform parsing operations.
*Samizdat* provides both syntactic and library support for parsing.

Briefly, parser functions work very similar to generators. The
majore differences are:

* Parser functions are `Function` values (and not values that bind other
  methods).

* Parser functions take two or more arguments, in this order:
  * a yield box.
  * The "text" to parse.
  * Optionally, a trailing context of one or more already-parsed values.

See the "Parsing" section for more details.
