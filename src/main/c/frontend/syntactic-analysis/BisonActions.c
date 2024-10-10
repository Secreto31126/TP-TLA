#include "BisonActions.h"

/* MODULE INTERNAL STATE */

static Logger *_logger = NULL;

void initializeBisonActionsModule()
{
	_logger = createLogger("BisonActions");
}

void shutdownBisonActionsModule()
{
	if (_logger != NULL)
	{
		destroyLogger(_logger);
	}
}

/** IMPORTED FUNCTIONS */

extern unsigned int flexCurrentContext(void);

/* PRIVATE FUNCTIONS */

static void _logSyntacticAnalyzerAction(const char *functionName);

/**
 * Logs a syntactic-analyzer action in DEBUGGING level.
 */
static void _logSyntacticAnalyzerAction(const char *functionName)
{
	logDebugging(_logger, "%s", functionName);
}

/* PUBLIC FUNCTIONS */

StructureType* StructureTypeSemanticAction(Token st){
	_logSyntacticAnalyzerAction(__FUNCTION__);
	StructureType* structureType = calloc(1, sizeof(StructureType));
	structureType->n=st;
	return structureType;
}

Cells* CellsSemanticAction(CellValue* cv, char* l , Cells* c){
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Cells* cell = calloc(1, sizeof(Cells));
	cell->value = cv;
	cell->label = l;
	cell->next = c;
	return cell;
}

CellValue* CellValueSemanticAction(char* v){
	_logSyntacticAnalyzerAction(__FUNCTION__);
	CellValue* cellValue = calloc(1, sizeof(CellValue));
	cellValue->value=v;
	cellValue->type='f';
	return cellValue;
}

CellValue* CellUnorderedSemanticAction(Cells* c){
	_logSyntacticAnalyzerAction(__FUNCTION__);
	CellValue* cellValue = calloc(1, sizeof(CellValue));
	cellValue->type='u';
	cellValue->cells=c;
	return cellValue;
}

CellValue* CellOrderedValueSemanticAction(Cells* c){
	_logSyntacticAnalyzerAction(__FUNCTION__);
	CellValue* cellValue = calloc(1, sizeof(CellValue));
	cellValue->type='o';
	cellValue->cells=c;
	return cellValue;
}

AnnotationList* AnnotationListSemanticAction(Annotation* a, AnnotationList* al){
	_logSyntacticAnalyzerAction(__FUNCTION__);
	AnnotationList* annotationList = calloc(1, sizeof(AnnotationList));
	annotationList->value=a;
	annotationList->next=al;
	return annotationList;
}

Annotation* AnnotationStyleSemanticAction(char* t, Styles* s){
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Annotation* annotation = calloc(1, sizeof(Annotation));
	annotation->style=s;
	annotation->target=t;
	return annotation;
}

Styles* StylesSemanticAction(char* p, char* r, Styles* n){
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Styles* style = calloc(1, sizeof(Styles));
	style->property=p;
	style->rule=r;
	style->next=n;
	return style;
}

StyleVariable* StyleVariableSemanticAction(char* name, Styles* s, StyleVariable* n){
	_logSyntacticAnalyzerAction(__FUNCTION__);
	StyleVariable* styleVar = calloc(1, sizeof(StyleVariable));
	styleVar->name=name;
	styleVar->styles=s;
	styleVar->next=n;
	return styleVar;
}

Program *StructureProgramSemanticAction(CompilerState *compilerState, Structure *structure)
{
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Program *program = calloc(1, sizeof(Program));
	program->structure = structure;
	compilerState->abstractSyntaxtTree = program;
	if (0 < flexCurrentContext())
	{
		logError(_logger, "The final context is not the default (0): %d", flexCurrentContext());
		compilerState->succeed = false;
	}
	else
	{
		compilerState->succeed = true;
	}
	return program;
}
