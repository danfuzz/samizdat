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
import core.Format;
import core.Generator :: filterPump;
import core.Lang0;
import core.LangNode :: *;
import core.Peg;
import core.Range :: makeInclusiveRange;
import core.Sequence :: reverse;


##
## Private Definitions
##

## Reports the given list of pending tokens as part of error processing.
fn reportError(pending) {
    note("Pending tokens:");

    filterPump(pending, makeInclusiveRange(1, 50))
        { token, . -> note("    ", $Format::source(token)) };

    die("\nExtra tokens at end of program.")
};

## Set-like map of all lowercase identifier characters. Used to figure
## out if we're looking at a keyword in the `identifierSymbol` rule.
def LOWER_ALPHA = {
    makeInclusiveRange("a", "z")*: true
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
def parPexBlock;

## Forward declarations.
def parAssignExpression;
def parRawClosure;

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

    { withoutInterpolate(ex) }
:};

## Parses a "name" of some sort. This is just an identifier, but with the
## result being the string payload (not wrapped in `@identifier{...}`).
def parNameString = {:
    nameIdent = @identifier
    { nameIdent::value }
:};

## Parses a non-empty comma-separated list of "names." A "name" is as per
## `parNameString` above. The result is a list of strings (per se).
def parNameStringList = {:
    first = parNameString
    rest = (@"," parNameString)*
    { [first, rest*] }
:};

## Parses a variable reference. Returns a variable "fetch" with an `lvalue`
## binding for constructing a "store" as appropriate.
def parVarLvalue = {:
    name = parNameString
    { makeVarFetchLvalue(name) }
:};

## Parses a variable box reference.
def parVarRef = {:
    @var
    name = parNameString
    { makeVarRef(name) }
:};

## Parses an identifier, identifier-like keyword, or string literal,
## returning a symbol literal in all cases.
def parIdentifierSymbol = {:
    s = @string
    { makeSymbolLiteral(s::value) }
|
    name = parNameString
    { makeSymbolLiteral(name) }
|
    token = .
    {
        ifNot { token.dataOf() }
            {
                def name = get_classNameString(token);
                def firstCh = name.nth(0);
                ifIs { LOWER_ALPHA.get(firstCh) }
                    { makeSymbolLiteral(name) }
            }
    }
:};

## Parses a simple data literal, including literal booleans, ints, and
## strings.
##
## **Note:** This includes parsing a `-` prefix on ints, to allow for
## negative int literals in layer 0.
def parLiteral = {:
    @"-"
    i = @int
    { makeLiteral(i::value.neg()) }
|
    i = @int
    { makeLiteral(i::value) }
|
    s = @string
    { makeLiteral(s::value) }
|
    @false
    { makeLiteral(false) }
|
    @true
    { makeLiteral(true) }
|
    @null
    { makeLiteral(null) }
|
    @"@"
    @"."
    parIdentifierSymbol
:};

## Parses a map key.
def parKey = {:
    key = parIdentifierSymbol
    @":"
    { makeLiteral(symbolString(key::value)) }
|
    key = parExpression
    @":"
    { key }
:};

## Parses a mapping (element of a map).
def parMapping = {:
    keys = parKey+
    value = parExpression

    { makeCallOrApply(REFS::makeValueMap, keys*, withoutInterpolate(value)) }
|
    ## An expression is valid only if it's an interpolation, in which case we
    ## take the interpolation variant of the node.
    value = parExpression
    { value::interpolate }
|
    ## Otherwise, it's got to be a raw name, representing a binding of that
    ## name to its value as a variable.
    name = parNameString
    { makeCall(REFS::makeValueMap, makeLiteral(name), makeVarFetch(name)) }
:};

## Parses a map literal.
def parMap = {:
    @"{"

    result = (
        one = parMapping
        rest = (@"," parMapping)*
        {
            ifIs { eq(rest, []) }
                { one }
                { makeCall(REFS::SYM_cat, one, rest*) }
        }
    |
        { makeLiteral({}) }
    )

    @"}"

    { result }
:};

## Parses a list item or function call argument. This handles all of:
##
## * accepting general expressions
## * rejecting expressions that look like `key:value` mappings. This is
##   effectively "reserved syntax" (for future expansion); rejecting this
##   here means that `x:y` won't be mistaken for other valid syntax.
def parListItem = {:
    parIdentifierSymbol
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
    { [one, rest*] }
|
    { [] }
:};

## Parses a list literal.
def parList = {:
    @"["
    expressions = parUnadornedList
    @"]"
    {
        ifIs { eq(expressions, []) }
            { makeLiteral([]) }
            { makeCallOrApply(REFS::makeList, expressions*) }
    }
:};

