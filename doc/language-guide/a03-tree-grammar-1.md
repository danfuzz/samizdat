Samizdat Language Guide
=======================

Appendix: Layer 1 Tree Grammar
------------------------------

The following is the complete tree grammar for Samizdat Layer 1,
written in Samizdat Layer 1, with commentary calling out the parts
that are needed specifically for Layer 1 as well as how Layer 2 hooks in.
Anything left unmarked is also needed for Layer 0.

A program is parsed by matching the `program` rule, which yields a
`closure` node. For simple error handling, the rule `programOrError`
can be used.

```
def $Format    = moduleLoad(["core", "Format"]);
def $Generator = moduleLoad(["core", "Generator"]);
def $Lang0     = moduleLoad(["core", "Lang0"]);
def $Number    = moduleLoad(["proto", "Number"]);
def $Peg       = moduleLoad(["core", "Peg"]);
def $Range     = moduleLoad(["core", "Range"]);
def $Sequence  = moduleLoad(["core", "Sequence"]);

def $Lang0Node = moduleLoad(["core", "Lang0Node"]);
def REFS               = $Lang0Node::REFS;
def formalsMaxArgs     = $Lang0Node::formalsMaxArgs;
def formalsMinArgs     = $Lang0Node::formalsMinArgs;
def get_formals        = $Lang0Node::get_formals;
def get_interpolate    = $Lang0Node::get_interpolate;
def get_name           = $Lang0Node::get_name;
def get_nodeValue      = $Lang0Node::get_nodeValue;
def get_statements     = $Lang0Node::get_statements;
def get_yieldDef       = $Lang0Node::get_yieldDef;
def makeApply          = $Lang0Node::makeApply;
def makeCall           = $Lang0Node::makeCall;
def makeCallOrApply    = $Lang0Node::makeCallOrApply;
def makeCallThunks     = $Lang0Node::makeCallThunks;
def makeGet            = $Lang0Node::makeGet;
def makeInterpolate    = $Lang0Node::makeInterpolate;
def makeJump           = $Lang0Node::makeJump;
def makeLiteral        = $Lang0Node::makeLiteral;
def makeOptValue       = $Lang0Node::makeOptValue;
def makeThunk          = $Lang0Node::makeThunk;
def makeVarBind        = $Lang0Node::makeVarBind;
def makeVarDef         = $Lang0Node::makeVarDef;
def makeVarDefMutable  = $Lang0Node::makeVarDefMutable;
def makeVarRef         = $Lang0Node::makeVarRef;
def makeVarRefLvalue   = $Lang0Node::makeVarRefLvalue;
def withFormals        = $Lang0Node::withFormals;
def withoutInterpolate = $Lang0Node::withoutInterpolate;


##
## Private Definitions
##

## Reports the given list of pending tokens as part of error processing.
fn reportError(pending) {
    note("Pending tokens:");

    $Generator::filterPump(pending, $Range::makeInclusiveRange(1, 50))
        { token, . -> note(cat("    ", $Format::source(token))) };

    die("\nExtra tokens at end of program.")
};

## Set-like map of all lowercase identifier characters. Used to figure
## out if we're looking at a keyword in the `identifierString` rule.
def LOWER_ALPHA = {
    ($Range::makeInclusiveRange("a", "z"))*: true
};


##
## Layer 0 Rules
##
## This section consists of the definitions required to implement Layer 0,
## with comments indicating the "hooks" for higher layers. Subsections are
## used here to separate three related sets of rules (and hopefully make the
## grammar easier to follow).
##

## Forward declarations required for layer 2. These are all add-ons to
## layer 0 or 1 rules, used to expand the syntactic possibilities of the
## indicated base forms.
def parExpression2;
def parPostfixOperator2;
def parPrefixOperator2;
def parTerm2;

## Forward declaration required for integrating layer 1 definitions.
def parParser;

## Forward declarations.
def parAssignExpression;
def parClosure;

## Forward declaration for the "top" rule which parses operator expressions.
## This gets bound to `parUnaryExpression` in the Layer 0 and 1 grammars, but
## it's different in Layer 2.
def parOpExpression;

##
## Layer 0: Terms and expressions
##

## Parses an expression in general.
def parExpression = {:
    ## This one's the top level "regular-looking" expression (in that it
    ## covers the territory of C-style expressions).
    %parAssignExpression
|
    ## Note: Layer 2 adds additional rules here.
    %parExpression2
:};

## Parses a parenthesized expression. This produces a result identical to
## the inner `expression` node, except without an `interpolate` binding.
def parParenExpression = {:
    @"("
    ex = parExpression
    @")"

    { <> withoutInterpolate(ex) }
:};

## Parses a "name" of some sort. This is just an identifier, but with the
## result being the string payload (not wrapped in `@identifier(...)`).
def parName = {:
    nameIdent = @identifier
    { <> dataOf(nameIdent) }
:};

## Parses an integer literal. Note: This includes parsing a `-` prefix,
## so that simple negative constants aren't turned into complicated function
## calls.
def parInt = {:
    @"-"
    i = @int
    { <> makeLiteral($Number::neg(dataOf(i))) }
|
    i = @int
    { <> makeLiteral(dataOf(i)) }
:};

## Parses a string literal.
def parString = {:
    s = @string
    { <> makeLiteral(dataOf(s)) }
:};

## Parses an identifier, identifier-like keyword, or string literal,
## returning a string literal in all cases.
def parIdentifierString = {:
    parString
|
    name = parName
    { <> makeLiteral(name) }
|
    token = .
    {
        <> ifNot { <> dataOf(token) }
            {
                def type = get_typeName(token);
                def firstCh = nth(type, 0);
                <> ifIs { <> get(LOWER_ALPHA, firstCh) }
                    { <> makeLiteral(type) }
            }
    }
:};

## Parses a map key.
def parKey = {:
    key = parIdentifierString
    @":"
    { <> key }
|
    key = parExpression
    @":"
    { <> key }
:};

## Parses a mapping (element of a map).
def parMapping = {:
    keys = parKey*
    value = parExpression

    {
        <> ifIs { <> eq(keys, []) }
            { <out> ->
                ## No keys were specified, so the value must be either a
                ## whole-map interpolation or a variable-name-to-its-value
                ## binding.
                ifValue { <> get_interpolate(value) }
                    { interp -> <out> interp };
                ifIs { <> hasType(value, @@varRef) }
                    {
                        <out> makeCall(REFS::makeValueMap,
                            makeLiteral(get_name(value)), value)
                    }
            }
            {
                ## One or more keys.
                <> makeCallOrApply(REFS::makeValueMap,
                    keys*, withoutInterpolate(value))
            }
    }
:};

## Parses a map literal.
def parMap = {:
    @"{"

    result = (
        one = parMapping
        rest = (@"," parMapping)*
        {
            <> ifIs { <> eq(rest, []) }
                { <> one }
                { <> makeCall(REFS::cat, one, rest*) }
        }
    |
        { <> makeLiteral({}) }
    )

    @"}"

    { <> result }
:};

## Parses a list item or function call argument. This handles all of:
##
## * accepting general expressions
## * rejecting expressions that look like `key:value` mappings. This is
##   effectively "reserved syntax" (for future expansion); rejecting this
##   here means that `x:y` won't be mistaken for other valid syntax.
def parListItem = {:
    parIdentifierString
    @":"
    { die("Mapping syntax not valid as a list item or call argument.") }
|
    parExpression
:};

## Parses an "unadorned" (no bracketing) list. Yields a list (per se)
## of contents.
def parUnadornedList = {:
    one = parListItem
    rest = (@"," parListItem)*
    { <> [one, rest*] }
|
    { <> [] }
:};

## Parses a list literal.
def parList = {:
    @"["
    expressions = parUnadornedList
    @"]"
    {
        <> ifIs { <> eq(expressions, []) }
            { <> makeLiteral([]) }
            { <> makeCallOrApply(REFS::makeList, expressions*) }
    }
:};

## Parses a type name, yielding a type value. If the name is a blatant
## literal, then the result of this rule is also a literal. If not, the
## result of this rule is a call to `makeDerivedDataType`.
def parTypeName = {:
    name = (parIdentifierString | parParenExpression)

    {
        <> ifIs { <> hasType(name, @@literal) }
            { <> makeLiteral(@@(get_nodeValue(name))) }
            { <> makeCall(REFS::makeDerivedDataType, name) }
    }
:};

## Parses a type literal form.
def parType = {:
    @"@@"
    parTypeName
:};

## Parses a literal in derived value form.
def parDeriv = {:
    @"@"

    type = parTypeName
    value = (parParenExpression | parMap | parList)?

    { <> makeCall(REFS::makeValue, type, value*) }
:};

## Parses a variable reference.
def parVarRef = {:
    name = parName
    { <> makeVarRefLvalue(name) }
:};

## Parses a closure, but with a lookahead to make it less likely we'll
## have to do any heavier-weight parsing in failure cases.
def parClosureWithLookahead = {:
    &@"{"
    %parClosure
:};

## Parses a closure which must not define any formal arguments. This is done
## by parsing an arbitrary closure and then verifying that it does not
## declare formals. This is preferable to not-including formal argument
## syntax, because (a) no rule wants to differentiate these cases (rules
## either want an arbitrary closure or a specifically-constrained kind); (b)
## it reduces redundancy in the syntax, and (c) the error case on the former
## would be more obscure (as in just something like "unexpected token" on
## the would-be formal argument).
def parNullaryClosure = {:
    c = parClosureWithLookahead

    {
        def formals = get_formals(c);
        ifIs { <> ne(formals, []) }
            { die("Invalid formal argument in code block.") };
        <> c
    }
:};

## Parses a term (basic expression unit).
def parTerm = {:
    parVarRef | parInt | parString | parMap | parList |
    parDeriv | parType | parClosureWithLookahead | parParenExpression
|
    ## Defined by Samizdat Layer 1. The lookahead is just to make it clear
    ## that Layer 1 can only be "activated" with that one specific token.
    &@"{:" %parParser
|
    ## Defined by Samizdat Layer 2.
    &[@interpolatedString @"(" @"["] %parTerm2
:};

## Parses a list of "actual" (as opposed to formal) arguments to a function.
## Yields a list of expression nodes.
def parActualsList = {:
    @"("
    normalActuals = parUnadornedList
    @")"
    closureActuals = parClosureWithLookahead*
    { <> [closureActuals*, normalActuals*] }
|
    parClosureWithLookahead+
:};

## Parses a unary postfix operator. This yields a function (per se) to call
## in order to construct a node that represents the appropriate ultimate
## function call.
def parPostfixOperator = {:
    actuals = parActualsList
    { <> { node <> makeCallOrApply(node, actuals*) } }
|
    ## This is sorta-kinda a binary operator, but in terms of precedence it
    ## fits better here.
    @"::"
    key = parIdentifierString
    { <> { node <> makeGet(node, key) } }
|
    ## The lookahead failure here is to make the grammar prefer `*` to be
    ## treated as a binary op. (`*` is only defined as postfix in Layer 0,
    ## but higher layers augment its meaning.)
    @"*" !parExpression
    { <> { node <> makeInterpolate(node) } }
|
    @"?"
    { <> { node <> makeOptValue(node) } }
|
    ## Note: Layer 2 adds additional rules here.
    %parPostfixOperator2
:};

## Parses a unary expression. This is a term, optionally surrounded on
## either side by any number of unary operators. Postfix operators
## take precedence over (are applied before) the prefix operators.
def parUnaryExpression = {:
    ## The rule is written this way in order to ensure that the `-`
    ## in front of a numeric constant gets parsed as a term and not as
    ## a unary expression.
    basePrefixes = (
        base = parTerm
        { <> {base, prefixes: []} }
    |
        ## Note: Layer 2 adds prefix operator parsing here.
        prefixes = (%parPrefixOperator2)*
        base = parTerm
        ## Reverse the `prefixes` list, so that prefixes are applied
        ## in outward order from the base term.
        { <> {base, prefixes: $Sequence::reverse(prefixes)} }
    )

    postfixes = parPostfixOperator*

    {
        def prefixes = basePrefixes::prefixes;
        var result = basePrefixes::base;

        $Generator::filterPump(postfixes) { op -> result := op(result) };
        $Generator::filterPump(prefixes) { op -> result := op(result) };
        <> result
    }
:};

## Parses an assignment expression, or passes through to parse a regular
## `opExpression`. An lvalue is parsed here by first parsing an arbitrary
## `opExpression` and then extracting the `lvalue` constructor out of it.
## This fails (gracefully) if there is no `lvalue` to extract from a given
## expression.
parAssignExpression := {:
    base = %parOpExpression

    (
        @":="
        lvalue = { <> get_lvalue(base) }
        ex = parExpression
        { <> lvalue(ex) }
    |
        { <> base }
    )
:};

##
## Layer 0: Statements and yields
##

## Parses a nonlocal exit / return. All of the forms matched by this rule
## have the dual properties of (a) necessarily being at the end of a code
## block, and (b) being represented as a `jump` call in the underlying
## tree representation.
def parNonlocalExit = {:
    name = (
        @"<"
        n = parVarRef
        @">"
        { <> n }
    |
        op = [@break @continue @return]
        { <> makeVarRef(get_typeName(op)) }
    )

    optValue = parExpression?

    { <> makeJump(name, optValue*) }
:};

## Parses a local yield / return.
def parYield = {:
    @"<>"
    (
        ex = parExpression
        { <> {yield: ex} }
    |
        { <> {} }
    )
:};

## Parses an immutable variable definition, or forward declaration of same.
def parVarDef = {:
    @def
    name = parName
    optExpr = (@"=" parExpression)?

    { <> makeVarDef(name, optExpr*) }
:};

## Parses a mutable variable definition, or forward declaration of same.
def parVarDefMutable = {:
    @var
    name = parName
    optExpr = (@"=" parExpression)?

    { <> makeVarDefMutable(name, optExpr*) }
:};

## Parses a yield / nonlocal exit definition, yielding the def name.
def parYieldDef = {:
    @"<"
    name = parName
    @">"
    { <> name }
:};

## Parses an optional yield / nonlocal exit definition, always yielding
## a map (an empty map if no yield def was present).
def parOptYieldDef = {:
    y = parYieldDef
    { <> {yieldDef: y} }
|
    { <> {} }
:};

## Parses a formal argument decalaration.
def parFormal = {:
    name = (
        n = parName
        { <> {name: n} }
    |
        @"." { <> {} }
    )

    repeat = (
        r = [@"?" @"*" @"+"]
        { <> {repeat: get_typeName(r)} }
    |
        { <> {} }
    )

    { <> {name*, repeat*} }
:};

## Parses a list of formal arguments, with no surrounding parentheses.
def parFormalsList = {:
    one = parFormal
    rest = (@"," parFormal)*
    { <> [one, rest*] }
|
    { <> [] }
:};

## Parses program / function declarations.
def parClosureDeclarations = {:
    yieldDef = parOptYieldDef

    rest = (
        name = (
            n = parName
            { <> {name: n} }
        |
            { <> {} }
        )
        @"("
        formals = parFormalsList
        @")"
        { <> {name*, formals}}
    |
        formals = parFormalsList
        { <> {formals} }
    )

    (@"->" | &@"<>")

    { <> {yieldDef*, rest*} }
|
    { <> {formals: []} }
:};

## Parses the common part of function definition and generic function binding.
## The result of this rule is a `@closure` node, translated along these lines:
##
## ```
## name(arg1, arg2) { <out> -> stat1; stat2 }
## ```
## =>
## ```
## { <\"return"> name(arg1, arg2) ->
##     def out = \"return";
##     stat1;
##     stat2
## }
## ```
##
## except without a yield def binding statement if an explicit yield def was
## not present.
def parFunctionCommon = {:
    name = parName
    @"("
    formals = parFormalsList
    @")"
    code = parNullaryClosure

    {
        def returnDef = ifValue { <> code::yieldDef }
            { yieldDef ->
                ## The closure has a yield def, but we need to also bind
                ## it as `return`, so we add an extra local variable binding
                ## here.
                <> [makeVarDef(yieldDef, REFS::return)]
            }
            { <> [] };

        <> @closure{
            dataOf(code)*,
            formals,
            name,
            yieldDef: "return",
            statements: [returnDef*, get_statements(code)*]
        }
    }
:};

