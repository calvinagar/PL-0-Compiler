// HW1 Virtual P-Machine by Tahsin Islam and Calvin Agar
// COP 3402: Systems Software
// Dr. Montagne
// Fall 2021

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "compiler.h"

#define MAX_PAS_LENGTH 3000

int base(int L, int BP, int *pas);
void print_execution(int line, char *opname, int *IR, int PC, int BP, int SP, int DP, int *pas, int GP);


void execute_program(instruction *code, int printFlag)
{

    int  *IR = (int *)malloc(sizeof(int) * 3); // Instruction Register
    int *pas = (int *)malloc(sizeof(int) * MAX_PAS_LENGTH); // Process Address Space

    // Initialize PAS before we occupy it with
    // the data from the input file
    for (int i = 0; i < MAX_PAS_LENGTH; i++)
        pas[i] = 0;

    int IC = 0; // Instruction Counter
    int index = 0;

    // Copy the code into the PAS
    while (code[index].opcode != -1)
    {
        pas[IC] = code[index].opcode;
        pas[IC + 1] = code[index].l;
        pas[IC + 2] = code[index].m;
        IC += 3;
        index++;
    }


    // Initialize rest of values
    int PC = 0; // Program Counter
    int BP = IC; // Base Pointer
    int SP = MAX_PAS_LENGTH; // Stack Pointer
    int DP = IC - 1; // Data Pointer
    int FREE = IC + 40; // Heap Location
    int GP = IC; // Global Pointer
    int HALT = 1; // 0 = Halt Program

    if (printFlag == 1)
    {
        printf("\t\t\t\tPC\tBP\tSP\tDP\tdata\n");
        printf("Initial values:\t\t\t%d\t%d\t%d\t%d\n", PC, BP, SP, DP);
    }


    // While the program is running,
    // fetch the next instruction and
    // execute it
    while (HALT == 1)
    {

        /**********************************************/
        /*                   Fetch                    */
        /**********************************************/

        // Place values from our current instruction
        // into our Instruction Register
        IR[0] = pas[PC];
        IR[1] = pas[PC + 1];
        IR[2] = pas[PC + 2];

        // Since three inputs per line,
        // Program Counter is increased by 3
        PC += 3;


        /**********************************************/
        /*                  Execute                   */
        /**********************************************/

        // Current Instruction
        int opcode = IR[0]; 
        int L = IR[1]; 
        int M = IR[2];

        // Find line number for printing
        // data before we manipulate PC
        int line = (PC / 3) - 1;

        char opname[4] = "nul";

        // Switch statement for deciding which
        // instruction to use using the opcode
        switch(opcode)
        {
            // LIT - Pushes a constant value (literal) M onto
            // the stack or into the data section of the PAS
            case 1:

                strcpy(opname, "LIT");

                if (BP == GP)
                {
                    DP += 1;
                    pas[DP] = M;
                }
                else
                {
                    SP -= 1;
                    pas[SP] = M;
                }
                break;


            // OPR - Arithmetic to be performed on the data at the
            // top of the stack or in the data section of the PAS
            case 2:

                // RTN - return from subroutine
                if (M == 0)
                {
                    strcpy(opname, "RTN");

                    SP = BP + 1;
                    BP = pas[SP - 3];
                    PC = pas[SP - 4];
                }

                // NEG - negate the data
                else if (M == 1)
                {
                    strcpy(opname, "NEG");

                    if (BP == GP)
                        pas[DP] = -(pas[DP]);
                    else
                        pas[SP] = -(pas[SP]);
                }

                // ADD - add top two numbers in data or stack
                else if (M == 2)
                {
                    strcpy(opname, "ADD");

                    if (BP == GP)
                    {
                        DP -= 1;
                        pas[DP] = pas[DP] + pas[DP + 1];
                    }
                    else
                    {
                        SP += 1;
                        pas[SP] = pas[SP] + pas[SP - 1];
                    }
                }

                // SUB - subtract the top two numbers in data or stack
                else if (M == 3)
                {
                    strcpy(opname, "SUB");

                    if (BP == GP)
                    {
                        DP -= 1;
                        pas[DP] = pas[DP] - pas[DP + 1];
                    }
                    else
                    {
                        SP += 1;
                        pas[SP] = pas[SP] - pas[SP - 1];
                    }
                }

                // MUL - multiply the top two numbers in data or stack
                else if (M == 4)
                {
                    strcpy(opname, "MUL");

                    if (BP == GP)
                    {
                        DP -= 1;
                        pas[DP] = pas[DP] * pas[DP + 1];
                    }
                    else
                    {
                        SP += 1;
                        pas[SP] = pas[SP] * pas[SP - 1];
                    }
                }

                // DIV - divide the top two numbers in data or stack
                else if (M == 5)
                {
                    strcpy(opname, "DIV");

                    if (BP == GP)
                    {
                        DP -= 1;
                        pas[DP] = pas[DP] / pas[DP + 1];
                    }
                    else
                    {
                        SP += 1;
                        pas[SP] = pas[SP] / pas[SP - 1];
                    }
                }

                // ODD - mod the top number in data or stack with 2
                else if (M == 6)
                {
                    strcpy(opname, "ODD");

                    if (BP == GP)
                        pas[DP] = pas[DP] % 2;

                    else
                        pas[SP] = pas[SP] % 2;

                }

                // MOD - mod the top two numbers in data or stack
                else if (M == 7)
                {
                    strcpy(opname, "MOD");

                    if (BP == GP)
                    {
                        DP -= 1;
                        pas[DP] = pas[DP] % pas[DP + 1];
                    }
                    else
                    {
                        SP += 1;
                        pas[SP] = pas[SP] % pas[SP - 1];
                    }
                }

                // EQL - Set the data position or top of the stack to a 1
                // if the top two numbers in data or stack are equal, 0
                // if not equal
                else if (M == 8)
                {
                    strcpy(opname, "EQL");

                    if (BP == GP)
                    {
                        DP -= 1;

                        if (pas[DP] == pas[DP + 1])
                            pas[DP] = 1;
                        else
                            pas[DP] = 0;
                    }
                    else
                    {
                        SP += 1;

                        if (pas[SP] == pas[SP - 1])
                            pas[SP] = 1;
                        else
                            pas[SP] = 0;
                    }
                }

                // NEQ - Set the data position or top of the stack to a 1
                // if the top two numbers in data or stack are not equal, 0
                // if equal
                else if (M == 9)
                {
                    strcpy(opname, "NEQ");

                    if (BP == GP)
                    {
                        DP -= 1;

                        if (pas[DP] != pas[DP + 1])
                            pas[DP] = 1;
                        else
                            pas[DP] = 0;
                    }
                    else
                    {
                        SP += 1;

                        if (pas[SP] != pas[SP - 1])
                            pas[SP] = 1;
                        else
                            pas[SP] = 0;
                    }
                }

                // LSS - Set the data position or top of the stack to a 1
                // if the second top number is less than the
                // first top number, 0 if not less than
                else if (M == 10)
                {
                    strcpy(opname, "LSS");

                    if (BP == GP)
                    {
                        DP -= 1;

                        if (pas[DP] < pas[DP + 1])
                            pas[DP] = 1;
                        else
                            pas[DP] = 0;
                    }
                    else
                    {
                        SP += 1;

                        if (pas[SP] < pas[SP - 1])
                            pas[SP] = 1;
                        else
                            pas[SP] = 0;
                    }
                }

                // LEQ - Set the data position or top of the stack to a 1
                // if the second top number is less than or equal to the
                // first top number, 0 if not less than or equal
                else if (M == 11)
                {
                    strcpy(opname, "LEQ");

                    if (BP == GP)
                    {
                        DP -= 1;

                        if (pas[DP] <= pas[DP + 1])
                            pas[DP] = 1;
                        else
                            pas[DP] = 0;
                    }
                    else
                    {
                        SP += 1;

                        if (pas[SP] <= pas[SP - 1])
                            pas[SP] = 1;
                        else
                            pas[SP] = 0;
                    }
                }

                // GTR - Set the data position or top of the stack to a 1
                // if the second top number is greater than the
                // first top number, 0 if not greater than
                else if (M == 12)
                {
                    strcpy(opname, "GTR");

                    if (BP == GP)
                    {
                        DP -= 1;

                        if (pas[DP] > pas[DP + 1])
                            pas[DP] = 1;
                        else
                            pas[DP] = 0;
                    }
                    else
                    {
                        SP += 1;

                        if (pas[SP] > pas[SP - 1])
                            pas[SP] = 1;
                        else
                            pas[SP] = 0;
                    }
                }

                // GEQ - Set the data position or top of the stack to a 1
                // if the second top number is greater than or equal to the
                // first top number, 0 if not greater than or equal 
                else if (M == 13)
                {
                    strcpy(opname, "GEQ");

                    if (BP == GP)
                    {
                        DP -= 1;

                        if (pas[DP] >= pas[DP + 1])
                            pas[DP] = 1;
                        else
                            pas[DP] = 0;
                    }
                    else
                    {
                        SP += 1;

                        if (pas[SP] >= pas[SP - 1])
                            pas[SP] = 1;
                        else
                            pas[SP] = 0;
                    }
                }
                break;


            // LOD - Load value to top of stack from the stack location
            // at offset M, L lexicographical levels down
            case 3:

                strcpy(opname, "LOD");

                if (BP == GP)
                {
                    DP += 1;
                    pas[DP] = pas[GP + M];
                }
                else
                {
                    if (base(L, BP, pas) == GP)
                    {
                        SP -= 1;
                        pas[SP] = pas[GP + M];
                    }
                    else
                    {
                        SP -= 1;
                        pas[SP] = pas[base(L, BP, pas) - M];
                    }
                }
                break;


            // STO - Store value at top of stack in the stack
            // location at offset M, L lexicographical levels down
            case 4:

                strcpy(opname, "STO");

                if (BP == GP)
                {
                    pas[GP + M] = pas[DP];
                    DP -= 1;
                }
                else
                {
                    if (base(L, BP, pas) == GP)
                    {
                        pas[GP + M] = pas[SP];
                        SP += 1;
                    }
                    else
                    {
                        pas[base(L, BP, pas) - M] = pas[SP];
                        SP += 1;
                    }
                }
                break;


            // CAL - Call procedure at code index M
            // (generates new Activation Record and PC <- M)
            case 5:

                strcpy(opname, "CAL");

                pas[SP - 1] = 0; // Functional Value
                pas[SP - 2] = base(L, BP, pas);
                pas[SP - 3] = BP;
                pas[SP - 4] = PC;
                BP = SP - 1;
                PC = M;

                break;


            // INC - Allocate M memory words (increment SP by M)
            case 6:

                strcpy(opname, "INC");

                if (BP == GP)
                    DP += M;
                else
                    SP -= M;

                break;


            // JMP - Jump to instruction M
            case 7:

                strcpy(opname, "JMP");

                PC = M;

                break;


            // JPC - Jump to instruction M if top stack or data element is 0
            case 8:

                strcpy(opname, "JPC");

                if (BP == GP)
                {
                    if (pas[DP] == 0)
                        PC = M;

                    DP -= 1;
                }
                else
                {
                    if (pas[SP] == 0)
                        PC = M;

                    SP += 1;
                }

                break;


            // SYS - handles storing and loading memory
            // and halting the program
            case 9:

                // SYS 0, 1 - Write the top stack element or data element to the screen
                strcpy(opname, "SYS");
                if (M == 1)
                {
                    printf("Top of Stack Value: ");
                    if (BP == GP)
                    {
                        printf("%d\n", pas[DP]);
                        DP -= 1;
                    }
                    else
                    {
                        printf("%d\n", pas[SP]);
                        SP += 1;
                    }
                }

                // SYS 0, 2 - Read in input from the user and store it on top of the stack or the data section
                else if (M == 2)
                {
                    printf("Please Enter an Integer: ");
                    if (BP == GP)
                    {
                        DP += 1;
                        scanf("%d", &pas[DP]);
                    }
                    else
                    {
                        SP -= 1;
                        scanf("%d", &pas[SP]);
                    }
                }

                // SYS 0, 3 - End of program (Set Halt flag to zero)
                else if (M == 3)
                {
                    HALT = 0;
                }
        }

        // Print out what we just did with our instruction
        if (printFlag == 1) 
            print_execution(line, opname, IR, PC, BP, SP, DP, pas, GP);

        /**********************************************/
        /*              End of Execute                */
        /*             Go Back to Fetch               */
        /**********************************************/
    }
}

/**********************************************/
/*              Given Functions               */
/**********************************************/

// Find activation record base L levels down
int base(int L, int BP, int *pas)
{
    int arb = BP; // arb = activation record base

    while (L > 0) // find base L levels down
    {
        arb = pas[arb - 1];
        L--;
    }

    return arb;
}

// Prints out information regarding the current
// values inside the machine after an instruction
// is executed
void print_execution(int line, char *opname, int *IR, int PC, int BP, int SP, int DP, int *pas, int GP)
{
    int i;

    // Print out instruction and registers 
    printf("%2d\t%s\t%d\t%d\t%d\t%d\t%d\t%d\t", line, opname, IR[1], IR[2], PC, BP, SP, DP);

    // Print data section
    for (i = GP; i <= DP; i++)
        printf("%d ", pas[i]); printf("\n");

    // Print stack
    printf("\tstack : ");
    for (i = MAX_PAS_LENGTH - 1; i >= SP; i--)
        printf("%d ", pas[i]);
    printf("\n");
}
