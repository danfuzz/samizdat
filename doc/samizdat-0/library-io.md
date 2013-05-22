Samizdat Layer 0: Core Library
==============================

I/O
---

<br><br>
### Primitive Definitions

#### `io0Die string? <> !. (exits)`

Prints the given string to the system console (as if with `io0Note`)
if supplied, and terminates the runtime with a failure status code (`1`).

#### `io0FlatCwd <> flatString`

Returns the current working directory of the process, as a
string.

This function is a thin veneer over the standard Posix call `getcwd()`.

#### `io0FlatReadLink path <> flatString | !.`

Checks the filesystem to see if the given path refers to a symbolic
link. If it does, then this returns the string which represents the
direct resolution of that link. It does not try to re-resolve
the result iteratively, so the result may not actually refer to a
real file (for example). It also does not convert the result into a
componentized path; it's just a string.

If the path does not refer to a symbolic link, then this function returns
void.

`pathList` must be a list of the form described by `io0PathFromFlat`
(see which). See `io0ReadFileUtf8` for further discussion.

This function is a thin veneer over the standard Posix call `readlink()`.

#### `io0Note string <> !.`

Writes out a newline-terminated note to the system console or equivalent.
This is intended for debugging, and as such this will generally end up
emitting to the standard-error stream.

#### `io0ReadFileUtf8 path <> string`

Reads the named file, using the underlying OS's functionality,
interpreting the contents as UTF-8 encoded text. Returns a string
of the read and decoded text.

`pathList` must be a list of the form described by `io0PathFromFlat`
(see which). It is invalid (terminating the runtime) for a component to
be any of `""` `"."` `".."` or to contain a slash (`/`).

#### `io0WriteFileUtf8 path text <> !.`

Writes out the given text to the named file, using the underlying OS's
functionality, and encoding the text (a string) as a stream of UTF-8 bytes.

`pathList` must be a list of the form described by `io0PathFromFlat`
(see which). See `io0ReadFileUtf8` for further discussion.


<br><br>
### In-Language Definitions

#### `io0FlatFromPath path <> flatString`

Converts the given path list to an absolute "Posix-style" flat string.
This is the reverse of the operation specified in `io0PathFromFlat`.

It is an error (terminating the runtime) if any of the following
constraints are violated.

* No path component may be `"."` or `".."`.

* No path component may contain a character of value `"/"` or `"\0"`.

* No path component other than the final one may be empty (that is,
  equal to `""`).

#### `io0PathFromFlat flatString <> path`

Converts the given path string to an absolute form, in the "form factor"
that is used internally. The input `string` is expected to be a
"Posix-style" path:

* Path components are separated by slashes (`"/"`).

* A path-initial slash indicates an absolute path.

* If there is no path-initial slash, then the result has the system's
  "current working directory" prepended.

* Empty path components (that is, if there are two slashes in a row)
  are ignored.

* Path components of value `"."` (canonically representing "this directory")
  are ignored.

* Path components of value `".."` cause the previous non-`..` path component
  to be discarded. It is invalid (terminating the runtime) for such a
  component to "back up" beyond the filesystem root.

The result is a list of path components, representing the absolute path.
None of the components will be the empty string (`""`), except possibly
the last. If the last component is empty, that is an indication that the
original path ended with a trailing slash.

It is an error (terminating the runtime) if `string` is empty (`""`).

#### `io0ReadLink path <> path | !.`

Checks the filesystem to see if the given path refers to a symbolic
link. If it does, then this returns the path which represents the
direct resolution of that link. It does not try to re-resolve
the result iteratively, so the result may not actually refer to a
real file (for example).

If the path does not refer to a symbolic link, then this function returns
void.

`pathList` must be a list of the form described by `io0PathFromFlat`
(see which). See `io0ReadFileUtf8` for further discussion.

#### `io0SandboxedReader directory <> function`

Returns a file reader function which is limited to *only* reading
files from underneath the named directory (a path-list as
described in `io0PathFromFlat`). The return value from this call
behaves like `ioReadFileUtf8`, as if the given directory is both the
root of the filesystem and is the current working directory. Symbolic
links are respected, but only if the link target is under the named
directory.

This function is meant to help enable a "supervisor" to build a sandbox
from which untrusted code can read its own files.
