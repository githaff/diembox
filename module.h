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


/*
 * Tool command option description.
 * Repeats 'struct option' definition adding some field for docs
 * auto-generation.
 */
struct module_opt {
    /* Equivalent of 'struct option' */
    const char *name_long;
    int has_arg;
    int *flag;
    int val;

    /* Added fields */
    char name_short;
    const char *arg_name;
    char *desc;
};

#define OPTS_END { 0, 0, 0, 0, 0, 0, 0 }


#endif /* __MODULE_H */
