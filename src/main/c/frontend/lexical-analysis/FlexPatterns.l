%{

#include "FlexActions.h"

%}

/**
 * Enable start condition manipulation functions.
 */
%option stack

/**
 * Flex contexts (a.k.a. start conditions).
 *
 * @see https://westes.github.io/flex/manual/Start-Conditions.html
 */
%x MULTILINE_COMMENT
%x STRING

/**
 * Reusable patterns.
 *
 * @see https://westes.github.io/flex/manual/Matching.html
 * @see https://westes.github.io/flex/manual/Patterns.html
 */

%%

"/*"								{ BEGIN(MULTILINE_COMMENT); BeginMultilineCommentLexemeAction(createLexicalAnalyzerContext()); }
<MULTILINE_COMMENT>"*/"				{ EndMultilineCommentLexemeAction(createLexicalAnalyzerContext()); BEGIN(INITIAL); }
<MULTILINE_COMMENT>[[:space:]]+		{ IgnoredLexemeAction(createLexicalAnalyzerContext()); }
<MULTILINE_COMMENT>[^*]+			{ IgnoredLexemeAction(createLexicalAnalyzerContext()); }
<MULTILINE_COMMENT>.				{ IgnoredLexemeAction(createLexicalAnalyzerContext()); }

"-"									{ return ArithmeticOperatorLexemeAction(createLexicalAnalyzerContext(), SUB); }
"*"									{ return ArithmeticOperatorLexemeAction(createLexicalAnalyzerContext(), MUL); }
"/"									{ return ArithmeticOperatorLexemeAction(createLexicalAnalyzerContext(), DIV); }
"+"									{ return ArithmeticOperatorLexemeAction(createLexicalAnalyzerContext(), ADD); }

"("									{ return ParenthesisLexemeAction(createLexicalAnalyzerContext(), OPEN_PARENTHESIS); }
")"									{ return ParenthesisLexemeAction(createLexicalAnalyzerContext(), CLOSE_PARENTHESIS); }

[[:digit:]]+						{ return IntegerLexemeAction(createLexicalAnalyzerContext()); }

"@Default"							{ return ModifierLexemeAction(createLexicalAnalyzerContext(), DEFAULT_ANNOTATION); }
"@Customize"						{ return ModifierLexemeAction(createLexicalAnalyzerContext(), CUSTOMIZE_ANNOTATION); }

"Array"								{ return ClassLexemeAction(createLexicalAnalyzerContext(), ARRAY); }
"List"								{ return ClassLexemeAction(createLexicalAnalyzerContext(), LIST); }
"LinkedList"						{ return ClassLexemeAction(createLexicalAnalyzerContext(), LINKED_LIST); }
"DoubleLinkedList"					{ return ClassLexemeAction(createLexicalAnalyzerContext(), DOUBLE_LINKED_LIST); }
"Tree"								{ return ClassLexemeAction(createLexicalAnalyzerContext(), TREE); }
"Graph"								{ return ClassLexemeAction(createLexicalAnalyzerContext(), GRAPH); }
"DirectedGraph"						{ return ClassLexemeAction(createLexicalAnalyzerContext(), DIRECTED_GRAPH); }
"Table"								{ return ClassLexemeAction(createLexicalAnalyzerContext(), TABLE); }

"{"									{ return BracesLexemeAction(createLexicalAnalyzerContext(), OPEN_BRACES); }
"}"									{ return BracesLexemeAction(createLexicalAnalyzerContext(), CLOSE_BRACES); }

"["									{ return BracketsLexemeAction(createLexicalAnalyzerContext(), OPEN_BRACKETS); }
"]"									{ return BracketsLexemeAction(createLexicalAnalyzerContext(), CLOSE_BRACKETS); }

","									{ return CommaLexemeAction(createLexicalAnalyzerContext()); }
":"									{ return ColonLexemeAction(createLexicalAnalyzerContext()); }
";"									{ return SemiColonLexemeAction(createLexicalAnalyzerContext()); }
"$"[[:alnum:]]+					    { return StyleVariableLexemeAction(createLexicalAnalyzerContext()); }

"\""								{ BEGIN(STRING); BeginStringLexemeAction(createLexicalAnalyzerContext()); }
<STRING>"\""						{ EndStringLexemeAction(createLexicalAnalyzerContext()); BEGIN(INITIAL); }
<STRING>[^"]*						{ return StringLexemeAction(createLexicalAnalyzerContext()); }

[[:space:]]+						{ IgnoredLexemeAction(createLexicalAnalyzerContext()); }
.									{ return UnknownLexemeAction(createLexicalAnalyzerContext()); }

%%

#include "FlexExport.h"

@Default(background
(red))
