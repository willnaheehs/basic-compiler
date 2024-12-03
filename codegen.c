/*
***********************************************************************
  CODEGEN.C : IMPLEMENT CODE GENERATION HERE
************************************************************************
*/
#include "codegen.h"

int argCounter;
int lastUsedOffset;
char lastOffsetUsed[100];
FILE *fptr;
regInfo *regList, *regHead, *regLast;
varStoreInfo *varList, *varHead, *varLast;

/*
*************************************************************************************
   YOUR CODE IS TO BE FILLED IN THE GIVEN TODO BLANKS. YOU CAN CHOOSE TO USE ALL
   UTILITY FUNCTIONS OR NONE. YOU CAN ADD NEW FUNCTIONS. BUT DO NOT FORGET TO
   DECLARE THEM IN THE HEADER FILE.
**************************************************************************************
*/

/*
***********************************************************************
  FUNCTION TO INITIALIZE THE ASSEMBLY FILE WITH FUNCTION DETAILS
************************************************************************
*/
void InitAsm(char* funcName) {
    fprintf(fptr, "\n.globl %s", funcName);
    fprintf(fptr, "\n%s:", funcName); 

    // Init stack and base ptr
    fprintf(fptr, "\npushq %%rbp");  
    fprintf(fptr, "\nmovq %%rsp, %%rbp"); 
}

/*
***************************************************************************
   FUNCTION TO WRITE THE RETURNING CODE OF A FUNCTION IN THE ASSEMBLY FILE
****************************************************************************
*/
void RetAsm() {
    fprintf(fptr,"\npopq  %%rbp");
    fprintf(fptr, "\nretq\n");
} 

/*
***************************************************************************
  FUNCTION TO CONVERT OFFSET FROM LONG TO CHAR STRING 
****************************************************************************
*/
void LongToCharOffset() {
     lastUsedOffset = lastUsedOffset - 8;
     snprintf(lastOffsetUsed, 100,"%d", lastUsedOffset);
     strcat(lastOffsetUsed,"(%rbp)");
}

/*
***************************************************************************
  FUNCTION TO CONVERT CONSTANT VALUE TO CHAR STRING
****************************************************************************
*/
void ProcessConstant(Node* opNode) {
     char value[10];
     LongToCharOffset();
     snprintf(value, 10,"%ld", opNode->value);
     char str[100];
     snprintf(str, 100,"%d", lastUsedOffset);
     strcat(str,"(%rbp)");
     AddVarInfo("", str, opNode->value, true);
     fprintf(fptr, "\nmovq  $%s, %s", value, str);
}

/*
***************************************************************************
  FUNCTION TO SAVE VALUE IN ACCUMULATOR (RAX)
****************************************************************************
*/
void SaveValInRax(char* name) {
    char *tempReg;
    tempReg = GetNextAvailReg(true);
    if(!(strcmp(tempReg, "NoReg"))) {
        LongToCharOffset();
        fprintf(fptr, "\n movq %%rax, %s", lastOffsetUsed);
        UpdateVarInfo(name, lastOffsetUsed, INVAL, false);
        UpdateRegInfo("%rax", 1);
    }
    else {
        fprintf(fptr, "\nmovq %%rax, %s", tempReg);
        UpdateRegInfo(tempReg, 0);
        UpdateVarInfo(name, tempReg, INVAL, false);
        UpdateRegInfo("%rax", 1);
    }
}



/*
***********************************************************************
  FUNCTION TO ADD VARIABLE INFORMATION TO THE VARIABLE INFO LIST
************************************************************************
*/
void AddVarInfo(char* varName, char* location, long val, bool isConst) {
   varStoreInfo* node = malloc(sizeof(varStoreInfo));
   node->varName = varName;
   node->value = val;
   strcpy(node->location,location);
   node->isConst = isConst;
   node->next = NULL;
   node->prev = varLast;
   if(varHead==NULL) {
       varHead = node;
       varLast = node;;
       varList = node;
   } else {
       //node->prev = varLast;
       varLast->next = node;
       varLast = varLast->next;
   }
   varList = varHead;
}

