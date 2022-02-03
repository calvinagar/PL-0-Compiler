#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include "compiler.h"

#define MAX_CODE_LENGTH 1000
#define MAX_SYMBOL_TABLE_SIZE 500

#define UNMARKED 0
#define MARKED 1

// Functions

// Given Functions
void emit(int opname, int level, int mvalue);
void addToSymbolTable(int k, char n[], int v, int l, int a, int m);
void printparseerror(int err_code);
void printsymboltable();
void printassemblycode();

// Our Functions
void program(lexeme *list);
void block(lexeme *list);
void constDeclaration(lexeme *list);
int varDeclaration(lexeme *list);
void procedureDeclaration(lexeme *list);
void statement(lexeme *list);
void condition(lexeme *list);
void expression(lexeme *list);
void term(lexeme *list);
void factor(lexeme *list);
void mark();
int findSymbol(lexeme token, int kind);
int multipleDeclarationCheck(lexeme token);

// Global Variables

instruction *code;
int cIndex = 0;

symbol *table;
int tIndex = 0;

// current token index
int tokenIndex = 0;

// lexicographical level
int level = -1;

int symidx = 0;

int beenHere = 0;

instruction *parse(lexeme *list, int printTable, int printCode)
{
	code = malloc(sizeof(instruction) * MAX_CODE_LENGTH);
	table = malloc(sizeof(symbol) * MAX_SYMBOL_TABLE_SIZE);

	program(list);

	if (code != NULL)
	{
		code[cIndex].opcode = -1;

		if (printTable == 1) printsymboltable();
		if (printCode == 1) printassemblycode();
	}

	return code;
}

void program(lexeme *list)
{
	emit(7, 0, 0); // JMP

	addToSymbolTable(3, "main", 0, 0, 0, UNMARKED);

	block(list);
	if (code == NULL) return;

	if (list[tokenIndex].type != periodsym)
	{
		// ERROR
		printparseerror(1);
		code = NULL;
		return;
	}

	emit(9, 0, 3); // HALT

	for (int i = 0; i < cIndex; i++)
	{
		if (code[i].opcode == 5) // if CAL
			code[i].m = table[code[i].m].addr;
	}

	code[0].m = table[0].addr;
}

void block(lexeme *list)
{
	level++;
	int procedure_idx = (tIndex == 0) ? 0 : tIndex - 1;

	constDeclaration(list);
	if (code == NULL) return;
	
	int x = varDeclaration(list);
	if (code == NULL) return;

	procedureDeclaration(list);
	if (code == NULL) return;	

	table[procedure_idx].addr = cIndex * 3;

	if (level == 0)
	{
		emit(6, 0, x); // INC
	}
	else
	{
		emit(6, 0, x + 4); // INC
	}

	statement(list);
	if (code == NULL) return;

	mark();
	level--;
}

void constDeclaration(lexeme *list)
{
	char identName[12];
	lexeme token;

	if (list[tokenIndex].type == constsym)
	{
		do
		{
			tokenIndex++;

			if(list[tokenIndex].type != identsym)
			{
				// ERROR
				printparseerror(2);
				code = NULL;
				return;
			}

			symidx = multipleDeclarationCheck(list[tokenIndex]);

			if (symidx != -1)
			{
				// ERROR
				printparseerror(18);
				code = NULL;
				return;
			}

			strcpy(identName, list[tokenIndex].name);
			tokenIndex++;

			if (list[tokenIndex].type != assignsym)
			{
				// ERROR
				printparseerror(2);
				code = NULL;
				return;
			}

			tokenIndex++;

			if (list[tokenIndex].type != numbersym)
			{
				// ERROR
				printparseerror(2);
				code = NULL;
				return;
			}

			addToSymbolTable(1, identName, list[tokenIndex].value, level, 0, UNMARKED);
			tokenIndex++;

		} while (list[tokenIndex].type == commasym);

		if (list[tokenIndex].type != semicolonsym)
		{
			if (list[tokenIndex].type == identsym)
			{
				// ERROR
				printparseerror(13);
				code = NULL;
				return;
			}
			else
			{
				// ERROR
				printparseerror(14);
				code = NULL;
				return;
			}
		}
		tokenIndex++;
	}
}

