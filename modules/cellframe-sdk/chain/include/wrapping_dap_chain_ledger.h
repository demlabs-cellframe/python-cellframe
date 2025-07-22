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
#include "dap_chain_datum_tx_items.h"


#ifdef __cplusplus
extern "C" {
#endif

typedef struct PyDapChainLedger{
    PyObject_HEAD
    dap_ledger_t *ledger;
}PyDapChainLedgerObject;

typedef struct PyDapChainLedgerEvent{
    PyObject_HEAD
    dap_chain_tx_event_t *event;
    bool origin;
}PyDapChainLedgerEventObject;

//construct
PyObject *DapChainLedgerObject_create(PyTypeObject *type_object, PyObject *args, PyObject *kwds);
//destructor
void DapChainLedgerObject_free(PyDapChainLedgerObject* object);

PyObject *DapChainLedgerEventObject_create(PyTypeObject *type_object, PyObject *args, PyObject *kwds);
void DapChainLedgerEventObject_free(PyDapChainLedgerEventObject* object);

PyObject *dap_chain_ledger_set_local_cell_id_py(PyObject *self, PyObject *args);
PyObject *dap_chain_node_datum_tx_calc_hash_py(PyObject *self, PyObject *args);
PyObject *dap_chain_ledger_tx_add_py(PyObject *self, PyObject *args);
PyObject *dap_chain_ledger_token_add_py(PyObject *self, PyObject *args);
PyObject *dap_chain_ledger_token_emission_load_py(PyObject *self, PyObject *args);
PyObject *dap_chain_ledger_token_emission_find_py(PyObject *self, PyObject *args);
PyObject *dap_chain_ledger_tx_get_token_ticker_by_hash_py(PyObject *self, PyObject *args);
PyObject *dap_chain_ledger_addr_get_token_ticker_all_py(PyObject *self, PyObject *args);
PyObject *dap_chain_ledger_addr_get_token_ticker_all_fast_py(PyObject *self, PyObject *args);
PyObject *dap_chain_node_datum_tx_cache_check_py(PyObject *self, PyObject *args);
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
PyObject *dap_chain_ledger_get_txs_py(PyObject *self, PyObject *args);
PyObject *dap_chain_ledger_tx_add_notify_py(PyObject *self, PyObject *args);
PyObject *dap_chain_ledger_token_auth_signs_total_py(PyObject *self, PyObject *args);
PyObject *dap_chain_ledger_token_auth_signs_valid_py(PyObject *self, PyObject *args);
PyObject *dap_chain_ledger_token_auth_pkeys_hashes_py(PyObject *self, PyObject *args);
PyObject *dap_chain_ledger_tx_hash_is_used_out_item_hash_py(PyObject *self, PyObject *args);
PyObject *dap_chain_ledger_tx_get_main_ticker_py(PyObject *self, PyObject *args);
PyObject *dap_chain_ledger_get_unspent_outputs_for_amount_py(PyObject *self, PyObject *args);

// Event functions
PyObject *dap_chain_ledger_event_find_py(PyObject *self, PyObject *args);
PyObject *dap_chain_ledger_event_get_list_py(PyObject *self, PyObject *args);
PyObject *dap_chain_ledger_event_pkey_list_py(PyObject *self, PyObject *args);

// Event object methods
PyObject *dap_chain_ledger_event_get_group_name_py(PyObject *self, void *closure);
PyObject *dap_chain_ledger_event_get_type_py(PyObject *self, void *closure);
PyObject *dap_chain_ledger_event_get_data_py(PyObject *self, void *closure);

extern PyTypeObject DapChainLedgerObjectType;
extern PyTypeObject DapChainLedgerEventObjectType;

static char*** ListStringToArrayStringFormatChar(PyObject *list);
static size_t *ListIntToSizeT(PyObject *list);


#ifdef __cplusplus
}
#endif

#endif //_WRAPPING_DAP_CHAIN_LEDGER_
