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

"/*"											{ BEGIN(MULTILINE_COMMENT); BeginMultilineCommentLexemeAction(createLexicalAnalyzerContext()); }
<MULTILINE_COMMENT>"*/"							{ EndMultilineCommentLexemeAction(createLexicalAnalyzerContext()); BEGIN(INITIAL); }
<MULTILINE_COMMENT>[[:space:]]+					{ IgnoredLexemeAction(createLexicalAnalyzerContext()); }
<MULTILINE_COMMENT>[^*]+						{ IgnoredLexemeAction(createLexicalAnalyzerContext()); }
<MULTILINE_COMMENT>.							{ IgnoredLexemeAction(createLexicalAnalyzerContext()); }

"\""											{ BEGIN(STRING); BeginStringLexemeAction(createLexicalAnalyzerContext()); }
<STRING>"\""									{ EndStringLexemeAction(createLexicalAnalyzerContext()); BEGIN(INITIAL); }
<STRING>[^"]*									{ return StringLexemeAction(createLexicalAnalyzerContext()); }

"//".*"\n"										{ IgnoredLexemeAction(createLexicalAnalyzerContext()); }
"#".*"\n"										{ IgnoredLexemeAction(createLexicalAnalyzerContext()); }

"("												{ return ParenthesisLexemeAction(createLexicalAnalyzerContext(), OPEN_PARENTHESIS); }
")"												{ return ParenthesisLexemeAction(createLexicalAnalyzerContext(), CLOSE_PARENTHESIS); }

"@Default"										{ return ModifierLexemeAction(createLexicalAnalyzerContext(), DEFAULT_ANNOTATION); }
"@Customize"									{ return ModifierLexemeAction(createLexicalAnalyzerContext(), CUSTOMIZE_ANNOTATION); }

"Array"											{ return StructureTypeLexemeAction(createLexicalAnalyzerContext(), ARRAY); }
"List"											{ return StructureTypeLexemeAction(createLexicalAnalyzerContext(), LIST); }
"LinkedList"									{ return StructureTypeLexemeAction(createLexicalAnalyzerContext(), LINKED_LIST); }
"DoubleLinkedList"								{ return StructureTypeLexemeAction(createLexicalAnalyzerContext(), DOUBLE_LINKED_LIST); }
"Tree"											{ return StructureTypeLexemeAction(createLexicalAnalyzerContext(), TREE); }
"Graph"											{ return StructureTypeLexemeAction(createLexicalAnalyzerContext(), GRAPH); }
"DirectedGraph"									{ return StructureTypeLexemeAction(createLexicalAnalyzerContext(), DIRECTED_GRAPH); }
"Table"											{ return StructureTypeLexemeAction(createLexicalAnalyzerContext(), TABLE); }

"{"												{ return BracesLexemeAction(createLexicalAnalyzerContext(), OPEN_BRACES); }
"}"												{ return BracesLexemeAction(createLexicalAnalyzerContext(), CLOSE_BRACES); }

"["												{ return BracketsLexemeAction(createLexicalAnalyzerContext(), OPEN_BRACKETS); }
"]"												{ return BracketsLexemeAction(createLexicalAnalyzerContext(), CLOSE_BRACKETS); }

","												{ return CommaLexemeAction(createLexicalAnalyzerContext()); }
":"												{ return ColonLexemeAction(createLexicalAnalyzerContext()); }
";"												{ return SemiColonLexemeAction(createLexicalAnalyzerContext()); }
"$"[[:alpha:]]									{ return StyleVariableLexemeAction(createLexicalAnalyzerContext()); }
"$"[[:alpha:]][[:alnum:]\-]*[[:alnum:]]			{ return StyleVariableLexemeAction(createLexicalAnalyzerContext()); }
[[:alpha:]][[:alnum:]]* 						{ return LabelLexemeAction(createLexicalAnalyzerContext()); }

[[:space:]]+									{ IgnoredLexemeAction(createLexicalAnalyzerContext()); }
.												{ return UnknownLexemeAction(createLexicalAnalyzerContext()); }

%%

#include "FlexExport.h"