## Parses a `fn` definition statement. This wraps a `@closure` result of
## `parFunctionCommon` in a `@topDeclaration`.
##
## **Note:** `@topDeclaration` nodes are split apart in the `parClosureBody`
## rule.
def parFunctionDef = {:
    @fn
    closure = parFunctionCommon

    {
        def name = get_name(closure);
        <> @topDeclaration{
            top:  makeVarDef(name),
            main: makeVarBind(name, closure)
        }
    }
:};

## Parses a generic function binding. This wraps a `@closure` result of
## `parFunctionCommon` in a `@call`. The closure also gets a new `this`
## formal argument.
def parGenericBind = {:
    @fn
    bind = (parVarRef | parType)
    @"."
    closure = parFunctionCommon

    {
        def formals = get_formals(closure);
        def name = get_name(closure);
        def fullClosure = withFormals(closure, [{name: "this"}, formals*]);
        <> makeCall(REFS::genericBind, makeVarRef(name), bind, fullClosure)
    }
:};

## Parses a generic function definition. The translation is along these lines:
##
## ```
## fn .name(arg1, arg2);
## ```
## =>
## ```
## def name = makeRegularGeneric("name", 2, 2);
## ```
##
## with different numbers depending on the shape of the arguments, and with
## the function `makeUnitypeGeneric` if a `*` precedes the `.name`.
def parGenericDef = {:
    @fn
    optStar = @"*"?
    @"."
    name = parName
    @"("
    formals = parFormalsList
    @")"

    {
        def fullFormals = [{}, formals*]; ## First one is `this`.
        def func = ifIs { <> eq(optStar, []) }
            { <> REFS::makeRegularGeneric }
            { <> REFS::makeUnitypeGeneric };
        def call = makeCall(
            func,
            makeLiteral(name),
            makeLiteral(formalsMinArgs(fullFormals)),
            makeLiteral(formalsMaxArgs(fullFormals)));

        ## See `parFunction` above about `@topDeclaration`.
        <> @topDeclaration{
            top:  makeVarDef(name),
            main: makeVarBind(name, call)
        }
    }
:};

