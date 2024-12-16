#include <stdio.h>
#include <string.h>

#include "../../src/pathparser.h"
#include "../../src/picohttpparser.h"

int assert_int(int expected, int value, char *message) {
    if(expected != value){
        printf("expected %d, received %d. message = %s\n", expected, value, message);
        return 1;
    }
    return 0;
}

int assert_char_ptr(char *expected, char *ptr, int len, char *message) {
    char buf[len];
    memset(buf, 0, len);
    memcpy(buf, ptr, len);
    if(strncmp(expected, buf, len) != 0){
        printf("expected %s, received %s len %d. message = %s\n", expected, buf, len, message);
        return 1;
    }
    return 0;
}

int main() {

    char *fullpath = "/home?name=pedro&age=30&type=1"; //#top";
    char *path, *frag;
    size_t fullpath_len = strlen(fullpath), path_len, frag_len, num_queries, ret;
    struct phr_queries queries[100];

    printf("run parse\n");
    ret = parse_path(fullpath, fullpath_len, &path, &path_len, &frag, &frag_len, queries, &num_queries);


    if(assert_int(5, path_len, "wrong path len")) return 1;
    if(assert_char_ptr("/home", path, path_len, "wong path")) return 1;

    //if(assert_int(3, frag_len, "wrong frag len")) return 1;
    //if(assert_char_ptr("top", frag, frag_len, "wong frag")) return 1;


    if(assert_int(3, num_queries, "wrong query len")) return 1;

    // query key name
    if(assert_int(4, queries[0].name_len, "wrong query name len")) return 1;
    if(assert_char_ptr("name", queries[0].name,  queries[0].name_len, "wrong query name")) return 1;

    // query val pedro
    if(assert_int(5, queries[0].value_len, "wrong query value len")) return 1;
    if(assert_char_ptr("pedro", queries[0].value,  queries[0].value_len, "wrong query value")) return 1;

    // query key age
    if(assert_int(3, queries[1].name_len, "wrong query name len")) return 1;
    if(assert_char_ptr("age", queries[1].name,  queries[1].name_len, "wrong query name")) return 1;

    // query val 30
    if(assert_int(2, queries[1].value_len, "wrong query value len")) return 1;
    if(assert_char_ptr("30", queries[1].value,  queries[1].value_len, "wrong query value")) return 1;

    // query key type
    if(assert_int(4, queries[2].name_len, "wrong query name len")) return 1;
    if(assert_char_ptr("type", queries[2].name,  queries[2].name_len, "wrong query name")) return 1;

    // query val 1
    if(assert_int(1, queries[2].value_len, "wrong query value len")) return 1;
    if(assert_char_ptr("1", queries[2].value,  queries[2].value_len, "wrong query value")) return 1;

    return 0;
}
