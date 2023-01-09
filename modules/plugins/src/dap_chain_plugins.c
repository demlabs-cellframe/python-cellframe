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

PyThreadState *s_thread_state;

typedef struct _dap_chain_plugins_module{
    PyObject *module;
    char *name;
    struct _dap_chain_plugins_module *next;
}_dap_chain_plugins_module_t;

_dap_chain_plugins_module_t *_s_modules = NULL;

int dap_chain_plugins_init(dap_config_t *a_config){
    if(dap_config_get_item_bool_default(a_config, "plugins", "py_load", false)){
        const char *l_defaule_path_plugins = dap_strjoin(NULL, "/opt/", dap_get_appname(), "/var/plugins/", NULL);
        const char *l_plugins_root_path = dap_config_get_item_str_default(a_config, "plugins", "py_path",
                                                            l_defaule_path_plugins);
        s_plugins_root_path = dap_strjoin(NULL, l_plugins_root_path, "/", NULL);
        log_it(L_INFO, "Start initialization of python plugins. Path plugins: %s", s_plugins_root_path);
        if (!dap_dir_test(s_plugins_root_path)){
            log_it(L_ERROR, "Can't find \"%s\" directory", s_plugins_root_path);
            return -1;
        }
        PyImport_AppendInittab("DAP", PyInit_libDAP);
        PyImport_AppendInittab("CellFrame", PyInit_libCellFrame);
        #ifdef DAP_BUILD_WITH_PYTHON_ENV
            const wchar_t *l_python_env_path = L"/opt/cellframe-node/lib/python3.7";
            Py_SetPath(l_python_env_path);
        #endif
        Py_Initialize();
        PyEval_InitThreads();
        PyObject *l_sys_module = PyImport_ImportModule("sys");
        s_sys_path = PyObject_GetAttrString(l_sys_module, "path");

        #ifdef DAP_BUILD_WITH_PYTHON_ENV
            PyObject *l_obj_dir_path_dynload = PyUnicode_FromString("/opt/cellframe-node/lib/python3.7/lib-dynload");
            PyList_Append(s_sys_path, l_obj_dir_path_dynload);

            PyObject *l_obj_dir_path_sp = PyUnicode_FromString("/opt/cellframe-node/lib/python3.7/site-packages");
            PyList_Append(s_sys_path, l_obj_dir_path_sp);
        #endif    

        //Get list files
        dap_list_name_directories_t *l_list_plugins_name = dap_get_subs(s_plugins_root_path);
        dap_list_name_directories_t *l_element;
        //Loading manifest and start module
        log_it(L_NOTICE, "Start registration of manifests");
        dap_chain_plugins_manifest_list_create();
        dap_chain_plugins_list_init();
        char *l_name_file = NULL;
        LL_FOREACH(l_list_plugins_name, l_element){
            log_it(L_NOTICE, "Registration of \"%s\" manifest", l_element->name_directory);
            l_name_file = dap_strjoin("",l_plugins_root_path, "/", l_element->name_directory, "/manifest.json", NULL);
            if (!dap_chain_plugins_manifest_list_add_from_file(l_name_file)){
                log_it(L_ERROR, "Registration of \"%s\" manifest is failed", l_element->name_directory);
            }
            DAP_FREE(l_name_file);
        }
        dap_chain_plugins_loading();
        dap_chain_plugins_command_create();
        s_thread_state = PyEval_SaveThread();

    }else{
        return 0;
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
            log_it(L_ERROR, "Can't load a plugin, file not found");
            break;
        }
        log_it(L_NOTICE, "Check dependencies for plugin: %s", l_man->name);
        if (l_man->dependencies != NULL){
            log_it(L_NOTICE, "Plugin has dependencies");
            if (!dap_chain_plugins_list_check_load_plugins(l_man->dependencies)){
                log_it(L_NOTICE, "Plugin %s add treshold", l_man->name);
                LL_APPEND(l_treshold, l_man);
            }else{
                dap_chain_plugins_load_plugin_importing(dap_strjoin("", s_plugins_root_path, l_man->name, "/", NULL), l_man->name);
            }
        }else{
            dap_chain_plugins_load_plugin_importing(dap_strjoin("", s_plugins_root_path, l_man->name, "/", NULL), l_man->name);
        }
    }

    int l_c_processed = 0;
    int l_step_processed = 0;
    int l_len;
    LL_COUNT(l_treshold, l_man, l_len);
    do{
        LL_FOREACH_SAFE(l_treshold, l_man, l_tmp){
            if (dap_chain_plugins_list_check_load_plugins(l_man->dependencies)){
                log_it(L_NOTICE, "For \"%s\" plugin all dependecies is loaded", l_man->name);
                dap_chain_plugins_load_plugin_importing(dap_strjoin("", s_plugins_root_path, l_man->name, "/", NULL), l_man->name);
                LL_DELETE(l_treshold, l_man);
                l_c_processed++;
                l_step_processed = 0;
            }
        }
        l_step_processed++;
    }while(l_c_processed < l_len && l_step_processed <= 2);
    dap_chain_plugins_load_plugin_initialization();
    //Check loading all treshold
    LL_COUNT(l_treshold, l_man, l_len);
    if (l_len > 0){
        log_it(L_WARNING, "Can't load some plugins from list treshold");
        LL_FOREACH(l_treshold, l_man){
            log_it(L_ERROR, "The \"%s\" plugin isn't loaded with a dependency resolution error", l_man->name);
        }
    }
}