int varDeclaration(lexeme *list)
{
	int numVars = 0;
	char varName[12];

	if (list[tokenIndex].type == varsym)
	{
		do
		{
			numVars++;
			tokenIndex++;

			if (list[tokenIndex].type != identsym)
			{
				// ERROR
				printparseerror(3);
				code = NULL;
				return -1;
			}

			symidx = multipleDeclarationCheck(list[tokenIndex]);

			if (symidx != -1)
			{
				// ERROR
				printparseerror(18);
				code = NULL;
				return -1;
			}

			strcpy(varName, list[tokenIndex].name);

			if (level == 0)
				addToSymbolTable(2, varName, 0, level, numVars - 1, UNMARKED);
			else
				addToSymbolTable(2, varName, 0, level, numVars + 3, UNMARKED);

			tokenIndex++;

		} while (list[tokenIndex].type == commasym);

		if (list[tokenIndex].type != semicolonsym)
		{
			if (list[tokenIndex].type == identsym)
			{
				// ERROR
				printparseerror(13);
				code = NULL;
				return -1;
			}
			else
			{
				// ERROR
				printparseerror(14);
				code = NULL;
				return -1;
			}
		}

		tokenIndex++;
	}

	return numVars;
}

void procedureDeclaration(lexeme *list)
{
	char procName[12];

	while (list[tokenIndex].type == procsym)
	{
		tokenIndex++;

		if (list[tokenIndex].type != identsym)
		{
			// ERROR
			printparseerror(4);
			code = NULL;
			return;
		}

		symidx = multipleDeclarationCheck(list[tokenIndex]);

		if (symidx != -1)
		{
			// ERROR
			printparseerror(18);
			code = NULL;
			return;
		}

		strcpy(procName, list[tokenIndex].name);
		addToSymbolTable(3, procName, 0, level, 0, UNMARKED);

		tokenIndex++;

		if (list[tokenIndex].type != semicolonsym)
		{
			// ERROR
			printparseerror(4);
			code = NULL;
			return;
		}

		tokenIndex++;

		block(list);
		if (code == NULL) return;

		if (list[tokenIndex].type != semicolonsym)
		{
			// ERROR
			printparseerror(14);
			code = NULL;
			return;
		}

		tokenIndex++;

		emit(2, 0, 0); // RTN
	}
}

void statement(lexeme *list)
{
	int jpcIdx = 0;
	int jmpIdx = 0;
	int loopIdx = 0;

	if (list[tokenIndex].type == identsym)
	{
		symidx = findSymbol(list[tokenIndex], 2);

		if (symidx == -1)
		{
			if (findSymbol(list[tokenIndex], 1) != findSymbol(list[tokenIndex], 3))
			{
				// ERROR
				printparseerror(6);
				code = NULL;
				return;
			}
			else
			{
				// ERROR
				printparseerror(19);
				code = NULL;
				return;
			}
		}

		tokenIndex++;

		if (list[tokenIndex].type != assignsym)
		{
			// ERROR
			printparseerror(5);
			code = NULL;
			return;
		}

		tokenIndex++;

		expression(list);
		if (code == NULL) return;

		emit(4, level - table[symidx].level, table[symidx].addr);
		return;
	}

	if (list[tokenIndex].type == dosym)
	{
		do
		{
			tokenIndex++;
			statement(list);
			if (code == NULL) return;

		} while (list[tokenIndex].type == semicolonsym);

		if (list[tokenIndex].type != odsym)
		{
			if (list[tokenIndex].type == identsym ||
				list[tokenIndex].type == dosym ||
				list[tokenIndex].type == whensym ||
				list[tokenIndex].type == whilesym ||
				list[tokenIndex].type == readsym ||
				list[tokenIndex].type == writesym ||
				list[tokenIndex].type == callsym)
			{
				// ERROR
				printparseerror(15);
				code = NULL;
				return;
			}
			else
			{
				// ERROR
				printparseerror(16);
				code = NULL;
				return;
			}
		}
		tokenIndex++;
		return;
	}

	if (list[tokenIndex].type == whensym)
	{
		tokenIndex++;
		condition(list);
		if (code == NULL) return;

		jpcIdx = cIndex;
		emit (8, 0, 0); // JPC

		if (list[tokenIndex].type != dosym)
		{
			// ERROR
			printparseerror(8);
			code = NULL;
			return;
		}

		tokenIndex++;

		statement(list);
		if (code == NULL) return;

		if (list[tokenIndex].type == elsedosym)
		{
			jmpIdx = cIndex;
			emit(7, 0, 0); // JMP

			code[jpcIdx].m = cIndex * 3;

			tokenIndex++;

			statement(list);
			if (code == NULL) return;

			code[jmpIdx].m = cIndex * 3;
		}
		else
		{
			code[jpcIdx].m = cIndex * 3;
		}

		return;
	}

	if (list[tokenIndex].type == whilesym)
	{
		tokenIndex++;
		loopIdx = cIndex;

		condition(list);
		if (code == NULL) return;

		if (list[tokenIndex].type != dosym)
		{
			// ERROR
			printparseerror(9);
			code = NULL;
			return;
		}

		tokenIndex++;

		jpcIdx = cIndex;
		emit(8, 0, 0); // JPC

		statement(list);
		if (code == NULL) return;

		emit(7, 0, loopIdx * 3); // JMP

		code[jpcIdx].m = cIndex * 3;

		return;
	}

	if (list[tokenIndex].type == readsym)
	{
		tokenIndex++;

		if (list[tokenIndex].type != identsym)
		{
			// ERROR
			printparseerror(6);
			code = NULL;
			return;
		}

		symidx = findSymbol(list[tokenIndex], 2);

		if (symidx == -1)
		{
			if (findSymbol(list[tokenIndex], 1) != findSymbol(list[tokenIndex], 3))
			{
				// ERROR
				printparseerror(6);
				code = NULL;
				return;
			}
			else
			{
				// ERROR
				printparseerror(19);
				code = NULL;
				return;
			}
		}

		tokenIndex++;

		emit(9, 0, 2); // READ
		emit(4, level - table[symidx].level, table[symidx].addr); // STO
		return;
	}

	if (list[tokenIndex].type == writesym)
	{
		tokenIndex++;

		expression(list);
		if (code == NULL) return;

		emit(9, 0, 1); // WRITE
		return;
	}

	if (list[tokenIndex].type == callsym)
	{
		tokenIndex++;

		symidx = findSymbol(list[tokenIndex], 3);

		if (symidx == -1)
		{
			if (findSymbol(list[tokenIndex], 1) != findSymbol(list[tokenIndex], 2))
			{
				// ERROR
				printparseerror(7);
				code = NULL;
				return;
			}
			else
			{
				// ERROR
				printparseerror(7);
				code = NULL;
				return;
			}
		}

		tokenIndex++;
		emit(5, level - table[symidx].level, symidx); // CAL
	}
}

