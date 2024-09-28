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

	Structure * structure;
	InnerStructure * inner_structure;
	Target * target;
	Property * property;
	Rule * rule;
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

%token <token> DEFAULT_ANNOTATION
%token <token> CUSTOMIZE_ANNOTATION

%token <token> UNKNOWN

/** Non-terminals. */
%type <constant> constant
%type <expression> expression
%type <factor> factor
%type <program> program

%type <structure> structure
%type <inner_structure> inner_structure
%type <target> target
%type <property> property
%type <rule> rule
%type <style> style
%type <style_variable> style_set_variable
%type <annotations> annotations

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

style: property[property] OPEN_PARENTHESIS rule[rule] CLOSE_PARENTHESIS		{ $$ = NULL; } // { $$ = StyleSemanticAction($property, $rule); }
	| STYLE_VARIABLE														{ $$ = NULL; } // { $$ = StyleSemanticAction("$", $rule + 1); }
	| style style															{ $$ = NULL; } // ?
	;

style_set_variable: style_set_variable STYLE_VARIABLE COLON style SEMICOLON			{ $$ = NULL; }
	| %empty																		{ $$ = NULL; }
	;

annotations: DEFAULT_ANNOTATION OPEN_PARENTHESIS style[s] CLOSE_PARENTHESIS								{ $$ = NULL; } // { $$ = DefaultStyleSemanticAction($s); }
	| CUSTOMIZE_ANNOTATION OPEN_PARENTHESIS target[t] COMMA style[s] CLOSE_PARENTHESIS					{ $$ = NULL; } // { $$ = CustomizeStyleSemanticAction(null, $t, $s); }
	| annotations[a] CUSTOMIZE_ANNOTATION OPEN_PARENTHESIS target[t] COMMA style[s] CLOSE_PARENTHESIS 	{ $$ = NULL; } // { $$ = CustomizeStyleSemanticAction($a, $t, $s); }
	;

inner_structure: OPEN_BRACES inner_structure CLOSE_BRACES SEMICOLON	{ $$ = NULL; }
	| OPEN_BRACKETS inner_structure CLOSE_BRACKETS SEMICOLON		{ $$ = NULL; }
	| UNKNOWN												{ $$ = NULL; }
	;

structure: annotations OPEN_BRACES inner_structure CLOSE_BRACES SEMICOLON	{ $$ = NULL; }
	| annotations OPEN_BRACKETS inner_structure CLOSE_BRACKETS SEMICOLON	{ $$ = NULL; }
	| UNKNOWN														{ $$ = NULL; }
	;

%%
