#include "wrapping_dap_chain_tx_pkey.h"

static PyGetSetDef DapChainTxPkeyGetsSetsDef[] = {
        {"pkeyType", (getter)wrapping_dap_chain_tx_pkey_get_type, NULL, NULL, NULL},
        {"pkeySize", (getter)wrapping_dap_chain_tx_pkey_get_size, NULL, NULL, NULL},
        {"pkey", (getter)wrapping_dap_chain_tx_pkey_get_pkey, NULL, NULL, NULL},
        {}
};

PyTypeObject DapChainTxPkeyObjectType = DAP_PY_TYPE_OBJECT(
        "CellFrame.ChainTxPkey", sizeof(PyDapChainTXPkeyObject),
        "Chain tx pkey object",
        .tp_getset = DapChainTxPkeyGetsSetsDef);

PyObject *wrapping_dap_chain_tx_pkey_get_type(PyObject *self, void *closure){
    (void)closure;
    PyDapPkeyTypeObject *obj_pkey_type = PyObject_New(PyDapPkeyTypeObject, &DapCryproPkeyTypeObjectType);
    obj_pkey_type->pkey_type = ((PyDapChainTXPkeyObject*)self)->tx_pkey->header.type;
    return (PyObject*)obj_pkey_type;
}
PyObject *wrapping_dap_chain_tx_pkey_get_size(PyObject *self, void *closure){
    (void)closure;
    return Py_BuildValue("I", ((PyDapChainTXPkeyObject*)self)->tx_pkey->header.size);
}

PyObject *wrapping_dap_chain_tx_pkey_get_pkey(PyObject *self, void *closure){
    (void)closure;
    PyDapPkeyObject *obj_pkey = PyObject_NEW(PyDapPkeyObject, &DapPkeyObject_DapPkeyObjectType);
    obj_pkey->pkey = (dap_pkey_t *)((PyDapChainTXPkeyObject*)self)->tx_pkey->pkey;
    return (PyObject*)obj_pkey;
}
