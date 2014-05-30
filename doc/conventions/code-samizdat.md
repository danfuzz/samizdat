Conventions, Decisions, and Guidelines
======================================

Code: Samizdat
--------------

### File Organization

Samizdat mandates a module structure, the details of which can be found
elsewhere.

### Import order

`imports` are separated into several sections. Separate each section with
a single blank line, and sort the imports within each section.

* The major order is non-exported and then re-exported imports.

* Under each major section, list imports from least to most private.
  So, core library modules come first, then other shared library modules,
  then captive external modules, then internal modules.

* List resource imports after internal module imports.

* If importing a selection with just one to four imports, do it all on
  one line. If more, use multiple lines, indent the list, and list it
  as a separate section *after* the single-line imports.

* Brief end-of-line comments on import lines are fine.

* If any import requires one or more full-line comments, list it after all
  the other imports in its section, and separate it with a blank line.

* Err on the side of full-line comments before re-exported imports.
