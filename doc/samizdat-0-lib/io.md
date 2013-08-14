Samizdat Layer 0: Core Library
==============================

I/O
---

<br><br>
### Primitive Definitions

#### `io0Die(string?) <> n/a # Terminates the runtime.`

Prints the given string to the system console (as if with `io0Note`)
if supplied, and terminates the runtime with a failure status code (`1`).

#### `io0FlatCwd() <> flatPath`

Returns the current working directory of the process, as a
string.

This function is a thin veneer over the standard Posix call `getcwd()`.

#### `io0FlatReadLink(flatPath) <> flatPath | void`

Checks the filesystem to see if the given path (given as a flat string)
refers to a symbolic link. If it does, then this returns the string which
represents the direct resolution of that link. It does not try to re-resolve
the result iteratively, so the result may not actually refer to a
real file (for example).

If the path does not refer to a symbolic link, then this function returns
void.

This function is a thin veneer over the standard Posix call `readlink()`.

#### `io0Note(string) <> void`

Writes out a newline-terminated note to the system console or equivalent.
This is intended for debugging, and as such this will generally end up
emitting to the standard-error stream.

#### `io0FlatReadFileUtf8(flatPath) <> string`

Reads the named file, using the underlying OS's functionality,
interpreting the contents as UTF-8 encoded text. Returns a string
of the read and decoded text.

#### `io0FlatWriteFileUtf8(flatPath, text) <> void`

Writes out the given text to the named file, using the underlying OS's
functionality, and encoding the text (a string) as a stream of UTF-8 bytes.


<br><br>
### In-Language Definitions

#### `io0FlatFromPath(path) <> flatPath`

Converts the given path list to an absolute "Posix-style" flat string.
This is the reverse of the operation specified in `io0PathFromFlat`.

It is an error (terminating the runtime) if any of the following
constraints are violated.

* `path` must not be an empty list.

* No path component may be `"."` or `".."`.

* No path component may contain a character of value `"/"` or `"\0"`.

* No path component other than the final one may be empty (that is,
  equal to `""`).

#### `io0PathFromFlat(flatPath) <> path`

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

#### `io0ReadFileUtf8(path) <> string`

Reads the named file, using the underlying OS's functionality,
interpreting the contents as UTF-8 encoded text. Returns a string
of the read and decoded text. `path` must be a componentized path-list,
such as might have been returned from `io0PathFromFlat`.

#### `io0ReadLink(path) <> path | void`

Checks the filesystem to see if the given path refers to a symbolic
link. If it does, then this returns the path which represents the
direct resolution of that link. It does not try to re-resolve
the result iteratively, so the result may not actually refer to a
real file (for example).

If the path does not refer to a symbolic link, then this function returns
void.

`pathList` must be a list of the form described by `io0PathFromFlat`
(see which). See `io0ReadFileUtf8` for further discussion.

#### `io0SandboxedReader(directory) <> function`

Returns a file reader function which is limited to *only* reading
files from underneath the named directory (a path-list as
described in `io0PathFromFlat`). The return value from this call
behaves like `ioFlatReadFileUtf8`, as if the given directory is both the
root of the filesystem and is the current working directory. Symbolic
links are respected, but only if the link target is under the named
directory.

This function is meant to help enable a "supervisor" to build a sandbox
from which untrusted code can read its own files.

#### `io0WriteFileUtf8(flatPath, text) <> void`

Writes out the given text to the named file, using the underlying OS's
functionality, and encoding the text (a string) as a stream of UTF-8 bytes.
`path` must be a componentized path-list, such as might have been returned
from `io0PathFromFlat`.
