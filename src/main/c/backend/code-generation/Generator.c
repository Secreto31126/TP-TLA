#include "Generator.h"
#include <stdbool.h>

#include "../domain-specific/Validator.h"

#define _getDefaultProperties(styles, p) _getStyleProperties(styles, p, false, false, false)
#define _getCustomProperties(styles, p) _getStyleProperties(styles, p, true, true, true)

typedef struct properties
{
	struct
	{
		char *value;
		bool modified;
	} color;
	struct
	{
		char *value;
		bool modified;
	} fontsize;
	struct
	{
		char *value;
		bool modified;
	} style;
} properties;

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

static void _getStyleProperties(const Styles *styles, properties *p, bool overrideColor, bool overrideFontsize, bool overrideStyle)
{
	const Styles *current = styles;
	while (current)
	{
		switch (current->property)
		{
		case PROPERTY_COLOR:
			if (overrideColor || !p->color.modified)
			{
				p->color.value = current->rule;
				overrideColor = true;
			}
			p->color.modified = true;
			break;
		case PROPERTY_SIZE:
			if (overrideFontsize || !p->fontsize.modified)
			{
				p->fontsize.value = current->rule;
				overrideFontsize = true;
			}
			p->fontsize.modified = true;
			break;
		case PROPERTY_BORDER:
			if (overrideStyle || !p->style.modified)
			{
				p->style.value = current->rule;
				overrideStyle = true;
			}
			p->style.modified = true;
			break;
		case PROPERTY_VARIABLE:
			const StyleVariable *variable = getStyleVariableByReference(current->rule);
			if (variable)
				_getStyleProperties(variable->styles, p, overrideColor, overrideFontsize, overrideStyle);
		}

		current = current->next;
	}
}

static properties _getProperties(const Structure *structure, const Cells *cell)
{
	properties p = {0};
	p.color.value = "black";
	p.fontsize.value = "11";
	p.style.value = "solid";

	if(structure == NULL)
	{
		return p;
	}

	AnnotationList *annotationList = structure->annotations;
	while (annotationList)
	{
		if (!annotationList->value->target)
		{
			_getDefaultProperties(annotationList->value->style, &p);
		}
		else if (cell->label)
		{
			Annotation *annotation = annotationList->value;

			if (strcmp(annotation->target, cell->label) == 0)
			{
				_getCustomProperties(annotation->style, &p);
			}
		}

		annotationList = annotationList->next;
	}

	return p;
}

