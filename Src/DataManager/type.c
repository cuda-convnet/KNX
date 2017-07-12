#include "type.h"

type_reg * type_registry;

obj * spawnType(lexeme target, token * args){
    target = KW2TYPE(target);

    for(int i = 0; i < type_registry->registered_types; ++i){
        if (target != type_registry->slots[i].type)
            continue;

        return type_registry->slots[i].constructor(args);
    }

    return NULL;
}

void * castTo(void * src, lexeme from, lexeme to){
    if (!src || (from == to))
        return src;

    for(int i = 0; i < type_registry->registered_types; ++i){
        if (from != type_registry->slots[i].type)
            continue;
        
        for (int j = 0; j < type_registry->slots[i].cast_count; ++j){
            if (to == type_registry->slots[i].cast_map[j]){
                return type_registry->slots[i].cast_table[j](src);
            }
        }

        break;
    }

    return NULL;
}

void * copyValue(void * src, lexeme type){

    if (!src) return NULL;

    for(int i = 0; i < type_registry->registered_types; ++i){
        if (type != type_registry->slots[i].type)
            continue;

        return type_registry->slots[i].copyConstructor(src);
    }

    return NULL;
}

void * typeMath(void * l, void * r, lexeme lt, lexeme rt, lexeme word){
    if (!l || !r)
        return NULL;
    
    for(int i = 0; i < type_registry->registered_types; ++i){
        if (lt != type_registry->slots[i].type)
            continue;

        return type_registry->slots[i].math(l, r, rt, word);
    }

    return NULL;
}