void condition(lexeme *list)
{
	if (list[tokenIndex].type == oddsym)
	{
		tokenIndex++;
		expression(list);
		if (code == NULL) return;

		emit(2, 0, 6); // ODD
	}
	else
	{
		expression(list);
		if (code == NULL) return;

		if (list[tokenIndex].type == eqlsym)
		{
			tokenIndex++;
			expression(list);
			if (code == NULL) return;

			emit(2, 0, 8); // EQL
		}
		else if (list[tokenIndex].type == neqsym)
		{
			tokenIndex++;
			expression(list);
			if (code == NULL) return;

			emit(2, 0, 9); // NEQ
		}
		else if (list[tokenIndex].type == lsssym)
		{
			tokenIndex++;
			expression(list);
			if (code == NULL) return;

			emit(2, 0, 10); // LSS
		}
		else if (list[tokenIndex].type == leqsym)
		{
			tokenIndex++;
			expression(list);
			if (code == NULL) return;

			emit(2, 0, 11); // LEQ
		}
		else if (list[tokenIndex].type == gtrsym)
		{
			tokenIndex++;
			expression(list);
			if (code == NULL) return;

			emit(2, 0, 12); // GTR
		}
		else if (list[tokenIndex].type == geqsym)
		{
			tokenIndex++;
			expression(list);
			if (code == NULL) return;
			
			emit(2, 0, 13); // GEQ
		}
		else
		{
			// ERROR
			printparseerror(10);
			code = NULL;
			return;
		}
	}
}

void expression(lexeme *list)
{
	if (list[tokenIndex].type == subsym)
	{
		tokenIndex++;
		term(list);
		if (code == NULL) return;

		emit(2, 0, 1); // NEG

		while (list[tokenIndex].type == addsym || list[tokenIndex].type == subsym)
		{
			if (list[tokenIndex].type == addsym)
			{
				tokenIndex++;
				term(list);
				if (code == NULL) return;

				emit(2, 0, 2); // ADD
			}
			else
			{
				tokenIndex++;
				term(list);
				if (code == NULL) return;

				emit(2, 0, 3); // SUB
			}
		}
	}
	else
	{
		if (list[tokenIndex].type == addsym)
			tokenIndex++;

		term(list);
		if (code == NULL) return;

		while(list[tokenIndex].type == addsym || list[tokenIndex].type == subsym)
		{
			if (list[tokenIndex].type == addsym)
			{
				tokenIndex++;
				term(list);
				if (code == NULL) return;

				emit(2, 0, 2); // ADD
			}
			else
			{
				tokenIndex++;
				term(list);
				if (code == NULL) return;

				emit(2, 0, 3); // SUB
			}
		}
	}

	if (list[tokenIndex].type == lparensym ||
		list[tokenIndex].type == identsym ||
		list[tokenIndex].type == numbersym ||
		list[tokenIndex].type == oddsym ||
		list[tokenIndex].type == addsym ||
		list[tokenIndex].type == subsym ||
		list[tokenIndex].type == multsym ||
		list[tokenIndex].type == divsym ||
		list[tokenIndex].type == modsym)
	{
		// ERROR
		printparseerror(17);
		code = NULL;
		return;
	}
}

