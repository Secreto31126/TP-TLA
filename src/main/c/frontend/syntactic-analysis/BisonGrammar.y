%{

#include "BisonActions.h"

%}

// You touch this, and you die.
%define api.value.union.name SemanticValue

%union {
	/** Terminals. */

	char *string;
	Token token;

	/** Non-terminals. */

	Program *program;

	// typedef struct Structure { StyleVariable *variables; AnnotationList *annotations; StructureType *type; char order; Cells *cells; } Structure;
	Structure *structure;

	// typedef struct StructureType { Token *name; } StructureType;
	StructureType *type;

	// typedef struct { CellValue *value; char *label; Cells *next; } Cells;
	Cells *cells;

	// typedef { char type; union { char *value; Cells *cells; }; } CellValue;
	// type == 'f' => char *value (final)
	// type == 'o' => Cells *cells (ordered [])
	// type == 'u' => Cells *cells (unordered {})
	CellValue *cell_value;

	// typedef struct AnnotationList { Annotation *head; } AnnotationList;
	AnnotationList *annotations;

	// typedef struct Annotation { char *target; Styles *style; Annotation *prev; } Annotation;
	// @Default(background(color)) => {null, {"background", "color"}, null}
	// @Customize(target, background(color)) => {"target", {"background", "color"}, (Annotation * | null)}
	Annotation *annotation;

	// typedef struct Styles { char *property; char *rule; Styles *next; } Styles;
	// "background(color)" => {"background", "color"}
	// "$my-rule" => {"$", "my-rule"}
	Styles *styles;

	// typedef struct StyleVariable { char *name; Styles *styles; StyleVariable *next; } StyleVariable;
	// Not to be confused with Styles that may contain a _reference_ to a StyleVariable _name_
	StyleVariable *style_variable;
}

/**
 * Destructors. This functions are executed after the parsing ends, so if the
 * AST must be used in the following phases of the compiler you shouldn't used
 * this approach. To use this mechanism, the AST must be translated into
 * another structure.
 *
 * @see https://www.gnu.org/software/bison/manual/html_node/Destructor-Decl.html
 */
/*
%destructor { releaseConstant($$); } <constant>
%destructor { releaseExpression($$); } <expression>
%destructor { releaseFactor($$); } <factor>
%destructor { releaseProgram($$); } <program>
*/

/** Terminals. */
%token <string> STRING
%token <string> LABEL
%token <string> STYLE_VARIABLE

%token <token> OPEN_PARENTHESIS
%token <token> CLOSE_PARENTHESIS
%token <token> OPEN_BRACES
%token <token> CLOSE_BRACES
%token <token> OPEN_BRACKETS
%token <token> CLOSE_BRACKETS

%token <token> COMMA
%token <token> COLON
%token <token> SEMICOLON

%token <token> DEFAULT_ANNOTATION
%token <token> CUSTOMIZE_ANNOTATION

%token <token> ARRAY
%token <token> LIST
%token <token> LINKED_LIST
%token <token> DOUBLE_LINKED_LIST
%token <token> TREE
%token <token> GRAPH
%token <token> DIRECTED_GRAPH
%token <token> TABLE

%token <token> UNKNOWN

/** Non-terminals. */
%type <program> program

%type <structure> structure
%type <annotations> annotations
%type <type> structure_type

%type <styles> styles
%type <style_variable> set_style_variable

%type <annotation> default_annotation
%type <annotation> customize_annotation

%type <cell_value> cell_value
%type <cells> cells

/**
 * Precedence and associativity.
 *
 * @see https://www.gnu.org/software/bison/manual/html_node/Precedence.html
 */
// %left ADD SUB
// %left MUL DIV

%%

// IMPORTANT: To use λ in the following grammar, use the %empty symbol.

program: structure													{ $$ = NULL; } // { $$ = StructureProgramSemanticAction(currentCompilerState(), $1); }
	;

structure: set_style_variable[v0] annotations[a0] structure_type[t0] COLON OPEN_BRACES cells[c0] CLOSE_BRACES SEMICOLON		{ $$ = NULL; } // { $$ = StructureSemanticAction($t0, $c0, $v0, $a0); }
	| set_style_variable[v1] annotations[a1] structure_type[t1] COLON OPEN_BRACKETS cells[c1] CLOSE_BRACKETS SEMICOLON		{ $$ = NULL; } // { $$ = StructureSemanticAction($t1, $c1, $v1, $a1); }
	;

