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

* Void *is* "conditional false."
  * "Void vs. value" is the distinction used for conditional logic operations,
    not "value-called-`true` vs. value-called-`false`."
  * `true` and `false` *are* defined as values, for use in boolean logic and
    to hold booleans "at rest" in variables, and the like.
