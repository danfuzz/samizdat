Samizdat Layer 0: Core Library
==============================

core.Io0
--------

This module defines primitive filesystem I/O operations.


<br><br>
### Functions

#### `cwd() -> path`

Returns the current working directory of the process, as a
string.

This function is a thin veneer over the standard Posix call `getcwd()`.

#### `fileType(path) -> isa Symbol`

Returns the type of the file whose `path` is as given. This function always
returns a symbol, one of:

* `@absent` &mdash; Indicates a nonexistent path (including a nonexistent
  non-final path component).
* `@file` &mdash; Indicates a regular file.
* `@directory` &mdash; Indicates a directory.
* `@other` &mdash; Any other existing file (e.g., a named pipe).

#### `readDirectory(path) -> isa Map | void`

Reads the contents of the indicated directory, using the underlying OS's
functionality. Returns a map from names (strings) to types (also strings).
Types are as with `fileType`, with the addition of:

* `@symlink` &mdash; Indicates a symbolic link.

If `path` is not an existing directory (e.g. if it doesn't exist, period, or
it exists but is not a directory), then this function returns void.

**Note:** The result map will not contain mappings for `"."` (directory
self-reference) or `".."` (parent directory reference).

#### `readFileUtf8(path) -> isa String`

Reads the named file, using the underlying OS's functionality,
interpreting the contents as UTF-8 encoded text. Returns a string
of the read and decoded text.

#### `readLink(path) -> isa String | void`

Checks the filesystem to see if the given filesystem path refers to a symbolic
link. If it does, then this returns the string which represents the direct
resolution of that link. It does not try to re-resolve the result iteratively,
so the result might not actually refer to a real file (for example).

If the path does not refer to a symbolic link, then this function returns
void.

This function is a thin veneer over the standard Posix call `readlink()`.

#### `writeFileUtf8(path, text) -> void`

Writes out the given text to the named file, using the underlying OS's
functionality, and encoding the text (a string) as a stream of UTF-8 bytes.
