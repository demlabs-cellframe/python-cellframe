#include "wrapping_dap_enc_http.h"

PyObject *enc_http_add_proc_py(PyObject *self, PyObject *args){
    PyObject *obj_server;
    const char *HTTP_URL;
    if (!PyArg_ParseTuple(args, "O|s", &obj_server, &HTTP_URL))
        return NULL;
    enc_http_add_proc(DAP_HTTP(((PyDapServerObject*)obj_server)->t_server), HTTP_URL);
    return PyLong_FromLong(0);
}
