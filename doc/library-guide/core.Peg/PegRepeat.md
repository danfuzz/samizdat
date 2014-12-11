Samizdat Layer 0: Core Library
==============================

core.Peg :: PegRepeat
---------------------

An instance of this class is a parser rule which matches another rule
multiple times, yielding a list of resulting matches. Instances have a
minimum number of repetitions and an *optional* maximum number of repetitions.
If the maximum number of repetitions is not specified, then there is no
limit.


<br><br>
### Class Method Definitions

#### `class.new(rule, minSize, optMaxSize?) -> isa PegRepeat`

Creates an instance of this class. The constructed instance requires at
least `minSize` repetitions of the target `rule` in order for this instance
to succeed at parsing. If `optMaxSize*` is specified, then this instance
immediately succeeds as soon as it parses that many repetitions.

#### `class.newOpt(rule) -> isa PegRepeat`

Creates an instance of this class with minimum size 0 and maximum size 1.
This is equivalent to the syntax `{: rule? :}`.

#### `class.newPlus(rule) -> isa PegRepeat`

Creates an instance of this class with minimum size 1 and no maximum size.
This is equivalent to the syntax `{: rule+ :}`.

#### `class.newStar(rule) -> isa PegRepeat`

Creates an instance of this class with minimum size 0 and no maximum size.
This is equivalent to the syntax `{: rule* :}`.

<br><br>
### Method Definitions: `Parser` protocol.

Works as documented per the specification for the protocol.
