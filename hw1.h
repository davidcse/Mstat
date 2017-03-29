#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef struct stat{      
    int count; // Number of instructions
    int R; // Number of R Types
    int I; // Number of I Types
    int J; // Number of J Types
    char *regNames[32]; // Contains the names of the registers
    int regCountI_Type[32]; //Stores reg count for I Types
    int regCountJ_Type[32]; // Stores the reg Count for J Type
    int regCountR_Type[32]; // Stores the reg Count for R Type. 
    int regCount[32]; // Stores the register use.
    int functionCount[64]; // Stores the Use of Func code for R Types
    int opCount[64]; // Stores the use of Opcodes for All Types
     

} Statistics;

char *flagIHeader = "TYPE\tCOUNT\tPERCENT";
char *flagRHeader = "REG\tUSE\tR-TYPE\tI-TYPE\tJ-TYPE\tPERCENT";
char *flagOHeader = "OPCODE\tCOUNT\tPERCENTAGE";
char *functionHeader = "FUNC\tCOUNT\tPERCENTAGE";
void countFunctionsRType(unsigned long long num,Statistics *stat);

void executeI(_Bool detailedFlag,Statistics *stat);
void executeR(_Bool detailedFlag,Statistics *stat);
void executeO(_Bool detailedFlag,Statistics *stat);

void flagI(char mipsLine[],Statistics *stat);
void flagR(char mipsLine[],Statistics *stat);
void flagO(char mipsLine[],Statistics *stat);

void commandI(_Bool detail,Statistics *stat);
void commandR(_Bool detail,Statistics *stat);
void commandO(_Bool detail,Statistics *stat);

void commandCaller(int num,_Bool detailedFlag,Statistics *stat);
char *scanLine(char mipsLine[]);

unsigned long long convertMipsLongLong(char mipsString[]);

unsigned long long  findRsValue(unsigned long long num);
unsigned long long findRtValue(unsigned long long num);
unsigned long long findRdValue(unsigned long long num);

unsigned long long findOpcodeInLongInstr(unsigned long long num);
int determineTypeFromOpcode(unsigned long long opcode);
void opcodeCount(unsigned long long actualOpcodeValue,Statistics *stat);

void countRegistersI_Type( unsigned long long num, Statistics *stat);    
void countRegistersR_Type( unsigned long long num, Statistics *stat);     
void countFunctions_RType(unsigned long long num,Statistics *stat);


void printRegCount(Statistics *stat,_Bool detail);
void printOpcodeCount(Statistics *stat);
void printFunctionCount(Statistics *stat);
void printHelp(void);
void printTypeCount(Statistics *stat);
int isValidHexadecimal(char mipsLine[]);

