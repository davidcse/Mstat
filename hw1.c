#include "hw1.h"
int main(int argc, char *argv[]){
  if ((argc ==2) |( argc ==3)){// Takes in only one or two flags
    int argCount;
    char *command;
    _Bool uFlag =0;
   Statistics stat ={0,0,0,0,
   {"$zero","$at","$v0","$v1","$a0","$a1","$a2","$a3","$t0","$t1","$t2","$t3",
   "$t4","$t5","$t6","$t7","$s0","$s1","$s2","$s3","$s4","$s5","$s6","$s7","$t8","$t9",
   "$k0","$k1","$gp","$sp","$fp","$ra"
   },{0},{0},{0},{0},{0},{0}}; 
   for(argCount=(argc-1); argCount >=1; argCount--){
     command = argv[argCount];
     if( ( strcmp(command,"-h")==0) &(argCount != 2) ){
       printHelp();
       exit(EXIT_SUCCESS);    
     }
     else if(strcmp(command,"-i")==0){
       commandCaller(0,uFlag,&stat);
     }
     else if (strcmp(command,"-r")==0){
       commandCaller(1,uFlag,&stat);   
     }
     else if (strcmp(command,"-o")==0){
       commandCaller(2,uFlag,&stat);
     }
     else if((strcmp(command,"-u")==0)&(argCount==2)){
       uFlag=1;
     }
     else{
       printHelp(); 
       exit(EXIT_FAILURE);
     }
   }
 }else{
    printHelp();
    exit(EXIT_FAILURE);
  }
 return 0; 
}


/* Prints the help menu*/
void printHelp(){
  char *mstatString = "./mstat";
  char *symbolLine[]={"[OPTION]","-h","-i [-u]","-r [-u]","-o [-u]","-u"};
  char *detailLine[] ={"","Displays this help menu","Displays statistics about instruction types.",
  "Displays information about the registers", "Displays number and percentage of opcodes used.",
  "Displays human readable headers for the different outputs."};
   printf("Usage:\t%s %s\t%s\n",mstatString,symbolLine[0],detailLine[0]);
   int i; 
   for(i=1;i<5;i++){
     printf("\t%s %s\t%s\n",mstatString,symbolLine[i],detailLine[i]);
   }
   printf("\n\t%s\n\t%s\t%s\n","Optional flags:",symbolLine[5],detailLine[5]);
}

              /*Controller Functions*/
/* Controller function for scanning and executing */
void commandCaller(int num,_Bool detailedFlag,Statistics *stat){ 
   switch(num){
       case 0: commandI(detailedFlag,stat);
               break;
       case 1: commandR(detailedFlag,stat);
               break;
       case 2: commandO(detailedFlag,stat);
               break;
       default: break;
   }
}
void commandI(_Bool detail,Statistics *stat){
   if(detail==1) printf("%s\n",flagIHeader);
   char mipsLine[11]={'\0'};
   while( scanLine(mipsLine)!=NULL){ // Exists Line. Do something.
       flagI(mipsLine,stat);            
   }
   printTypeCount(stat);
}

void commandR(_Bool detail,Statistics *stat){
   char mipsLine[11]={'\0'};
   while( scanLine(mipsLine)!=NULL){ // Exists Line. Do something.
        flagR(mipsLine,stat);
        (*stat).count++;        
   }
   printRegCount(stat,detail);
}
void commandO(_Bool detail, Statistics *stat){
   char mipsLine[11]={'\0'};
   while( scanLine(mipsLine)!=NULL){ // Exists Line. Do something.
        flagO(mipsLine,stat);        
   }
   if(detail==1) printf("%s\n",flagOHeader);
   printOpcodeCount(stat);
   if(detail==1) printf("\n%s",functionHeader);
   printFunctionCount(stat);
}
/* Scans line by line, if exists. Else return null. */
char *scanLine(char mipsLine[]){
   if(fscanf(stdin,"%s",mipsLine)!=EOF){
      // we have a line of mips 
      if(isValidHexadecimal(mipsLine) > 0){
        //is valid
        return mipsLine;
     }else{
       fprintf(stderr,"%s\n",mipsLine);
       exit(EXIT_FAILURE);// error invalid
     }
   }else return NULL; // natural end of file
}

           /* Doer Functions */
