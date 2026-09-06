# KIWI Language Specification

## Grammar (EBNF)

``` ebnf 
(* Top Level *)
programs   = decl* EOF ;

(* Declaration *)
Declr      = VarDeclr
           | FuncDeclr 
           ;

VarDeclr   = type IDENTIFIER ("=" Expr)? ";" ;
FuncDeclr  = "fn" IDENTIFIER "(" params? ")" "->" type block ;
params     = type IDENTIFIER ("," type IDENTIFIER)* ;

(* Statements *)
block      = "{" (decl | stmt)* "}" ;

stmt       = returnStmt | assign ; 
assign     = IDENTIFIER "=" expr ";" ;
returnStmt = "return" expr ";" ;

(* Expressions *)
expr       = unary (OP unary)* ;

unary      = "-" unary | primary;
primary    = NUMBER_INT
           | NUMBER_FLOAT
           | IDENTIFIER
           | "(" expr ")"
           ;

(* Types *)
type       = "int8"  | "int16"  | "int32"  | "int64"
           | "float8" | "float16" | "float32" | "float64"
           | "bool"
           | "string"
           | "char"
           ;

(* Terminals *)
NUMBER_INT   = DIGIT+ ;
NUMBER_FLOAT =  DIGIT+ "." DIGIT+ ;
OP           = "+" | "-" | "*" | "/" ;
IDENTIFIER   = (ALPHA | "_") (ALPHA | DIGIT | "_")* ;
STRING       = '"' (ANY CHARACTER EXCEPT ") '"' ;
ALPHA        = [a-zA-Z] ;
DIGIT        = [0-9] ;

```
