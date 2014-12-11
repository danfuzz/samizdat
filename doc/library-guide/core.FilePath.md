Samizdat Layer 0: Core Library
==============================

core.FilePath
-------------

This module defines a set of utility functions that operate on filesystem
(or filesystem-like) path strings.

Path strings consist of a series of component names separated by slashes.
The component `.` generally indicates "the current directory," and the
component `..` generally indicates "the parent directory." A path that starts
with a slash (`/`) indicates an absolute path, and one that starts with
anything else indicates a relative path. Slashes at the end of a path
usually have no meaning; that is, a path with trailing slashes usually means
the same thing as the same path but without the trailing slashes.

The definitions in this module are pure functions. They do not perform any
I/O.


<br><br>
### Functions

#### `fixPath(path, basePaths*) -> isa String`

"Fixes" the given `path` if relative, making it *less* relative (and possibly
absolute) by using the given `basePaths` (if any) as prefixes. Cases:

* It is an error (terminating the runtime) if `path` is either empty or is
  not a string.
* If `path` is absolute, or if no `basePaths` are supplied, then this returns
  `path` directly.
* Otherwise, `fixPath(basePaths*)` is called. This function returns that
  `fixPath` result, concatenated with `"/"` and the original `path`.

#### `get_directory(path) -> isa String`

Returns the directory part of the given `path`. Cases:

* It is an error (terminating the runtime) if `path` is either empty or is
  not a string.
* If `path` is just one or more slashes (`/`), this returns `"/"`.
* Otherwise, if `path` ends with any number of slashes, then the result
  is the same as if those slashes were removed.
* If `path` is relative and does not contain a slash, then the result is
  `"."`.
* Otherwise, the result is the prefix of the given `path` up to but not
  including the last slash.

#### `get_file(path) -> isa String`

Returns the fila part (that is, the final component) of the given `path`.
Cases:

* It is an error (terminating the runtime) if `path` is either empty or is
  not a string.
* If `path` is just one or more slashes (`/`), this returns `"/"`.
* Otherwise, if `path` ends with any number of slashes, then the result
  is the same as if those slashes were removed.
* If `path` is relative and does not contain a slash, then the result is
  `path` itself.
* Otherwise, the result is the suffix of the given `path` after but not
  including the last slash.
