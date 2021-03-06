#ifndef KDK_CONFIG
#define KDK_CONFIG

#include <stdbool.h>

#define MAX_TYPE_COUNT  400

typedef struct Config
{
    //flags
    bool debug;//execute debugging blocks
    bool verbose;//print long messages if applicable
    bool suppresswarn;//do not show warnings on screen
    bool werr;//treat all warnings as errors
    bool force;//ignore all errors
    bool nolog;//do not log system messages

    //limits
    int maxMem;
    int maxNodes;
    int maxThreads;
    int maxCores;

} Config;

extern Config * _config;

Config * generateConfig();
Config * loadDefaultConfig();
Config * getConfig();

#endif