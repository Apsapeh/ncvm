/*#ifdef __NCVM_DEFAULT_LOADERS*/
#include <ncvm.h>

#include <stdlib.h>
#include <dlfcn.h>

_export ncvm_default_lib_loader ncvm_default_lib_loader_init(
    const char** lib_names,
    unsigned long lib_names_count
) {
    void** lib_handlers = (void**)malloc(lib_names_count * sizeof(void*));

    unsigned long i = 0;
    for (i = 0; i < lib_names_count; ++i) {
        lib_handlers[i] = dlopen(lib_names[i], RTLD_LAZY);
        if (lib_handlers[i] == (void*)0) {
            /* fprintf(stderr, "Failed to load library %s\n", lib_names[i]); */
            /* exit(1); */
        }
    }
    
    return (ncvm_default_lib_loader){
        lib_handlers,
        lib_names_count
    };
}


_export ncvm_lib_function ncvm_default_get_lib_function(
    const char* name,
    void* loader
) {
    unsigned long i = 0;
    for (i = 0; i < ((ncvm_default_lib_loader*)loader)->lib_handlers_count; ++i) {
        void* handle = ((ncvm_default_lib_loader*)loader)->lib_handlers[i];
        if (handle == (void*)0)
            continue;

        ncvm_lib_function func = (ncvm_lib_function)dlsym(handle, name);
        if (func != (ncvm_lib_function)0)
            return func;
    }
    return (ncvm_lib_function)0;
}


_export void ncvm_default_lib_function_loader_free(
    ncvm_default_lib_loader* loader
) {
    unsigned long i = 0;
    for (i = 0; i < loader->lib_handlers_count; ++i)
        dlclose(loader->lib_handlers[i]);
    free(loader->lib_handlers);
    loader->lib_handlers = 0;
}
/*#endif*/