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

structure: set_style_variable[v] annotations[a] structure_type[t] COLON OPEN_BRACES cells[cells] CLOSE_BRACES SEMICOLON		{ $$ = NULL; } // { $$ = StructureSemanticAction($t, $cells, $v, $a); }
	| set_style_variable[v] annotations[a] structure_type[t] COLON OPEN_BRACKETS cells[cells] CLOSE_BRACKETS SEMICOLON		{ $$ = NULL; } // { $$ = StructureSemanticAction($t, $cells, $v, $a); }
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

cells: cell_value[value]								{ $$ = NULL; } // { $$ = CellsSemanticAction($value, null, null); }
	| cell_value[value] COMMA cells[next]				{ $$ = NULL; } // { $$ = CellsSemanticAction($value, null, $next); }
	| LABEL[label] COLON cell_value						{ $$ = NULL; } // { $$ = CellsSemanticAction($value, $label, null); }
	| LABEL[label] COLON cell_value COMMA cells[next]	{ $$ = NULL; } // { $$ = CellsSemanticAction($value, $label, $next); }
	| LABEL[label]										{ $$ = NULL; } // { $$ = CellsSemanticAction(null, $label, null); }
	| LABEL[label] COMMA cells[next]					{ $$ = NULL; } // { $$ = CellsSemanticAction(null, $label, $next); }
	;

cell_value: STRING[value]								{ $$ = NULL; } // { $$ = CellValueSemanticAction($value); }
	| OPEN_BRACES cells[cells] CLOSE_BRACES				{ $$ = NULL; } // { $$ = CellUnorderedSemanticAction($cells); }
	| OPEN_BRACKETS cells[cells] CLOSE_BRACKETS			{ $$ = NULL; } // { $$ = CellOrderedValueSemanticAction($cells); }
	;

annotations: default_annotation[a]					{ $$ = NULL; } // { $$ = AnnotationListSemanticAction($a, null); }
	| customize_annotation[a]						{ $$ = NULL; } // { $$ = AnnotationListSemanticAction($a, null); }
	| annotations[prev] customize_annotation[a] 	{ $$ = NULL; } // { $$ = AnnotationListSemanticAction($a, $prev); }
	| %empty										{ $$ = NULL; }
	;

/* A -> @d ( S ) */
default_annotation: DEFAULT_ANNOTATION OPEN_PARENTHESIS styles[s] CLOSE_PARENTHESIS	{ $$ = NULL; } // { $$ = DefaultStyleSemanticAction($s) }

customize_annotation: CUSTOMIZE_ANNOTATION OPEN_PARENTHESIS LABEL[t] COMMA styles[s] CLOSE_PARENTHESIS	{ $$ = NULL; } // { $$ = AnnotationStyleSemanticAction($t, $s) }

styles: LABEL[property] OPEN_PARENTHESIS LABEL[rule] CLOSE_PARENTHESIS				{ $$ = NULL; } // { $$ = StyleSemanticAction($property, $rule, null); }
	| LABEL[property] OPEN_PARENTHESIS LABEL[rule] CLOSE_PARENTHESIS styles[next]	{ $$ = NULL; } // { $$ = StyleSemanticAction($property, $rule, $next); }
	| STYLE_VARIABLE																{ $$ = NULL; } // { $$ = StyleSemanticAction("$", $rule + 1, null); }
	| STYLE_VARIABLE styles[next]													{ $$ = NULL; } // { $$ = StyleSemanticAction("$", $rule + 1, $next); }
	;

set_style_variable: STYLE_VARIABLE[name] COLON styles[s] SEMICOLON set_style_variable[next]		{ $$ = NULL; } // { $$ = StyleVariableSemanticAction($name, $s, $next); }
	| %empty																					{ $$ = NULL; }
	;

%%
