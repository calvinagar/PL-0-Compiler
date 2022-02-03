PL/0+ Compiler by Tahsin Islam and Calvin Agar 

This program takes in a file with code written in PL/0+ as the input, converts the file into tokens, analyzes the tokens for syntax errors, and runs the output in a virtual machine if there are no errors.

===========================
      EBNF of PL/0+
===========================

program ::= block "." .

block ::= const-declaration var-declaration procedure-declaration statement. 

const-declaration ::= ["const" ident ":=" number {"," ident ":=" number} ";"]. 

var-declaration ::= [ "var "ident {"," ident} “;"].

procedure-declaration ::= { "procedure" ident ";" block ";" }.

 statement ::= [ ident ":=" expression | "call" ident
				     | "do" statement { ";" statement } "od"
				     | "when" condition "do" statement ["elsedo" statement]
				     | "while" condition "do" statement
				     | "read" ident
				     | "write" expression
				     |ε].

condition ::= "odd" expression
	     | expression rel-op expression.

rel-op ::= "="|“!="|"<"|"<="|">"|">=“.

expression ::= [ "+"|"-"] term { ("+"|"-") term}.

term ::= factor {("*"|"/"|”%”) factor}.

factor ::= ident | number | "(" expression ")“.

number ::= digit {digit}.

ident ::= letter {letter | digit}.

digit ;;= "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9“. 

letter ::= "a" | "b" | ... | "y" | "z" | "A" | "B" | ... | "Y" | "Z".


===========================
	  To run
===========================
make
./a.out <filename.txt>