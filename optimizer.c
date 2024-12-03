#include "optimizer.h"

void Optimizer(NodeList *funcdecls) {
    bool madeChange;  
    do {
        madeChange = false;  // reset flag at the start of each iteration

        // iterate through the list of function declarations
        NodeList* currentFuncDecl = funcdecls;
        while (currentFuncDecl) {
            Node* function = currentFuncDecl->node;
            madeChange = ConstantFolding(currentFuncDecl);
            madeChange = ConstProp(function->statements);
            currentFuncDecl = currentFuncDecl->next; 
         
        }
        currentFuncDecl = funcdecls;
        while (currentFuncDecl) {
            for (int i=0; i<10; i++){
                madeChange = DeadAssign(funcdecls);
            }
            currentFuncDecl = currentFuncDecl->next; 
        }
        madeChange = false;


    } while (madeChange);  // continue until no changes are left
}

// constfolding takes a  nodelist of functions
// const prop takes a nodelist of statements
// dead assign takes a nodelist of functions

