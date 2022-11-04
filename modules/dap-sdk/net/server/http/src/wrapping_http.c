#include"libdap-python.h"
#include "wrapping_http.h"

static PyMethodDef DapHttpMethods[] = {
        {"new", dap_http_new_py, METH_VARARGS | METH_STATIC, ""},
        {}
};

PyTypeObject DapHttpObjectType = DAP_PY_TYPE_OBJECT(
        "DAP.Net.Http", sizeof(PyDapHttpObject),
        "Dap Http object",
        .tp_methods = DapHttpMethods);

PyObject *dap_http_new_py(PyObject *self, PyObject *args){
    PyObject *obj_server;
    const char * name;
    if (!PyArg_ParseTuple(args, "O|s", &obj_server, &name))
        return NULL;
    int res = dap_http_new(((PyDapServerObject*)obj_server)->t_server, name);
    return PyLong_FromLong(res);
}
