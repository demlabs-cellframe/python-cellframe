#ifndef _WRAPPING_DAP_CHAIN_LEDGER_
#define _WRAPPING_DAP_CHAIN_LEDGER_

#include "Python.h"
#include "dap_chain_ledger.h"
#include "wrapping_dap_chain_common.h"
#include "wrapping_dap_chain_datum_tx.h"
#include "wrapping_dap_chain_datum_token.h"
#include "wrapping_dap_hash.h"
#include "libdap-python.h"
#include "wrapping_dap_chain_tx_out_cond.h"


#ifdef __cplusplus
extern "C" {
#endif

typedef struct PyDapChainLedger{
    PyObject_HEAD
    dap_ledger_t *ledger;
}PyDapChainLedgerObject;

//construct
PyObject *DapChainLedgerObject_create(PyTypeObject *type_object, PyObject *args, PyObject *kwds);
//destructor
void DapChainLedgerObject_free(PyDapChainLedgerObject* object);

PyObject *dap_chain_ledger_set_local_cell_id_py(PyObject *self, PyObject *args);
PyObject *dap_chain_node_datum_tx_calc_hash_py(PyObject *self, PyObject *args);
PyObject *dap_chain_ledger_tx_add_py(PyObject *self, PyObject *args);
PyObject *dap_chain_ledger_token_add_py(PyObject *self, PyObject *args);
PyObject *dap_chain_ledger_token_emission_add_py(PyObject *self, PyObject *args);
PyObject *dap_chain_ledger_token_emission_find_py(PyObject *self, PyObject *args);
PyObject *dap_chain_ledger_tx_get_token_ticker_by_hash_py(PyObject *self, PyObject *args);
PyObject *dap_chain_ledger_addr_get_token_ticker_all_py(PyObject *self, PyObject *args);
PyObject *dap_chain_ledger_addr_get_token_ticker_all_fast_py(PyObject *self, PyObject *args);
PyObject *dap_chain_ledger_tx_cache_check_py(PyObject *self, PyObject *args);
PyObject *dap_chain_node_datum_tx_cache_check_py(PyObject *self, PyObject *args);
PyObject *dap_chain_ledger_tx_remove_py(PyObject *self, PyObject *args);
PyObject *dap_chain_ledger_purge_py(PyObject *self, PyObject *args);
PyObject *dap_chain_ledger_count_py(PyObject *self, PyObject *args);
PyObject *dap_chain_ledger_count_from_to_py(PyObject *self, PyObject *args);
PyObject *dap_chain_ledger_tx_hash_is_used_out_item_py(PyObject *self, PyObject *args);
PyObject *dap_chain_ledger_calc_balance_py(PyObject *self, PyObject *args);
PyObject *dap_chain_ledger_calc_balance_full_py(PyObject *self, PyObject *args);
PyObject *dap_chain_ledger_tx_find_by_hash_py(PyObject *self, PyObject *args);
PyObject *dap_chain_ledger_tx_find_by_addr_py(PyObject *self, PyObject *args);
PyObject *dap_chain_ledger_tx_find_by_pkey_py(PyObject *self, PyObject *args);
PyObject *dap_chain_ledger_tx_cache_find_out_cond_py(PyObject *self, PyObject *args);
PyObject *dap_chain_ledger_tx_cache_get_out_cond_value_py(PyObject *self, PyObject *args);

static PyMethodDef DapChainLedgerMethods[] = {
    {"setLocalCellId", (PyCFunction)dap_chain_ledger_set_local_cell_id_py, METH_VARARGS, ""},
    {"nodeDatumTxCalcHash", (PyCFunction)dap_chain_node_datum_tx_calc_hash_py, METH_VARARGS, ""},
    {"txAdd", (PyCFunction)dap_chain_ledger_tx_add_py, METH_VARARGS, ""},
    {"tokenAdd", (PyCFunction)dap_chain_ledger_token_add_py, METH_VARARGS, ""},
    {"tokenEmissionAdd", (PyCFunction)dap_chain_ledger_token_emission_add_py, METH_VARARGS, ""},
    {"tokenEmissionFind", (PyCFunction)dap_chain_ledger_token_emission_find_py, METH_VARARGS, ""},
    {"txGetTokenTickerByHash", (PyCFunction)dap_chain_ledger_tx_get_token_ticker_by_hash_py, METH_VARARGS, ""},
    {"addrGetTokenTickerAll", (PyCFunction)dap_chain_ledger_addr_get_token_ticker_all_py, METH_VARARGS, ""},
    {"addrGetTokenTickerAllFast", (PyCFunction)dap_chain_ledger_addr_get_token_ticker_all_fast_py, METH_VARARGS, ""},
    {"txCacheCheck", (PyCFunction)dap_chain_ledger_tx_cache_check_py, METH_VARARGS, ""},
    {"datumTxCacheCheck", (PyCFunction)dap_chain_node_datum_tx_cache_check_py, METH_VARARGS, ""},
    {"txRemove", (PyCFunction)dap_chain_ledger_tx_remove_py, METH_VARARGS, ""},
    {"purge", (PyCFunction)dap_chain_ledger_purge_py, METH_VARARGS, ""},
    {"count", (PyCFunction)dap_chain_ledger_count_py, METH_VARARGS, ""},
    {"countFromTo", (PyCFunction)dap_chain_ledger_count_from_to_py, METH_VARARGS, ""},
    {"txHashIsUsedOutItem", (PyCFunction)dap_chain_ledger_tx_hash_is_used_out_item_py, METH_VARARGS, ""},
    {"calcBalance", (PyCFunction)dap_chain_ledger_calc_balance_py, METH_VARARGS, ""},
    {"calcBalanceFull", (PyCFunction)dap_chain_ledger_calc_balance_full_py, METH_VARARGS, ""},
    {"txFindByHash", (PyCFunction)dap_chain_ledger_tx_find_by_hash_py, METH_VARARGS, ""},
    {"txFindByAddr", (PyCFunction)dap_chain_ledger_tx_find_by_addr_py, METH_VARARGS, ""},
    {"txFindByPkey", (PyCFunction)dap_chain_ledger_tx_find_by_pkey_py, METH_VARARGS, ""},
    {"txCacheFindOutCond", (PyCFunction)dap_chain_ledger_tx_cache_find_out_cond_py, METH_VARARGS, ""},
    {"txCacheGetOutCondValue", (PyCFunction)dap_chain_ledger_tx_cache_get_out_cond_value_py, METH_VARARGS, ""},

    {NULL, NULL, 0, NULL}
};

static PyTypeObject DapChainLedger_DapChainLedgerType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "CellFrame.Chain.Ledger",                                      /* tp_name */
    sizeof(PyDapChainLedgerObject),                               /* tp_basicsize */
    0,                                                            /* tp_itemsize */
    0,                                                            /* tp_dealloc */
    0,                                                            /* tp_print */
    0,                                                            /* tp_getattr */
    0,                                                            /* tp_setattr */
    0,                                                            /* tp_reserved */
    0,                                                            /* tp_repr */
    0,                                                            /* tp_as_number */
    0,                                                            /* tp_as_sequence */
    0,                                                            /* tp_as_mapping */
    0,                                                            /* tp_hash  */
    0,                                                            /* tp_call */
    0,                                                            /* tp_str */
    0,                                                            /* tp_getattro */
    0,                                                            /* tp_setattro */
    0,                                                            /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT |
        Py_TPFLAGS_BASETYPE,                                      /* tp_flags */
    "Chain ledger objects",                                              /* tp_doc */
    0,		                                                      /* tp_traverse */
    0,		                                                      /* tp_clear */
    0,		                                                      /* tp_richcompare */
    0,		                                                      /* tp_weaklistoffset */
    0,		                                                      /* tp_iter */
    0,		                                                      /* tp_iternext */
    DapChainLedgerMethods,                                        /* tp_methods */
    0,                                                            /* tp_members */
    0,                                                            /* tp_getset */
    0,                                                            /* tp_base */
    0,                                                            /* tp_dict */
    0,                                                            /* tp_descr_get */
    0,                                                            /* tp_descr_set */
    0,                                                            /* tp_dictoffset */
    0,                                                            /* tp_init */
    0,                                                            /* tp_alloc */
    PyType_GenericNew,                                            /* tp_new */

};

static char*** ListStringToArrayStringFormatChar(PyObject *list);
static size_t *ListIntToSizeT(PyObject *list);


#ifdef __cplusplus
}
#endif

#endif //_WRAPPING_DAP_CHAIN_LEDGER_
