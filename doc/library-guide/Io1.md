Samizdat Layer 0: Core Library
==============================

core::Io1
---------

This module defines simple I/O operations.


<br><br>
### Generic Function Definitions

(none)


<br><br>
### Function Definitions

#### `die(string?) <> n/a ## Terminates the runtime.`

Prints the given string to the system console (as if with `Io1::note`)
if supplied, and terminates the runtime with a failure status code (`1`).

#### `fileExists(path) <> logic`

Returns `path` if it corresponds to an already-existing file.
Returns void if not. `path` must be a componentized path-list,
such as might have been returned from `pathFromFlat`.

#### `flatFromPath(path) <> flatPath`

Converts the given path list to an absolute "Posix-style" flat string.
This is the reverse of the operation specified in `pathFromFlat`.

It is an error (terminating the runtime) if any of the following
constraints are violated.

* `path` must not be an empty list.

* No path component may be `"."` or `".."`.

* No path component may contain a character of value `"/"` or `"\0"`.

* No path component other than the final one may be empty (that is,
  equal to `""`).

#### `note(string) <> void`

Writes out a newline-terminated note to the system console or equivalent.
This is intended for debugging, and as such this will generally end up
emitting to the standard-error stream.

#### `pathFromFlat(flatPath) <> path`

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

#### `pathListFromFlat(flatPathList) <> [path*]`

Converts the given path list string to a list (per se) of absolute
internal-form paths. The given `flatPathList` is taken to be a colon-separated
list of flat paths. It is split apart on colons, and each split path is
processed as if by `pathFromFlat`.

#### `readFileUtf8(path) <> string`

Reads the named file, using the underlying OS's functionality,
interpreting the contents as UTF-8 encoded text. Returns a string
of the read and decoded text. `path` must be a componentized path-list,
such as might have been returned from `pathFromFlat`.

#### `readLink(path) <> path | void`

Checks the filesystem to see if the given path refers to a symbolic
link. If it does, then this returns the path which represents the
direct resolution of that link. It does not try to re-resolve
the result iteratively, so the result may not actually refer to a
real file (for example).

If the path does not refer to a symbolic link, then this function returns
void.

`pathList` must be a list of the form described by `pathFromFlat`
(see which). See `readFileUtf8` for further discussion.

#### `sandboxedReader(directory) <> function`

Returns a file reader function which is limited to *only* reading
files from underneath the named directory (a path-list as
described in `pathFromFlat`). The return value from this call
behaves like `flatReadFileUtf8`, as if the given directory is both the
root of the filesystem and is the current working directory. Symbolic
links are respected, but only if the link target is under the named
directory.

This function is meant to help enable a "supervisor" to build a sandbox
from which untrusted code can read its own files.

#### `writeFileUtf8(flatPath, text) <> void`

Writes out the given text to the named file, using the underlying OS's
functionality, and encoding the text (a string) as a stream of UTF-8 bytes.
`path` must be a componentized path-list, such as might have been returned
from `pathFromFlat`.