static void _generateTreeNodes(Structure *tree, Cells *treeCell, unsigned int *n, bool big_brother)
{
	const unsigned int id = *n;
	(*n)++;

	properties p = _getProperties(tree, treeCell);

	_output(1, "node%d [label=\"%s\" color=%s fontsize=%s style=%s]\n", id, treeCell->value->value, p.color.value, p.fontsize.value, p.style.value);

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

static void _generateGraphNodes(Structure *graph, Cells *graphCell, bool directed)
{
	if (!graphCell)
	{
		return;
	}

	properties p = _getProperties(graph, graphCell);

	if (graphCell->value->type == CELL_FINAL)
	{
		_output(1, "node%s [label=\"%s\" color=%s fontsize=%s style=%s]\n", graphCell->label, graphCell->value->value, p.color.value, p.fontsize.value, p.style.value);
		_generateGraphNodes(graph, graphCell->next, directed);
		return;
	}

	char *label = graphCell->value->cells->value->value;
	_output(1, "node%s [label=\"%s\" color=%s fontsize=%s style=%s]\n", graphCell->label, label, p.color.value, p.fontsize.value, p.style.value);

	Cells *child = graphCell->value->cells->next;
	while (child)
	{
		if (!child->label)
		{
			child = child->next;
			continue;
		}

		_output(1, "node%s -%s node%s\n", graphCell->label, directed ? ">" : "-", child->label);

		if (child->value && child->value->type != CELL_FINAL)
		{
			_generateGraphNodes(graph, child, directed);
		}

		child = child->next;
	}

	_generateGraphNodes(graph, graphCell->next, directed);
}

static void _generateGraph(Structure *graph)
{
	bool directed = graph->type == STRUCTURE_DIRECTED_GRAPH;
	_output(0, "%sgraph AnyGraph {\n", directed ? "di" : "");
	_generateGraphNodes(graph, graph->cells, directed);
	_output(0, "}\n");
}

static void _generateList(Structure *list)
{
	bool doubled = list->type == STRUCTURE_DOUBLE_LINKED_LIST;
	bool linked = doubled || list->type == STRUCTURE_LINKED_LIST;

	_output(0, "digraph %s%sList {\n", doubled ? "Double" : "", linked ? "Linked" : "");
	_output(1, "rankdir=LR\n");
	_output(1, "edge [dir=%s]\n", doubled ? "both" : linked ? "forward"
															: "none");

	size_t n = 0;
	Cells *current = list->cells;
	while (current)
	{
		properties p = _getProperties(list, current);

		size_t id = n++;
		_output(1, "node%d [label=\"%s\" color=%s fontsize=%s style=%s]\n", id, current->value->value, p.color.value, p.fontsize.value, p.style.value);

		if (current->next)
		{
			_output(1, "node%d -> node%d\n", id, n);
		}

		current = current->next;
	}

	_output(0, "}\n");
}

static void _generateRow(Structure *row, Cells *cell, int column)
{
	_output(3, "<tr>\n");

	properties defaults = _getProperties(NULL, NULL);

	Cells *current = cell;
	for(int i = 0;  (column == -1 && current) || i < column; i++){
		if(current)
		{
			properties p = _getProperties(row, current);
			_output(4, "<td color=\"%s\" style=\"%s\"><font point-size=\"%s\">%s</font></td>\n", p.color.value, p.style.value, p.fontsize.value, current->value->value);
			current = current->next;
		}
		else
		{
			_output(4, "<td color=\"%s\" style=\"%s\"><font point-size=\"%s\"> </font></td>\n",  defaults.color.value, defaults.style.value, defaults.fontsize.value);
		}
	}

	_output(3, "</tr>\n");
}

static int _getMaxRows(Structure *structure)
{
	int max = 0;
	Cells *current = structure->cells;
	while(current)
	{
		int i = 0;
		Cells *cell = current->value->cells;
		while(cell)
		{
			i++;
			cell = cell->next;
		}
		if(i > max)
		{
			max = i;
		}
		current = current->next;
	}

	return max;

}

static void _generateArray(Structure *array)
{
	_output(0, "digraph Array {\n");
	_output(1, "node [shape=plaintext]\n");
	_output(1, "array [label=<\n");
	_output(2, "<table border=\"1\" cellborder=\"1\" cellpadding=\"5\" cellspacing=\"2\">\n");

	_generateRow(array, array->cells, -1);

	_output(2, "</table>\n");
	_output(1, ">]\n");
	_output(0, "}\n");

}


static void _generateTable(Structure *table)
{
	_output(0, "digraph Table {\n");
	_output(1, "node [shape=plaintext]\n");
	_output(1, "table [label=<\n");
	_output(1, "<table border=\"1\" cellborder=\"1\" cellpadding=\"5\" cellspacing=\"2\">\n");

	int max = _getMaxRows(table);

	Cells *current = table->cells;
	while(current)
	{
		_generateRow(table,current->value->cells, max);
		current = current->next;
	}

	_output(2, "</table>\n");
	_output(1, ">]\n");
	_output(0, "}\n");
}


static void _generateStructure(Structure *structure)
{
	if (!structure)
	{
		return;
	}

	switch (structure->type)
	{
	case STRUCTURE_TREE:
		_generateTree(structure);
		break;
	case STRUCTURE_LIST:
	case STRUCTURE_LINKED_LIST:
	case STRUCTURE_DOUBLE_LINKED_LIST:
		_generateList(structure);
		break;
	case STRUCTURE_GRAPH:
	case STRUCTURE_DIRECTED_GRAPH:
		_generateGraph(structure);
		break;
	case STRUCTURE_ARRAY:
		_generateArray(structure);
		break;
	case STRUCTURE_TABLE:
		_generateTable(structure);
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
