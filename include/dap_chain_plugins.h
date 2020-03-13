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

static PyObject *sys_path;

const char *plugins_root_path;

int dap_chain_plugins_init();
void dap_chain_plugins_deinit();
void dap_chain_plugins_loading();
void dap_chain_plugins_load_plugin(const char *dir_path, const char *name);

#ifdef __cplusplus
}
#endif

#endif //_DAP_CHAIN_PLUGINS_
