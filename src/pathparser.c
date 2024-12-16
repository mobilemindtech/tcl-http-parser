
#include "pathparser.h"
#include <stdio.h>

size_t
parse_path(char *fullpath,
            size_t fullpath_len,
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
    size_t WRONG_KEY = -2;
    size_t WRONG_VAL = -3;
    char *pt = fullpath;
    char *start = fullpath;
    size_t count = 0;
    *num_queries = 0;
    *path_len = 0;
    *frag_len = 0;

    while(count++ < fullpath_len) {
        ch = *pt;

        switch(ch){
        case '?':
            if(curr_state == path_start){
                *path = start;
                *path_len = pt - start;
            }else{
                return WRONG_KEY;
            }

            start = pt+1;
            curr_state = key_start;
            break;
        case '#':

            if(curr_state == val_start){
                queries[*num_queries].value = start;
                queries[*num_queries].value_len = pt - start;
                (*num_queries)++;
            }

            curr_state = frag_start;
            *frag = pt + 1;
            *frag_len = fullpath_len - count;
            count = fullpath_len;
            break;
        case '&':
            if(curr_state == val_start){
                queries[*num_queries].value = start;
                queries[*num_queries].value_len = pt - start;
                (*num_queries)++;
            }else {
                return WRONG_VAL;
            }
            start = pt+1;
            curr_state = key_start;
            break;
        case '=':
            if(curr_state == key_start){
                queries[*num_queries].name = start;
                queries[*num_queries].name_len = pt - start;
                queries[*num_queries].value_len = 0;
            }else {
                return WRONG_KEY;
            }
            start = pt+1;
            curr_state = val_start;
        }

        pt++;
    }

    if(curr_state == val_start){
        queries[*num_queries].value = start;
        queries[*num_queries].value_len = pt - start;
        (*num_queries)++;
    }else if(curr_state == path_start){
        *path = start;
        *path_len = pt - start;
    }

    // print_queries(queries, num_queries);

    return 0;
}

void print_queries(struct phr_queries *queries,
		   size_t *num_queries) {
  
    for(int i = 0; i < *num_queries; i++){
      printf("\nname=");
      for(int j = 0; j < queries[i].name_len; j++){
	printf("%c", queries[i].name[j]);
      }
      printf(" [%d], value=", queries[i].name_len);
      for(int j = 0; j < queries[i].value_len; j++){
	printf("%c", queries[i].value[j]);
      }
      printf(" [%d]\n", queries[i].value_len);
    }
}
