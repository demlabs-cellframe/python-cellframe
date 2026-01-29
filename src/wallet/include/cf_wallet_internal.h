#ifndef CELLFRAME_WALLET_INTERNAL_H
#define CELLFRAME_WALLET_INTERNAL_H

#include "cellframe.h"
#include "cf_common_utils.h"
#include "dap_chain_wallet_ops.h"
#include "dap_chain_wallet_cli_error_codes.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LOG_TAG "python_cellframe_wallet"

PyMethodDef *cellframe_wallet_core_get_methods(void);
PyMethodDef *cellframe_wallet_ops_get_methods(void);
PyMethodDef *cellframe_wallet_cli_error_codes_get_methods(void);
PyMethodDef *cellframe_wallet_cli_get_methods(void);
PyMethodDef *cellframe_wallet_cache_get_methods(void);
PyMethodDef *cellframe_wallet_shared_get_methods(void);
PyMethodDef *cellframe_wallet_legacy_get_methods(void);

int dap_chain_wallet_cli_init(void);
void dap_chain_wallet_cli_deinit(void);

#ifdef __cplusplus
}
#endif

#endif /* CELLFRAME_WALLET_INTERNAL_H */