## Parses a statement form that is `export`able.
def parExportableStatement = {:
    parFunctionDef | parGenericDef | parVarDef
:};

## Parses a statement form (direct closure / program element). This includes
## all the `export`able statements and a few additional forms.
def parStatement = {:
    parExportableStatement | parGenericBind | parVarDefMutable | parExpression
:};

##
## Layer 0: Closures and programs
##

## Parses a closure body (statements plus optional yield).
def parClosureBody = {:
    @";"*

    most = (
        s = parStatement
        @";"+
        { <> s }
    )*

    last = (
        s = (parStatement | parNonlocalExit)
        { <> {statements: [s]} }
    |
        y = parYield
        { <> {statements: [], y*} }
    |
        { <> {statements: []} }
    )

    @";"*

    {
        def rawStatements = [most*, last::statements*];
        def tops = $Generator::filterAll(rawStatements)
            { s ->
                <> ifIs { <> hasType(s, @@topDeclaration) }
                    { <> s::top }
            };
        def mains = $Generator::filterAll(rawStatements)
            { s ->
                <> ifIs { <> hasType(s, @@topDeclaration) }
                    { <> s::main }
                    { <> s }
            };
        <> {last*, statements: [tops*, mains*]}
    }
:};

## Parses a closure (in-line anonymous function, with no extra bindings).
parClosure := {:
    @"{"
    decls = parClosureDeclarations
    body = parClosureBody
    @"}"
    { <> @closure{decls*, body*} }
:};

