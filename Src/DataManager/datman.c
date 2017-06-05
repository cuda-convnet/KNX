#include <stdio.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"

#include "datamanager.h"

static void startup() __attribute__((constructor));
static void shutdown() __attribute__((destructor));

nodereg * node_reg;
Config * config;

//Constructor/Destructor
void startup()
{
    
    config = getConfig();
    
    node_reg = malloc(sizeof(nodereg));
    node_reg->usedIdIndices = malloc(sizeof(int *) * config->maxNodes);

    //
    for (unsigned i = 0; i < config->maxNodes; ++i){
        node_reg->usedIdIndices[i]=-1;
    }

    node_reg->idTable = primeCipher(config->maxNodes);

    printf("Data Manager Loaded\r\n");
}

void shutdown()
{

    printf("Data Manager Unloaded\r\n");
}

//Basic functions

nodereg * getNodeReg()
{
    return node_reg;
}
