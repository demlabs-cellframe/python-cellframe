#ifndef CELLFRAME_SERVICES_EXT_HELPERS_H
#define CELLFRAME_SERVICES_EXT_HELPERS_H

#include "cellframe.h"
#include "dap_chain_net_srv_stake_common.h"
#include "dap_chain_net_srv_stake_ext.h"

#ifdef __cplusplus
extern "C" {
#endif

uint256_t py_obj_to_uint256(PyObject *a_obj);
PyObject *cf_services_uint256_to_py_str(uint256_t a_value);
PyObject *cf_services_stake_item_to_dict(const dap_chain_net_srv_stake_item_t *a_item);
PyObject *cf_services_stake_ext_to_dict(const dap_chain_net_srv_stake_ext_t *a_item, bool a_include_positions);
dap_chain_net_t *cf_services_get_net(PyObject *a_obj);

#ifdef __cplusplus
}
#endif

#endif /* CELLFRAME_SERVICES_EXT_HELPERS_H */
