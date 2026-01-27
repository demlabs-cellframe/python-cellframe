#include "include/cf_wallet_internal.h"

int cellframe_wallet_init(PyObject *module) {
    if (cf_common_register_methods(module, cellframe_wallet_core_get_methods()) != 0) {
        return -1;
    }
    if (cf_common_register_methods(module, cellframe_wallet_ops_get_methods()) != 0) {
        return -1;
    }
    if (cf_common_register_methods(module, cellframe_wallet_cli_error_codes_get_methods()) != 0) {
        return -1;
    }
    if (cf_common_register_methods(module, cellframe_wallet_cli_get_methods()) != 0) {
        return -1;
    }
    if (cf_common_register_methods(module, cellframe_wallet_cache_get_methods()) != 0) {
        return -1;
    }
    if (cf_common_register_methods(module, cellframe_wallet_shared_get_methods()) != 0) {
        return -1;
    }
    if (cf_common_register_methods(module, cellframe_wallet_legacy_get_methods()) != 0) {
        return -1;
    }
    return 0;
}
