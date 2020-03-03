#ifndef _DAP_CHAIN_PLUGINS_
#define _DAP_CHAIN_PLUGINS_
#include <Python.h>
#include "dap_config.h"
#include "dap_common.h"
#include "dap_file_utils.h"
#include "dap_chain_plugins_manifest.h"
#include "dap_chain_plugins_list.h"

#ifdef __cplusplus
extern "C"{
#endif

#undef LOG_TAG
#define LOG_TAG "dap_chain_plugins"

typedef struct dap_chain_plugin_init_list{
    //
}dap_chain_plugin_init_list_t;

dap_list_t *list_module_plugins;

const char *plugins_root_path;

int dap_chain_plugins_init();
void dap_chain_plugins_deinit();
void dap_chain_plugins_loading();
void dap_chain_plugins_load_plugin(const char *dir_path, const char *name);
//void dap_chain_plugins_loading_treshold();
bool dap_chain_plugins_check_load_plugin(manifest_t *man);
bool dap_chain_plugins_check_load_plugins(dap_list_t *list);

//int call_f_init_py(const char *path_script, const char *func);

#ifdef __cplusplus
}
#endif

#endif //_DAP_CHAIN_PLUGINS_
