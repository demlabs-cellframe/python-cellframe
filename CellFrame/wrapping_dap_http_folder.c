#include "wrapping_dap_http_folder.h"

PyObject *dap_http_folder_add_py(PyObject *self, PyObject *args){
    PyObject *obj_server;
    const char *url_path;
    const char *local_path;
    if (!PyArg_ParseTuple(args, "O|s|s", &obj_server, &url_path, &local_path))
        return NULL;
    int res = dap_http_folder_add(DAP_HTTP(((PyDapServerObject*)obj_server)->t_server), url_path, local_path);
    return PyLong_FromLong(res);
}
