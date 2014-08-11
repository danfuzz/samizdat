Conventions, Decisions, and Guidelines
======================================

Code: Samizdat
--------------

### File Organization

Samizdat mandates a module structure, the details of which can be found
elsewhere.

### Intra-file Arrangement

Every file starts with a standard license header &mdash; which is always
mandatory &mdash; and is followed by a file header, then imports, and then
a number of code sections. Each section starts with a section header of the
form:

```
##
## Section Title
##
```

or:

```
##
## Section Title
##
## Additional information about the section
## goes here.
```

This is also the form of file headers.

Sections are separated from each other with *two* blank lines.

As an exception to all of the above, files of under 60 lines do not
require section headers, and if the purpose of a short file is *particularly*
obvious, the file header is also optional.

**Rationale:** If a file is small enough to fit on a page, then there's
not much point in providing the heavy-weight visual anchors.

Standard sections include "Private Definitions," "Exported Definitions,"
and "Class Definitions." Other sections can be used as make sense.

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