/* Takes a line of mips, and does the -I Flag on it */
void flagI(char mipsLine[],Statistics *stat){
    unsigned long long longInstruction = convertMipsLongLong(mipsLine); 
    unsigned long long opcode = findOpcodeInLongInstr(longInstruction);
    int resultType = determineTypeFromOpcode(opcode);
    switch(resultType){
      case 0: (*stat).R++;
              break;
      case 1: (*stat).I++;
              break;
      case 2: (*stat).J++;
              break;
      default: break;
    }
}
/* Takes a line of mips, and does the -R Flag on it */
void flagR(char mipsLine[],Statistics *stat){
    unsigned long long longInstruction = convertMipsLongLong(mipsLine); 
    unsigned long long opcode = findOpcodeInLongInstr(longInstruction);
    int resultType = determineTypeFromOpcode(opcode);
    if(resultType==0){
      countRegistersR_Type(longInstruction,stat);
    }else if(resultType==1){
      countRegistersI_Type(longInstruction,stat);
    }        

}
/* Takes a line of mips, and does the -O Flag on it */
void flagO(char mipsLine[],Statistics *stat){
    unsigned long long longInstruction = convertMipsLongLong(mipsLine); 
    unsigned long long opcode = findOpcodeInLongInstr(longInstruction);
    opcodeCount(opcode,stat);
    int resultType = determineTypeFromOpcode(opcode);
    if(resultType==1){ countFunctions_RType(longInstruction,stat);}
    
}


         /*Basic Helper Register Functions*/
/* Takes a line of hex mips and gives you the Unsigned long long*/ 
unsigned long long convertMipsLongLong(char mipsString[]){
    return strtoull(mipsString,NULL,16);
}
/* Takes a long long and finds the RS Bits value */
unsigned long long  findRsValue(unsigned long long num){
    return ((num<<38)>>59);
}
/* Takes a long long and finds the Rt Bits Value */
unsigned long long findRtValue(unsigned long long num){
    return ((num<<43)>>59);
}
/* Takes a long long and finds the Rd Bits Value */
unsigned long long findRdValue(unsigned long long num){
    return ((num<<48)>>59);
}


         /*Basic Helper Opcode Functions*/

/*Finds the opcode of an unsigned long long */
unsigned long long findOpcodeInLongInstr(unsigned long long num){
    return((num<<32)>>58);
}
/*Finds the type when given an shifted-to-right Opcode value. R,I,J.*/
int determineTypeFromOpcode(unsigned long long opcode){
    if(opcode ==0) return 0;// 0 = R Type;
    else if((opcode ==2) |(opcode==3)) return 2; //2 = J Type. 
    else if (opcode > 0) return 1; // 1 = I type.
    else return -1; // means its a neg opcode. return error. 
}
/*Records the opCode's count in the Opcode Array*/
void opcodeCount(unsigned long long actualOpcodeValue,Statistics *stat){
    (*stat).opCount[actualOpcodeValue]++;
}

              /*Instruction Type Specific Functions*/

/* Given an I-Type format, search for and record the register used from that format.*/
void countRegistersI_Type( unsigned long long num, Statistics *stat){    
    int *regCountI_Type = (*stat).regCountI_Type;
    regCountI_Type[findRsValue(num)]++;
    regCountI_Type[findRtValue(num)]++;
    
    (*stat).regCount[findRsValue(num)]++;
    (*stat).regCount[findRtValue(num)]++;
}
/* Given an R-Type format, search for and record the register used from that format.*/
void countRegistersR_Type( unsigned long long num, Statistics *stat){     
   
    int *regCountR_Type = (*stat).regCountR_Type;
    regCountR_Type[findRsValue(num)]++;
    regCountR_Type[findRtValue(num)]++;
    regCountR_Type[findRdValue(num)]++;
    
    // Update Total Use 
    (*stat).regCount[findRsValue(num)]++;
    (*stat).regCount[findRtValue(num)]++;
    (*stat).regCount[findRdValue(num)]++;
}
/* Given an R-Type format, search for and record the functions used from that format.*/
void countFunctions_RType(unsigned long long num,Statistics *stat){
    // clear the first few bits.
    unsigned long long functionNum = ((num<<(58))>>(58));
    (*stat).functionCount[functionNum]++;
}



          /*Printer Helper Functions*/
