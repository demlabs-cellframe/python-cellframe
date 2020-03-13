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
        PyObject *sys_module = PyImport_ImportModule("sys");
        sys_path = PyObject_GetAttrString(sys_module, "path");
        //Get list files
        dap_list_name_directories_t *list_plugins_name = dap_get_subs(plugins_root_path);
        dap_list_name_directories_t *element;
        //Loading manifest and start module
        log_it(L_NOTICE, "Registration manifests");
        dap_chain_plugins_manifest_list_create();
        dap_chain_plugins_list_init();
        char *name_file = NULL;
        LL_FOREACH(list_plugins_name, element){
            log_it(L_NOTICE, "Registration %s manifest", element->name_directory);
            name_file = dap_strjoin("",plugins_root_path, element->name_directory, "/manifest.json", NULL);
            if (!dap_chain_plugins_manifest_list_add_from_file(name_file)){
                log_it(L_ERROR, "Registration %s manifest fail", element->name_directory);
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
    log_it(L_NOTICE, "Loading plugins");
    dap_chain_plugins_list_manifest_t *l_manifests = dap_chain_plugins_manifests_get_list();
    dap_chain_plugins_list_manifest_t *man = NULL;
    dap_chain_plugins_list_manifest_t *treshold = NULL;
    dap_chain_plugins_list_manifest_t *tmp = NULL;
    LL_FOREACH_SAFE(l_manifests, man, tmp){
        if (man->name == NULL){
            log_it(L_ERROR, "Can loading plugin, file not found");
            break;
        }
        log_it(L_NOTICE, "Check dependencys for plugin %s", man->name);
        if (man->dependencys != NULL){
            log_it(L_NOTICE, "Plugin have dependencys");
            if (!dap_chain_plugins_list_check_load_plugins(man->dependencys)){
                log_it(L_NOTICE, "Plugin %s add treshold", man->name);
                LL_APPEND(treshold, man);
            }else{
                dap_chain_plugins_load_plugin(dap_strjoin("", plugins_root_path, man->name, "/", NULL), man->name);
            }
        }else{
            dap_chain_plugins_load_plugin(dap_strjoin("", plugins_root_path, man->name, "/", NULL), man->name);
        }
    }

    int c_processed = 0;
    int step_processed = 0;
    int len;
    LL_COUNT(treshold, man, len);
    do{
        LL_FOREACH_SAFE(treshold, man, tmp){
            if (dap_chain_plugins_list_check_load_plugins(man->dependencys)){
                log_it(L_NOTICE, "For plugin %s loading all dependecys", man->name);
                dap_chain_plugins_load_plugin(dap_strjoin("", plugins_root_path, man->name, "/", NULL), man->name);
                LL_DELETE(treshold, man);
                c_processed++;
                step_processed = 0;
            }
        }
        step_processed++;
    }while(c_processed < len && step_processed <= 2);
    //Check loading all treshold
    LL_COUNT(treshold, man, len);
    if (len > 0){
        log_it(L_WARNING, "I can't loading some plugins from list treshold");
        LL_FOREACH(treshold, man){
            log_it(L_ERROR, "The plugin %s does not load with a dependency resolution error.", man->name);
        }
    }
}
void dap_chain_plugins_load_plugin(const char *dir_path, const char *name){
    log_it(L_NOTICE, "Loading %s plugin directory %s", name, dir_path);
    PyErr_Clear();

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
    if (func_deinit == NULL || !PyCallable_Check(func_deinit)){
        log_it(L_WARNING, "For plugins %s don't found function deinit", name);
    }
}