/*
***********************************************************************
  FUNCTION TO FREE THE VARIABLE INFORMATION LIST
************************************************************************
*/
void FreeVarList()
{  
   varStoreInfo* tmp;
   while (varHead != NULL)
    {  
       tmp = varHead;
       varHead = varHead->next;
       free(tmp);
    }

}

/*
***********************************************************************
  FUNCTION TO LOOKUP VARIABLE INFORMATION FROM THE VARINFO LIST
************************************************************************
*/
char* LookUpVarInfo(char* name, long val) {
    varList = varLast;
    if(varList == NULL) printf("NULL varlist");
    while(varList!=NULL) {
        if(varList->isConst == true) {
            if(varList->value == val) return varList->location;
        }
        else {
            if(!strcmp(name,varList->varName)) return varList->location;
        }
        varList = varList->prev;
    }
    varList = varHead;
    return "";
}

/*
***********************************************************************
  FUNCTION TO UPDATE VARIABLE INFORMATION 
************************************************************************
*/
void UpdateVarInfo(char* varName, char* location, long val, bool isConst) {
  
   if(!(strcmp(LookUpVarInfo(varName, val), ""))) {
       AddVarInfo(varName, location, val, isConst);
   }
   else {
       varList = varHead;
       if(varList == NULL) printf("NULL varlist");
       while(varList!=NULL) {
           if(!strcmp(varList->varName,varName)) {
               varList->value = val;
               strcpy(varList->location,location);
               varList->isConst = isConst;
               break;
        }
        varList = varList->next;
       }
    }
    varList = varHead;
}

/*
***********************************************************************
  FUNCTION TO PRINT THE VARIABLE INFORMATION LIST
************************************************************************
*/
void PrintVarListInfo() {
    varList = varHead;
    if(varList == NULL) printf("NULL varlist");
    while(varList!=NULL) {
        if(!varList->isConst) {
            printf("\t %s : %s", varList->varName, varList->location);
        }
        else {
            printf("\t %ld : %s", varList->value, varList->location);
        }
        varList = varList->next;
    }
    varList = varHead;
}

/*
***********************************************************************
  FUNCTION TO ADD NEW REGISTER INFORMATION TO THE REGISTER INFO LIST
************************************************************************
*/
void AddRegInfo(char* name, int avail) {

   regInfo* node = malloc(sizeof(regInfo));
   node->regName = name;
   node->avail = avail;
   node->next = NULL; 

   if(regHead==NULL) {
       regHead = node;
       regList = node;
       regLast = node;
   } else {
       regLast->next = node;
       regLast = node;
   }
   regList = regHead;
}

/*
***********************************************************************
  FUNCTION TO FREE REGISTER INFORMATION LIST
************************************************************************
*/
void FreeRegList()
{  
   regInfo* tmp;
   while (regHead != NULL)
    {  
       tmp = regHead;
       regHead = regHead->next;
       free(tmp);
    }
    varLast=NULL;
}

/*
***********************************************************************
  FUNCTION TO UPDATE THE AVAILIBILITY OF REGISTERS IN THE REG INFO LIST
************************************************************************
*/
void UpdateRegInfo(char* regName, int avail) {
    while(regList!=NULL) {
        if(regName == regList->regName) {
            regList->avail = avail;
        }
        regList = regList->next;
    }
    regList = regHead;
}

/*
***********************************************************************
  FUNCTION TO RETURN THE NEXT AVAILABLE REGISTER
************************************************************************
*/
char* GetNextAvailReg(bool noAcc) {
    regList = regHead;
    if(regList == NULL) printf("NULL reglist");
    while(regList!=NULL) {
        if(regList->avail == 1) {
            if(!noAcc) return regList->regName;
            // if not rax and dont return accumulator set to true, return the other reg
            // if rax and noAcc == true, skip to next avail
            if(noAcc && strcmp(regList->regName, "%rax")) { 
                return regList->regName;
            }
        }
        regList = regList->next;
    }
    regList = regHead;
    return "NoReg";
}

