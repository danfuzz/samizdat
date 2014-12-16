Startup Walk-Through
====================

This is a brief sketch of what happens when the runtime gets started. This
applies to both `samex-naif` and `samex-tot`.

1.  The binary executable is kicked off by the underlying operating system.
    It will have been passed arbitrary command-line arguments.

2.  The C code in `main/main.c` figures out where the binary is located
    in the filesystem, and uses that to derive a filesystem path to the
    core library. If the binary is `/x/y/samex-foo/samex`, then the core
    library is expected to be in a directory named `/x/y/samex-foo/corelib`.

3.  The C code in `lib/init.c` puts together the primitive global environment
    (that is, a map of names to primitively-defined functions and constants),
    and it "manually" loads the module system using that environment. This is
    the "bootstrap module system." The code for the module system comes from
    within the core library, in particular from
    `corelib/modules/core.ModuleSystem/main`.

4.  In `lib/init.c`, the bootstrap module system is asked to load the core
    library as a module, using the module system's `main` function, whose
    sole purpose is doing this particular bit of setup.

5.  The module system's `main` function loads and evaluates the library's
    `main`. This immediately causes a couple modules to be loaded within the
    bootstrap module loader &mdash; `core.Globals` and `core.ModuleSystem`
    &mdash; and it returns the core library's top-level binding map. This is
    a map with a single binding of `"main"` to a function (which is the usual
    case for "application modules").

6.  The module system's `main` function then looks up the core library's
    `main` binding, and calls it as a function, passing it the primitive
    global environment and the filesystem path to the core library. These
    two arguments are what it was passed from `lib/init.c`.

7.  The core library's `main` &mdash; this is the code in
    `samlib-naif/main.sam` &mdash; uses the module `core.Globals`, as loaded
    by the bootstrap module loader, to create a new global environment which
    combines a bunch of in-language library defintions with the bindings of
    the original primitive environment.

8.  Using this more complete global environment, the core library's `main`
    then creates a new module system instance, and asks it to load the module
    `core.Globals`. It asks this module for the "full" global environment.
    This full environment is returned to `core.ModuleSystem::main`, which in
    turn returns it to `lib/init.c`, which in turn returns it to `main/main.c`.

9.  `main/main.c` looks up `runCommandLine` in the global environment, and
    calls it, passing it the original (arbitrary) command-line arguments.

10. `runCommandLine` &mdash; defined in the module `core.CommandLine` &mdash;
    parses the command-line arguments. It is (in general) being asked to
    either run a standalone file or an "application module."

    * If it is asked to run a standalone file, then it directly loads and
      runs the file.
    * If it is asked to run an application module, then it creates a new
      module system instance which points at the application's directory,
      and uses that in a manner analogous to how the core library was loaded.

During this process, any time a file is being loaded, the system will
always first check to see if there is a compiled binary for the file,
named `baseName.samb`. If that exists, then it's loaded directly. If not,
the system checks for source named `baseName.sam`, which then gets parsed
and evaluated via one of the `core.Lang*` modules.
