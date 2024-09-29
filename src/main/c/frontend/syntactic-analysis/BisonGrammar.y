%{

#include "BisonActions.h"

%}

// You touch this, and you die.
%define api.value.union.name SemanticValue

%union {
	/** Terminals. */

	int integer;
	char * string;
	Token token;

	/** Non-terminals. */

	Constant * constant;
	Expression * expression;
	Factor * factor;
	Program * program;

	StructureType type;
	Structure * structure;
	InnerStructure * inner_structure;
	Target * target;
	Property * property;
	Rule * rule;
	// typedef struct { Style * list }
	Styles * styles;
	// typedef struct { Property *property, Rule *rule } Style;
	// "background(color)" => {"background", "color"}
	// "$my-rule" => {"$", "my-rule"}
	Style * style;
	StyleVariable * style_variable;
	Annotations * annotations;
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
%token <integer> INTEGER
%token <token> ADD
%token <token> DIV
%token <token> MUL
%token <token> SUB

%token <string> STRING

%token <token> OPEN_PARENTHESIS
%token <token> CLOSE_PARENTHESIS
%token <token> OPEN_BRACES
%token <token> CLOSE_BRACES
%token <token> OPEN_BRACKETS
%token <token> CLOSE_BRACKETS

%token <token> COMMA
%token <token> COLON
%token <token> SEMICOLON
%token <token> STYLE_VARIABLE
%token <token> LABEL

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
%type <constant> constant
%type <expression> expression
%type <factor> factor
%type <program> program

%type <structure> structure
%type <annotations> annotations
%type <type> structure_type

%type <target> target
%type <property> property
%type <rule> rule
%type <styles> styles
%type <style> style
%type <style_variable> style_set_variable

/**
 * Precedence and associativity.
 *
 * @see https://www.gnu.org/software/bison/manual/html_node/Precedence.html
 */
%left ADD SUB
%left MUL DIV

%%

// IMPORTANT: To use λ in the following grammar, use the %empty symbol.

program: expression													{ $$ = ExpressionProgramSemanticAction(currentCompilerState(), $1); }
	;

expression: expression[left] ADD expression[right]					{ $$ = ArithmeticExpressionSemanticAction($left, $right, ADDITION); }
	| expression[left] DIV expression[right]						{ $$ = ArithmeticExpressionSemanticAction($left, $right, DIVISION); }
	| expression[left] MUL expression[right]						{ $$ = ArithmeticExpressionSemanticAction($left, $right, MULTIPLICATION); }
	| expression[left] SUB expression[right]						{ $$ = ArithmeticExpressionSemanticAction($left, $right, SUBTRACTION); }
	| factor														{ $$ = FactorExpressionSemanticAction($1); }
	;

factor: OPEN_PARENTHESIS expression CLOSE_PARENTHESIS				{ $$ = ExpressionFactorSemanticAction($2); }
	| constant														{ $$ = ConstantFactorSemanticAction($1); }
	;

constant: INTEGER													{ $$ = IntegerConstantSemanticAction($1); }
	;

target: UNKNOWN														{ $$ = NULL; }
	;

property: UNKNOWN													{ $$ = NULL; }
	;

rule: UNKNOWN														{ $$ = NULL; }
	;

style: property[property] OPEN_PARENTHESIS rule[rule] CLOSE_PARENTHESIS				{ $$ = NULL; } // { $$ = StyleSemanticAction($property, $rule, null); }
	| STYLE_VARIABLE																{ $$ = NULL; } // { $$ = StyleSemanticAction("$", $rule + 1, null); }
	| property[property] OPEN_PARENTHESIS rule[rule] CLOSE_PARENTHESIS style[next]	{ $$ = NULL; } // { $$ = StyleSemanticAction($property, $rule, $next); }
	| STYLE_VARIABLE style[next]													{ $$ = NULL; } // { $$ = StyleSemanticAction("$", $rule + 1, $next); }
	;

styles: style[s]															{ $$ = NULL; }
	;

style_set_variable: STYLE_VARIABLE[name] COLON style[style] SEMICOLON style_set_variable[next]		{ $$ = NULL; } // { $$ = StyleVariableSemanticAction($name, $style, $next); }
	| %empty																						{ $$ = NULL; }
	;

/* A -> @d ( S ) */
annotations: DEFAULT_ANNOTATION OPEN_PARENTHESIS style[s] CLOSE_PARENTHESIS									{ $$ = NULL; } // { $$ = DefaultStyleSemanticAction($s); }
	| CUSTOMIZE_ANNOTATION OPEN_PARENTHESIS target[t] COMMA style[s] CLOSE_PARENTHESIS						{ $$ = NULL; } // { $$ = CustomizeStyleSemanticAction($t, $s, null); }
	| annotations[prev] CUSTOMIZE_ANNOTATION OPEN_PARENTHESIS target[t] COMMA style[s] CLOSE_PARENTHESIS 	{ $$ = NULL; } // { $$ = CustomizeStyleSemanticAction($t, $s, $prev); }
	| %empty																								{ $$ = NULL; }
	;

/* 
@Custom
@Default
@Custom
@Custom
 */

cell_value: STRING[value]								{ $$ = NULL; } // { $$ = CellValueSemanticAction($value); }
	| OPEN_BRACES cells[cells] CLOSE_BRACES				{ $$ = NULL; } // { $$ = CellUnorderedSemanticAction($cells); }
	| OPEN_BRACKETS cells[cells] CLOSE_BRACKETS			{ $$ = NULL; } // { $$ = CellOrderedValueSemanticAction($cells); }
	;

cells: cell_value[value]								{ $$ = NULL; } // { $$ = CellsSemanticAction($value, null, null); }
	| cell_value[value] COMMA cells[next]				{ $$ = NULL; } // { $$ = CellsSemanticAction($value, null, $next); }
	| LABEL[label] COLON cell_value						{ $$ = NULL; } // { $$ = CellsSemanticAction($value, $label, null); }
	| LABEL[label] COLON cell_value COMMA cells[next]	{ $$ = NULL; } // { $$ = CellsSemanticAction($value, $label, $next); }
	| LABEL[label]										{ $$ = NULL; } // { $$ = CellsSemanticAction(null, $label, null); }
	| LABEL[label] COMMA cells[next]					{ $$ = NULL; } // { $$ = CellsSemanticAction(null, $label, $next); }
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

structure: annotations[annotations] structure_type[type] COLON OPEN_BRACES cells[cells] CLOSE_BRACES SEMICOLON		{ $$ = NULL; } // { $$ = StructureSemanticAction($type, $cells, $annotations); }
	| annotations[annotations] structure_type COLON OPEN_BRACKETS cells[cells] CLOSE_BRACKETS SEMICOLON				{ $$ = NULL; } // { $$ = StructureSemanticAction($type, $cells, $annotations); }
	;

/* 
inner_ordered_structure: f OPEN_BRACES inner_structure CLOSE_BRACES COMMA inner_ordered_structure		{ $$ = NULL; }
	| f OPEN_BRACES inner_unordered_structure CLOSE_BRACES COMMA inner_unordered_structure							{ $$ = NULL; }
	;

ordered_structure: annotations OPEN_BRACES inner_structure CLOSE_BRACES SEMICOLON		{ $$ = NULL; } */
	;

/* node: UNKNOWN

inner_tree_structure: OPEN_BRACKETS f CLOSE_BRACKETS				{ $$ = NULL; }
	| OPEN_BRACKETS f CLOSE_BRACKETS COMMA inner_tree_structure	{ $$ = NULL; }
	;

tree_structure: annotations TREE OPEN_BRACES inner_tree_structure CLOSE_BRACES SEMICOLON		{ $$ = NULL; }
	; */

/* 
inner_brackets_structure: OPEN_BRACKETS inner_structure CLOSE_BRACKETS		{ $$ = NULL; }
	;

braces_structure: annotations DOUBLE_LINKED_LIST OPEN_BRACES inner_braces_structure CLOSE_BRACES SEMICOLON	{ $$ = NULL; }
	;

brackets_structure: annotations OPEN_BRACKETS inner_structure CLOSE_BRACKETS SEMICOLON	{ $$ = NULL; } */

%%
