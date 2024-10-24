#ifndef ABSTRACT_SYNTAX_TREE_HEADER
#define ABSTRACT_SYNTAX_TREE_HEADER

#include "../../shared/Logger.h"
#include <stdlib.h>

/** Initialize module's internal state. */
void initializeAbstractSyntaxTreeModule();

/** Shutdown module's internal state. */
void shutdownAbstractSyntaxTreeModule();

/**
 * This typedefs allows self-referencing types.
 */

typedef enum ExpressionType ExpressionType;
typedef enum FactorType FactorType;

typedef struct Constant Constant;
typedef struct Expression Expression;
typedef struct Factor Factor;

/**
 * Node types for the Abstract Syntax Tree (AST).
 */

enum ExpressionType
{
	ADDITION,
	DIVISION,
	FACTOR,
	MULTIPLICATION,
	SUBTRACTION
};

enum FactorType
{
	CONSTANT,
	EXPRESSION
};

struct Constant
{
	int value;
};

struct Factor
{
	union
	{
		Constant *constant;
		Expression *expression;
	};
	FactorType type;
};

struct Expression
{
	union
	{
		Factor *factor;
		struct
		{
			Expression *leftExpression;
			Expression *rightExpression;
		};
	};
	ExpressionType type;
};

// TODO: Remove everything above after backend is created

typedef enum CellType
{
	CELL_FINAL = 'f',
	CELL_ORDERED = 'o',
	CELL_UNORDERED = 'u'
} CellType;

typedef struct StyleVariable
{
	char *name;
	struct Styles *styles;
	struct StyleVariable *next;
} StyleVariable;

typedef struct Styles
{
	char *property;
	char *rule;
	struct Styles *next;
} Styles;

typedef struct Annotation
{
	char *target;
	Styles *style;
} Annotation;

typedef struct AnnotationList
{
	Annotation *value;
	struct AnnotationList *next;
} AnnotationList;

typedef struct CellValue
{
	CellType type;
	union
	{
		char *value;
		struct Cells *cells;
	};
} CellValue;

typedef struct Cells
{
	char *label;
	CellValue *value;
	struct Cells *next;
} Cells;

typedef enum StructureType
{
	STRUCTURE_ARRAY = 0,
	STRUCTURE_LIST,
	STRUCTURE_LINKED_LIST,
	STRUCTURE_DOUBLE_LINKED_LIST,
	STRUCTURE_TREE,
	STRUCTURE_GRAPH,
	STRUCTURE_DIRECTED_GRAPH,
	STRUCTURE_TABLE
} StructureType;

typedef struct Structure
{
	StyleVariable *variables;
	AnnotationList *annotations;
	StructureType type;
	CellType order;
	Cells *cells;
	struct Structure *next;
} Structure;

typedef struct Program
{
	Structure *structure;
} Program;

/**
 * Node recursive destructors.
 */
void releaseStyles(Styles *styles);
void releaseStyleVariable(StyleVariable *styleVariable);
void releaseAnnotation(Annotation *annotation);
void releaseAnnotationList(AnnotationList *annotationList);
void releaseCellValue(CellValue *cellValue);
void releaseCells(Cells *cells);
void releaseStructure(Structure *structure);
void releaseProgram(Program *program);

#endif
