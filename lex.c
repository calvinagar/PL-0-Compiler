// HW2 Lexical Analyzer by Tahsin Islam and Calvin Agar
// COP 3402: Systems Software
// Fall 2021

/*
	This is the lex.c file for the UCF Fall 2021 Systems Software Project.
	For HW2, you must implement the function lexeme *lexanalyzer(char *input).
	You may add as many constants, global variables, and support functions
	as you desire.

	If you choose to alter the printing functions or delete list or lex_index,
	you MUST make a note of that in you readme file, otherwise you will lose
	5 points.
*/


#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "compiler.h"
#define MAX_NUMBER_TOKENS 1000
#define MAX_IDENT_LEN 11
#define MAX_NUMBER_LEN 5


void printlexerror(int type);
void printtokens();
void addToList(lexeme token);



lexeme *list;
int lex_index;


lexeme *lexanalyzer(char *input, int printFlag)
{
	list = (lexeme *)malloc(MAX_NUMBER_TOKENS * sizeof(lexeme));
	lex_index = 0;

	int i = 0, k = 0;

	// loop through entire input
	while (input[i] != '\0')
	{

		lexeme token;

		if (iscntrl(input[i]) || input[i] == ' ')
		{
			i++;
			continue;
		}

		// skip to next line if comment
		if (input[i] == '/' && input[i+1] == '/')
		{
			while (input[i] != '\n')
			{
				i++;
			}
			i++;
		}
		// checks if token starts with a letter
		if(isalpha(input[i]))
		{
			for(k = 0; k < MAX_IDENT_LEN; k++)
			{
				// if its not whitespace, and its a letter or number
				if ((input[i] != ' ' || !(iscntrl(input[i]))) && (isalpha(input[i]) || isdigit(input[i])))
				{
					token.name[k] = input[i];
					if (isdigit(input[i+1]) || isalpha(input[i+1]))
					{
						if (k == MAX_IDENT_LEN - 1)
						{
							printlexerror(4);
							return NULL;
						}
						i++;
					}
					else
					{
						token.name[k+1] = '\0';
						break;
					}
				}
			}


			// assigns token's type based on name
			if(strcmp(token.name, "const") == 0)
			{
				token.type = constsym;
				token.value = 1;
			}
			else if(strcmp(token.name, "var") == 0)
			{
				token.type = varsym;
				token.value = 2;
			}
			else if(strcmp(token.name, "procedure") == 0)
			{
				token.type = procsym;
				token.value = 3;
			}
			else if(strcmp(token.name, "elsedo") == 0)
			{
				token.type = elsedosym;
				token.value = 8;
			}
			else if(strcmp(token.name, "call") == 0)
			{
				token.type = callsym;
				token.value = 9;
			}
			else if(strcmp(token.name, "write") == 0)
			{
				token.type = writesym;
				token.value = 10;
			}
			else if (strcmp(token.name, "read") == 0)
			{
				token.type = readsym;
				token.value = 11;
			}
			else if(strcmp(token.name, "while") == 0)
			{
				token.type = whilesym;
				token.value = 6;
			}
			else if(strcmp(token.name, "do") == 0)
			{
				token.type = dosym;
				token.value = 4;
			}
			else if(strcmp(token.name, "od") == 0)
			{
				token.type = odsym;
				token.value = 5;
			}
			else if(strcmp(token.name, "when") == 0)
			{
				token.type = whensym;
				token.value = 6;
			}
			else if(strcmp(token.name, "odd") == 0)
			{
				token.type = oddsym;
				token.value = 26;
			}
			else
			{
				token.type = identsym;
				token.value = 12;
			}

			if (lex_index == MAX_NUMBER_TOKENS)
			{
				printlexerror(5);
				return NULL;
			}

			// add our token to the list
			list[lex_index] = token;
			lex_index++;
		}

		// checks if token starts with a number
		else if(isdigit(input[i]))
		{
			char numString[MAX_NUMBER_LEN + 1];

			for(k = 0; k < MAX_NUMBER_LEN; k++)
			{
				if(isalpha(input[i]))
				{
					printlexerror(2);
					return NULL;
				}

				numString[k] = input[i];

				// if next letter is a letter or number, max number length exceeded
				if (isdigit(input[i+1]) || isalpha(input[i+1]))
				{
					if (k == MAX_NUMBER_LEN - 1)
					{
						printlexerror(3);
						return NULL;
					}
					i++;
				}
				else
					break;
			}
			numString[k+1] = '\0';

			// turns string into number
			token.value = atoi(numString);
			token.type = numbersym;


			if (lex_index == MAX_NUMBER_TOKENS)
			{
				printlexerror(5);
				return NULL;
			}

			// add our token to the list
			list[lex_index] = token;
			lex_index++;
		}

		// Catches all important symbols and unknown symbols
		else
		{
			if (iscntrl(input[i]) || input[i] == ' ')
			{
				i++;
				continue;
			}

			// :=
			if (input[i] == ':' && input[i+1] == '=')
			{
				token.type = assignsym;
				token.value = 14;
				token.name[0] = ':';
				token.name[1] = '=';

				// Increment i because we looked one ahead already
				i++;
			}

			// Arithmetic: +, -, *, /, %
			else if (input[i] == '+')
			{
				token.type = addsym;
				token.value = 15;
				token.name[0] = '=';
			}
			else if (input[i] == '-')
			{
				token.type = subsym;
				token.value = 16;
				token.name[0] = '-';
			}
			else if (input[i] == '*')
			{
				token.type = multsym;
				token.value = 17;
				token.name[0] = '*';
			}
			else if (input[i] == '/')
			{
				token.type = divsym;
				token.value = 18;
				token.name[0] = '/';
			}
			else if (input[i] == '%')
			{
				token.type = modsym;
				token.value = 19;
				token.name[0] = '%';
			}

			// = and !=
			else if (input[i] == '=')
			{
				token.type = eqlsym;
				token.value = 20;
				token.name[0] = '=';
			}
			else if (input[i] == '!' && input[i+1] == '=')
			{
				token.type = neqsym;
				token.value = 21;
				token.name[0] = '!';
				token.name[1] = '=';
				i++;
			}

			// <= and <
			else if (input[i] == '<')
			{
				// <=
				if (input[i+1] == '=')
				{
					token.type = leqsym;
					token.value = 23;
					token.name[0] = '<';
					token.name[1] = '=';
					i++;
				}
				// <
				else
				{
					token.type = lsssym;
					token.value = 22;
					token.name[0] = '<';
				}
			}

			// >= and >
			else if (input[i] == '>')
			{
				// >=
				if (input[i+1] == '=')
				{
					token.type = geqsym;
					token.value = 25;
					token.name[0] = '>';
					token.name[1] = '=';
					i++;
				}
				// >
				else
				{
					token.type = gtrsym;
					token.value = 24;
					token.name[0] = '>';
				}
			}

			// ( and )
			else if (input[i] == '(')
			{
				token.type = lparensym;
				token.value = 27;
				token.name[0] = '(';
			}
			else if (input[i] == ')')
			{
				token.type = rparensym;
				token.value = 28;
				token.name[0] = ')';
			}

			// , and . and ;
			else if (input[i] == ',')
			{
				token.type = commasym;
				token.value = 29;
				token.name[0] = ',';
			}
			else if (input[i] == '.')
			{
				token.type = periodsym;
				token.value = 30;
				token.name[0] = '.';
			}
			else if (input[i] == ';')
			{
				token.type = semicolonsym;
				token.value = 31;
				token.name[0] = ';';
			}

			// Unrecognized symbol
			else
			{
				printf("%c", input[i]);
				printlexerror(1);
				return NULL;

			}

			// Check for too many tokens
			if (lex_index == MAX_NUMBER_TOKENS)
			{
				printlexerror(5);
				return NULL;
			}

			// add our token to the list
			list[lex_index] = token;
			lex_index++;
		}

		i++;
	}

	if (printFlag == 1) printtokens();
	return list;
}