void term(lexeme *list)
{
	factor(list);
	if (code == NULL) return;

	while (list[tokenIndex].type == multsym ||
		   list[tokenIndex].type == divsym ||
		   list[tokenIndex].type == modsym)
	{
		if (list[tokenIndex].type == multsym)
		{
			tokenIndex++;
			factor(list);
			if (code == NULL) return;

			emit(2, 0, 4); // MUL
		}
		else if (list[tokenIndex].type == divsym)
		{
			tokenIndex++;
			factor(list);
			if (code == NULL) return;

			emit(2, 0, 5); // DIV
		}
		else
		{
			tokenIndex++;
			factor(list);
			if (code == NULL) return;

			emit(2, 0, 7); // MOD
		}
	}
}

void factor(lexeme *list)
{
	if (list[tokenIndex].type == identsym)
	{
		int symIdx_var = findSymbol(list[tokenIndex], 2);
		int symIdx_const = findSymbol(list[tokenIndex], 1);

		if (symIdx_const == -1 && symIdx_var == -1)
		{
			if (findSymbol(list[tokenIndex], 3) == -1)
			{
				// ERROR
				printparseerror(19);
				code = NULL;
				return;
			}
			else
			{
				// ERROR
				printparseerror(11);
				code = NULL;
				return;
			}
		}

		if (symIdx_var == -1)
		{
			emit(1, 0, table[symIdx_const].val); // LIT
		}
		else if (symIdx_const == -1 || table[symIdx_var].level > table[symIdx_const].level)
		{
			emit(3, level - table[symIdx_var].level, table[symIdx_var].addr); // LOD
		}
		else
		{
			emit(1, 0, table[symIdx_const].val); // LIT
		}

		tokenIndex++;
	}
	else if (list[tokenIndex].type == numbersym)
	{
		emit(1, 0, list[tokenIndex].value); // LIT, M could be wrong here. fix?
		tokenIndex++;
	}
	else if (list[tokenIndex].type == lparensym)
	{
		tokenIndex++;
		expression(list);
		if (code == NULL) return;

		if (list[tokenIndex].type != rparensym)
		{
			// ERROR
			printparseerror(12);
			code = NULL;
			return;
		}

		tokenIndex++;
	}
	else
	{
		// ERROR
		printparseerror(11);
		code = NULL;
		return;
	}
}

void mark()
{
	int i;

	for (i = tIndex - 1; i >= 0; i--)
	{
		if (table[i].mark == UNMARKED)
		{
			if (table[i].level == level)
			{
				table[i].mark = MARKED;
			}
			else if (table[i].level < level)
			{
				return;
			}
		}
	}
}

int findSymbol(lexeme token, int kind)
{
	int i, bestIndex = -1, bestDiff = INT_MAX;

	for (i = 0; i < tIndex; i++)
	{
		if(strcmp(table[i].name, token.name) == 0 &&
		   kind == table[i].kind && table[i].mark == UNMARKED)
		{
			// maximize level (find level closest to current level)
			if (abs(table[i].level - level) < bestDiff)
			{
				bestDiff = abs(table[i].level - level);
				bestIndex = i;
			}
		}
	}

	return bestIndex;
}

int multipleDeclarationCheck(lexeme token)
{
	int i;

	for (i = 0; i < tIndex; i++)
	{
		if (strcmp(table[i].name, token.name) == 0)
		{
			if (table[i].mark == UNMARKED)
			{
				if (table[i].level == level)
				{
					return i;
				}
			}
		}
	}

	return -1;
}

void emit(int opname, int level, int mvalue)
{
	code[cIndex].opcode = opname;
	code[cIndex].l = level;
	code[cIndex].m = mvalue;
	cIndex++;
}

void addToSymbolTable(int k, char n[], int v, int l, int a, int m)
{
	table[tIndex].kind = k;
	strcpy(table[tIndex].name, n);
	table[tIndex].val = v;
	table[tIndex].level = l;
	table[tIndex].addr = a;
	table[tIndex].mark = m;
	tIndex++;

}