## Parses a program (top-level program or contents inside function braces).
def parProgram = {:
    body = parClosureBody
    { <> @closure{formals: [], body*} }
:};

##
## Layer 0: Entry points
##

## Top-level rule to parse an expression with possible error afterwards.
def parExpressionOrError = {:
    ex = parExpression
    (
        pending = .+
        { reportError(pending) }
    )?
    { <> ex }
:};

## Top-level rule to parse a program with possible error afterwards.
## Note that an empty string is a valid program.
def parProgramOrError = {:
    prog = parProgram
    (
        pending = .+
        { reportError(pending) }
    )?
    { <> prog }
:};


##
## Layer 1 Rules
##
## This section consists of the definitions required to implement Layer 1,
## above and beyond the preceding section.
##
## **Note:** The grammar uses the label "pex" to denote various
## "Parser EXpression" types.
##

## Forward declaration.
def parChoicePex;

## Map from parser token types to derived value types for pexes.
def PEX_TYPES = {
    @@"&": "lookaheadSuccess",
    @@"!": "lookaheadFailure",
    @@"?": "opt",
    @@"*": "star",
    @@"+": "plus"
};

## Parses a parser function.
parParser := {:
    @"{:"
    pex = %parChoicePex
    @":}"
    { <> @parser{value: pex} }
:};

