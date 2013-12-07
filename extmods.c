#include <stdio.h>
#include <string.h>

#include "extopts.h"
#include "extmods.h"


extern struct extmod __start___extmods[];
extern struct extmod __stop___extmods[];

/*
 * Find extmodule with specified name.
 */
struct extmod *extmod_find(char *name)
{
    struct extmod *module = __start___extmods;

    for (; module < __stop___extmods; module++) {
        if (!strcmp(name, module->name))
            return module;
    }
    
    return 0;
}

int module_exec(int argc, char *argv[], struct extmod *module)
{
    if (module && module->exec)
        return module->exec(argc, argv);

    return 1;
}
