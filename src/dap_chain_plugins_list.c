#include "dap_chain_plugins_list.h"

void dap_chain_plugins_list_init(){
    m_dap_chain_plugins_module_list = NULL;
}

void dap_chain_plugins_list_add(PyObject *module, const char *name){
    dap_chain_plugin_list_module_t *elemnet = (dap_chain_plugin_list_module_t*)DAP_NEW(dap_chain_plugin_list_module_t);
    elemnet->name = dap_strdup(name);
    elemnet->obj_module = module;
    LL_APPEND(m_dap_chain_plugins_module_list, elemnet);
}

dap_chain_plugin_list_module_t* dap_chain_plugins_list_get(){
    return m_dap_chain_plugins_module_list;
}

int dap_chain_plugins_list_cmp(dap_chain_plugin_list_module_t *e1, dap_chain_plugin_list_module_t *e2){
    return strcmp(e1->name, e2->name);
}

bool dap_chain_plugins_list_check_load_plugins(dap_chain_plugins_list_char_t *list){
    dap_chain_plugins_list_char_t *value_from_list = NULL;
    dap_chain_plugin_list_module_t *element_from_list_module;
    dap_chain_plugin_list_module_t *element_lnk = (dap_chain_plugin_list_module_t*)DAP_NEW(
                dap_chain_plugin_list_module_t);

    int lenght;
    LL_COUNT(m_dap_chain_plugins_module_list, element_from_list_module, lenght);
    if (lenght == 0)
        return false;
    LL_FOREACH(list, value_from_list){
        element_lnk->name = value_from_list->value;
        LL_SEARCH(m_dap_chain_plugins_module_list, element_from_list_module, element_lnk, dap_chain_plugins_list_cmp);
        if (!element_from_list_module)
            return false;
    }
    return true;
}

void dap_chain_plugins_list_name_del(const char *name){
    dap_chain_plugin_list_module_t *plugin;
    dap_chain_plugin_list_module_t *tmp;
    bool plugin_searcging = false;
    LL_FOREACH_SAFE(m_dap_chain_plugins_module_list, plugin, tmp){
        if (strcmp(plugin->name, name) == 0){
            DAP_FREE(plugin->name);
            Py_XDECREF(plugin->obj_module);
            LL_DELETE(m_dap_chain_plugins_module_list, plugin);
            plugin_searcging = true;
        }
    }
    if (!plugin_searcging){
        log_it(L_WARNING, "Can't searching plugins %s for delete", name);
    }
}

