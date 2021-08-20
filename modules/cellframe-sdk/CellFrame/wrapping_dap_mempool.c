#include "wrapping_dap_mempool.h"

PyObject *dap_chain_mempool_add_proc_py(PyObject *self, PyObject *args){
    PyObject *obj_server;
    const char *MEMPOOL_URL;
    if (!PyArg_ParseTuple(args, "O|s", &obj_server, &MEMPOOL_URL))
        return NULL;
    dap_chain_mempool_add_proc(DAP_HTTP(((PyDapServerObject*)obj_server)->t_server), MEMPOOL_URL);
    return PyLong_FromLong(0);
}