#include "wrapping_dap_stream_ctl.h"

#define LOG_TAG "wrapping_dap_stream_ctl"


PyObject *dap_stream_ctl_add_proc_py(PyObject *self, PyObject *args){
    PyObject *obj_server;
    const char *STREAM_CTL_URL;
    if (!PyArg_ParseTuple(args, "O|s", &obj_server, &STREAM_CTL_URL))
        return  NULL;
    dap_stream_ctl_add_proc(DAP_HTTP(((PyDapServerObject*)obj_server)->t_server), STREAM_CTL_URL);
    return PyLong_FromLong(0);
}

int dap_stream_ctl_init_py(uint32_t size){
    return dap_stream_ctl_init(size);
}
