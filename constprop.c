/*
********************************************************************************
  CONSTPROP.C : IMPLEMENT THE DOWNSTREAM CONSTANT PROPOGATION OPTIMIZATION HERE
*********************************************************************************
*/

#include "constprop.h"

refConst *lastNode, *headNode;
/*
*************************************************************************************
   YOUR CODE IS TO BE FILLED IN THE GIVEN TODO BLANKS. YOU CAN CHOOSE TO USE ALL
   UTILITY FUNCTIONS OR NONE. YOU CAN ADD NEW FUNCTIONS. BUT DO NOT FORGET TO
   DECLARE THEM IN THE HEADER FILE.
**************************************************************************************
*/

/*
***********************************************************************
  FUNCTION TO FREE THE CONSTANTS-ASSOCIATED VARIABLES LIST
************************************************************************
*/
void FreeConstList()
{
   refConst* tmp;
   while (headNode != NULL)
    {
       tmp = headNode;
       headNode = headNode->next;
       free(tmp);
    }

}

/*
*************************************************************************
  FUNCTION TO ADD A CONSTANT VALUE AND THE ASSOCIATED VARIABLE TO THE LIST
**************************************************************************
*/
void UpdateConstList(char* name, long val) {
    refConst* node = malloc(sizeof(refConst));
    if (node == NULL) return;
    node->name = name;
    node->val = val;
    node->next = NULL;
    if(headNode == NULL) {
        lastNode = node;
        headNode = node;
    }
    else {
        lastNode->next = node;
        lastNode = node;
    }
}

/*
*****************************************************************************
  FUNCTION TO LOOKUP IF A CONSTANT ASSOCIATED VARIABLE IS ALREADY IN THE LIST
******************************************************************************
*/
refConst* LookupConstList(char* name) {
    refConst *node;
    node = headNode; 
    while(node!=NULL){
        if(!strcmp(name, node->name))
            return node;
        node = node->next;
    }
    return NULL;
}

/*
**********************************************************************************************************************************
 YOU CAN MAKE CHANGES/ADD AUXILLIARY FUNCTIONS BELOW THIS LINE
**********************************************************************************************************************************
*/

// for debugging
void PrintConstList() {
    refConst* current = headNode;  // start at the head of the constant list

    if (current == NULL) {
        printf("const list empty.\n");
        return;
    }

    printf("constant list:\n");
    while (current != NULL) {
        printf("variable name: %s, constant value: %ld\n", current->name, current->val);
        current = current->next;
    }
}


/*
************************************************************************************
  THIS FUNCTION IS MEANT TO UPDATE THE CONSTANT LIST WITH THE ASSOCIATED VARIABLE
  AND CONSTANT VALUE WHEN ONE IS SEEN. IT SHOULD ALSO PROPOGATE THE CONSTANTS WHEN 
  WHEN APPLICABLE. YOU CAN ADD A NEW FUNCTION IF YOU WISH TO MODULARIZE BETTER.
*************************************************************************************
*/
void TrackConst(NodeList* statements) {
    Node* node;
    while(statements != NULL) {
         node = statements->node;

        // check if node is an assignment
        if (node->stmtCode == ASSIGN) {
            // create the lhs node using hiven helper function (for some reason it doesnt work otherwise?)
            Node* lhs = CreateVariable(node->name, NULL); 

            // get rhs expression
            Node* rhs = node->right;

            // check if rhs is a constant expression
            if (rhs != NULL && rhs->exprCode == CONSTANT) {
                // update constant list with the new var and val
                UpdateConstList(lhs->name, rhs->value);
            }
        }
        statements = statements->next;
    }
}

void PropagateConstants(Node* expr) {
    if (expr == NULL) return;

    // if the expression is a variable -> try to replace with a constant from the list
    if (expr->exprCode == VARIABLE) {
        // check if the variable is a function parameter (don't want to replace)
        if (expr->type == EXPRESSION && expr->exprCode == PARAMETER) {
            return;
        }

        // look for the variable in the constant list
        refConst* constEntry = LookupConstList(expr->name);
        if (constEntry != NULL) {
            // replace the variable with constant value
            expr->exprCode = CONSTANT;
            expr->value = constEntry->val;
        }
    }

    // if the expression is a function call -> propagate constants into the arguments
    if (expr->opCode == FUNCTIONCALL){
        Node* functionDecl = expr->left; 
        NodeList* arguments = expr->arguments;  // the args passed in the function call

        // iterate through arguments and propagate constants
        NodeList* arg_list = arguments;

        while (arg_list != NULL){
            Node* arg = arg_list->node;

            // propagate constants within the arg
            PropagateConstants(arg);

            arg_list = arg_list->next;
        }

        // constant propagation for the body of the function
        PropagateConstants(functionDecl->statements->node);
    }

    // recursively propagate constants in left and right nodes
    PropagateConstants(expr->left);
    PropagateConstants(expr->right);
}

// propogate per statement (takes a nodelist of statements of a function)
bool ConstProp(NodeList* worklist) {
    bool madeChange = false;

    // loop through 
    while (worklist != NULL) {
        Node* stmt = worklist->node;
        //printf("entered while loop\n");
        // check if the statement is an assignment
        if (stmt->stmtCode == ASSIGN) {
            Node* lhs = CreateVariable(stmt->name, stmt->left);   // variable being assigned (once again, need to use this helper to create the function)
            Node* rhs = stmt->right;  // epression on the rhs

            // if rhs is a constant -> update the constant list
            if (rhs->exprCode == CONSTANT) {
                //printf("Propagating constant: %s = %ld\n", lhs->name, rhs->value);
                UpdateConstList(lhs->name, rhs->value);
            }

            // if rhs is an expression -> propagate constants
            else {
                PropagateConstants(rhs);
                madeChange = true;
            }
        }
        
        // if the statement is a function call -> propagate constants into the called function
        if (stmt->opCode == FUNCTIONCALL) {
            PropagateConstants(stmt);
            madeChange = true;
        }

        // if the statement is a return -> propagate constants in the expression
        if (stmt->stmtCode == RETURN) {
            PropagateConstants(stmt->left);
            madeChange = true;
        }


        worklist = worklist->next;
    }
    FreeConstList();
    // PrintConstList();
    //printf("exited ConstProp\n");
    return madeChange;
}


/*
**********************************************************************************************************************************
 YOU CAN MAKE CHANGES/ADD AUXILLIARY FUNCTIONS ABOVE THIS LINE
**********************************************************************************************************************************
*/
