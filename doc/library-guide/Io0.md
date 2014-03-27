Samizdat Layer 0: Core Library
==============================

core.Io0
--------

This module defines primitive I/O operations. It is only meant to be used
by the lowest layer of core library.


<br><br>
### Generic Function Definitions

(none)


<br><br>
### Function Definitions

#### `absolutePath(path, optBase?) <> path`

Returns an absolute filesystem path, based on the given `path` and optional
base directory. If `path` is already absolute, then this returns it as-is.
Otherwise, this prefixes it with either `optBase*` (if given) or the current
directory (if not), followed by a slash.

This function does not attempt to resolve symbolic links, nor does it
"flatten" away any `.` or `..` path components.

If `optBase*` is used and is itself relative, then it is made absolute, as
if by a recursive call to this function.

It is an error (terminating the runtime) if `path` is empty or is not a
string.

#### `cwd() <> flatPath`

Returns the current working directory of the process, as a
string.

This function is a thin veneer over the standard Posix call `getcwd()`.

#### `directoryOf(path) <> path`

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

#### `fileExists(flatPath) <> logic`

Returns `flatPath` if it corresponds to an already-existing regular file.
Returns void if not a regular file.

This returns void if the file doesn't exist at all, or if it exists but
is either a directory or a "special" file.

#### `fileOf(path) <> path`

Returns the final component part of the given `path`. Cases:

* It is an error (terminating the runtime) if `path` is either empty or is
  not a string.
* If `path` is just one or more slashes (`/`), this returns `"/"`.
* Otherwise, if `path` ends with any number of slashes, then the result
  is the same as if those slashes were removed.
* If `path` is relative and does not contain a slash, then the result is
  `path` itself.
* Otherwise, the result is the suffix of the given `path` after but not
  including the last slash.

#### `readFileUtf8(flatPath) <> string`

Reads the named file, using the underlying OS's functionality,
interpreting the contents as UTF-8 encoded text. Returns a string
of the read and decoded text.

#### `readLink(flatPath) <> flatPath | void`

Checks the filesystem to see if the given path (given as a flat string)
refers to a symbolic link. If it does, then this returns the string which
represents the direct resolution of that link. It does not try to re-resolve
the result iteratively, so the result may not actually refer to a
real file (for example).

If the path does not refer to a symbolic link, then this function returns
void.

This function is a thin veneer over the standard Posix call `readlink()`.

#### `writeFileUtf8(flatPath, text) <> void`

Writes out the given text to the named file, using the underlying OS's
functionality, and encoding the text (a string) as a stream of UTF-8 bytes.