void dap_chain_plugins_load_plugin_importing(const char *a_dir_path, const char *a_name){
    log_it(L_NOTICE, "Import \"%s\" module from \"%s\" directory", a_name, a_dir_path);
    PyObject *l_obj_dir_path = PyUnicode_FromString(a_dir_path);
    PyList_Append(s_sys_path, l_obj_dir_path);
    Py_XDECREF(l_obj_dir_path);
    PyObject *l_module = PyImport_ImportModule(a_name);
    if (!l_module){
        python_error_in_log_it(LOG_TAG);
        PyErr_Clear();
        return;
    }
    _dap_chain_plugins_module_t *module = DAP_NEW(_dap_chain_plugins_module_t);
    module->module = l_module;
    module->name = dap_strdup(a_name);
    Py_INCREF(l_module);
    LL_APPEND(_s_modules, module);
}

void dap_chain_plugins_load_plugin_initialization(){
    _dap_chain_plugins_module_t  *l_container = NULL;
    LL_FOREACH(_s_modules, l_container){
        PyObject *l_func_init = PyObject_GetAttrString(l_container->module, "init");
        PyObject *l_func_deinit = PyObject_GetAttrString(l_container->module, "deinit");
        PyObject *l_res_int = NULL;
        PyErr_Clear();
        if (PyCallable_Check(l_func_init)) {
            l_res_int = PyEval_CallObject(l_func_init, NULL);
            if (l_res_int && PyLong_Check(l_res_int)) {
                if (_PyLong_AsInt(l_res_int) == 0) {
                    dap_chain_plugins_list_add(l_container->module, l_container->name);
                    Py_INCREF(l_container->module);
                } else {
                    python_error_in_log_it(LOG_TAG);
                    log_it(L_ERROR, "Can't initialize \"%s\" plugin. Code error: %i", l_container->name,
                     _PyLong_AsInt(l_res_int));
                }
            } else {
                python_error_in_log_it(LOG_TAG);
                log_it(L_ERROR, "The 'init' function of \"%s\" plugin didn't return an integer value", l_container->name);
            }
            Py_XDECREF(l_res_int);
        } else {
            log_it(L_ERROR, "Can't find 'init' function of \"%s\" plugin", l_container->name);
        }
        if (l_func_deinit == NULL || !PyCallable_Check(l_func_deinit)){
            log_it(L_WARNING, "Can't find 'deinit' function of \"%s\" plugin", l_container->name);
        }
    }
}

