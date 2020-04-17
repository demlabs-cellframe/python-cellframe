#ifndef _DAP_CHAIN_WALLET_PYTHON_
#define _DAP_CHAIN_WALLET_PYTHON_

#include <Python.h>
#include "dap_common.h"
#include "dap_chain_wallet.h"

#ifdef __cplusplus
extern "C"{
#endif

#undef LOG_TAG
#define LOG_TAG "dap_chain_wallet_python"

typedef struct PyDapChainWallet{
    PyObject_HEAD
    dap_chain_wallet_t *wallet;
}PyDapChainWalletObject;

int dap_chain_wallet_init_py(void);
void dap_chain_wallet_deinit_py(void);

PyObject *dap_chain_wallet_get_path_py(PyObject *self, PyObject *argv);

PyObject *dap_chain_wallet_create_with_seed_py(PyObject *self, PyObject *argv);
PyObject *dap_chain_wallet_create_py(PyObject *self, PyObject *argv);
PyObject *dap_chain_wallet_open_file_py(PyObject *self, PyObject *argv);
PyObject *dap_chain_wallet_open_py(PyObject *self, PyObject *argv);
PyObject *dap_chain_wallet_save_py(PyObject *self, PyObject *argv);

PyObject *dap_chain_wallet_close_py(PyObject *self, PyObject *argv);

PyObject *dap_cert_to_addr_py(PyObject *self, PyObject *argv);

PyObject *dap_chain_wallet_get_addr_py(PyObject *self, PyObject *argv);
PyObject *dap_chain_wallet_get_certs_number_py(PyObject *self, PyObject *argv);
PyObject *dap_chain_wallet_get_pkey_py(PyObject *self, PyObject *argv);
PyObject *dap_chain_wallet_get_key_p(PyObject *self, PyObject *argv);

PyObject *dap_chain_wallet_save_file_py(PyObject *self, PyObject *argv);

static PyMethodDef ChainWalletMethods[] = {
        {NULL, NULL, 0, NULL}
};

static PyTypeObject DapChainWallet_dapChainWalletType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "ChainWallet",             /* tp_name */
    sizeof(PyDapChainWalletObject),         /* tp_basicsize */
    0,                         /* tp_itemsize */
    0,                         /* tp_dealloc */
    0,                         /* tp_print */
    0,                         /* tp_getattr */
    0,                         /* tp_setattr */
    0,                         /* tp_reserved */
    0,                         /* tp_repr */
    0,                         /* tp_as_number */
    0,                         /* tp_as_sequence */
    0,                         /* tp_as_mapping */
    0,                         /* tp_hash  */
    0,                         /* tp_call */
    0,                         /* tp_str */
    0,                         /* tp_getattro */
    0,                         /* tp_setattro */
    0,                         /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT |
        Py_TPFLAGS_BASETYPE,   /* tp_flags */
    "Chain wallet object",           /* tp_doc */
    0,		               /* tp_traverse */
    0,		               /* tp_clear */
    0,		               /* tp_richcompare */
    0,		               /* tp_weaklistoffset */
    0,		               /* tp_iter */
    0,		               /* tp_iternext */
    ChainWalletMethods,             /* tp_methods */
    0,                         /* tp_members */
    0,                         /* tp_getset */
    0,                         /* tp_base */
    0,                         /* tp_dict */
    0,                         /* tp_descr_get */
    0,                         /* tp_descr_set */
    0,                         /* tp_dictoffset */
    0,                         /* tp_init */
    0,                         /* tp_alloc */
    PyType_GenericNew,         /* tp_new */
    0,                         /* tp_free */
    0,                         /* tp_is_gc*/
    0,                          /* tp_bases*/
    0,                           /* tp_mro */
    0,                           /* tp_cache */
    0,                           /* tp_subclasses */
    0,                           /* tp_weaklist */
    0,                           /* tp_del */
    0,                          /* tp_version_tag*/
    0,                         /* tp_finalize*/
};


#ifdef __cplusplus
}
#endif


#endif // _DAP_CHAIN_WALLET_PYTHON_