## Parses a parenthesized parsing expression.
def parParenPex = {:
    @"("
    pex = %parChoicePex
    @")"
    { <> pex }
:};

## Parses a string literal parsing expression.
def parParserString = {:
    @string
:};

## Parses a token literal parsing expression.
def parParserToken = {:
    @"@"
    type = parIdentifierString
    { <> @token(@@(get_nodeValue(type))) }
:};

## Parses a string or character range parsing expression, used when defining
## sets. Yields a string per se (not a token).
def parParserSetString = {:
    s = @string

    (
        @".."
        end = @string

        { <out> ->
            def startChar = dataOf(s);
            def endChar = dataOf(end);

            ## Reject non-single-character strings.
            ifIs { <> ne(1, get_size(startChar)) } { <out> };
            ifIs { <> ne(1, get_size(endChar)) } { <out> };

            <> cat($Range::makeInclusiveRange(startChar, endChar)*)
        }
    |
        { <> dataOf(s) }
    )
:};

## Parses a set (or set complement) parsing expression.
def parParserSet = {:
    @"["

    type = (
        @"!" { <> "tokenSetComplement" }
    |
        { <> "tokenSet" }
    )

    terminals = (
        strings = parParserSetString+
        { <> collect(cat(strings*), { ch <> @@(ch) }) }
    |
        tokens = parParserToken+
        { <> collect(tokens, dataOf) }
    |
        { <> [] }
    )

    @"]"

    { <> @(type)(terminals) }
:};