/*
***********************************************************************
  FUNCTION TO DETERMINE IF ANY REGISTER APART FROM OR INCLUDING 
  THE ACCUMULATOR(RAX) IS AVAILABLE
************************************************************************
*/
int IfAvailReg(bool noAcc) {
    regList = regHead;
    if(regList == NULL) printf("NULL reglist");
    while(regList!=NULL) {
        if(regList->avail == 1) {
            // registers available
            if(!noAcc) return 1;
            if(noAcc && strcmp(regList->regName, "%rax")) {
                return 1;
            }
        }
        regList = regList->next;
    }
    regList = regHead;
    return 0;
}

/*
***********************************************************************
  FUNCTION TO DETERMINE IF A SPECIFIC REGISTER IS AVAILABLE
************************************************************************
*/
bool IsAvailReg(char* name) {
    regList = regHead;
    if(regList == NULL) printf("NULL reglist");
    while(regList!=NULL) {
        if(!strcmp(regList->regName, name)) {
           if(regList->avail == 1) {
               return true;
           } 
        }
        regList = regList->next;
    }
    regList = regHead;
    return false;
}

/*
***********************************************************************
  FUNCTION TO PRINT THE REGISTER INFORMATION
************************************************************************
*/
void PrintRegListInfo() {
    regList = regHead;
    if(regList == NULL) printf("NULL reglist");
    while(regList!=NULL) {
        printf("\t %s : %d", regList->regName, regList->avail);
        regList = regList->next;
    }
    regList = regHead;
}

/*
***********************************************************************
  FUNCTION TO CREATE THE REGISTER LIST
************************************************************************
*/
    // Create the initial reglist which can be used to store variables.
    // 4 general purpose registers : AX, BX, CX, DX
    // 4 special purpose : SP, BP, SI , DI. 
    // Other registers: r8, r9
    // You need to decide which registers you will add in the register list 
    // use. Can you use all of the above registers?
    // general purpose registers (64-bit registers)

    // I do not use this function
void CreateRegList() {
    AddRegInfo("%rax", 1);  // primary accumulator
    AddRegInfo("%rcx", 1);  // secondary register

    // optionally add other registers but mark them as unavailable:
    AddRegInfo("%rbx", 0); 
    AddRegInfo("%rdx", 0);  
    AddRegInfo("%r8", 0);  
    AddRegInfo("%r9", 0); 
}

// move args to the stack (dont push, move to specific location)
// first 6 args located in registers
// at references , searchw  helper, perform operation
// move result from register seperate 
// can only operate in registers -> move operations results from register to new stack location that corresponds to IR
// dont use register list (get next avaialble register)
// just pick two registers RDI RSI and use them for all operations


/*
***********************************************************************
  THIS FUNCTION IS MEANT TO PUT THE FUNCTION ARGUMENTS ON STACK
************************************************************************
*/
int PutArgumentsOnStack(NodeList* arguments) {
    int argIdx = 0;
    while (arguments != NULL) {
        char* reg = NULL;
        char* argName = arguments->node->name; // assume Node has name for the arg

        // assign the right register based on the argument index
        switch (argIdx) {
            case 0: reg = "%rdi"; break;
            case 1: reg = "%rsi"; break;
            case 2: reg = "%rdx"; break;
            case 3: reg = "%rcx"; break;
            case 4: reg = "%r8"; break;
            case 5: reg = "%r9"; break;
            default:
                return -1;
        }

        // move the argument from register to stack
        LongToCharOffset(); // updates lastOffsetUsed to next stack offset
        fprintf(fptr, "\nmovq %s, %s", reg, lastOffsetUsed);

        // add the argument to the varList with its stack offset
        AddVarInfo(argName, lastOffsetUsed, INVAL, false);

        arguments = arguments->next;
        argIdx++;
    }
    return argIdx;
}