structure_type: ARRAY							{ $$ = NULL; } // { $$ = StructureTypeSemanticAction($1); }
	| LIST										{ $$ = NULL; } // { $$ = StructureTypeSemanticAction($1); }
	| LINKED_LIST								{ $$ = NULL; } // { $$ = StructureTypeSemanticAction($1); }
	| DOUBLE_LINKED_LIST						{ $$ = NULL; } // { $$ = StructureTypeSemanticAction($1); }
	| TREE										{ $$ = NULL; } // { $$ = StructureTypeSemanticAction($1); }
	| GRAPH										{ $$ = NULL; } // { $$ = StructureTypeSemanticAction($1); }
	| DIRECTED_GRAPH							{ $$ = NULL; } // { $$ = StructureTypeSemanticAction($1); }
	| TABLE										{ $$ = NULL; } // { $$ = StructureTypeSemanticAction($1); }
	;

cells: cell_value[v0]										{ $$ = NULL; } // { $$ = CellsSemanticAction($v0, null, null); }
	| cell_value[v1] COMMA cells[n1]						{ $$ = NULL; } // { $$ = CellsSemanticAction($v1, null, $n1); }
	| LABEL[l2] COLON cell_value[v2]						{ $$ = NULL; } // { $$ = CellsSemanticAction($v2, $l2, null); }
	| LABEL[l3] COLON cell_value[v3] COMMA cells[n3]		{ $$ = NULL; } // { $$ = CellsSemanticAction($v3, $l3, $n3); }
	/* This 2 ugly guys below are because DirectedGraph contains labels to the nodes it targets */
	| LABEL[l4]												{ $$ = NULL; } // { $$ = CellsSemanticAction(null, $l4, null); }
	| LABEL[l5] COMMA cells[n5]								{ $$ = NULL; } // { $$ = CellsSemanticAction(null, $l5, $n5); }
	;

cell_value: STRING[v0]										{ $$ = NULL; } // { $$ = CellValueSemanticAction($v0); }
	| OPEN_BRACES cells[c1] CLOSE_BRACES					{ $$ = NULL; } // { $$ = CellUnorderedSemanticAction($c1); }
	| OPEN_BRACKETS cells[c2] CLOSE_BRACKETS				{ $$ = NULL; } // { $$ = CellOrderedValueSemanticAction($c2); }
	;

annotations: default_annotation[a0]					{ $$ = NULL; } // { $$ = AnnotationListSemanticAction($a0, null); }
	| customize_annotation[a1]						{ $$ = NULL; } // { $$ = AnnotationListSemanticAction($a1, null); }
	| annotations[p2] customize_annotation[a2] 		{ $$ = NULL; } // { $$ = AnnotationListSemanticAction($a2, $p2); }
	| %empty										{ $$ = NULL; }
	;

/* A -> @d ( S ) */
default_annotation: DEFAULT_ANNOTATION OPEN_PARENTHESIS styles[s0] CLOSE_PARENTHESIS	{ $$ = NULL; } // { $$ = DefaultStyleSemanticAction($s0) }
	;

customize_annotation: CUSTOMIZE_ANNOTATION OPEN_PARENTHESIS LABEL[t0] COMMA styles[s0] CLOSE_PARENTHESIS	{ $$ = NULL; } // { $$ = AnnotationStyleSemanticAction($t0, $s0) }
	;

styles: LABEL[p0] OPEN_PARENTHESIS LABEL[r0] CLOSE_PARENTHESIS				{ $$ = NULL; } // { $$ = StyleSemanticAction($p0, $r0, null); }
	| LABEL[p1] OPEN_PARENTHESIS LABEL[r1] CLOSE_PARENTHESIS styles[n1]		{ $$ = NULL; } // { $$ = StyleSemanticAction($p1, $r1, $n1); }
	| STYLE_VARIABLE[v2]													{ $$ = NULL; } // { $$ = StyleSemanticAction("$", $v2 + 1, null); }
	| STYLE_VARIABLE[v3] styles[n3]											{ $$ = NULL; } // { $$ = StyleSemanticAction("$", $v3 + 1, $n3); }
	;

set_style_variable: STYLE_VARIABLE[n0] COLON styles[s0] SEMICOLON set_style_variable[n0]		{ $$ = NULL; } // { $$ = StyleVariableSemanticAction($n0, $s0, $n0); }
	| %empty																					{ $$ = NULL; }
	;

%%
