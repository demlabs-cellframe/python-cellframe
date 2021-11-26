#include "wrapping_dap_chain_tx_out_ext.h"

PyObject *wrapping_dap_chain_tx_out_ext_get_addr(PyObject *self, void *closure){
    (void)closure;
    PyObject *obj_addr = _PyObject_New(&DapChainAddrObject_DapChainAddrObjectType);
    obj_addr = PyObject_Init(obj_addr, &DapChainAddrObject_DapChainAddrObjectType);
    ((PyDapChainAddrObject*)obj_addr)->addr = &((PyDapChainTXOutExtObject*)self)->out_ext->addr;
    return obj_addr;
}
PyObject *wrapping_dap_chain_tx_out_ext_get_token(PyObject *self, void *closure){
    (void)closure;
    return Py_BuildValue("s", ((PyDapChainTXOutExtObject*)self)->out_ext->token);
}
PyObject *wrapping_dap_chain_tx_out_ext_get_value(PyObject *self, void *closure){
    (void)closure;
    return Py_BuildValue("k", ((PyDapChainTXOutExtObject*)self)->out_ext->header.value);
}