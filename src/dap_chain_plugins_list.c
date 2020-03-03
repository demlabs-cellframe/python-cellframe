#include "dap_chain_plugins_list.h"

int dap_chain_plugins_list_init(){
    dap_chain_plugins_module_list = dap_list_alloc();
}
void dap_chain_plugins_list_deint(){
    unsigned int len = dap_chain_plugins_list_lenght();
    if (len > 0){
        for (unsigned int i=0; i < len; i++){
            dap_chain_plugins_list_id_del(i);
        }
    }
    dap_list_free(dap_chain_plugins_module_list);
}

void dap_chain_plugins_list_add(PyObject *module, const char *name){
    dap_chain_plugins_module_t *module_s = (dap_chain_plugins_module_t*)malloc(sizeof (dap_chain_plugins_module_t));
    module_s->name = dap_strdup(name);
    module_s->obj_module = module;
    module_s->isFuncOnGdbUpdated = true;
    module_s->isFuncOnChainsUpdated = true;
    module_s->isFuncOnNetStatusChanged = true;
    //Checking func
    PyObject *F_FuncOnGdbUpdated = PyObject_GetAttrString(module_s->obj_module, "onGdbUpdated");
    PyObject *F_FuncOnChainsUpdated = PyObject_GetAttrString(module_s->obj_module, "onChainsUpdated");
    PyObject *F_FuncOnNetStatusChanged = PyObject_GetAttrString(module_s->obj_module, "onNetStatusChanged");
    if (F_FuncOnGdbUpdated == NULL || !PyCallable_Check(F_FuncOnGdbUpdated)){
        log_it(L_WARNING, "Plugin %s don't callable function onGdbUpdated", module_s->name);
        module_s->isFuncOnGdbUpdated = false;
    }
    if (F_FuncOnChainsUpdated == NULL || !PyCallable_Check(F_FuncOnChainsUpdated)){
        log_it(L_WARNING, "Plugin %s don't callable function onChainsUpdated", module_s->name);
        module_s->isFuncOnChainsUpdated = false;
    }
    if (F_FuncOnNetStatusChanged == NULL || !PyCallable_Check(F_FuncOnNetStatusChanged)){
        log_it(L_WARNING, "Plugin %s don't callable function onNetStatusChanged", module_s->name);
        module_s->isFuncOnNetStatusChanged = false;
    }
    //Added structur to list
    dap_list_append(dap_chain_plugins_module_list, module_s);
}

unsigned int dap_chain_plugins_list_lenght(){
    return dap_list_length(dap_chain_plugins_module_list);
}

dap_chain_plugins_module_t *dap_chain_plugins_list_get_module(unsigned int id){
    if (dap_chain_plugins_list_lenght() < id)
        return NULL;
    return (dap_chain_plugins_module_t*)dap_list_nth_data(dap_chain_plugins_module_list, id);
}
PyObject *dap_chain_plugins_list_get_object(unsigned int id){
    if (dap_chain_plugins_list_lenght() < id)
        return NULL;
    return (dap_chain_plugins_list_get_module(id))->obj_module;
}

bool dap_chai_plugins_list_module_check_on_chains_updated(unsigned int id){
    if (dap_chain_plugins_list_lenght() < id)
        return NULL;
    dap_chain_plugins_module_t *module = (dap_chain_plugins_module_t*)dap_list_nth_data(
                dap_chain_plugins_module_list, id
                );
    return module->isFuncOnChainsUpdated;
}
bool dap_chai_plugins_list_module_check_on_gdb_updated(unsigned int id){
    if (dap_chain_plugins_list_lenght() < id)
        return NULL;
    dap_chain_plugins_module_t *module = (dap_chain_plugins_module_t*)dap_list_nth_data(
                dap_chain_plugins_module_list, id
                );
    return module->isFuncOnGdbUpdated;
}
bool dap_chai_plugins_list_module_check_on_net_status_updated(unsigned int id){
    if (dap_chain_plugins_list_lenght() < id)
        return NULL;
    dap_chain_plugins_module_t *module = (dap_chain_plugins_module_t*)dap_list_nth_data(
                dap_chain_plugins_module_list, id
                );
    return module->isFuncOnNetStatusChanged;
}

bool dap_chain_plugins_list_module_del(dap_chain_plugins_module_t *module){
    for (unsigned int i=0; i < dap_chain_plugins_list_lenght(); i++){
        if (((dap_chain_plugins_module_t*)dap_list_nth_data(dap_chain_plugins_module_list, i)) == module){
            dap_chain_plugins_list_id_del(i);
            return true;
        }
    }
    return false;
}
bool dap_chain_plugins_list_id_del(unsigned int id_module){
    if (dap_chain_plugins_list_lenght() < id_module)
        return false;
    dap_chain_plugins_module_t *mod = (dap_chain_plugins_module_t*)dap_list_nth_data(dap_chain_plugins_module_list, id_module);
    Py_XDECREF(mod->obj_module);
    free(mod->name);
    dap_list_remove(dap_chain_plugins_module_list, dap_list_nth_data(dap_chain_plugins_module_list, id_module));
    return true;
}
bool dap_chain_plugins_list_name_del(const char *name_module){
    for (unsigned int i=0; i < dap_chain_plugins_list_lenght(); i++){
        if (strcmp(((dap_chain_plugins_module_t*)dap_list_nth_data(dap_chain_plugins_module_list, i))->name, name_module) == 0){
            dap_chain_plugins_list_id_del(i);
            return true;
        }
    }
    return false;
}
