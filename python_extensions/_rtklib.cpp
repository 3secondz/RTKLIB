#include <Python.h>

#include <iostream>

#include <util.hpp>

extern "C" {
    #include "rtklib.h"
    #include "convbin.h"
    #include "rnx2rtkp.h"
}

struct module_state {
    PyObject *error;
};

#define GETSTATE(m) ((struct module_state*)PyModule_GetState(m))

static int extension_traverse(PyObject *m, visitproc visit, void *arg) {
    Py_VISIT(GETSTATE(m)->error);
}

static int extension_clear(PyObject *m) {
    Py_CLEAR(GETSTATE(m)->error);
    return 0;
}

static PyObject *
method_convbin(PyObject *self, PyObject *args) {
    static char* kwlist[] = {"filename",NULL};\
    PyObject *dict;
    option_t option;
    char *filename;

    if(!PyArg_ParseTuple(args,"O!",&PyDict_Type,&dict)) {
        return NULL;
    }

    option.ts = get_dict_item_str(dict, "ts");
    option.te = get_dict_item_str(dict, "te");
    option.tr = get_dict_item_str(dict, "tr");
    option.ti = get_dict_item_str(dict, "ti");
    option.span = get_dict_item_str(dict, "span");
    option.r = get_dict_item_str(dict, "r");
    option.ro = get_dict_item_str(dict, "ro");
    option.f = get_dict_item_str(dict, "f");
    option.hc = get_dict_item_str(dict, "hc");
    option.hm = get_dict_item_str(dict, "hm");
    option.hn = get_dict_item_str(dict, "hn");
    option.ht = get_dict_item_str(dict, "ht");
    option.ho = get_dict_item_str(dict, "ho");
    option.hr = get_dict_item_str(dict, "hr");
    option.ha = get_dict_item_str(dict, "ha");
    option.hp = get_dict_item_str(dict, "hp");
    option.hd = get_dict_item_str(dict, "hd");
    option.v = get_dict_item_str(dict, "v");
    option.od = get_dict_item_str(dict, "od");
    option.os = get_dict_item_str(dict, "os");
    option.oi = get_dict_item_str(dict, "oi");
    option.ot = get_dict_item_str(dict, "ot");
    option.ol = get_dict_item_str(dict, "ol");
    option.scan = get_dict_item_str(dict, "scan");
    option.mask = get_dict_item_str(dict, "mask");
    option.x = get_dict_item_str(dict, "x");
    option.y = get_dict_item_str(dict, "y");
    option.d = get_dict_item_str(dict, "d");
    option.c = get_dict_item_str(dict, "c");
    option.o = get_dict_item_str(dict, "o");
    option.n = get_dict_item_str(dict, "n");
    option.g = get_dict_item_str(dict, "g");
    option.h = get_dict_item_str(dict, "h");
    option.q = get_dict_item_str(dict, "q");
    option.l = get_dict_item_str(dict, "l");
    option.s = get_dict_item_str(dict, "s");
    option.trace = get_dict_item_str(dict, "trace");
    option.filename = get_dict_item_str(dict, "filename");

    return PyBool_FromLong(convbin_func(&option));
}

static PyObject *
method_rnx2rtkp(PyObject *self, PyObject *args) {

    PyObject *dict;
    rnx_opt_t opt;

    if(!PyArg_ParseTuple(args,"O!",&PyDict_Type,&dict)) {
        return NULL;
    }

    opt.k = get_dict_item_str(dict, "k");
    opt.o = get_dict_item_str(dict, "o");
    opt.ts = get_dict_item_str(dict, "ts");
    opt.te = get_dict_item_str(dict, "te");
    opt.ti = get_dict_item_str(dict, "ti");
    opt.p = get_dict_item_str(dict, "p");
    opt.f = get_dict_item_str(dict, "f");
    opt.m = get_dict_item_str(dict, "m");
    opt.v = get_dict_item_str(dict, "v");
    opt.s = get_dict_item_str(dict, "s");
    opt.d = get_dict_item_str(dict, "d");
    opt.b = get_dict_item_str(dict, "b");
    opt.c = get_dict_item_str(dict, "c");
    opt.i = get_dict_item_str(dict, "i");
    opt.h = get_dict_item_str(dict, "h");
    opt.t = get_dict_item_str(dict, "t");
    opt.u = get_dict_item_str(dict, "u");
    opt.e = get_dict_item_str(dict, "e");
    opt.a = get_dict_item_str(dict, "a");
    opt.n = get_dict_item_str(dict, "n");
    opt.g = get_dict_item_str(dict, "g");
    opt.r = get_dict_item_str(dict, "r");
    opt.l = get_dict_item_str(dict, "l");
    opt.y = get_dict_item_list(dict, "y");
    opt.x = get_dict_item_list(dict, "x");
    opt.filename = get_dict_item_list(dict, "filename");

    return PyBool_FromLong(rnx2rtkp_func(&opt));
}

static PyMethodDef extension_methods[] = {
    {"convbin", (PyCFunction)method_convbin, METH_VARARGS, "convbin"},
    {"rnx2rtkp", (PyCFunction)method_rnx2rtkp, METH_VARARGS, "rnx2rtkp"},
    {NULL, NULL, 0, NULL}
};

static struct PyModuleDef rtklib_module = {
    PyModuleDef_HEAD_INIT,
    "_rtklib",
    "_rtklib module docstring",
    sizeof(struct module_state),
    extension_methods,
    NULL,
    extension_traverse,
    extension_clear,
    NULL
};

PyMODINIT_FUNC
PyInit__rtklib(void) {
    PyObject *module = PyModule_Create(&rtklib_module);

    if (module==NULL) {
        return 0;
    }

    struct module_state *st = GETSTATE(module);

    st->error = PyErr_NewException("_rtklib.Error", NULL, NULL);
    if (st->error==NULL) {
        Py_DECREF(module);
        return NULL;
    }

    return module;
}
