#ifndef FLEX_ACTIONS_HEADER
#define FLEX_ACTIONS_HEADER

#include "../../shared/Environment.h"
#include "../../shared/Logger.h"
#include "../../shared/String.h"
#include "../../shared/Type.h"
#include "../syntactic-analysis/AbstractSyntaxTree.h"
#include "../syntactic-analysis/BisonParser.h"
#include "LexicalAnalyzerContext.h"
#include <stdio.h>
#include <stdlib.h>

/** Initialize module's internal state. */
void initializeFlexActionsModule();

/** Shutdown module's internal state. */
void shutdownFlexActionsModule();

/**
 * Flex lexeme processing actions.
 */

void BeginMultilineCommentLexemeAction(LexicalAnalyzerContext *lexicalAnalyzerContext);
void EndMultilineCommentLexemeAction(LexicalAnalyzerContext *lexicalAnalyzerContext);

void BeginStringLexemeAction(LexicalAnalyzerContext *lexicalAnalyzerContext);
void EndStringLexemeAction(LexicalAnalyzerContext *lexicalAnalyzerContext);
Token StringLexemeAction(LexicalAnalyzerContext *lexicalAnalyzerContext);

Token ParenthesisLexemeAction(LexicalAnalyzerContext *lexicalAnalyzerContext, Token token);
Token BracesLexemeAction(LexicalAnalyzerContext *lexicalAnalyzerContext, Token token);
Token BracketsLexemeAction(LexicalAnalyzerContext *lexicalAnalyzerContext, Token token);

Token CommaLexemeAction(LexicalAnalyzerContext *lexicalAnalyzerContext);
Token ColonLexemeAction(LexicalAnalyzerContext *lexicalAnalyzerContext);
Token SemiColonLexemeAction(LexicalAnalyzerContext *lexicalAnalyzerContext);

Token StructureTypeLexemeAction(LexicalAnalyzerContext *lexicalAnalyzerContext, Token token);
Token ModifierLexemeAction(LexicalAnalyzerContext *lexicalAnalyzerContext, Token token);
Token StyleVariableLexemeAction(LexicalAnalyzerContext *lexicalAnalyzerContext);
Token LabelLexemeAction(LexicalAnalyzerContext *lexicalAnalyzerContext);

void IgnoredLexemeAction(LexicalAnalyzerContext *lexicalAnalyzerContext);
Token UnknownLexemeAction(LexicalAnalyzerContext *lexicalAnalyzerContext);

#endif
