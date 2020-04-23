#ifndef _DAP_CHAIN_PLUGINS_LIST_
#define _DAP_CHAIN_PLUGINS_LIST_

#include "Python.h"
#include "stdbool.h"
//#include "dap_list.h"
#include "dap_common.h"
#include "dap_strfuncs.h"
#include "utlist.h"
#include "dap_chain_plugins_manifest.h"

#ifdef __cplusplus
extern "C" {
#endif

#undef LOG_TAG
#define LOG_TAG "dap_chain_plugins_list"

typedef struct dap_chain_list_plugin_module{
    char *name;
    PyObject *obj_module;
    struct dap_chain_list_plugin_module *next;
}dap_chain_plugin_list_module_t;

static dap_chain_plugin_list_module_t* m_dap_chain_plugins_module_list;

void dap_chain_plugins_list_init();

dap_chain_plugin_list_module_t* dap_chain_plugins_list_get();

bool dap_chain_plugins_list_check_load_plugins(dap_chain_plugins_list_char_t *list);

void dap_chain_plugins_list_add(PyObject *module, const char *name);
void dap_chain_plugins_list_name_del(const char *name);
int dap_chain_plugins_list_name_cmp(dap_chain_plugin_list_module_t *element, const char *name);

#ifdef __cplusplus
extern "C" {
#endif
#endif // _DAP_CHAIN_PLUGINS_LIST_
