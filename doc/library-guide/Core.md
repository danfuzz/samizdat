Samizdat Layer 0: Core Library
==============================

Core
----

`Core` is the superclass of all concrete low-level core classes, *not*
including classes and metaclasses. It defines no methods. Its main utility
is in allowing code to understand (and/or limit) the values it is passed.

Notably, if a class inherits from `Core` it is guaranteed that there is
no other *different* class which also inherits from `Core` but has the
same name.


<br><br>
### Method Definitions: `Core` protocol

(none)
