Samizdat Layer 0: Core Library
==============================

core.Peg :: PegRepeat
---------------------

An instance of this class is a parser rule which matches another rule
multiple times, yielding a list of resulting matches. If `optMinSize*` is
specified, then this rule fails unless the size of the yielded list is at
least as given. If `optMaxSize*` is specified, then this rule will only ever
match at most that many repetitions.


<br><br>
### Class Method Definitions

#### `class.new(rule, optMinSize?, optMaxSize?) -> :PegRepeat`

Creates an instance of this class.

#### `class.newOpt(rule) -> :PegRepeat`

Creates an instance of this class with minimum size 0 and maximum size 1.
This is equivalent to the syntax `{: rule? :}`.

#### `class.newPlus(rule) -> :PegRepeat`

Creates an instance of this class with minimum size 1 and no maximum size.
This is equivalent to the syntax `{: rule+ :}`.

#### `class.newStar(rule) -> :PegRepeat`

Creates an instance of this class with minimum size 0 and no maximum size.
This is equivalent to the syntax `{: rule* :}`.

<br><br>
### Method Definitions: `Parser` protocol.

Works as documented per the specification for the protocol.
