Version History
===============

Refer to the [old changelog](doc/old-changelog.md) for older versions.

* 0.5.0 &mdash; 27-jan-2014 &mdash; "Scratching the Surface"

  * **Milestone:** First round of syntax and semantics changes prompted by
    "real world" experience (in particular, of starting to build `samtoc`).

  * Revised syntax for ranges, collection access, and comments.

  * Revised and simplified execuction tree semantics.

  * New syntax for collection size, parser thunks, and variable definition
    variants.

  * New semantics for (local) variable binding.

* 0.6.0 &mdash; 14-mar-2014 &mdash; "Birthday Pi"

  * **Milestone:** The Samizdat project is now one year old.

  * First cut of a real compiler mode in `samtoc`, which emits C code that
    *doesn't* end up relying on a tree interpreter.

  * Notable syntax+semantics changes:
    * Added mutable variables (`var` keyword).
    * Removed void contagion (prefix operator `&`) and associated `voidable`
      execution tree type.
    * Reworked type system, so that type values are treated more consistently
      and uniformly. New form `@@name` to refer to derived data types.

  * Other notable semantics changes:
    * Got rid of the `reduce*` family of library functions, because mutable
      variables are more "natural feeling" for the salient use cases.
    * Defined new execution tree types `apply` and `jump`. These are now used
      for calls with interpolated arguments and nonlocal exits (respectively).
    * Removed execution tree types `interpolate` and `expression`.

  * Other notable syntax changes:
    * Simplified identifiers.
    * Switched to `{: ... :}` for delimiting parser blocks.

  * Now built using a "real" build system (Blur).

* 0.7.0 &mdash; 30-may-2014 &mdash; "Import-Export Business"

  * **Milestone:** Module system rewritten and fully fleshed out. This
    includes a fairly straightforward semantic model and a set of `import`
    and `export` statement forms used to get things hooked up.

  * Reworked the `fn` syntax, which is now only used as a statement and can
    now be used to define and bind generic functions. Expanded the `{ ... }`
    block syntax to handle the otherwise-orphaned use cases.

  * Progress on the type system. Derived data values are now defined in
    a saner way, and their respective types have better usability in the
    language, with the new `@@name` / `@@(name)` syntax.

  * Introduced the concept of and syntax for "directives" in the tokenizer.
    This is now used instead of filename suffixes to control what language
    layer is used to parse files.

  * Made the comparison operators return their left-hand side, which is
    less surprising than the former right-hand side behavior. (The latter
    made it easier to implement chained comparisons, but that turned out
    not to be a great thing to optimize for.)

  * File I/O functions reworked to take string paths, just like every other
    language in the world.

  * Lots of miscellaneous tweaks and additions to the core library.

* 0.8.0 &mdash; 13-jun-2014 &mdash; "Many Happy Returns"

  * **Milestone:** Major rework of closure yield / return syntax and
    semantics.

    * No more `<>` operator; it's replaced by `yield`. But in
      many cases direct yield is now unmarked (in particular, when returning
      from "semantically lightweight" closures).

    * No more `<name>` syntax, replaced by `/name`.

    * Addition of "maybe yield" syntax with postfix `?` on the yield
      keywords, and made the unmarked variants *not* accept expressions
      that evaluate to void.

  * Added a few new execution tree node types (`maybe`, `noYield`,
    `nonlocalExit`, and `void`) and removed one (`jump`). Tweaked the
    definitions of a couple more (`apply`, `call`, and `closure`) to
    use `void` nodes for bindings instead of having optional bindings.

* 0.9.0 &mdash; 17-sep-2014 &mdash; "Class Struggle"

  * **Milestone:** Replacement of method binding and dispatch with a new
    Smalltalk-style class system.

    * The type `Type` is now the class `Class`.

    * Removed generic functions from the system.

    * Removed the ability to define methods on derived data classes.

    * Introduced new classes `Bool` and `Null`, as replacements for the
      former derived data classes. This was needed, so that they could have
      the right set of methods bound instead of just having what derived
      data classes had, which would have been too little for `Bool` and too
      much for `Null`.

    * Added new class definition function to define a class along with its
      methods atomically. **Note:** There isn't yet syntax to cover this
      functionality, and incremental method binding is still possible in the
      mean time. This will change in a future release.

  * New "symbol" functionality:

    * Introduced a `Symbol` class, akin to the same-named Lisp type and
      similar to a selector in Smalltalk. The syntax `@name` is now used
      for literal symbol references instead of payload-free derived data
      values. Symbols are now used for almost every "name-ish" thing in
      the language, e.g. variable names, method names, exports from
      modules, and so on.

    * Introduced a `SymbolTable` class, a map-like container where keys
      are restricted to being symbols. Used for specifying method bindings
      and as the payloads for derived data values and objects. The syntax
      `@{...}` (like a map with a `@` prefix) is used to construct symbol
      tables.

  * Reworked single-value interpolation and imperative assignment:

    * Single-value interpolation is now based on calling `.fetch()`,
      which `Box` already defined. Generators now define `.fetch()` too,
      specified to be an error if a generator could generate more than one
      value.

    * Added imperative assignment variant `x* := y`, to be based on
      calling `.store()`, making it work naturally when `x` is a box.

    * New expression syntax `var name` to refer to the box holding a
      variable. This is sort of like `&name` in C. `(var x)* := y` now
      means the same thing as `x := y`, with the handy thing being that
      you can, say, pass `var x` in as an argument to a function and
      that function can then perform assignment on it in a reasonably
      sane way.

* 0.10.0 &mdash; 17-oct-2014 &mdash; "I Never Metaclass I Didn't Like"

  * **Milestone:** The object model has grown to include metaclasses and
    class methods.

    * Class methods are now used for construction of values and creation of
      subclasses, replacing a bunch of globally-exported functions.

    * Defined a generalized "casting" (value conversion) mechanism, based
      on a class method `.castFrom(value)` and an instance method
      `.castToward(ClassName)`.

    * Introduced new and much cleaner syntax for defining classes. Underlying
      semantics is much better than before as well. **Note:** The syntax is
      still in flux and is missing a lot of functionality.

  * Reworked record values (formerly known as "derived data values").
    There is now only one class `Record`, instead of a class per record
    name.

  * Split `Box` into a family of classes, for the various standard box
    behaviors.

* 0.11.0 &mdash; 15-dec-2014 &mdash; "That's Classified Information"

  * **Milestone:** Major progress on "class-centric" rework. The system is
    moving away from functions / closures as the most primitive bearers of
    behavior and variable state, in favor of giving to classes this
    responsibility.

    * `call` and `apply` execution nodes now represent method calls, not
      function calls. Function calls are no more and no less than a call to
      the method named `.call()`.

    * Reworked all library constructor functions to instead be defined as
      class methods, usually called `.new()`.

    * Moved most global functions to instead be class methods on new
      utility (uninstantiable) classes.

  * Other major syntactic changes:

    * Replaced rvalue syntax `var x` by slightly generalizing the postfix
      `?` syntax.

    * Introduced infix operators `as` (cast to class/type) and `isa` (check
      for class/type match).

  * Other major semantic changes:

    * Reworked and generally simplified the Parser protocol, making it
      easier to both understand and implement.

  * Other notable changes:

    * Implemented "shared content" lists, to enable more efficient list
      deconstruction.

    * Nearly completely rewrote the core interpreter, to avoid all runtime
      record binding lookup, and to do more up-front validation of closures.

    * Did major restructuring on the C code, to make it a bit cleaner and
      more maintainable going forward.

* 0.11.1 &mdash; 7-jan-2015

  * Fixed bugs that prevented successful building and running on Linux.
