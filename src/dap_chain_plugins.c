#include "dap_chain_plugins.h"

int dap_chain_plugins_init(dap_config_t *config){
    if(dap_config_get_item_bool_default(config, "plugins", "py_load", false)){
        plugins_root_path = dap_config_get_item_str_default(config, "plugins", "py_path",
                                                            "/opt/cellframe-node/var/plugins/");
        log_it(L_INFO, "Start initialize python plugins. Path plugins %s", plugins_root_path);
        if (!dap_dir_test(plugins_root_path)){
            log_it(L_ERROR, "The directory %s was not found.", plugins_root_path);
            return -1;
        }
        Py_Initialize();
        //Get list files
        dap_list_t *list_plugins_name = dap_get_subs(plugins_root_path);
        //Loading manifest and start module
        log_it(L_INFO, "Registration manifests");
        dap_chain_plugins_manifest_list_create();
        char *name_file = NULL;
        for (unsigned int i = 1; i < dap_list_length(list_plugins_name); i++){
            if ((char*)dap_list_nth_data(list_plugins_name, i) == NULL)
                break;
            log_it(L_NOTICE, "Registration %s manifest", (char*)dap_list_nth_data(list_plugins_name, i));
            name_file = dap_strjoin("",plugins_root_path, (char*)dap_list_nth_data(list_plugins_name, i),"/manifest.json", NULL);
            if (!dap_chain_plugins_manifest_list_add_from_file(name_file)){
                log_it(L_ERROR, "Registration %s manifest fail", (char*)dap_list_nth_data(list_plugins_name, i));
            }
            DAP_FREE(name_file);
        }
        dap_chain_plugins_loading();
    }else{
        log_it(L_NOTICE, "Permission to initialize python plugins has not been obtained.");
        return -2;
    }
    return 0;
}
void dap_chain_plugins_deinit(){
    Py_Finalize();
}

void dap_chain_plugins_loading(){
    //
    log_it(L_NOTICE, "Loading plugins");
    manifest_t *man = NULL;
    dap_list_t *treshold = NULL;
    for (unsigned int i=0; i < dap_chain_plugins_manifests_get_lenght();i++){
        man = dap_chain_plugins_manifest_get_list(i);
        log_it(L_NOTICE, "Check dependencys for plugin %s", man->name);
        if (man->dependencys != NULL ){
            if (!dap_chain_plugins_check_load_plugins(man->dependencys)){
                log_it(L_DEBUG, "Plugin %s add treshold", man->name);
                dap_list_append(treshold, man);
                break;
            }
        }
        dap_chain_plugins_load_plugin(dap_strjoin("", plugins_root_path, man->name, "/", NULL), man->name);
    }
    unsigned int c_processed = 0;
    unsigned int step_processed = 0;
    unsigned int len = dap_list_length(treshold);
    do{
        for (unsigned i=1; i < dap_list_length(treshold);i++){
            man = (manifest_t*)dap_list_nth_data(treshold, i);
            if (dap_chain_plugins_check_load_plugins(man->dependencys)){
                dap_chain_plugins_load_plugin(dap_strjoin("", plugins_root_path, man->name, "/", NULL), man->name);
                dap_list_remove(treshold, man);
                c_processed++;
                step_processed = 0;
                break;
            }
        }
        step_processed++;
    }while(c_processed < len || step_processed == 2);
    //Check loading all treshold
    if (dap_list_length(treshold) > 0){
        log_it(L_WARNING, "I can't loading some plugins from list treshold");
        for (unsigned i=0; i < dap_list_length(treshold);i++){
            man = (manifest_t*)dap_list_nth_data(treshold, i);
            log_it(L_ERROR, "The plugin %s does not load with a dependency resolution error.", man->name);
            dap_list_remove(treshold, man);
            dap_chain_plugins_manifest_free(man);
        }
    }
}
void dap_chain_plugins_load_plugin(const char *dir_path, const char *name){
    log_it(L_NOTICE, "Loading %s plugin directory %s", name, dir_path);
    PyObject *sys_module = PyImport_ImportModule("sys");
    PyObject *sys_path = PyObject_GetAttrString(sys_module, "path");
    PyObject *obj_dir_path = PyUnicode_FromString(dir_path);
    PyList_Append(sys_path, obj_dir_path);
    Py_XDECREF(obj_dir_path);
    PyObject *module = PyImport_ImportModule(name);
    PyObject *func_init = PyObject_GetAttrString(module, "init");
    PyObject *func_deinit = PyObject_GetAttrString(module, "deinit");
    PyObject *res_int = NULL;
    PyErr_Clear();
    if (func_init != NULL && PyCallable_Check(func_init)){
        res_int = PyEval_CallObject(func_init, NULL);
        if (res_int && PyLong_Check(res_int)){
            if (_PyLong_AsInt(res_int) == 0){
                dap_chain_plugins_list_add(module, name);
            } else {
                log_it(L_ERROR, "Code error %i at initialization %s plugin", _PyLong_AsInt(res_int), name);
            }
        } else {
            log_it(L_ERROR, "Function initialization %s plugin don't reterned integer value", name);
        }
        Py_XDECREF(res_int);
    }else {
        log_it(L_ERROR, "For plugins %s don't found function init", name);
    }
//    Py_XDECREF(tuple);
    if (func_deinit == NULL || !PyCallable_Check(func_deinit)){
        log_it(L_WARNING, "For plugins %s don't found function deinit", name);
    }
//    PyObject *res_deint = PyObject_CallObject(func_deinit, NULL);
//    log_it(L_ERROR, "resInt: %zu type obj: %s", res_deint, res_deint->ob_type->tp_name);
}
bool dap_chain_plugins_check_load_plugin(manifest_t *man){
    for (unsigned int i =1; i < dap_chain_plugins_list_lenght(); i++){
        if (strcmp(dap_chain_plugins_list_get_module(i)->name, man->name) == 0)
            return true;
    }
    return false;
}
bool dap_chain_plugins_check_load_plugins(dap_list_t *list){
    for (unsigned int i=1; i < dap_list_length(list); i++){
        for (unsigned int i =0; i < dap_chain_plugins_list_lenght(); i++){
            if (strcmp(dap_chain_plugins_list_get_module(i)->name, (char*)dap_list_nth_data(list, i)) != 0)
                return false;
        }
    }
    return true;
}
