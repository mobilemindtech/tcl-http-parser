

#include <stdio.h>
#include <tcl.h>
#include <string.h>
#include "picohttpparser.h"
#include "pathparser.h"


#define MAX_BODY_SIZE 4096
#define PROTO_VERSION_SIZE 2
#define HEADERS_SIZE 100

int
Http_parser_parse_request(ClientData data, Tcl_Interp *interp, int objc, Tcl_Obj * const objv[]);
int
parse_buf(Tcl_Interp *interp, char *buf);
int
parse_channel(Tcl_Interp *interp, char *channelName);

int
Http_parser_Init(Tcl_Interp *interp)
{
    if(Tcl_InitStubs(interp, TCL_VERSION, 0) == NULL){
        return TCL_ERROR;
    }
    Tcl_CreateObjCommand(interp, "parse_request", Http_parser_parse_request, NULL, NULL);
    return TCL_OK;
}


/**
 * @brief Http_parser_parse_request
 * Extract channel content and try parse content by pico http parser. Return a dict with keys:
 *  - method: string
 *  - path: string
 *  - version: string
 *  - headers: dict
 *  - body: string
 * @param data
 * @param interp
 * @param objc
 * @param objv
 * @return Success or failure
 */
int
Http_parser_parse_request(ClientData data, Tcl_Interp *interp, int objc, Tcl_Obj * const objv[])
{
    int ret, mode = TCL_READABLE, resultCode = TCL_OK, buf_len;
    char *channelName, *opt, *buf;
    Tcl_Channel channel;

    if(objc != 3) {
        Tcl_WrongNumArgs(interp, 1, objv, "Usage: parse_request -channel ?channel? -buffer ?string?");
        return TCL_ERROR;
    }

    opt = Tcl_GetString(objv[1]);

    if(strcmp(opt, "-channel") == 0) {
        channelName = Tcl_GetString(objv[2]);
        return parse_channel(interp, channelName);
    } else if(strcmp(opt, "-buffer") == 0){
        buf= Tcl_GetString(objv[2]);
        return parse_buf(interp, buf);
    } else {
        Tcl_WrongNumArgs(interp, 1, objv, "Usage: parse_request -channel ?channel? -buffer ?string?");
        return TCL_ERROR;
    }

}

int
parse_channel(Tcl_Interp *interp, char *channelName)
{
    int ret, mode = TCL_READABLE, resultCode = TCL_OK, buf_len;
    char *buf;
    Tcl_Channel channel;
    Tcl_Obj *objPtr = Tcl_NewObj();


    if((channel = Tcl_GetChannel(interp, channelName, &mode)) == NULL) {
        Tcl_AppendResult(interp, "can't get channel: ", channelName, NULL);
        return TCL_ERROR;
    }

    Tcl_IncrRefCount(objPtr);

    // (Tcl_Channel channel, Tcl_Obj *objPtr, int charsToRead, int appendFlag)
    ret = Tcl_ReadChars(channel, objPtr, -1, 0);

    if(ret == -1) {
        Tcl_AppendResult(interp, "channel read error", NULL);
        resultCode = TCL_ERROR;
    } else {
        buf = Tcl_GetStringFromObj(objPtr, &buf_len);
    }

    Tcl_DecrRefCount(objPtr);

    if(resultCode == TCL_ERROR){
        return resultCode;
    }

    return parse_buf(interp, buf);
}

int
parse_buf(Tcl_Interp *interp, char* buf)
{
    size_t buf_len, pret, method_len, fullpath_len, path_len, minor_version, num_headers, num_queries, body_len, frag_len;
    char *method, *fullpath, *path, *body, *frag;
    char version[PROTO_VERSION_SIZE];
    struct phr_header headers[HEADERS_SIZE];
    struct phr_queries queries[HEADERS_SIZE];
    Tcl_Obj *dictPtr = Tcl_NewDictObj();
    Tcl_Obj *dictHeadersPtr = Tcl_NewDictObj();
    Tcl_Obj *dictQueriesPtr = Tcl_NewDictObj();

    //buf_len += ret;
    //num_headers = sizeof(headers) / sizeof(headers[0]);
    pret = phr_parse_request(buf, buf_len, &method, &method_len, &fullpath, &fullpath_len,
                             &minor_version, headers, &num_headers, 0);

    if (pret > 0){ /* successfully parsed the request */
        body = buf + pret;
        body_len = strlen(body);
    }else if (pret == -1) {
        Tcl_AppendResult(interp, "parse request error", NULL);
        return TCL_ERROR;
    }

    pret = parse_path(fullpath, &fullpath_len, &path, &path_len, &frag, &frag_len, &queries, &num_queries);

    if(pret > 0) {
        Tcl_AppendResult(interp, "parse path error", NULL);
        return TCL_ERROR;
    }

    /* request is incomplete, continue the loop */
    //assert(pret == -2);

    sprintf(version, "1.%d", 2, (int)minor_version);

    // https://wiki.tcl-lang.org/page/Tcl+C+API+Design+Principles
    Tcl_DictObjPut(
        interp,
        dictPtr,
        Tcl_NewStringObj("method", 6) ,
        Tcl_NewStringObj(method, (int)method_len));

    Tcl_DictObjPut(
        interp,
        dictPtr,
        Tcl_NewStringObj("path", 4),
        Tcl_NewStringObj(path, (int)path_len));

    Tcl_DictObjPut(
        interp,
        dictPtr,
        Tcl_NewStringObj("frag", 4),
        Tcl_NewStringObj(frag, (int)frag_len));

    Tcl_DictObjPut(
        interp,
        dictPtr,
        Tcl_NewStringObj("version", 7),
        Tcl_NewStringObj(version, 2));

    for(int i = 0; i < num_headers; i++){
        Tcl_DictObjPut(
            interp,
            dictHeadersPtr,
            Tcl_NewStringObj(headers[i].name, headers[i].name_len),
            Tcl_NewStringObj(headers[i].value, headers[i].value_len));
    }

    for(int i = 0; i < num_queries; i++){
        Tcl_DictObjPut(
            interp,
            dictQueriesPtr,
            Tcl_NewStringObj(queries[i].name, queries[i].name_len),
            Tcl_NewStringObj(queries[i].value, queries[i].value_len));
    }

    Tcl_DictObjPut(
        interp,
        dictPtr,
        Tcl_NewStringObj("headers", 7),
        dictHeadersPtr);

    Tcl_DictObjPut(
        interp,
        dictPtr,
        Tcl_NewStringObj("body", 4),
        Tcl_NewStringObj(body, body_len));

    // https://wiki.tcl-lang.org/page/Tcl%5FSetObjResult
    Tcl_SetObjResult(interp, dictPtr);

    return TCL_OK;
}