void printtokens()
{
	int i;
	printf("Lexeme Table:\n");
	printf("lexeme\t\ttoken type\n");
	for (i = 0; i < lex_index; i++)
	{
		switch (list[i].type)
		{
			case oddsym:
				printf("%11s\t%d", "odd", oddsym);
				break;
			case eqlsym:
				printf("%11s\t%d", "=", eqlsym);
				break;
			case neqsym:
				printf("%11s\t%d", "!=", neqsym);
				break;
			case lsssym:
				printf("%11s\t%d", "<", lsssym);
				break;
			case leqsym:
				printf("%11s\t%d", "<=", leqsym);
				break;
			case gtrsym:
				printf("%11s\t%d", ">", gtrsym);
				break;
			case geqsym:
				printf("%11s\t%d", ">=", geqsym);
				break;
			case modsym:
				printf("%11s\t%d", "%", modsym);
				break;
			case multsym:
				printf("%11s\t%d", "*", multsym);
				break;
			case divsym:
				printf("%11s\t%d", "/", divsym);
				break;
			case addsym:
				printf("%11s\t%d", "+", addsym);
				break;
			case subsym:
				printf("%11s\t%d", "-", subsym);
				break;
			case lparensym:
				printf("%11s\t%d", "(", lparensym);
				break;
			case rparensym:
				printf("%11s\t%d", ")", rparensym);
				break;
			case commasym:
				printf("%11s\t%d", ",", commasym);
				break;
			case periodsym:
				printf("%11s\t%d", ".", periodsym);
				break;
			case semicolonsym:
				printf("%11s\t%d", ";", semicolonsym);
				break;
			case assignsym:
				printf("%11s\t%d", ":=", assignsym);
				break;
			case dosym:
				printf("%11s\t%d", "do", dosym);
				break;
			case odsym:
				printf("%11s\t%d", "od", odsym);
				break;
			case elsedosym:
				printf("%11s\t%d", "elsedo", elsedosym);
				break;
			case whilesym:
				printf("%11s\t%d", "while", whilesym);
				break;
			case whensym:
				printf("%11s\t%d", "when", whensym);
				break;
			case callsym:
				printf("%11s\t%d", "call", callsym);
				break;
			case writesym:
				printf("%11s\t%d", "write", writesym);
				break;
			case readsym:
				printf("%11s\t%d", "read", readsym);
				break;
			case constsym:
				printf("%11s\t%d", "const", constsym);
				break;
			case varsym:
				printf("%11s\t%d", "var", varsym);
				break;
			case procsym:
				printf("%11s\t%d", "procedure", procsym);
				break;
			case identsym:
				printf("%11s\t%d", list[i].name, identsym);
				break;
			case numbersym:
				printf("%11d\t%d", list[i].value, numbersym);
				break;
		}
		printf("\n");
	}
	printf("\n");
	printf("Token List:\n");
	for (i = 0; i < lex_index; i++)
	{
		if (list[i].type == numbersym)
			printf("%d %d ", numbersym, list[i].value);
		else if (list[i].type == identsym)
			printf("%d %s ", identsym, list[i].name);
		else
			printf("%d ", list[i].type);
	}
	printf("\n");
	list[lex_index++].type = -1;
}

void printlexerror(int type)
{
	if (type == 1)
		printf("Lexical Analyzer Error: Invalid Symbol\n");
	else if (type == 2)
		printf("Lexical Analyzer Error: Invalid Identifier\n");
	else if (type == 3)
		printf("Lexical Analyzer Error: Excessive Number Length\n");
	else if (type == 4)
		printf("Lexical Analyzer Error: Excessive Identifier Length\n");
	else
		printf("Implementation Error: Unrecognized Error Type\n");

	free(list);
	return;
}
