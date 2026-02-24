#include "wrapping_dap_chain_tx_tsd.h"

PyGetSetDef PyDapChainTxTSDGetsSetsDef[] = {
    {"data", (getter)wrapping_dap_chain_tx_get_tsd_data, NULL, NULL, NULL},
    {"type", (getter)wrapping_dap_chain_tx_get_tsd_type, NULL, NULL, NULL},
    {}
};

PyTypeObject DapChainTxTSDObjectType = DAP_PY_TYPE_OBJECT(
        "CellFrame.ChainTxTSD", sizeof(PyDapChainTxTSDObject),
        "Chain tx tsd object",
        .tp_getset = PyDapChainTxTSDGetsSetsDef);

PyObject *wrapping_dap_chain_tx_get_tsd_data(PyObject *self, void *closure) {
    (void)closure;
    dap_chain_tx_tsd_t *l_item = ((PyDapChainTxTSDObject*)self)->tsd;
    if (!l_item)
        Py_RETURN_NONE;
    
    dap_tsd_t *l_tsd = (dap_tsd_t *)l_item->tsd;
    if (!l_tsd || l_tsd->size == 0)
        Py_RETURN_NONE;
    
    return PyBytes_FromStringAndSize((char*)l_tsd->data, l_tsd->size);
}

PyObject *wrapping_dap_chain_tx_get_tsd_type(PyObject *self, void *closure) {
    (void)closure;
    dap_chain_tx_tsd_t *l_item = ((PyDapChainTxTSDObject*)self)->tsd;
    if (!l_item)
        return Py_BuildValue("i", 0);
    
    dap_tsd_t *l_tsd = (dap_tsd_t *)l_item->tsd;
    if (!l_tsd)
        return Py_BuildValue("i", 0);
    
    return Py_BuildValue("i", l_tsd->type);
}

