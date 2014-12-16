Design Highlights
=================

This is a brief overview of the aspects of Samizdat's design that make
it interesting and/or different.

* Syntax from the Algol / C family.

* Object model from the Smalltalk family.

* Mostly-immutable data model.
  * Notably, all collections are immutable.

* Built-in support for code-as-data, specified as "execution tree nodes."

* Syntactic support for defining PEG parsers.

* Void is not a value.
  * Methods are allowed to not-return a value, which is called "returning
    void" in the language. This is *not* the same as returning `null`, or
    `undefined`, and so on, in that a void return value is *not* assignable
    to a variable, nor can it be passed as an argument to a method.
  * `null` *is* defined as a value, with two intended use cases:
    * A value is required, but it doesn't need to be anything in particular.
    * Compatibility with other systems that want to use a value called `null`.
  * Where there is a need to use expressions or method calls that might yield
    void, the language has two postfix operators:
    * `expr?` takes an expression that might evaluate to void and in turn
    yields either an empty list (if the the expression yielded void) or a
    single-element list (of a non-void expression yield).
    * `expr*` takes an expression that evaluates to either an empty or
    single-element list and in turn yields either void (if the list is empty)
    or the value in the list (if it is non-empty).

* Void *is* "conditional false."
  * "Void vs. value" is the distinction used for conditional logic operations,
    not "value-called-`true` vs. value-called-`false`."
  * `true` and `false` *are* defined as values, for use in boolean logic and
    to hold booleans "at rest" in variables, and the like. Perhaps
    surprisingly, boolean `false` counts as conditional true.
  * Where there is a need to covert between boolean values and conditional
    truth, the language has two postfix operators:
    * `expr??` takes an expression that might evaluate to void and in turn
    yields either `false` (if the expression yielded void) or `true` (if the
    expression yielded a value).
    * `expr**` takes an expression that evaluates to a boolean and in turn
    yields either void (for `false`) or `true` (for `true`).
