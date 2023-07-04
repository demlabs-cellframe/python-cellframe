#include "wrapping_dap_chain_tx_out_ext.h"

static PyGetSetDef DapChainTxOutExtGetsSetsDef[] = {
        {"addr", (getter)wrapping_dap_chain_tx_out_ext_get_addr, NULL, NULL, NULL},
        {"token", (getter)wrapping_dap_chain_tx_out_ext_get_token, NULL, NULL, NULL},
        {"value", (getter)wrapping_dap_chain_tx_out_ext_get_value, NULL, NULL, NULL},
        {}
};

PyTypeObject DapChainTxOutExtObjectType = DAP_PY_TYPE_OBJECT(
        "CellFrame.ChainTxOutExt", sizeof(PyDapChainTXOutExtObject),
        "Chain tx out ext object",
        .tp_getset = DapChainTxOutExtGetsSetsDef);

PyObject *wrapping_dap_chain_tx_out_ext_get_addr(PyObject *self, void *closure){
    (void)closure;
    PyObject *obj_addr = _PyObject_New(&DapChainAddrObjectType);
    obj_addr = PyObject_Init(obj_addr, &DapChainAddrObjectType);
    ((PyDapChainAddrObject*)obj_addr)->addr = &((PyDapChainTXOutExtObject*)self)->out_ext->addr;
    return obj_addr;
}
PyObject *wrapping_dap_chain_tx_out_ext_get_token(PyObject *self, void *closure){
    (void)closure;
    return Py_BuildValue("s", ((PyDapChainTXOutExtObject*)self)->out_ext->token);
}
PyObject *wrapping_dap_chain_tx_out_ext_get_value(PyObject *self, void *closure){
    (void)closure;
    DapMathObject *l_math = PyObject_New(DapMathObject, &DapMathObjectType);
    l_math->value = ((PyDapChainTXOutExtObject*)self)->out_ext->header.value;
    return (PyObject*)l_math;
}
