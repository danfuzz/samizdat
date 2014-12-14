Design Highlights
=================

This is a brief overview of the aspects of Samizdat's design that make
it interesting and/or different.

* Syntax from the Algol / C family.

* Object model from the Smalltalk family.

* Mostly-immutable data model.
  * Notably, all collections are immutable.

* Void is not a value.
  * Methods are allowed to not-return a value, which is called "returning
    void" in the language. This is *not* the same as returning `null`, or
    `undefined`, and so on, in that a void return value is *not* assignable
    to a variable, nor can it be passed as an argument to a method.

* Syntactic support for defining PEG parsers.

* Built-in support for code-as-data, specified as "execution tree nodes."
