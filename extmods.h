#ifndef __EXTMODS_H
#define __EXTMODS_H


struct extopt;

/*
 * Module structure.
 */
struct extmod {
    char *name;
    int (*exec)(int argc, char *argv[]);
    struct extopt *opts;
};

#define EXTMOD_DECL(NAME, EXEC, OPTS)           \
struct extmod extmod_##NAME = {                 \
	.name = #NAME,  						    \
	.exec = EXEC, 								\
	.opts = OPTS							   	\
};                                              \
struct extmod extmod_##NAME __attribute((section("__extmods"), aligned(8)));


inline static char mod_is_end(struct extmod opt)
{
    return opt.name == 0 &&
           opt.opts == 0 &&
           opt.exec == 0;
}

struct extmod *extmod_find(char *name);
int module_exec(int argc, char *argv[], struct extmod *module);

#endif /* __EXTMODS_H */
