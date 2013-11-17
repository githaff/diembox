#ifndef __MODULE_H
#define __MODULE_H


struct module_opt;

/*
 * Toolbox module structure.
 */
struct module {
    char *name;
    char *desc;
    struct module_opt *opts;
    
    int (*run)(int argc, char *argv[]);
};


#endif /* __MODULE_H */
