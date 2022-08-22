#include <stdlib.h>
#include <iostream>

#include <util.hpp>

extern "C" {
    #include "rtklib.h"
}

extern int showmsg(char *format, ...) {
    va_list arg;
    va_start(arg,format); vfprintf(stderr,format,arg); va_end(arg);
    fprintf(stderr,"\r");
    return 0;
}
extern void settspan(gtime_t ts, gtime_t te) {}
extern void settime(gtime_t time) {}

char * get_dict_item_str(PyObject *dict, const char *itemname) {
    PyObject *item = PyDict_GetItemString(dict, itemname);

    if (item) 
        return (char *)PyUnicode_AsUTF8(item);
    return NULL;   
}

char ** get_dict_item_list(PyObject *dict, const char *itemname) {
    PyObject *item = PyDict_GetItemString(dict, itemname);
    if (!item) return NULL;
    size_t len = (size_t)PyList_Size(item);
    char **list;
    list = (char **)malloc(sizeof(char) * len);

    if (item) {
        for (int i = 0; i < len; i++) {
            PyObject *ele = PyList_GetItem(item, i);
            list[i] = (char *)PyUnicode_AsUTF8(ele);
        }
    }

    return list;
}