## Parses a type literal form, yielding an expression node that produces a
## type value. If the name is a blatant literal, then the result of this rule
## is also a literal. If not, the result of this rule is a call to
## `makeDerivedDataClass`.
def parType = {:
    @"@@"

    (
        name = parIdentifierSymbol
        { makeLiteral(@@(name::value)) }
    |
        name = parParenExpression
        { makeCall(REFS::makeDerivedDataClass, name) }
    )
:};

## Parses a literal in derived value form.
def parDeriv = {:
    @"@"

    cls = (
        name = parIdentifierSymbol
        { makeLiteral(@@(name::value)) }
    |
        parParenExpression
    )

    value = (parParenExpression | parMap | parList)?

    { makeCall(REFS::makeData, cls, value*) }
:};

## Parses a closure, resulting in one that *always* has a `yield` binding.
def parFullClosure = {:
    ## The lookahead to makes it so we don't have to do any heavier-weight
    ## parsing in easy-failure cases.
    &@"{"
    raw = %parRawClosure

    {
        def closure = makeFullClosure(raw);
        withoutTops(closure)
    }
:};

## Parses a closure, resulting in one that does *not* necessarily have
## a `yield` binding.
def parBasicClosure = {:
    &@"{"  ## Lookahead for same reason as above.
    raw = %parRawClosure

    {
        def closure = makeBasicClosure(raw);
        withoutTops(closure)
    }
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
    c = parFullClosure

    {
        ifIs { ne(c::formals, []) }
            { die("Invalid formal argument in code block.") };
        c
    }
:};

## Like, `parNullaryClosure` (above), except returning a basic (no
## required `yield`) closure node.
def parBasicNullaryClosure = {:
    c = parBasicClosure

    {
        ifIs { ne(c::formals, []) }
            { die("Invalid formal argument in code block.") };
        c
    }
:};

## Parses a term (basic expression unit).
def parTerm = {:
    parVarLvalue | parVarRef | parLiteral | parMap | parList |
    parDeriv | parType | parFullClosure | parParenExpression
|
    ## Defined by Samizdat Layer 1. The lookahead is just to make it clear
    ## that Layer 1 can only be "activated" with that one specific token.
    &@"{:" %parPexBlock
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
    closureActuals = parFullClosure*
    { [closureActuals*, normalActuals*] }
|
    parFullClosure+
:};

## Parses a unary postfix operator. This yields a function (per se) to call
## in order to construct a node that represents the appropriate ultimate
## function call.
def parPostfixOperator = {:
    actuals = parActualsList
    { { node -> makeCallOrApply(node, actuals*) } }
|
    ## This is sorta-kinda a binary operator, but in terms of precedence it
    ## fits better here.
    @"::"
    key = parIdentifierSymbol
    { { node -> makeGet(node, makeLiteral(symbolString(key::value))) } }
|
    ## The lookahead failure here is to make the grammar prefer `*` to be
    ## treated as a binary op. (`*` is only defined as postfix in Layer 0,
    ## but higher layers augment its meaning.)
    @"*" !parExpression
    { { node -> makeInterpolate(node) } }
|
    @"?"
    { { node -> makeMaybeValue(node) } }
|
    ## This translates as follows:
    ##
    ## ```
    ## target.memberName(arg, ...)
    ## =>
    ## @.memberName(target, arg, ...)
    ## ```
    ##
    ## ```
    ## target.memberName
    ## =>
    ## @.get_memberName(target)
    ## ```
    ##
    ## ```
    ## target.memberName := expression
    ## =>
    ## @.set_memberName(target, expression)
    ## ```
    ##
    ## The setter variant works via an `lvalue` binding added to a parsed
    ## getter expression.
    ##
    @"."
    name = parNameString

    (
        ## `target.memberName(arg, ...)`
        actuals = parActualsList
        {
            { node -> makeCallOrApply(makeSymbolLiteral(name), node, actuals*) }
        }
    |
        ## `target.memberName` (includes parsing of both getters and setters)
        {
            def getterRef = makeSymbolLiteral("get_".cat(name));
            { node ->
                def getterCall = makeCall(getterRef, node);
                @(get_class(getterCall)){
                    getterCall.dataOf()*,
                    lvalue: { expr ->
                        def setterRef = makeSymbolLiteral("set_".cat(name));
                        makeCall(setterRef, node, expr)
                    }
                }
            }
        }
    )
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
        { {base, prefixes: []} }
    |
        ## Note: Layer 2 adds prefix operator parsing here.
        prefixes = (%parPrefixOperator2)*
        base = parTerm
        ## Reverse the `prefixes` list, so that prefixes are applied
        ## in outward order from the base term.
        { {base, prefixes: reverse(prefixes)} }
    )

    postfixes = parPostfixOperator*

    {
        def prefixes = basePrefixes::prefixes;
        var result = basePrefixes::base;

        filterPump(postfixes) { op -> result := op(result) };
        filterPump(prefixes) { op -> result := op(result) };
        result
    }
