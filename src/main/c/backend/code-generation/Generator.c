#include "Generator.h"
#include <stdbool.h>

#include "../domain-specific/Validator.h"

/* MODULE INTERNAL STATE */

const char _indentationCharacter = ' ';
const char _indentationSize = 4;
static Logger *_logger = NULL;

void initializeGeneratorModule()
{
	_logger = createLogger("Generator");
}

void shutdownGeneratorModule()
{
	if (_logger != NULL)
	{
		destroyLogger(_logger);
	}
}

/** PRIVATE FUNCTIONS */

static void _generateEpilogue(const int value);
static void _generateProgram(Program *program);
static void _generatePrologue(void);
static char *_indentation(const unsigned int indentationLevel);
static void _output(const unsigned int indentationLevel, const char *const format, ...);

static void _getStyleProperties(const Styles *styles, char **color, char **fontsize, char **style, bool *colorModified, bool *fontsizeModified, bool *styleModified, bool override)
{
	const Styles *current = styles;
	while (current)
	{
		switch (current->property)
		{
		case PROPERTY_COLOR:
			if (override || !*colorModified)
				*color = current->rule;
			*colorModified = true;
			break;
		case PROPERTY_SIZE:
			if (override || !*fontsizeModified)
				*fontsize = current->rule;
			*fontsizeModified = true;
			break;
		case PROPERTY_BORDER:
			if (override || !*styleModified)
				*style = current->rule;
			*styleModified = true;
			break;
		case PROPERTY_VARIABLE:
			const StyleVariable *variable = getStyleVariableByReference(current->rule);
			if (variable)
				_getStyleProperties(variable->styles, color, fontsize, style, colorModified, fontsizeModified, styleModified, override);
		}

		current = current->next;
	}
}

static void _generateTreeNodes(Structure *tree, Cells *treeCell, unsigned int *n, bool big_brother)
{
	const unsigned int id = *n;
	(*n)++;
	char *color = "black";
	char *fontsize = "11";
	char *style = "solid";

	bool colorModified = false;
	bool fontsizeModified = false;
	bool styleModified = false;

	AnnotationList *annotationList = tree->annotations;
	while (annotationList)
	{
		if (!annotationList->value->target)
		{
			_getStyleProperties(annotationList->value->style, &color, &fontsize, &style, &colorModified, &fontsizeModified, &styleModified, false);
		}
		else if (treeCell->label)
		{
			Annotation *annotation = annotationList->value;

			if (strcmp(annotation->target, treeCell->label) == 0)
			{
				_getStyleProperties(annotation->style, &color, &fontsize, &style, &colorModified, &fontsizeModified, &styleModified, true);
			}
		}

		annotationList = annotationList->next;
	}

	_output(1, "node%d [label=\"%s\" color=%s fontsize=%s style=%s]\n", id, treeCell->value->value, color, fontsize, style);

	if (!big_brother)
	{
		return;
	}

	Cells *current = treeCell->next;
	while (current)
	{
		_output(1, "node%d -> node%d\n", id, *n);
		bool is_final = current->value->type == CELL_FINAL;
		_generateTreeNodes(tree, is_final ? current : current->value->cells, n, !is_final);
		current = current->next;
	}
}

static void _generateTree(Structure *tree)
{
	unsigned int n = 0;
	_output(0, "digraph Tree {\n");
	_generateTreeNodes(tree, tree->cells, &n, true);
	_output(0, "}\n");
}

static void _generateStructure(Structure *structure)
{
	switch (structure->type)
	{
	case STRUCTURE_TREE:
		_generateTree(structure);
		break;
	}
}

/**
 * Creates the epilogue of the generated output, that is, the final lines that
 * completes a valid Latex document.
 */
static void _generateEpilogue(const int value)
{
}

/**
 * Generates the output of an expression.
 */
/**
 * Generates the output of the program.
 */
static void _generateProgram(Program *program)
{
	_generateStructure(program->structure);
}

/**
 * Creates the prologue of the generated output, a Latex document that renders
 * a tree thanks to the Forest package.
 *
 * @see https://ctan.dcc.uchile.cl/graphics/pgf/contrib/forest/forest-doc.pdf
 */
static void _generatePrologue(void)
{
	return;
	_output(0, "%s",
			"\\documentclass{standalone}\n\n"
			"\\usepackage[utf8]{inputenc}\n"
			"\\usepackage[T1]{fontenc}\n"
			"\\usepackage{amsmath}\n"
			"\\usepackage{forest}\n"
			"\\usepackage{microtype}\n\n"
			"\\begin{document}\n"
			"    \\centering\n"
			"    \\begin{forest}\n"
			"        [ \\text{$=$}, circle, draw, purple\n");
}

/**
 * Generates an indentation string for the specified level.
 */
static char *_indentation(const unsigned int level)
{
	return indentation(_indentationCharacter, level, _indentationSize);
}

/**
 * Outputs a formatted string to standard output. The "fflush" instruction
 * allows to see the output even close to a failure, because it drops the
 * buffering.
 */
static void _output(const unsigned int indentationLevel, const char *const format, ...)
{
	va_list arguments;
	va_start(arguments, format);
	char *indentation = _indentation(indentationLevel);
	char *effectiveFormat = concatenate(2, indentation, format);
	vfprintf(stdout, effectiveFormat, arguments);
	fflush(stdout);
	free(effectiveFormat);
	free(indentation);
	va_end(arguments);
}

/** PUBLIC FUNCTIONS */

void generate(CompilerState *compilerState)
{
	logDebugging(_logger, "Generating final output...");
	_generatePrologue();
	_generateProgram(compilerState->abstractSyntaxtTree);
	_generateEpilogue(compilerState->value);
	logDebugging(_logger, "Generation is done.");
}