void dap_chain_plugins_load_plugin(const char *a_dir_path, const char *a_name){
    log_it(L_NOTICE, "Loading \"%s\" plugin directory %s", a_name, a_dir_path);
    PyErr_Clear();

    PyObject *l_obj_dir_path = PyUnicode_FromString(a_dir_path);
    PyList_Append(s_sys_path, l_obj_dir_path);
    Py_XDECREF(l_obj_dir_path);
    PyObject *l_module = PyImport_ImportModule(a_name);
    python_error_in_log_it(LOG_TAG);
    PyObject *l_func_init = PyObject_GetAttrString(l_module, "init");
    PyObject *l_func_deinit = PyObject_GetAttrString(l_module, "deinit");
    PyObject *l_res_int = NULL;
    PyErr_Clear();
    if (l_func_init != NULL && PyCallable_Check(l_func_init)){
        l_res_int = PyEval_CallObject(l_func_init, NULL);
        if (l_res_int && PyLong_Check(l_res_int)){
            if (_PyLong_AsInt(l_res_int) == 0){
                dap_chain_plugins_list_add(l_module, a_name);
            } else {
                python_error_in_log_it(LOG_TAG);
                log_it(L_ERROR, "Can't initialize \"%s\" plugin. Code error: %i", a_name, _PyLong_AsInt(l_res_int));
            }
        } else {
            python_error_in_log_it(LOG_TAG);
            log_it(L_ERROR, "The 'init' function of \"%s\" plugin didn't return an integer value", a_name);
        }
        Py_XDECREF(l_res_int);
    }else {
        log_it(L_ERROR, "Can't find 'init' function of \"%s\" plugin", a_name);
    }
    if (l_func_deinit == NULL || !PyCallable_Check(l_func_deinit)){
        log_it(L_WARNING, "Can't find 'deinit' function of \"%s\" plugin", a_name);
    }
}

void dap_chain_plugins_deinit(){
    log_it(L_NOTICE, "Deinitialize python plugins");
    dap_chain_plugin_list_module_t *l_plugins = dap_chain_plugins_list_get();
    dap_chain_plugin_list_module_t *l_plugin;
    dap_chain_plugin_list_module_t *l_tmp;
    PyObject *l_res_int = NULL;
    LL_FOREACH_SAFE(l_plugins, l_plugin, l_tmp){
        PyObject *l_func_deinit = PyObject_GetAttrString(l_plugin->obj_module, "deinit");
        if (l_func_deinit != NULL || PyCallable_Check(l_func_deinit)){
            l_res_int = PyEval_CallObject(l_func_deinit, NULL);
        } else {
            log_it(L_WARNING, "The 'deinit' object of \"%s\" plugin isn't a callable", l_plugin->name);
        }
        DAP_FREE(l_plugin->name);
        Py_XDECREF(l_plugin->obj_module);
        LL_DELETE(l_plugins, l_plugin);
    }
    dap_chain_plugins_manifest_list_delete_all();
    PyEval_RestoreThread(s_thread_state);
    Py_Finalize();
}
int dap_chain_plugins_reload_plugin(const char * a_name_plugin){
    log_it(L_NOTICE, "Reload \"%s\" plugin", a_name_plugin);
    dap_chain_plugin_list_module_t *l_plugins = dap_chain_plugins_list_get();
    dap_chain_plugin_list_module_t *l_plugin = NULL;
    LL_SEARCH(l_plugins, l_plugin, a_name_plugin, dap_chain_plugins_list_name_cmp);
    if (l_plugin == NULL)
        return -4;
    PyObject *l_func_deinit = PyObject_GetAttrString(l_plugin->obj_module, "deinit");
    if (l_func_deinit != NULL || PyCallable_Check(l_func_deinit)){
        PyEval_CallObject(l_func_deinit, NULL);
    } else {
        log_it(L_WARNING, "The 'deinit' object of \"%s\" plugin isn't a callable", l_plugin->name);
    }
    DAP_FREE(l_plugin->name);
    Py_XDECREF(l_plugin->obj_module);
    LL_DELETE(l_plugins, l_plugin);
    dap_chain_plugins_manifest_list_delete_name(a_name_plugin);
    //Loading plugin
    char *l_path_plugin = dap_strjoin(NULL, s_plugins_root_path, a_name_plugin, NULL);
    char *l_name_file_manifest = dap_strjoin("",s_plugins_root_path, a_name_plugin, "/manifest.json", NULL);
    if (!dap_chain_plugins_manifest_list_add_from_file(l_name_file_manifest)){
        log_it(L_ERROR, "Registration of  \"%s\" manifest is failed", l_path_plugin);
        return -3;
    }
    DAP_FREE(l_name_file_manifest);
    dap_chain_plugins_list_manifest_t *l_manifest =  dap_chain_plugins_manifest_list_get_name(a_name_plugin);
    if (l_manifest->dependencies != NULL){
        if (!dap_chain_plugins_list_check_load_plugins(l_manifest->dependencies)){
            log_it(L_NOTICE, "\"%s\" plugin has unresolved dependencies, restart all plugins", l_manifest->name);
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