:};

## Parses an assignment expression, or passes through to parse a regular
## `opExpression`. An lvalue is parsed here by first parsing an arbitrary
## `opExpression` and then ensuring it's valid via a call to
## `makeAssignmentIfPossible`. This fails (gracefully) if the would-be lvalue
## isn't actually one.
parAssignExpression := {:
    target = %parOpExpression

    (
        @":="
        value = parExpression
        { makeAssignmentIfPossible(target, value) }
    |
        { target }
    )
:};

##
## Layer 0: Statements and yields
##

## Parses a yield or a nonlocal exit (named or implicit). All of the forms
## matched by this rule necessarily appear at the end of a block and are
## expected to be placed in a `yield` binding of a `closure` node. The
## nonlocal exit forms all end up being represented as a `noYield` node, so
## it's sorta funny that they hang off of `yield` (because they never actually
## yield, per se).
def parYieldOrNonlocal = {:
    op = [@break @continue @return @yield]
    optQuest = @"?"?

    name = (
        { hasClass(op, @@yield) }
        (
            @"/"
            parVarLvalue
        |
            ## Indicate that this is a regular (local) yield. Checked below.
            { @yield }
        )
    |
        { makeVarFetch(get_classNameString(op)) }
    )

    ## A value expression is mandatory if there is a `?` after the
    ## operator. Otherwise, it's optional.
    value = (
        v = parExpression
        { ifIs { optQuest* } { makeMaybe(v) } { v } }
    |
        { ifNot { optQuest* } { @void } }
    )

    {
        ifIs { eq(name, @yield) }
            { value }
            { makeNonlocalExit(name, value) }
    }
:};

## Parses an immutable variable definition, or forward declaration of same.
def parVarDef = {:
    @def
    name = parNameString
    optExpr = (@"=" parExpression)?

    { makeVarDef(name, optExpr*) }
:};

## Parses a mutable variable definition, or forward declaration of same.
def parVarDefMutable = {:
    @var
    name = parNameString
    optExpr = (@"=" parExpression)?

    { makeVarDefMutable(name, optExpr*) }
:};

## Parses a yield / nonlocal exit definition, yielding the def name.
def parYieldDef = {:
    @"/"
    parNameString
:};

## Parses an optional yield / nonlocal exit definition, always yielding
## a map (an empty map if no yield def was present).
def parOptYieldDef = {:
    y = parYieldDef
    { {yieldDef: y} }
|
    { {} }
:};

## Parses a formal argument decalaration.
def parFormal = {:
    name = (
        n = parNameString
        { {name: n} }
    |
        @"." { {} }
    )

    repeat = (
        r = [@"?" @"*" @"+"]
        { {repeat: get_classNameString(r)} }
    |
        { {} }
    )

    { {name*, repeat*} }
:};

## Parses a list of formal arguments, with no surrounding parentheses.
def parFormalsList = {:
    one = parFormal
    rest = (@"," parFormal)*
    { [one, rest*] }
|
    { [] }
:};

## Parses program / function declarations.
def parClosureDeclarations = {:
    most = (
        name = (
            n = parNameString
            { {name: n.toSymbol()} }
        |
            { {} }
        )
        @"("
        formals = parFormalsList
        @")"
        { {name*, formals}}
    |
        formals = parFormalsList
        { {formals} }
    )

    yieldDef = parOptYieldDef

    @"->"

    { {most*, yieldDef*} }
|
    { {formals: []} }
:};

## Parses the common part of function definition and generic function binding.
## The result of this rule is a `@closure` node, translated along these lines:
##
## ```
## name(arg1, arg2) { /out -> stat1; stat2 }
## ```
## =>
## ```
## { name(arg1, arg2) /\"return" ->
##     def out = \"return";
##     stat1;
##     stat2
## }
## ```
##
## except without a yield def binding statement if an explicit yield def was
## not present.
def parFunctionCommon = {:
    name = parNameString
    @"("
    formals = parFormalsList
    @")"
    code = parBasicNullaryClosure

    {
        def basic = withName(
            withFormals(
                withYieldDef(code, "return"),
                formals),
            name.toSymbol());

        makeFullClosure(basic)
    }
:};

