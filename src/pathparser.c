
#include "pathparser.h"

size_t
parse_path(char *fullpath,
            size_t *fullpath_len,
            char **path,
            size_t *path_len,
            char **frag,
            size_t *frag_len,
            struct phr_queries *queries,
            size_t *num_queries)
{
    char ch;
    //struct phr_header queries[HEADERS_SIZE];
    enum machine {
        path_start, key_start, val_start, frag_start
    };
    enum machine curr_state = path_start;
    size_t WRONG_FRAG = -1;
    size_t WRONG_KEY = -2;
    size_t WRONG_VAL = -3;
    char *pt = fullpath;
    char *pos = fullpath;
    size_t count = 0;
    *num_queries = 0;
    *path_len = 0;
    *frag_len = 0;

    while(++count < *fullpath_len) {
        ch = *pt;

        switch(ch){
        case '?':
            if(curr_state == path_start){
                *path = pos;
                *path_len = pt - pos;
            }else{
                return WRONG_KEY;
            }

            pos = pt;
            curr_state = key_start;
            break;
        case '#':

            if(curr_state == val_start){
                queries->value = pos + 1;
                queries->value_len = pt - pos - 1;
                queries++;
                (*num_queries)++;
            }

            curr_state = frag_start;
            *frag = pt + 1;
            *frag_len = *fullpath_len - count;
            count = *fullpath_len;
            break;
        case '&':
            if(curr_state == val_start){
                queries->value = pos + 1;
                queries->value_len = pt - pos - 1;
                queries++;
                (*num_queries)++;
            }else {
                return WRONG_VAL;
            }
            pos = pt;
            curr_state = key_start;
            break;
        case '=':
            if(curr_state == key_start){
                queries->name = pos + 1;
                queries->name_len = pt - pos - 1;
                queries->value_len = 0;
            }else {
                return WRONG_KEY;
            }
            pos = pt;
            curr_state = val_start;
        }

        pt++;
    }

    if(curr_state == val_start){
        queries->value = pos + 1;
        queries->value_len = pt - pos - 1;
        queries++;
        (*num_queries)++;
    }


    return 0;
}
