#include "wrapping_http.h"

PyObject *dap_http_new_py(PyObject *self, PyObject *args){
    PyObject *obj_server;
    const char * name;
    if (!PyArg_ParseTuple(args, "O|s", &obj_server, &name))
        return NULL;
    int res = dap_http_new(((PyDapServerObject*)obj_server)->t_server, name);
    return PyLong_FromLong(res);
}
