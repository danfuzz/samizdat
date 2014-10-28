Samizdat Layer 0: Core Library
==============================

core.Peg :: PegSequence
-----------------------

This class represents -- TODO: FILL ME IN!! --


<br><br>
### Class Method Definitions

#### `.new(...) -> :PegSequence`

Creates an instance of this class.


<br><br>
### Method Definitions: `Parser` protocol.

Works as documented per the specification for the protocol.


#### `makeSequence(rules*) -> rule`

Makes and returns a parser rule which runs a sequence of given other rules
(in order). This rule is successful only when all the given rules
also succeed. When successful, this rule yields the value yielded by the
last of its given rules, and returns updated state that reflects the
parsing in sequence of all the rules.

Each rule is passed a list of in-scope parsing results, starting with the
first result from the "closest" enclosing main sequence.

This is equivalent to the syntactic form `{: ... (rule1 rule2 etc) ... :}`.

#### `makeString(string) -> rule`

Makes and returns a parser rule which matches a sequence of characters
exactly, consuming them from the input upon success. `string` must be a
string. The result of successful parsing is an empty-payload token with
`@string` as its name.

This is equivalent to the syntactic form `{: "string" :}`.