int PutArgumentsOnStackFunctionCall(NodeList* arguments) {
    int argIdx = 0;
    while (arguments != NULL) {
        char* reg = NULL;
        char* argName = arguments->node->name;

        // assign the right register based on the argument index
        switch (argIdx) {
            case 0: reg = "%rdi"; break;
            case 1: reg = "%rsi"; break;
            case 2: reg = "%rdx"; break;
            case 3: reg = "%rcx"; break;
            case 4: reg = "%r8"; break;
            case 5: reg = "%r9"; break;
            default:
                return -1;
        }

        char* location = LookUpVarInfo(argName, 0);
        if (location[0] != '\0') {
            fprintf(fptr, "\nmovq %s, %s", location, reg);
        }

        arguments = arguments->next;
        argIdx++;
    }
    return argIdx;
}


/*
*************************************************************************
  THIS FUNCTION IS MEANT TO GET THE FUNCTION ARGUMENTS FROM THE  STACK
**************************************************************************
*/
void GetArgumentsFromStack(NodeList* arguments) {
    int argIdx = 0;
    while (arguments != NULL) {
        LongToCharOffset(); // recompute stack offset for each argument retrival

        if (argIdx == 0) {
            // move argument from stack to rax for processing
            fprintf(fptr, "\nmovq %s, %%rax", lastOffsetUsed);
        } else if (argIdx == 1) {
            // move argument from stack to rcx
            fprintf(fptr, "\nmovq %s, %%rcx", lastOffsetUsed);
        } else {
            // only handling the first two arguments
            printf(" 2 arguments handled\n");
        }

        arguments = arguments->next;
        argIdx++;
    }
}

/*
 ***********************************************************************
  THIS FUNCTION IS MEANT TO PROCESS EACH CODE STATEMENT AND GENERATE 
  ASSEMBLY FOR IT. 
  TIP: YOU CAN MODULARIZE BETTER AND ADD NEW SMALLER FUNCTIONS IF YOU 
  WANT THAT CAN BE CALLED FROM HERE.
 ************************************************************************
 */  
void ProcessStatements(NodeList* statements) {
    while (statements != NULL) {
        Node* stmt = statements->node;

        if (stmt->type == STATEMENT) {
            if (stmt->stmtCode == ASSIGN) {
                // handle assignment statement
                GenerateExpression(stmt->right); // evaluate rhs -> result in rax
                // assign the result to the lhs variable
                LongToCharOffset(); 
                AddVarInfo(stmt->name, lastOffsetUsed, 0, false); 
                fprintf(fptr, "\nmovq %%rax, %s", lastOffsetUsed);  // store the result from rax to the memory location
            } else if (stmt->stmtCode == RETURN) {
                // handle return statement
                GenerateExpression(stmt->left); // evaluate return expression -> result in rax
                RetAsm(); // generate function return
            } 
        }

        statements = statements->next;
    }
}


/*
 ***********************************************************************
  THIS FUNCTION IS MEANT TO DO CODEGEN FOR ALL THE FUNCTIONS IN THE FILE
 ************************************************************************
*/
void Codegen(NodeList* worklist) {
    fptr = fopen("assembly.s", "w+");
    if (fptr == NULL) {
        return;
    }
    
    while (worklist != NULL) {
        Node* func = worklist->node;
        if (func->type == FUNCTIONDECL) {
            // initialize function assembly
            InitAsm(func->name);
            lastUsedOffset = 0;
            // put function arguments on stack
            PutArgumentsOnStack(func->arguments);
            
            // process function body
            ProcessStatements(func->statements);
            
            // FreeVarList();
            // RetAsm();
        }
        
        worklist = worklist->next;
    }
    
    fclose(fptr);
}

/*
**********************************************************************************************************************************
 YOU CAN MAKE ADD AUXILLIARY FUNCTIONS BELOW THIS LINE. DO NOT FORGET TO DECLARE THEM IN THE HEADER
**********************************************************************************************************************************
*/

