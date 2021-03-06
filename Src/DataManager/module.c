#include <stdlib.h>
#include <string.h>

#include "module.h"
#include "mem.h"

HMODULE * root = NULL;

HMODULE * createHModule(char * str, node * n, memTree * scope, tBuffer * buffer){

    HMODULE * ret = malloc(sizeof(HMODULE));
    ret->owner = n;
    
    size_t len = strlen(str) + 1;
    ret->id = malloc(len);
    strncpy(ret->id, str, len);

    ret->memory = scope;
    ret->buffer = buffer;

    return ret;
}

void destroyHModule(HMODULE * module){
    free(module->id);
    free(module);
}

HMODULE * getRootHandle(){
    return root;
}

void setRootHandle(HMODULE * r){
    root = r;
}