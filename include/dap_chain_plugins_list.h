#ifndef _DAP_CHAIN_PLUGINS_LIST_
#define _DAP_CHAIN_PLUGINS_LIST_

#include "Python.h"
#include "stdbool.h"
#include "dap_list.h"
#include "dap_common.h"
#include "dap_strfuncs.h"

#ifdef __cplusplus
extern "C" {
#endif

#undef LOG_TAG
#define LOG_TAG "dap_chain_plugins_list"

typedef struct dap_chain_plugin_module{
    char *name;
    PyObject *obj_module;
    bool isFuncOnChainsUpdated;
    bool isFuncOnGdbUpdated;
    bool isFuncOnNetStatusChanged;
}dap_chain_plugins_module_t;

static dap_list_t* dap_chain_plugins_module_list;

int dap_chain_plugins_list_init();
void dap_chain_plugins_list_deint();

void dap_chain_plugins_list_add(PyObject *module, const char *name);

unsigned int dap_chain_plugins_list_lenght();

dap_chain_plugins_module_t *dap_chain_plugins_list_get_module(unsigned int id);
PyObject *dap_chain_plugins_list_get_object(unsigned int id);

bool dap_chai_plugins_list_module_check_on_chains_updated(unsigned int id);
bool dap_chai_plugins_list_module_check_on_gdb_updated(unsigned int id);
bool dap_chai_plugins_list_module_check_on_net_status_updated(unsigned int id);

bool dap_chain_plugins_list_module_del(dap_chain_plugins_module_t *module);
bool dap_chain_plugins_list_id_del(unsigned int id_module);
bool dap_chain_plugins_list_name_del(const char *name_module);


#ifdef __cplusplus
extern "C" {
#endif
#endif // _DAP_CHAIN_PLUGINS_LIST_