## Parses a `fn` definition statement. This wraps a `@closure` result of
## `parFunctionCommon` in a top-declaring `@varDef`.
def parFunctionDef = {:
    @fn
    closure = parFunctionCommon

    { withTop(makeVarDef(symbolString(closure::name), closure)) }
:};

## Parses a method binding. This wraps a `@closure` result of
## `parFunctionCommon` in a `@call`. The closure also gets a new `this`
## formal argument.
def parMethodBind = {:
    @fn
    bind = parVarLvalue
    @"."
    closure = parFunctionCommon

    {
        def formals = closure::formals;
        def name = closure::name;
        def fullClosure = withFormals(closure, [{name: "this"}, formals*]);
        makeCall(REFS::classAddMethod, bind, makeLiteral(name), fullClosure)
    }
:};

## Parses an optional binding name or name prefix for an `import` statement.
## This rule never fails. The result is always a map, empty if there was no
## name / prefix, or binding one of `name` or `prefix`.
def parImportName = {:
    name = parNameString

    key = (
        @"*" { "prefix" }
    |
        { "name" }
    )

    @"="
    { {(key): name} }
|
    { {} }
:};

## Parses an optional format name for an `import` statement. This rule never
## fails. The result is always a map, empty if there was no format name, or
## binding `format`.
def parImportFormat = {:
    @"@"
    f = parIdentifierSymbol
    { {format: f::value} }
|
    { {} }
:};

## Parses the source for an `import` statement. The result is either a name
## string as the payload of either an `@internal` or `@external` value.
def parImportSource = {:
    @"."
    @"/"

    first = parNameString
    rest = (
        @"/"
        n = parNameString
        { "/".cat(n) }
    )*
    optSuffix = (
        @"."
        n = parNameString
        { ".".cat(n) }
    )?

    {
        def name = first.cat(rest*, optSuffix*);
        @internal{name}
    }
|
    first = parNameString
    rest = (
        @"."
        n = parNameString
        { ".".cat(n) }
    )*

    {
        def name = first.cat(rest*);
        @external{name}
    }
:};

## Parses a list of binding names for an `import` statement. The result is
## a list of strings, or `@"*"` to indicate a wildcard of all names.
def parImportSelect = {:
    @"::"
    (
        @"*"
        { {select: @"*"} }
    |
        select = parNameStringList
        { {select} }
    )
|
    { {} }
:};

## Parses an `import` statement. This works by first parsing a very general
## version of the syntax which allows invalid combinations, and then checking
## in code that the actual combination is valid.
def parImportStatement = {:
    optExport = @export?
    @import
    nameOrPrefix = parImportName
    format = parImportFormat
    source = parImportSource
    select = parImportSelect

    {
        def data = {nameOrPrefix*, format*, select*, source};
        ifIs { optExport* }
            { makeExport(makeImport(data)) }
            { makeImport(data) }
    }
:};

## Parses an executable statement form that is `export`able. This does *not*
## include `import` statements.
def parExportableStatement = {:
    parFunctionDef | parVarDef
:};

## Parses an executable statement form (direct closure / program element).
## This includes all the `export`able statements and a few additional forms.
def parStatement = {:
    parExportableStatement | parMethodBind | parVarDefMutable | parExpression
:};

## Parses a program body statement form, including both regular executable
## statements and `export` statements (but not `import` statements).
def parProgramStatement = {:
    parStatement
|
    @export
    (
        select = parNameStringList
        { makeExportSelection(select*) }
    |
        stat = parExportableStatement
        { makeExport(stat) }
    )
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
        { s }
    )*

    last = (
        s = parStatement
        { {statements: [s]} }
    |
        y = parYieldOrNonlocal
        { {statements: [], yield: y} }
    |
        { {statements: []} }
    )

    @";"*

    { {last*, statements: [most*, last::statements*]} }
:};

## Parses a closure (in-line anonymous function, with no extra bindings).
## This results in a simple map of bindings.
parRawClosure := {:
    @"{"
    decls = parClosureDeclarations
    body = parClosureBody
    @"}"
    { {decls*, body*} }
:};

## Parses a program (list of statements, including imports and exports).
def parProgram = {:
    imports = (
        @";"*
        first = parImportStatement
        rest = (@";"+ parImportStatement)*
        { [first, rest*] }
    |
        { [] }
    )

    body = (
        (
            ## There was at least one import, so there needs to be at least
            ## one semicolon between the final import and first statement.
            { ne(imports, []) }
            @";"+
        |
            @";"*
        )

        first = parProgramStatement
        rest = (@";"+ parProgramStatement)*
        { [first, rest*] }
    |
        { [] }
    )

    @";"*

    {
        def closure = makeFullClosure({
            statements: [imports*, body*],
            yield:      @void
        });
        withoutTops(closure)
    }
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
    { ex }
:};