void GenerateExpression(Node* expr) {
    if (expr->type == EXPRESSION) {
        switch (expr->exprCode) {
            case CONSTANT:
                // handle constant value
                fprintf(fptr, "\nmovq $%ld, %%rax", expr->value);
                break;
            case VARIABLE:
                // handle variable (load from memory to rax)
                char* varLocation = LookUpVarInfo(expr->name, INVAL);
                if (varLocation == NULL || strlen(varLocation) == 0) {
                    printf("variable location for %s is not set?\n", expr->name);
                } else {
                    fprintf(fptr, "\nmovq %s, %%rax", varLocation);
                }
                break;
            case OPERATION:
                if (expr->opCode == FUNCTIONCALL) {
                    HandleFunctionCall(expr);
                    return;
                }

                GenerateExpression(expr->left); // moves left arg into rax
                // mov rax to rdi
                fprintf(fptr, "\nmovq %%rax, %%rdi");  
                // if this is not a negate
                if (expr->right){
                    GenerateExpression(expr->right); // moves right arg into rax
                    // mov rax to rsi
                    fprintf(fptr, "\nmovq %%rax, %%rsi"); 
                }

                switch(expr->opCode) {
                    case ADD:
                        fprintf(fptr, "\nmovq %%rdi, %%rax");  // load arg1 into rax
                        fprintf(fptr, "\naddq %%rsi, %%rax");  // add arg2 to rax
                        break;
                    case MULTIPLY:
                        fprintf(fptr, "\nmovq %%rdi, %%rax");  // load arg1 into rax
                        fprintf(fptr, "\nimulq %%rsi");  
                        break;
                    case DIVIDE:
                        // mov left arg into rax
                        fprintf(fptr, "\nmovq %%rdi, %%rax");  // load arg1 into rax
                        fprintf(fptr, "\ncqto");          // sign extend rax into rdx
                        fprintf(fptr, "\nidivq %%rsi");   // perform division
                        break;
                    case SUBTRACT:
                        fprintf(fptr, "\nmovq %%rdi, %%rax");  // load arg1 into rax
                        fprintf(fptr, "\nsubq %%rsi, %%rax");  // add arg2 to rax
                        break;
                    case NEGATE:
                        fprintf(fptr, "\nmovq %%rdi, %%rax");  // load arg1 into rax
                        fprintf(fptr, "\nnegq %%rax");
                        break;
                    case BSHR: // rsi is right, rdi is left
                        fprintf(fptr, "\nmovq $%ld, %%rcx", expr->right->value);  // move the shift amount to rcx
                        fprintf(fptr, "\nsar %%cl, %%rdi");  // shift rdi right by the amount in cl (which is rcx)
                        fprintf(fptr, "\nmovq %%rdi, %%rax");  // move the result to rax
                        break;
                    case BOR:
                        fprintf(fptr, "\norq %%rsi, %%rdi");
                        fprintf(fptr, "\nmovq %%rdi, %%rax"); // dividen into rax
                        break;
                    case BAND:
                        fprintf(fptr, "\nandq %%rsi, %%rdi");
                        fprintf(fptr, "\nmovq %%rdi, %%rax"); 
                        break;
                    case BXOR:
                        fprintf(fptr, "\nxorq %%rsi, %%rdi");
                        fprintf(fptr, "\nmovq %%rdi, %%rax"); 
                        break;
                    case BSHL:
                        fprintf(fptr, "\nmovq $%ld, %%rcx", expr->right->value); 
                        fprintf(fptr, "\nshl %%cl, %%rdi");  
                        fprintf(fptr, "\nmovq %%rdi, %%rax"); 
                        break;
                }
                    break;
                    default:
                    break;
            }
        }
}


void HandleFunctionCall(Node* funcCallNode) {

    NodeList* args = funcCallNode->arguments;

    // put function call arguments on stack
    PutArgumentsOnStackFunctionCall(args);
    // GenerateExpression(args->node); // evaluate the argument -> result in rax

    // call the function
    fprintf(fptr, "\ncallq %s", funcCallNode->left->name); 
}


/*
**********************************************************************************************************************************
 YOU CAN MAKE ADD AUXILLIARY FUNCTIONS ABOVE THIS LINE. DO NOT FORGET TO DECLARE THEM IN THE HEADER
**********************************************************************************************************************************
*/


