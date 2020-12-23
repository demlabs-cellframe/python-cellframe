#include "dap_config.h"
#include "dap_common.h"
#include "dap_file_utils.h"
#include "dap_chain_plugins_manifest.h"
#include "dap_chain_plugins_list.h"
#include "python-cellframe.h"
#include "dap_chain_plugins_command.h"

#include "dap_chain_plugins.h"

#undef LOG_TAG
#define LOG_TAG "dap_chain_plugins"

PyObject *s_sys_path = NULL;
const char *s_plugins_root_path = NULL;


int dap_chain_plugins_init(dap_config_t *a_config){
    if(dap_config_get_item_bool_default(a_config, "plugins", "py_load", false)){
        const char *l_defaule_path_plugins = dap_strjoin(NULL, "/opt/", dap_get_appname(), "/var/plugins/", NULL);
        const char *l_plugins_root_path = dap_config_get_item_str_default(a_config, "plugins", "py_path",
                                                            l_defaule_path_plugins);
        s_plugins_root_path = dap_strjoin(NULL, l_plugins_root_path, "/", NULL);
        log_it(L_INFO, "Start initialize python plugins. Path plugins %s", s_plugins_root_path);
        if (!dap_dir_test(s_plugins_root_path)){
            log_it(L_ERROR, "The directory %s was not found.", s_plugins_root_path);
            return -1;
        }
        PyImport_AppendInittab("API_CellFrame", PyInit_libCellFrame);
        Py_Initialize();
        PyEval_InitThreads();
        log_it(L_NOTICE, "Version python: %s", Py_GetVersion());
        PyObject *l_sys_module = PyImport_ImportModule("sys");
        s_sys_path = PyObject_GetAttrString(l_sys_module, "path");
        //Get list files
        dap_list_name_directories_t *l_list_plugins_name = dap_get_subs(s_plugins_root_path);
        dap_list_name_directories_t *l_element;
        //Loading manifest and start module
        log_it(L_NOTICE, "Registration manifests");
        dap_chain_plugins_manifest_list_create();
        dap_chain_plugins_list_init();
        char *l_name_file = NULL;
        LL_FOREACH(l_list_plugins_name, l_element){
            log_it(L_NOTICE, "Registration %s manifest", l_element->name_directory);
            l_name_file = dap_strjoin("",l_plugins_root_path, "/", l_element->name_directory, "/manifest.json", NULL);
            if (!dap_chain_plugins_manifest_list_add_from_file(l_name_file)){
                log_it(L_ERROR, "Registration %s manifest fail", l_element->name_directory);
            }
            DAP_FREE(l_name_file);
        }
        dap_chain_plugins_loading();
        dap_chain_plugins_command_create();
        PyThreadState *l_thread_state = PyEval_SaveThread();
    }else{
        log_it(L_NOTICE, "Permission to initialize python plugins has not been obtained.");
        return -2;
    }
    return 0;
}

void dap_chain_plugins_loading(){
    log_it(L_NOTICE, "Loading plugins");
    dap_chain_plugins_list_manifest_t *l_manifests = dap_chain_plugins_manifests_get_list();
    dap_chain_plugins_list_manifest_t *l_man = NULL;
    dap_chain_plugins_list_manifest_t *l_treshold = NULL;
    dap_chain_plugins_list_manifest_t *l_tmp = NULL;
    if (l_manifests == NULL)
        return;
    LL_FOREACH_SAFE(l_manifests, l_man, l_tmp){
        if (l_man->name == NULL){
            log_it(L_ERROR, "Can loading plugin, file not found");
            break;
        }
        log_it(L_NOTICE, "Check dependencys for plugin %s", l_man->name);
        if (l_man->dependencys != NULL){
            log_it(L_NOTICE, "Plugin have dependencys");
            if (!dap_chain_plugins_list_check_load_plugins(l_man->dependencys)){
                log_it(L_NOTICE, "Plugin %s add treshold", l_man->name);
                LL_APPEND(l_treshold, l_man);
            }else{
                dap_chain_plugins_load_plugin(dap_strjoin("", s_plugins_root_path, l_man->name, "/", NULL), l_man->name);
            }
        }else{
            dap_chain_plugins_load_plugin(dap_strjoin("", s_plugins_root_path, l_man->name, "/", NULL), l_man->name);
        }
    }

    int l_c_processed = 0;
    int l_step_processed = 0;
    int l_len;
    LL_COUNT(l_treshold, l_man, l_len);
    do{
        LL_FOREACH_SAFE(l_treshold, l_man, l_tmp){
            if (dap_chain_plugins_list_check_load_plugins(l_man->dependencys)){
                log_it(L_NOTICE, "For plugin %s loading all dependecys", l_man->name);
                dap_chain_plugins_load_plugin(dap_strjoin("", s_plugins_root_path, l_man->name, "/", NULL), l_man->name);
                LL_DELETE(l_treshold, l_man);
                l_c_processed++;
                l_step_processed = 0;
            }
        }
        l_step_processed++;
    }while(l_c_processed < l_len && l_step_processed <= 2);
    //Check loading all treshold
    LL_COUNT(l_treshold, l_man, l_len);
    if (l_len > 0){
        log_it(L_WARNING, "I can't loading some plugins from list treshold");
        LL_FOREACH(l_treshold, l_man){
            log_it(L_ERROR, "The plugin %s does not load with a dependency resolution error.", l_man->name);
        }
    }
}
void dap_chain_plugins_load_plugin(const char *a_dir_path, const char *a_name){
    log_it(L_NOTICE, "Loading %s plugin directory %s", a_name, a_dir_path);
    PyErr_Clear();
    PyObject *l_obj_dir_path = PyUnicode_FromString(a_dir_path);
    PyList_Append(s_sys_path, l_obj_dir_path);
//    Py_XDECREF(l_obj_dir_path);
    PyObject *l_name_obj = PyUnicode_FromString(a_name);
    log_it(L_ERROR, "Add module");
    PyObject *l_module = PyImport_Import(l_name_obj);
//    PyObject *l_module = PyImport_ImportModule(a_name);
//    PyImport_Import()
    PyObject *l_func_init = PyObject_GetAttrString(l_module, "init");
    PyObject *l_func_deinit = PyObject_GetAttrString(l_module, "deinit");
    PyObject *l_res_int = NULL;
    if (l_func_init != NULL && PyCallable_Check(l_func_init)){
        l_res_int = PyEval_CallObject(l_func_init, NULL);
        if (l_res_int && PyLong_Check(l_res_int)){
            if (_PyLong_AsInt(l_res_int) == 0){
                dap_chain_plugins_list_add(l_module, a_name);
            } else {
                PyErr_Print();
                log_it(L_ERROR, "Code error %i at initialization %s plugin", _PyLong_AsInt(l_res_int), a_name);
            }
        } else {
            PyErr_Print();
            log_it(L_ERROR, "Function initialization %s plugin don't reterned integer value", a_name);
        }
        Py_XDECREF(l_res_int);
    }else {
        log_it(L_ERROR, "For plugins %s don't found function init", a_name);
    }
    if (l_func_deinit == NULL || !PyCallable_Check(l_func_deinit)){
        log_it(L_WARNING, "For plugins %s don't found function deinit", a_name);
    }
}

