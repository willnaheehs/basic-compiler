/*
   CONSTFOLDING.C : THIS FILE IMPLEMENTS THE CONSTANT FOLDING OPTIMIZATION
*/

#include "constfolding.h"
/*
*************************************************************************************
   YOUR CODE IS TO BE FILLED IN THE GIVEN TODO BLANKS. YOU CAN CHOOSE TO USE ALL 
   UTILITY FUNCTIONS OR NONE. YOU CAN ADD NEW FUNCTIONS. BUT DO NOT FORGET TO 
   DECLARE THEM IN THE HEADER FILE.
**************************************************************************************
*/                                                                                                          
bool madeChange = false;

/*
******************************************************************************************
FUNCTION TO CALCULATE THE CONSTANT EXPRESSION VALUE 
OBSERVE THAT THIS IMPLEMENTATION CONSIDERS ADDITIONAL OPTIMIZATIONS SUCH AS:
1.  IDENTITY MULTIPLY = 1 * ANY_VALUE = ANY_VALUE - AVOID MULTIPLICATION CYCLE IN THIS CASE
2.  ZERO MULTIPLY = 0 * ANY_VALUE = 0 - AVOID MULTIPLICATION CYCLE
3.  DIVIDE BY ONE = ORIGINAL_VALUE - AVOID DIVISION CYCLE
4.  SUBTRACT BY ZERO = ORIGINAL_VALUE - AVOID SUBTRACTION
5.  MULTIPLICATION BY 2 = ADDITION BY SAME VALUE [STRENGTH REDUCTION]
******************************************************************************************
*/
long CalcExprValue(Node* node)
{
     long result;
     Node *leftNode, *rightNode;
     leftNode = node->left;
     rightNode = node->right; 
     switch(node->opCode){
         case MULTIPLY:
             if(leftNode->value == 1) {
                 result = rightNode->value;
             } 
             else if(rightNode->value == 1) {
                 result = leftNode->value;
             }
             else if(leftNode->value == 0 || rightNode->value == 0) {
                 result = 0;
             }
             else if(leftNode->value == 2) {
                 result = rightNode->value + rightNode->value;
             }              
             else if(rightNode->value == 2) {
                 result = leftNode->value + leftNode->value;
             }
             else {
                 result = leftNode->value * rightNode->value;
             }
             break;
         case DIVIDE:
             if(rightNode->value == 1) {
                 result = leftNode->value;
             }
             else {
                 result = leftNode->value / rightNode->value;
             }
             break;
         case ADD:
             result = leftNode->value + rightNode->value;
             break;
         case SUBTRACT:
             result = leftNode->value - rightNode->value;
             break;
         case NEGATE:
             result = -leftNode->value;
             break;
         default:
             break;
     }
     return result;
}


/*
**********************************************************************************************************************************
// YOU CAN MAKE CHANGES/ADD AUXILLIARY FUNCTIONS BELOW THIS LINE
**********************************************************************************************************************************
*/

/*
*****************************************************************************************************
THIS FUNCTION IS MEANT TO PROCESS THE CANDIDATE STATEMENTS AND PERFORM CONSTANT FOLDING 
WHEREVER APPLICABLE.
******************************************************************************************************
*/
long ConstFoldPerStatement(Node* stmtNodeRight){

    long result = -1;  // default value (no folding)

    // ensure the node is an operation
    if (stmtNodeRight->exprCode == OPERATION) {
        Node *leftNode = stmtNodeRight->left;
        Node *rightNode = stmtNodeRight->right;

        // check if left and right nodes are constants
        if (leftNode && leftNode->exprCode == CONSTANT && 
            (rightNode == NULL || rightNode->exprCode == CONSTANT)) {
            result = CalcExprValue(stmtNodeRight);
        }
    }

    return result;  // return folded result or -1 if no folding
}


/*
*****************************************************************************************************
THIS FUNCTION IS MEANT TO IDENTIFY THE STATEMENTS THAT ARE ACTUAL CANDIDATES FOR CONSTANT FOLDING
AND CALL THE APPROPRIATE FUNCTION FOR THE IDENTIFIED CANDIDATE'S CONSTANT FOLDING
******************************************************************************************************
*/
void ConstFoldPerFunction(Node* funcNode) {
      Node *rightNode, *leftNode, *stmtNodeRight;
      long result;
      NodeList* statements = funcNode->statements;
      while(statements != NULL) {
        // get current statement node
        Node* statement_node = statements->node;

        // check that statement is an assignement
        if (statement_node->stmtCode == ASSIGN){
            // get right side of the assignement    
            stmtNodeRight = statement_node->right;


            // ensure rhs is an expression that could be folded
            if (stmtNodeRight != NULL && stmtNodeRight->exprCode == OPERATION) {
                // fold constants on the rhs
                result = ConstFoldPerStatement(stmtNodeRight);
                
                
                // if constant folding happened -> update the value of the statement's right-hand side
                if (result != -1) {
                    // update rhs side of the statement to folded result
                    stmtNodeRight->value = result;
                    stmtNodeRight->exprCode = CONSTANT;  
                    stmtNodeRight->left = stmtNodeRight->right = NULL;  // clear children 
                }
            }
        }
                                                                                                                                     
	  statements = statements->next;
      }
     
}



/*
**********************************************************************************************************************************
// YOU CAN MAKE CHANGES/ADD AUXILLIARY FUNCTIONS ABOVE THIS LINE
********************************************************************************************************************************
*/

/*
*****************************************************************************************************
THIS FUNCTION ENSURES THAT THE CONSTANT FOLDING OPTIMIZATION IS DONE FOR EVERY FUNCTION IN THE PROGRAM
******************************************************************************************************
*/

bool ConstantFolding(NodeList* list) {
    madeChange = false;
    while(list != NULL) {
        // get the declaration
        Node* function_node = list->node;
        // make sure the node is a declaration
        if(function_node->type == FUNCTIONDECL){
            ConstFoldPerFunction(function_node);
            madeChange = true;
        }

	    list = list->next;
    }
    return madeChange;
}

/*
****************************************************************************************************************************
 END OF CONSTANT FOLDING
*****************************************************************************************************************************
*/                
