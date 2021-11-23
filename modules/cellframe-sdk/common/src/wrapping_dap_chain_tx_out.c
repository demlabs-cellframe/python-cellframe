#include "wrapping_dap_chain_tx_out.h"

PyObject *wrapping_dap_chain_tx_out_get_addr(PyObject *self, void *closure){
    (void)closure;
    PyObject *obj_addr = _PyObject_New(&DapChainAddrObject_DapChainAddrObjectType);
    obj_addr = PyObject_Init(obj_addr, &DapChainAddrObject_DapChainAddrObjectType);
    PyObject_Dir(obj_addr);
    ((PyDapChainAddrObject*)obj_addr)->addr = ((PyDapChainTXOutObject*)self)->tx_out->addr;
    return obj_addr;
}
PyObject *wrapping_dap_chain_tx_out_get_value(PyObject *self, void *closure){
    (void)closure;
    return Py_BuildValue("K", ((PyDapChainTXOutObject*)self)->tx_out->header.value);
}
