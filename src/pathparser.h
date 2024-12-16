#include <sys/types.h>

struct phr_queries {
    const char *name;
    size_t name_len;
    const char *value;
    size_t value_len;
};


size_t
parse_path(
    char *fullpath,
    size_t fullpath_len,
    char **path,
    size_t *path_len,
    char **frag,
    size_t *frag_len,
    struct phr_queries *queries,
    size_t *num_queries);