void printparseerror(int err_code)
{
	switch (err_code)
	{
		case 1:
			printf("Parser Error: Program must be closed by a period\n");
			break;
		case 2:
			printf("Parser Error: Constant declarations should follow the pattern 'ident := number {, ident := number}'\n");
			break;
		case 3:
			printf("Parser Error: Variable declarations should follow the pattern 'ident {, ident}'\n");
			break;
		case 4:
			printf("Parser Error: Procedure declarations should follow the pattern 'ident ;'\n");
			break;
		case 5:
			printf("Parser Error: Variables must be assigned using :=\n");
			break;
		case 6:
			printf("Parser Error: Only variables may be assigned to or read\n");
			break;
		case 7:
			printf("Parser Error: call must be followed by a procedure identifier\n");
			break;
		case 8:
			printf("Parser Error: when must be followed by do\n");
			break;
		case 9:
			printf("Parser Error: while must be followed by do\n");
			break;
		case 10:
			printf("Parser Error: Relational operator missing from condition\n");
			break;
		case 11:
			printf("Parser Error: Arithmetic expressions may only contain arithmetic operators, numbers, parentheses, constants, and variables\n");
			break;
		case 12:
			printf("Parser Error: ( must be followed by )\n");
			break;
		case 13:
			printf("Parser Error: Multiple symbols in variable and constant declarations must be separated by commas\n");
			break;
		case 14:
			printf("Parser Error: Symbol declarations should close with a semicolon\n");
			break;
		case 15:
			printf("Parser Error: Statements within do-od must be separated by a semicolon\n");
			break;
		case 16:
			printf("Parser Error: do must be followed by od\n");
			break;
		case 17:
			printf("Parser Error: Bad arithmetic\n");
			break;
		case 18:
			printf("Parser Error: Confliciting symbol declarations\n");
			break;
		case 19:
			printf("Parser Error: Undeclared identifier\n");
			break;
		default:
			printf("Implementation Error: unrecognized error code\n");
			break;
	}

	free(code);
	free(table);
}

void printsymboltable()
{
	int i;
	printf("Symbol Table:\n");
	printf("Kind | Name        | Value | Level | Address | Mark\n");
	printf("---------------------------------------------------\n");
	for (i = 0; i < tIndex; i++)
		printf("%4d | %11s | %5d | %5d | %5d | %5d\n", table[i].kind, table[i].name, table[i].val, table[i].level, table[i].addr, table[i].mark);

	free(table);
	table = NULL; 
}

void printassemblycode()
{
	int i;
	printf("Line\tOP Code\tOP Name\tL\tM\n");
	for (i = 0; i < cIndex; i++)
	{
		printf("%d\t", i);
		printf("%d\t", code[i].opcode);
		switch (code[i].opcode)
		{
			case 1:
				printf("LIT\t");
				break;
			case 2:
				switch (code[i].m)
				{
					case 0:
						printf("RTN\t");
						break;
					case 1:
						printf("NEG\t");
						break;
					case 2:
						printf("ADD\t");
						break;
					case 3:
						printf("SUB\t");
						break;
					case 4:
						printf("MUL\t");
						break;
					case 5:
						printf("DIV\t");
						break;
					case 6:
						printf("ODD\t");
						break;
					case 7:
						printf("MOD\t");
						break;
					case 8:
						printf("EQL\t");
						break;
					case 9:
						printf("NEQ\t");
						break;
					case 10:
						printf("LSS\t");
						break;
					case 11:
						printf("LEQ\t");
						break;
					case 12:
						printf("GTR\t");
						break;
					case 13:
						printf("GEQ\t");
						break;
					default:
						printf("err\t");
						break;
				}
				break;
			case 3:
				printf("LOD\t");
				break;
			case 4:
				printf("STO\t");
				break;
			case 5:
				printf("CAL\t");
				break;
			case 6:
				printf("INC\t");
				break;
			case 7:
				printf("JMP\t");
				break;
			case 8:
				printf("JPC\t");
				break;
			case 9:
				switch (code[i].m)
				{
					case 1:
						printf("WRT\t");
						break;
					case 2:
						printf("RED\t");
						break;
					case 3:
						printf("HAL\t");
						break;
					default:
						printf("err\t");
						break;
				}
				break;
			default:
				printf("err\t");
				break;
		}
		printf("%d\t%d\n", code[i].l, code[i].m);
	}
	if (table != NULL)
		free(table);
}