## Parses a code block parsing expression.
def parParserCode = {:
    closure = parNullaryClosure
    { <> @code(dataOf(closure)) }
:};

## Parses a thunk parsing expression.
def parParserThunk = {:
    @"%"
    term = parTerm
    { <> @thunk(term) }
:};

## Parses a parsing expression term.
def parParserTerm = {:
    @"."
    { <> @any }
|
    @"("
    @")"
    { <> @empty }
|
    parVarRef | parParserString | parParserToken | parParserSet |
    parParserCode | parParserThunk | parParenPex
:};

## Parses a repeat (or not) parsing expression.
def parRepeatPex = {:
    term = parParserTerm
    (
        repeat = [@"?" @"*" @"+"]
        { <> @(get(PEX_TYPES, get_type(repeat)))(term) }
    |
        { <> term }
    )
:};

## Parses a lookahead (or not) parsing expression. This covers both lookahead
## success and lookahead failure.
def parLookaheadPex = {:
    (
        lookahead = [@"&" @"!"]
        pex = parRepeatPex
        { <> @(get(PEX_TYPES, get_type(lookahead)))(pex) }
    )
|
    parRepeatPex
:};

## Parses a name (or not) parsing expression.
def parNamePex = {:
    (
        name = parName
        @"="
        pex = parLookaheadPex
        { <> @varDef{name, value: pex} }
    )
|
    parLookaheadPex
:};

## Parses a sequence parsing expression. This includes sequences of length
## one, but it does *not* parse empty (zero-length) sequences.
def parSequencePex = {:
    items = parNamePex+
    { <> @sequence(items) }
:};

## Parses a choice parsing expression. This includes a single choice.
parChoicePex := {:
    one = parSequencePex
    rest = (@"|" parSequencePex)*
    { <> @choice[one, rest*] }
:};


##
## Layer 2 Rule Stubs
##

## For more details, see comment where `parOpExpression` is declared, above.
parOpExpression := parUnaryExpression;

## In layer 2, these are all non-trivial, but here they are simply
## always-fail.

parExpression2      := {: !() :};
parPostfixOperator2 := {: !() :};
parPrefixOperator2  := {: !() :};
parTerm2            := {: !() :};
```
