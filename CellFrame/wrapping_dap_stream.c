#include "wrapping_dap_stream.h"

PyObject *dap_stream_add_proc_http_py(PyObject *self, PyObject *args){
    PyObject *obj_server;
    const char *STREAM_URL;
    if (!PyArg_ParseTuple(args, "O|s", &obj_server, &STREAM_URL))
        return NULL;
    dap_stream_add_proc_http(DAP_HTTP(((PyDapServerObject*)obj_server)->t_server), STREAM_URL);
    return PyLong_FromLong(0);
}
