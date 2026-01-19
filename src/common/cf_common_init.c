#include "cf_common_utils.h"

int cf_common_register_chain_atom(PyObject *module);
int cf_common_register_chain_block_callbacks(PyObject *module);
int cf_common_register_chain_common(PyObject *module);
int cf_common_register_chain_net_api(PyObject *module);
int cf_common_register_chain_rpc_callbacks(PyObject *module);
int cf_common_register_chain_types(PyObject *module);
int cf_common_register_cli_error_codes(PyObject *module);

int cellframe_common_init(PyObject *module) {
    if (cf_common_register_chain_atom(module) < 0) {
        return -1;
    }
    if (cf_common_register_chain_block_callbacks(module) < 0) {
        return -1;
    }
    if (cf_common_register_chain_common(module) < 0) {
        return -1;
    }
    if (cf_common_register_chain_net_api(module) < 0) {
        return -1;
    }
    if (cf_common_register_chain_rpc_callbacks(module) < 0) {
        return -1;
    }
    if (cf_common_register_chain_types(module) < 0) {
        return -1;
    }
    if (cf_common_register_cli_error_codes(module) < 0) {
        return -1;
    }
    return 0;
}
