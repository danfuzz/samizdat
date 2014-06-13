Version History
===============

* 0.0.1 &mdash; 14-mar-2013 &mdash; "Let Them Eat Pi"

  * **Milestone**: First commit to new repo, and the beginning of the
    third-or-so attempt to get the idea for the system into a concrete form.

* 0.1.0 &mdash; 29-jul-2013 &mdash; "Stake In The Ground"

  * **Milestone**: End of three month sprint to get to some kind of
    proof-of-concept.

  * Low level language concepts mostly settled and stable. High level
    language concepts still very much in flux.

* 0.2.0 &mdash; 3-sep-2013 &mdash; "Method To The Madness"

  * **Milestone**: Method dispatch.

  * Implemented basic type system and method dispatch mechanism based
    on generic functions.

* 0.3.0 &mdash; 6-nov-2013 &mdash; "Now I'm Feeling Modularized"

  * **Milestone:** System is sufficiently stable to start work on a
    self-hosted compiler.

  * Major changes to punctuation tokens.

  * Implemented module system mechanism, though with barely any syntactic
    support.

  * Arranged all core functionality into modules.

* 0.4.0 &mdash; 18-dec-2013 &mdash; "A Tree Grows In SoMa"

  * **Milestone:** Version of runtime where library code is compiled into
    binary form.

  * Runtime now supports loading of "addon" libraries.

  * New `samtoc` compiler produces C code that builds parse trees, for
    interpretation using the original runtime interpreter. As such, it's
    not a "real" compiler, but it does vet the process of building and
    loading binary "addon" libraries.

  * Major revision to the mechanics of module loading.

  * Various rework and changes to syntax:
    * Added dot-based syntax for getter and setter methods.
    * Totally new derived value syntax.
    * Implemented postfix `?` in all layers.

* 0.4.1 &mdash; 19-dec-2013

  * Docs improvements, and minor cleanup.

* 0.5.0 &mdash; 27-jan-2014 &mdash; "Scratching the Surface"

  * **Milestone:** First round of syntax and semantics changes prompted by
    "real world" experience (in particular, of starting to build `samtoc`).

  * Revised syntax for ranges, collection access, and comments.

  * Revised and simplified execuction tree semantics.

  * New syntax for collection size, parser thunks, and variable definition
    variants.

  * New semantics for (local) variable binding.

* 0.6.0 &mdash; 14-mar-2014  &mdash; "Birthday Pi"

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

* 0.7.0 &mdash; 30-may-2014  &mdash; "Import-Export Business"

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

* 0.8.0 &mdash; 13-jun-2014  &mdash; "Many Happy Returns"

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
