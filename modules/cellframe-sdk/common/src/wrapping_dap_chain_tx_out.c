#include "wrapping_dap_chain_tx_out.h"

static PyGetSetDef DapChainTxOutGetsSetsDef[] = {
        {"addr", (getter)wrapping_dap_chain_tx_out_get_addr, NULL, NULL, NULL},
        {"value", (getter)wrapping_dap_chain_tx_out_get_value, NULL, NULL, NULL},
        {NULL}
};

static PyMethodDef PyDapChainTxOutObjectMethods[] ={
        {NULL, NULL, 0, NULL}
};

PyTypeObject DapChainTxOutObjectType = DAP_PY_TYPE_OBJECT(
        "CellFrame.ChainTxOut", sizeof(PyDapChainTXOutObject),
        "Chain tx out object",
        .tp_methods = PyDapChainTxOutObjectMethods,
        .tp_getset = DapChainTxOutGetsSetsDef);

PyObject *wrapping_dap_chain_tx_out_get_addr(PyObject *self, void *closure){
    (void)closure;
    PyObject *obj_addr = _PyObject_New(&DapChainAddrObjectType);
    obj_addr = PyObject_Init(obj_addr, &DapChainAddrObjectType);
    PyObject_Dir(obj_addr);
    ((PyDapChainAddrObject*)obj_addr)->addr = &((PyDapChainTXOutObject*)self)->tx_out->addr;
    return obj_addr;
}
PyObject *wrapping_dap_chain_tx_out_get_value(PyObject *self, void *closure){
    (void)closure;
    DapMathObject *l_math = PyObject_New(DapMathObject, &DapMathObjectType);
    l_math->value = ((PyDapChainTXOutObject*)self)->tx_out->header.value;
    return (PyObject*)l_math;
}