void dap_chain_plugins_deinit(){
    log_it(L_NOTICE, "Deinit python plugins");
    dap_chain_plugin_list_module_t *l_plugins = dap_chain_plugins_list_get();
    dap_chain_plugin_list_module_t *l_plugin;
    dap_chain_plugin_list_module_t *l_tmp;
    PyObject *l_res_int = NULL;
    LL_FOREACH_SAFE(l_plugins, l_plugin, l_tmp){
        PyObject *l_func_deinit = PyObject_GetAttrString(l_plugin->obj_module, "deinit");
        if (l_func_deinit != NULL || PyCallable_Check(l_func_deinit)){
            l_res_int = PyEval_CallObject(l_func_deinit, NULL);
        } else {
            log_it(L_WARNING, "For plugin %s can't callable function deinit", l_plugin->name);
        }
        DAP_FREE(l_plugin->name);
        Py_XDECREF(l_plugin->obj_module);
        LL_DELETE(l_plugins, l_plugin);
    }
    dap_chain_plugins_manifest_list_delete_all();
    Py_Finalize();
}
int dap_chain_plugins_reload_plugin(const char * a_name_plugin){
    log_it(L_NOTICE, "Reload plugin %s", a_name_plugin);
    dap_chain_plugin_list_module_t *l_plugins = dap_chain_plugins_list_get();
    dap_chain_plugin_list_module_t *l_plugin = NULL;
    LL_SEARCH(l_plugins, l_plugin, a_name_plugin, dap_chain_plugins_list_name_cmp);
    if (l_plugin == NULL)
        return -4;
    PyObject *l_func_deinit = PyObject_GetAttrString(l_plugin->obj_module, "deinit");
    if (l_func_deinit != NULL || PyCallable_Check(l_func_deinit)){
        PyEval_CallObject(l_func_deinit, NULL);
    } else {
        log_it(L_WARNING, "For plugin %s can't callable function deinit", l_plugin->name);
    }
    DAP_FREE(l_plugin->name);
    Py_XDECREF(l_plugin->obj_module);
    LL_DELETE(l_plugins, l_plugin);
    dap_chain_plugins_manifest_list_delete_name(a_name_plugin);
    //Loading plugin
    char *l_path_plugin = dap_strjoin(NULL, s_plugins_root_path, a_name_plugin, NULL);
    char *l_name_file_manifest = dap_strjoin("",s_plugins_root_path, a_name_plugin, "/manifest.json", NULL);
    if (!dap_chain_plugins_manifest_list_add_from_file(l_name_file_manifest)){
        log_it(L_ERROR, "Registration %s manifest fail", l_path_plugin);
        return -3;
    }
    DAP_FREE(l_name_file_manifest);
    dap_chain_plugins_list_manifest_t *l_manifest =  dap_chain_plugins_manifest_list_get_name(a_name_plugin);
    if (l_manifest->dependencys != NULL){
        if (!dap_chain_plugins_list_check_load_plugins(l_manifest->dependencys)){
            log_it(L_NOTICE, "%s plugin has unresolved dependencys, restart all plagins", l_manifest->name);
            return -2;
        }else{
            dap_chain_plugins_load_plugin(dap_strjoin("", s_plugins_root_path, l_manifest->name, "/", NULL), l_manifest->name);
            return 0;
        }
    }else{
        dap_chain_plugins_load_plugin(dap_strjoin("", s_plugins_root_path, l_manifest->name, "/", NULL), l_manifest->name);
        return 0;
    }

    return -1;
}

