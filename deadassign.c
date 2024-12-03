/*
***********************************************************************
  DEADASSIGN.C : IMPLEMENT THE DEAD CODE ELIMINATION OPTIMIZATION HERE
************************************************************************
*/

#include "deadassign.h"

int change;
refVar *last, *head;

/*
*************************************************************************************
   YOUR CODE IS TO BE FILLED IN THE GIVEN TODO BLANKS. YOU CAN CHOOSE TO USE ALL
   UTILITY FUNCTIONS OR NONE. YOU CAN ADD NEW FUNCTIONS. BUT DO NOT FORGET TO
   DECLARE THEM IN THE HEADER FILE.
**************************************************************************************
*/

/*
***********************************************************************
  FUNCTION TO INITIALIZE HEAD AND LAST POINTERS FOR THE REFERENCED 
  VARIABLE LIST.
************************************************************************
*/

void init()
{ 
    head = NULL;
    last = head;
}

/*
***********************************************************************
  FUNCTION TO FREE THE REFERENCED VARIABLE LIST
************************************************************************
*/

void FreeList()
{
   refVar* tmp;
   while (head != NULL)
    {
       tmp = head;
       head = head->next;
       free(tmp);
    }

}

/*
***********************************************************************
  FUNCTION TO IDENTIFY IF A VARIABLE'S REFERENCE IS ALREADY TRACKED
************************************************************************
*/
bool VarExists(char* name) {
   refVar *node;
   node = head;
   while(node != NULL) {
       if(!strcmp(name, node->name)) {
           return true;
       }
       node = node->next;
    }
    return false;
}

/*
***********************************************************************
  FUNCTION TO ADD A REFERENCE TO THE REFERENCE LIST
************************************************************************
*/
void UpdateRefVarList(char* name) {
    refVar* node = malloc(sizeof(refVar));
    if (node == NULL) return;
    node->name = name;
    node->next = NULL;
    if(head == NULL) {
        last = node;
        head = node;
    }
    else {
        last->next = node;
        last = node;
    }
}

/*
****************************************************************************
  FUNCTION TO PRINT OUT THE LIST TO SEE ALL VARIABLES THAT ARE USED/REFERRED
  AFTER THEIR ASSIGNMENT. YOU CAN USE THIS FOR DEBUGGING PURPOSES OR TO CHECK
  IF YOUR LIST IS GETTING UPDATED CORRECTLY
******************************************************************************
*/
void PrintRefVarList() {
    refVar *node;
    node = head;
    if(node==NULL) {
        printf("\nList is empty"); 
        return;
    }
    while(node != NULL) {
        printf("\t %s", node->name);
        node = node->next;
    }
}

/*
***********************************************************************
  FUNCTION TO UPDATE THE REFERENCE LIST WHEN A VARIABLE IS REFERENCED 
  IF NOT DONE SO ALREADY.
************************************************************************
*/
void UpdateRef(Node* node) {
      if(node->right != NULL && node->right->exprCode == VARIABLE) {
          if(!VarExists(node->right->name)) {
              UpdateRefVarList(node->right->name);
          }
      }
      if(node->left != NULL && node->left->exprCode == VARIABLE) {
          if(!VarExists(node->left->name)) {
              UpdateRefVarList(node->left->name);
          }
      }
}

/*
**********************************************************************************************************************************
 YOU CAN MAKE CHANGES/ADD AUXILLIARY FUNCTIONS BELOW THIS LINE
**********************************************************************************************************************************
*/


/*
********************************************************************
  THIS FUNCTION IS MEANT TO TRACK THE REFERENCES OF EACH VARIABLE
  TO HELP DETERMINE IF IT WAS USED OR NOT LATER
********************************************************************
*/

// start with thte return statement variable, add it to referenced list
// add all of the vars in the lines containing the returned var to the list
// add all of the vars in lines containing vars from the list to the list
// remove all other lines as they are dead code


//add all RHS vars to list (all declerations)
// when removing check for the lhs in the list, if not then remove
// function call case



void TrackRef(Node* funcNode) {
    NodeList* statements = funcNode->statements;
    Node* node;

    while (statements != NULL) {
        node = statements->node;
        // track variables in assignments
        if (node->stmtCode == ASSIGN) {
          if (node->right->exprCode == VARIABLE){
              UpdateRef(node);
              // PrintRefVarList();
          } else if (node->right->exprCode == OPERATION){
              UpdateRef(node->right);
              // PrintRefVarList();
            } 
        } else if (node->stmtCode == RETURN){
            if (node->left->exprCode == VARIABLE){
              UpdateRef(node);
            }
        }

        // if we encounter function call -> add the parameters to the list
        // so that we dont delete the line
        if (node->right != NULL && node->right->opCode == FUNCTIONCALL){
          NodeList* arg_node = node->right->arguments;

          while(arg_node != NULL){  
              UpdateRefVarList(arg_node->node->name);
              arg_node = arg_node->next;
          }

        }

        statements = statements->next;
    }

}


/*
***************************************************************
  THIS FUNCTION IS MEANT TO DO THE ACTUAL DEADCODE REMOVAL
  BASED ON THE INFORMATION OF TRACKED REFERENCES
****************************************************************
*/

// when i have assignment, check if var on lhs exists in list (name of node)
// if not remove it from linked list (remove node and free it)


// remove dead code by checking if LHS variables are ever used
NodeList* RemoveDead(NodeList* statements) {
    NodeList* prev = NULL;
    NodeList* current = statements;
    NodeList* head = statements;

    while (current != NULL) {
        Node* node = current->node;
      
        // if this is an assignment -> check if the lhs variable is ever used
        if (node->stmtCode == ASSIGN || node->opCode == FUNCTIONCALL) {
            char* lhsVar = node->name;

            // if the lhs variable is not used in any rhs -> dead code
            if (!VarExists(lhsVar) || node->right->exprCode == CONSTANT) {
                // printf("Removing dead assignment: %s\n", lhsVar);

                // remove the current node
                if (prev != NULL) {
                    prev->next = current->next;
                } else {
                    head = current->next;  // update head if first node is removed
                }

                // free the current node
                NodeList* temp = current;
                current = current->next;
                free(temp);
                continue;  // skip to the next iteration
            }
        }

        prev = current;
        current = current->next;
    }

    return head;  // return updated list
}


/*
********************************************************************
  THIS FUNCTION SHOULD ENSURE THAT THE DEAD CODE REMOVAL PROCESS
  OCCURS CORRECTLY FOR ALL THE FUNCTIONS IN THE PROGRAM
********************************************************************
*/


bool DeadAssign(NodeList* worklist) {
    bool madeChange = false;
    change = 0;

    while (worklist != NULL) {
        Node* func = worklist->node;

        // track all rhs references in the function
        TrackRef(func);

        // remove dead assignments (variables assigned but never used)
        func->statements = RemoveDead(func->statements);

        // free reference list to avoid memory leaks
        FreeList();

        madeChange = true;
        worklist = worklist->next;
    }

    return madeChange;
}

/*
**********************************************************************************************************************************
 YOU CAN MAKE CHANGES/ADD AUXILLIARY FUNCTIONS ABOVE THIS LINE
**********************************************************************************************************************************
*/
 