void printTypeCount(Statistics *stat){
    
    double sum = (*stat).R+(*stat).I+(*stat).J;
    double rSum = (*stat).R/sum*100;
    double iSum = (*stat).I/sum*100;
    double jSum = (*stat).J/sum*100;
    if(sum==0) sum =1;
    printf("I-Type\t%d\t%.1f%%\n",(*stat).I,iSum);
    printf("J-Type\t%d\t%.1f%%\n",(*stat).J,jSum);    
    printf("R-Type\t%d\t%.1f%%\n",(*stat).R,rSum);
       
}
/* Prints the values of the register array */
void printRegCount(Statistics *stat,_Bool detail){
    int i; // counter 
    double instrSum=0;//number of instructions parsed from stdin
    instrSum = (*stat).count; 
      if(instrSum==0) instrSum=1; // avoid dividing by zero

    if(detail==0){
        for(i=0;i<32;i++){
         printf("$%d\t%d\t%d\t%d\t%d\t%.1f%%\n",i,(*stat).regCount[i],
         (*stat).regCountR_Type[i],(*stat).regCountI_Type[i],
         (*stat).regCountJ_Type[i],(100*((*stat).regCount[i])/instrSum));
      } 
    }else if(detail>0){
      printf("%s\n",flagRHeader);
      for(i=0;i<32;i++){
         printf("%s\t%d\t%d\t%d\t%d\t%.1f%%\n",(*stat).regNames[i],(*stat).regCount[i],
         (*stat).regCountR_Type[i],(*stat).regCountI_Type[i],
         (*stat).regCountJ_Type[i],(100*((*stat).regCount[i])/instrSum));
      }       
    }
}
/* Prints the value of the opcode array */
void printOpcodeCount(Statistics *stat){
    int i; // counter
    double opcodeSum=0;
    for(i=0;i<64;i++) opcodeSum += (*stat).opCount[i];    
    if(opcodeSum==0) opcodeSum=1; // avoid dividing by zero
    // First line is special to print out 0x0 instead of just 0
    printf("%s\t%d\t%.1f%%\n","0x0",(*stat).opCount[0],(100*((*stat)
       .opCount[0]/opcodeSum)));
    // Print out regular iteration
    for(i=1;i<16;i++){
       printf("%#3x\t%d\t%.1f%%\n",i,(*stat).opCount[i],(100*((*stat)
       .opCount[i]/opcodeSum)));
    }
    for(i=16;i<64;i++){
       printf("%#4x\t%d\t%.1f%%\n",i,(*stat).opCount[i],(100*((*stat)
       .opCount[i]/opcodeSum)));
    } 
}
/* Prints the value of the function count array */
void printFunctionCount(Statistics *stat){
    int i; // counter 
    double functionSum=0;
    for(i=0;i<64;i++) functionSum += (*stat).functionCount[i];    
    if(functionSum==0) functionSum=1; // avoid dividing by zero
    // First line is special to print out 0x0 instead of just 0
    printf("\n%s\t%d\t%.1f%%\n","0x0",(*stat).functionCount[0]
       ,(((*stat).functionCount[0]/functionSum)*100));
    // Print out regular iteration
    for(i=1;i<16;i++){
       printf("%#3x\t%d\t%.1f%%\n",i,(*stat).functionCount[i]
       ,(((*stat).functionCount[i]/functionSum)*100));
    }
    for(i=16;i<64;i++){
       printf("%#4x\t%d\t%.1f%%\n",i,(*stat).functionCount[i]
       ,(((*stat).functionCount[i]/functionSum)*100));
    }   

}

int isValidHexadecimal(char mipsLine[]){
    if(strlen(mipsLine)!=10)return 0;
    else{
      int i;
      for(i=0;i<10;i++){
        char ch = mipsLine[i];
        int chVal = ch +0;
        if(chVal<48) return 0;
        else if((chVal>57)&(chVal<65))return 0;
        else if((chVal>70)&(chVal<97)){
          if(chVal!=88)
             return 0;
        }
        else if((chVal>102)&(chVal!=120))return 0;
      }
    }
    return 1;
}
/*if(mipsLine[0]!='0')return 0;
    else if((mipsLine[1]!='X')|(mipsLine[1]!='x')) return 0;
    else {
      int i;
      for(i=0;i<10;i++){
        int value = mipsLine[i]+'0';
        if((value >= 48) & (value <= 57)) return 1;
        else if ((value >= 65) & (value <= 70)) return 1;
        else if ((value >= 97) & (value <= 102)) return 1;
        else return 0;
      }
    }
    return 1;
*/

