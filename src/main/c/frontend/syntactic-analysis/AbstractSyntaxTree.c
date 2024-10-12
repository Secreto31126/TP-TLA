#include "AbstractSyntaxTree.h"

/* MODULE INTERNAL STATE */

static Logger *_logger = NULL;

void initializeAbstractSyntaxTreeModule()
{
	_logger = createLogger("AbstractSyntxTree");
}

void shutdownAbstractSyntaxTreeModule()
{
	if (_logger != NULL)
	{
		destroyLogger(_logger);
	}
}

/** PUBLIC FUNCTIONS */

void releaseStyles(Styles *styles)
{
	if (!styles)
	{
		return;
	}

	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);

	releaseStyles(styles->next);

	// This is unfortunate colateral damage from the Bison design.
	if (*styles->property == '$')
	{
		free(styles->rule - 1);
	}
	else
	{
		free(styles->property);
		free(styles->rule);
	}

	free(styles);
}

void releaseStyleVariable(StyleVariable *styleVariable)
{
	if (!styleVariable)
	{
		return;
	}

	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);

	releaseStyleVariable(styleVariable->next);
	releaseStyles(styleVariable->styles);
	free(styleVariable->name);
	free(styleVariable);
}

void releaseAnnotation(Annotation *annotation)
{
	if (!annotation)
	{
		return;
	}

	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);

	releaseStyles(annotation->style);
	free(annotation->target);
	free(annotation);
}

void releaseAnnotationList(AnnotationList *annotationList)
{
	if (!annotationList)
	{
		return;
	}

	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);

	releaseAnnotationList(annotationList->next);
	releaseAnnotation(annotationList->value);
	free(annotationList);
}

void releaseCellValue(CellValue *cellValue)
{
	if (!cellValue)
	{
		return;
	}

	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);

	if (cellValue->type == CELL_FINAL)
	{
		free(cellValue->value);
	}
	else
	{
		releaseCells(cellValue->cells);
	}

	free(cellValue);
}

void releaseCells(Cells *cells)
{
	if (!cells)
	{
		return;
	}

	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);

	releaseCells(cells->next);
	releaseCellValue(cells->value);
	free(cells->label);
	free(cells);
}

void releaseStructure(Structure *structure)
{
	if (!structure)
	{
		return;
	}

	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);

	releaseStructure(structure->next);
	releaseStyleVariable(structure->variables);
	releaseAnnotationList(structure->annotations);
	releaseCells(structure->cells);
	free(structure);
}

void releaseProgram(Program *program)
{
	if (!program)
	{
		return;
	}

	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);

	releaseStructure(program->structure);
	free(program);
}
