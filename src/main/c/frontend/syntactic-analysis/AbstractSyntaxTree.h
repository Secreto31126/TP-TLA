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
typedef struct Program Program;

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

typedef struct StructureType
{
	int n;
} StructureType;

typedef struct Structure
{
	StyleVariable *variables;
	AnnotationList *annotations;
	StructureType *type;
	char order;
	Cells *cells;
} Structure;

struct Program
{
	Structure *structure;
	struct Program *program_next;
};

/**
 * Node recursive destructors.
 */
void releaseProgram(Program *program);

#endif
