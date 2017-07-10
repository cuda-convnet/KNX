#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "executor.h"

/*
command execution procedure: 12 step program addition

1)  move ITR to first command token
2)  copy command instruction
3)  set CONT to RIGHT of command token
4)  move ARG to
        FIRST lval if unary
        Second lval if binary\
5)  move LSAV to ARG->LEFT
6)  detach ARG
7)  destroy command token
8)  pass ARG and command instruction to runner
9)  if return value is not void:
10a)     attach return value to LSAV and CONT
    else
10b) attach LSAV to CONT
11) attach ITR to LSAV
12) GOTO 1
*/

token * run(node * nd, token * arg, lexeme command){

    if (isKeyword(command)){
        return runKeyword(nd, arg, command);
    } else {
        return runOperator(arg, command);
    }
}

token * execute(node * nd){
    
    token * itr = nd->buffer.tokens;
    token * cont = NULL,
    * arg = NULL,
    * lsav = NULL,
    * ret = NULL;

    while(itr){

        ret = itr;

        //move to command token
        while (itr && !isCommand(itr->type))
            itr = itr->right;
        if (!itr){
            break;
        } 

        lexeme com = itr->type;
        bool unary = isUnary (com);
        cont = itr->right;
        arg = unary ?
            itr->left :
            itr->left ?
                itr->left->left :
                itr->left;

        if(arg)
            lsav = arg->left;

        //detach ARG
        if (arg && arg->left)
            arg->left->right = NULL;
        if (itr->left)
            itr->left->right = NULL;
        
        free(itr);

        itr = run(nd, arg, com);
        destroyTokenStrand(arg);

        coupleTokens(lsav, itr, cont);
    }   

    nd->buffer.tokens = ret; 

    return ret;
}