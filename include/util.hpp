#pragma once

#include <Python.h>

char * get_dict_item_str(PyObject *dict, const char *itemname);

char ** get_dict_item_list(PyObject *dict, const char *itemname);
