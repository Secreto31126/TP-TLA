#ifndef BISON_ACTIONS_HEADER
#define BISON_ACTIONS_HEADER

#include "../../shared/CompilerState.h"
#include "../../shared/Logger.h"
#include "../../shared/Type.h"
#include "AbstractSyntaxTree.h"
#include "SyntacticAnalyzer.h"
#include <stdlib.h>

/** Initialize module's internal state. */
void initializeBisonActionsModule();

/** Shutdown module's internal state. */
void shutdownBisonActionsModule();

/**
 * Bison semantic actions.
 */
StructureType *StructureTypeSemanticAction(Token st);
Cells *CellsSemanticAction(CellValue *cv, char *l, Cells *c);
CellValue *CellValueSemanticAction(char *v);
CellValue *CellUnorderedSemanticAction(Cells *c);
CellValue *CellOrderedValueSemanticAction(Cells *c);
AnnotationList *AnnotationListSemanticAction(Annotation *a, AnnotationList *al);
Annotation *AnnotationStyleSemanticAction(char *t, Styles *s);
Styles *StylesSemanticAction(char *p, char *r, Styles *n);
StyleVariable *StyleVariableSemanticAction(char *name, Styles *s, StyleVariable *n);
Program *StructureProgramSemanticAction(CompilerState *compilerState, Structure *structure, Program *next);

#endif