## Top-level rule to parse a program with possible error afterwards.
## Note that an empty string is a valid program.
def parProgramOrError = {:
    prog = parProgram
    (
        pending = .+
        { reportError(pending) }
    )?
    { prog }
:};


##
## Layer 1 Rules
##
## This section consists of the definitions required to implement Layer 1,
## above and beyond the preceding section.
##
## **Note:** The grammar uses the label "pex" to denote various
## "Parser EXpression" types and rules.
##

## Forward declaration.
def parPexChoice;

## Map from parser token types to derived value types for pexes.
def PEX_TYPES = {
    @@"&": @@lookaheadSuccess,
    @@"!": @@lookaheadFailure,
    @@"?": @@opt,
    @@"*": @@star,
    @@"+": @@plus
};

## Parses a parser function.
parPexBlock := {:
    @"{:"
    pex = %parPexChoice
    @":}"
    { @parser{pex} }
:};

## Parses a parenthesized parsing expression.
def parPexParenExpression = {:
    @"("
    pex = %parPexChoice
    @")"
    { pex }
:};

## Parses a variable reference parsing expression.
def parPexVarRef = {:
    name = parNameString
    { makeVarRef(name) }
:};

## Parses a string literal parsing expression.
def parPexString = {:
    @string
:};

## Parses a token literal parsing expression.
def parPexToken = {:
    @"@"
    type = parIdentifierSymbol
    { @token{value: @@(type::value)} }
:};

## Parses a string or character range parsing expression, used when defining
## sets. Yields a string per se (not a token).
def parPexSetString = {:
    s = @string

    (
        @".."
        end = @string

        { /out ->
            def startChar = s::value;
            def endChar = end::value;

            ## Reject non-single-character strings.
            ifIs { ne(1, startChar.get_size()) } { yield /out };
            ifIs { ne(1, endChar.get_size()) } { yield /out };

            yield "".cat(makeInclusiveRange(startChar, endChar)*)
        }
    |
        { s::value }
    )
:};

## Parses a set (or set complement) parsing expression.
def parPexSet = {:
    @"["

    type = (
        @"!" { @@tokenSetComplement }
    |
        { @@tokenSet }
    )

    terminals = (
        strings = parPexSetString+
        { "".cat(strings*).collect { ch -> @@(ch.toSymbol()) } }
    |
        tokens = parPexToken+
        { tokens.collect({ n -> n::value }) }
    |
        { [] }
    )

    @"]"

    { @(type){values: terminals} }
:};

## Parses a code block parsing expression.
def parPexCode = {:
    closure = parNullaryClosure
    { @code(closure.dataOf()) }
:};

## Parses a thunk parsing expression.
def parPexThunk = {:
    @"%"
    value = parTerm
    { @thunk{value} }
:};

## Parses a parsing expression term.
def parPexTerm = {:
    @"."
    { @any }
|
    @"("
    @")"
    { @empty }
|
    parPexVarRef | parPexString | parPexToken | parPexSet |
    parPexCode | parPexThunk | parPexParenExpression
:};

## Parses a repeat (or not) parsing expression.
def parPexRepeat = {:
    pex = parPexTerm
    (
        repeat = [@"?" @"*" @"+"]
        { @(PEX_TYPES.get(get_class(repeat))){pex} }
    |
        { pex }
    )
:};

## Parses a lookahead (or not) parsing expression. This covers both lookahead
## success and lookahead failure.
def parPexLookahead = {:
    (
        lookahead = [@"&" @"!"]
        pex = parPexRepeat
        { @(PEX_TYPES.get(get_class(lookahead))){pex} }
    )
|
    parPexRepeat
:};

## Parses a name (or not) parsing expression.
def parPexName = {:
    (
        name = parNameString
        @"="
        pex = parPexLookahead
        { @varDef{name, value: pex} }
    )
|
    parPexLookahead
:};

## Parses a sequence parsing expression. This includes sequences of length
## one, but it does *not* parse empty (zero-length) sequences.
def parPexSequence = {:
    pexes = parPexName+
    { @sequence{pexes} }
:};

## Parses a choice parsing expression. This includes a single choice.
parPexChoice := {:
    one = parPexSequence
    rest = (@"|" parPexSequence)*
    { @choice{pexes: [one, rest*]} }
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
