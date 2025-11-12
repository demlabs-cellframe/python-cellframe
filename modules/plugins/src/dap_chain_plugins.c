#include "dap_config.h"
#include "dap_plugin.h"
#include "dap_common.h"
#include "dap_file_utils.h"
#include "python-cellframe.h"
#include "dap_chain_plugins.h"

#undef LOG_TAG
#define LOG_TAG "dap_chain_plugins"

static bool s_debug_more = false;

PyObject *s_sys_path = NULL;
const char *s_plugins_root_path = NULL;

PyThreadState *s_thread_state;

typedef struct _dap_chain_plugins_module{
    PyObject *module;
    char *name;
    char *path;
    struct _dap_chain_plugins_module *next;
}_dap_chain_plugins_module_t;

_dap_chain_plugins_module_t *_s_modules = NULL;

static int s_dap_chain_plugins_load(dap_plugin_manifest_t * a_manifest, void ** a_pvt_data, char ** a_error_str );
static int s_dap_chain_plugins_unload(dap_plugin_manifest_t * a_manifest, void * a_pvt_data, char ** a_error_str );
static void s_plugins_load_plugin_initialization(void* a_module);
static void s_plugins_load_plugin_uninitialization(void* a_module);
static void s_modules_purge_cache(const char *a_module_name, const char *a_plugin_path);
static bool s_module_originates_from(PyObject *a_module_obj, const char *a_plugin_path);
static char *s_prepare_plugin_path(const char *a_dir_path);
static char *s_path_canonicalize(const char *a_path);
static char *s_path_ensure_trailing_sep(char *a_path);
static bool s_path_is_within(const char *a_candidate, const char *a_parent);
static void s_normalize_slashes(char *a_path);

const char *pycfhelpers_path = "/opt/cellframe-node/python/lib/"PYTHON_VERSION"/site-packages/pycfhelpers";
const char *pycftools_path = "/opt/cellframe-node/python/lib/"PYTHON_VERSION"/site-packages/pycftools";

wchar_t *s_get_full_path(const char *a_prefix, const char *a_path)
{
    char *l_full_char_path = dap_strjoin("", a_prefix, "/", a_path, NULL);
    size_t l_len = strlen(l_full_char_path) + 1;
    wchar_t *l_ret = DAP_NEW_Z_SIZE(wchar_t, l_len * sizeof(wchar_t));
    mbstowcs(l_ret, l_full_char_path, l_len);
    DAP_DELETE(l_full_char_path);
    return l_ret;
}

static void s_normalize_slashes(char *a_path)
{
    if (!a_path)
        return;
#ifdef DAP_OS_WINDOWS
    for (char *p = a_path; *p; ++p)
        if (*p == '\\')
            *p = '/';
#else
    (void)a_path;
#endif
}

static char *s_path_canonicalize(const char *a_path)
{
    if (!a_path)
        return NULL;
    char *l_real = realpath(a_path, NULL);
    if (!l_real) {
        if (dap_path_is_absolute(a_path)) {
            l_real = dap_strdup(a_path);
        } else {
            char *l_cwd = dap_get_current_dir();
            if (!l_cwd)
                return NULL;
            char *l_full = dap_strjoin(NULL, l_cwd, DAP_DIR_SEPARATOR_S, a_path, NULL);
            DAP_DELETE(l_cwd);
            if (!l_full)
                return NULL;
            l_real = realpath(l_full, NULL);
            if (!l_real)
                l_real = l_full;
            else
                DAP_DELETE(l_full);
        }
    }
    if (!l_real)
        return NULL;
    s_normalize_slashes(l_real);
    return l_real;
}

static char *s_path_ensure_trailing_sep(char *a_path)
{
    if (!a_path)
        return NULL;
    size_t l_len = strlen(a_path);
    if (l_len && a_path[l_len - 1] == '/')
        return a_path;
    char *l_with_sep = dap_strjoin(NULL, a_path, "/", NULL);
    DAP_DELETE(a_path);
    return l_with_sep;
}

static char *s_prepare_plugin_path(const char *a_dir_path)
{
    char *l_path = s_path_canonicalize(a_dir_path);
    if (!l_path)
        return NULL;
    return s_path_ensure_trailing_sep(l_path);
}

static bool s_path_is_within(const char *a_candidate, const char *a_parent)
{
    if (!a_candidate || !a_parent)
        return false;
    size_t l_prefix_len = strlen(a_parent);
    if (!l_prefix_len)
        return false;
    return strncmp(a_candidate, a_parent, l_prefix_len) == 0;
}

static bool s_module_originates_from(PyObject *a_module_obj, const char *a_plugin_path)
{
    if (!a_module_obj || !a_plugin_path)
        return false;
    bool l_result = false;
    PyObject *l_file_attr = PyObject_GetAttrString(a_module_obj, "__file__");
    if (l_file_attr) {
        if (PyUnicode_Check(l_file_attr)) {
            const char *l_file_str = PyUnicode_AsUTF8(l_file_attr);
            if (l_file_str) {
                char *l_canon = s_path_canonicalize(l_file_str);
                if (l_canon) {
                    l_result = s_path_is_within(l_canon, a_plugin_path);
                    DAP_DELETE(l_canon);
                }
            }
        }
        Py_DECREF(l_file_attr);
    } else {
        PyErr_Clear();
    }
    if (l_result)
        return true;

    PyObject *l_path_attr = PyObject_GetAttrString(a_module_obj, "__path__");
    if (!l_path_attr) {
        PyErr_Clear();
        return false;
    }
    Py_ssize_t l_size = PySequence_Size(l_path_attr);
    if (l_size < 0) {
        PyErr_Clear();
        Py_DECREF(l_path_attr);
        return false;
    }
    for (Py_ssize_t i = 0; i < l_size && !l_result; ++i) {
        PyObject *l_item = PySequence_GetItem(l_path_attr, i);
        if (!l_item) {
            PyErr_Clear();
            continue;
        }
        if (PyUnicode_Check(l_item)) {
            const char *l_item_str = PyUnicode_AsUTF8(l_item);
            if (l_item_str) {
                char *l_canon = s_path_canonicalize(l_item_str);
                if (l_canon) {
                    l_result = s_path_is_within(l_canon, a_plugin_path);
                    DAP_DELETE(l_canon);
                }
            }
        }
        Py_DECREF(l_item);
    }
    Py_DECREF(l_path_attr);
    return l_result;
}

static void s_modules_purge_cache(const char *a_module_name, const char *a_plugin_path)
{
    PyObject *l_sys_modules = PyImport_GetModuleDict();
    if (!l_sys_modules || !PyDict_Check(l_sys_modules))
        return;

    PyObject *l_keys = PyDict_Keys(l_sys_modules);
    if (!l_keys)
        return;

    size_t l_name_len = a_module_name ? strlen(a_module_name) : 0;
    Py_ssize_t l_count = PyList_GET_SIZE(l_keys);
    for (Py_ssize_t i = 0; i < l_count; ++i) {
        PyObject *l_key = PyList_GET_ITEM(l_keys, i); // borrowed
        if (!PyUnicode_Check(l_key))
            continue;
        const char *l_key_str = PyUnicode_AsUTF8(l_key);
        if (!l_key_str) {
            PyErr_Clear();
            continue;
        }
        bool l_match = false;
        if (a_module_name && l_name_len) {
            bool l_is_self = strcmp(l_key_str, a_module_name) == 0;
            bool l_is_submodule = strncmp(l_key_str, a_module_name, l_name_len) == 0 && l_key_str[l_name_len] == '.';
            l_match = l_is_self || l_is_submodule;
        }
        if (!l_match && a_plugin_path) {
            PyObject *l_module_obj = PyDict_GetItem(l_sys_modules, l_key); // borrowed
            if (l_module_obj)
                l_match = s_module_originates_from(l_module_obj, a_plugin_path);
            else
                PyErr_Clear();
        }
        if (l_match) {
            if (PyDict_DelItem(l_sys_modules, l_key) < 0)
                PyErr_Clear();
        }
    }
    Py_DECREF(l_keys);
}

int dap_chain_plugins_init(dap_config_t *a_config)
{
    if (!dap_config_get_item_bool_default(a_config, "plugins", "py_load", false))
        return -1;

    dap_plugin_type_callbacks_t l_callbacks={};
    l_callbacks.load = s_dap_chain_plugins_load;
    l_callbacks.unload = s_dap_chain_plugins_unload;
    dap_plugin_type_create("python",&l_callbacks);

    s_debug_more = dap_config_get_item_bool_default(a_config, "plugins", "debug_more", s_debug_more);

    const char *l_default_path_plugins = dap_strjoin(NULL, g_sys_dir_path, "/var/plugins/", NULL);
    const char *l_plugins_root_path = dap_config_get_item_str_path_default(a_config, "plugins", "py_path",
                                                        l_default_path_plugins);
    s_plugins_root_path = dap_strjoin("", l_plugins_root_path, "/", NULL);
    DAP_DELETE(l_default_path_plugins);

    log_it(L_INFO, "Start initialization of python (%s) plugins. Path plugins: %s", PYTHON_VERSION, s_plugins_root_path);
    if (!dap_dir_test(s_plugins_root_path)){
        log_it(L_ERROR, "Can't find \"%s\" directory fff", s_plugins_root_path);
        return -1;
    }
    PyImport_AppendInittab("DAP", PyInit_libDAP);
    PyImport_AppendInittab("CellFrame", PyInit_libCellFrame);

    PyStatus l_status;
    PyPreConfig l_preconfig;
    PyConfig l_config;
#ifdef DAP_BUILD_WITH_PYTHON_ENV
    PyPreConfig_InitIsolatedConfig(&l_preconfig);
    l_preconfig.utf8_mode = 1;

    #if DAP_OS_DARWIN
    char *pypath  = "/Applications/CellframeNode.app/Contents/Frameworks/";
    #else
    char *pypath  = g_sys_dir_path;
    #endif

    PyConfig_InitIsolatedConfig(&l_config);
    l_config.module_search_paths_set = 1;
    wchar_t *l_path = s_get_full_path(pypath, "python/lib/" PYTHON_VERSION);
    l_status = PyWideStringList_Append(&l_config.module_search_paths, l_path);
    DAP_DELETE(l_path);
    if (PyStatus_Exception(l_status))
        goto excpt;
    l_path = s_get_full_path(pypath, "python/lib/" PYTHON_VERSION "/lib-dynload");
    l_status = PyWideStringList_Append(&l_config.module_search_paths, l_path);
    DAP_DELETE(l_path);
    if (PyStatus_Exception(l_status))
        goto excpt;
    l_path = s_get_full_path(pypath, "python/lib/" PYTHON_VERSION "/site-packages");
    l_status = PyWideStringList_Append(&l_config.module_search_paths, l_path);
    DAP_DELETE(l_path);
    if (PyStatus_Exception(l_status))
        goto excpt;

    l_path = s_get_full_path(pypath, "python");
    l_status = PyConfig_SetString(&l_config, &l_config.base_exec_prefix, l_path);
    if (PyStatus_Exception(l_status)) {
        DAP_DELETE(l_path);
        goto excpt;
    }
    l_status = PyConfig_SetString(&l_config, &l_config.base_prefix, l_path);
    if (PyStatus_Exception(l_status)) {
        DAP_DELETE(l_path);
        goto excpt;
    }
    l_status = PyConfig_SetString(&l_config, &l_config.exec_prefix, l_path);
    if (PyStatus_Exception(l_status)) {
        DAP_DELETE(l_path);
        goto excpt;
    }
    l_status = PyConfig_SetString(&l_config, &l_config.prefix, l_path);
    DAP_DELETE(l_path);
    if (PyStatus_Exception(l_status))
        goto excpt;

    l_path = s_get_full_path(pypath, "python/bin/" PYTHON_VERSION);
    l_status = PyConfig_SetString(&l_config, &l_config.executable, l_path);
    DAP_DELETE(l_path);
    if (PyStatus_Exception(l_status))
        goto excpt;
#else
    PyPreConfig_InitPythonConfig(&l_preconfig);
    PyConfig_InitPythonConfig(&l_config);
#endif

    l_status = Py_PreInitialize(&l_preconfig);
    if (PyStatus_Exception(l_status))
        Py_ExitStatusException(l_status);

#ifdef DAP_OS_WINDOWS
    wchar_t l_progam_name[] = L"python";
#else
    wchar_t l_program_name[] = L"python3";
#endif
    l_status = PyConfig_SetString(&l_config, &l_config.program_name, l_program_name);
    if (PyStatus_Exception(l_status))
        goto excpt;

    l_status = Py_InitializeFromConfig(&l_config);

excpt:
    PyConfig_Clear(&l_config);
    if (PyStatus_Exception(l_status))
        Py_ExitStatusException(l_status);


    Py_Initialize();
    char py_path[1024];
    wchar_t *py_wpath = Py_GetPythonHome();
    if (!py_wpath)
        py_wpath = Py_GetPrefix();
    if (!py_wpath)
        py_wpath = Py_GetExecPrefix();
    wcstombs(py_path, py_wpath, 1024);
    log_it(L_NOTICE, "Python interpreter initialized from %s with version %s",
                                py_path, Py_GetVersion());

    PyObject *l_sys_module = PyImport_ImportModule("sys");
    s_sys_path = PyObject_GetAttrString(l_sys_module, "path");

    if (s_debug_more) {
        PyRun_SimpleString("import tracemalloc\n"
                           "tracemalloc.start(10)\n");
        PyRun_SimpleString("import sys\n"
                           "for attr in dir(sys):\n"
                               "\tprint(\"sys.%s = %r\" % (attr, getattr(sys, attr)))\n");
    }

    return 0;
}

void dap_chain_plugins_save_thread(dap_config_t *a_config)
{
    if (dap_config_get_item_bool_default(a_config, "plugins", "py_load", false))
        s_thread_state = PyEval_SaveThread();
}

static int s_dap_chain_plugins_load(dap_plugin_manifest_t * a_manifest, void ** a_pvt_data, char ** a_error_str ){
    log_it(L_NOTICE, "Loading plugins");
    dap_plugin_manifest_t *l_manifest = a_manifest;
    void *l_pvt_data = NULL;
    if (l_manifest == NULL){
        return -100;
    }

    if (*l_manifest->name == 0){
        log_it(L_ERROR, "Can't load a plugin, file not found");
        return -101;
    }
    log_it(L_NOTICE, "Check dependencies for plugin: %s", l_manifest->name);

    PyGILState_STATE l_gil_state;
    l_gil_state = PyGILState_Ensure();
    char * module_path = dap_strjoin("", s_plugins_root_path, l_manifest->name, "/", NULL);
    l_pvt_data = dap_chain_plugins_load_plugin_importing(module_path, l_manifest->name);
    DAP_DEL_Z(module_path);

    if (!l_pvt_data){
        PyGILState_Release(l_gil_state);
        return -102;
    }

    *a_pvt_data = l_pvt_data;
    s_plugins_load_plugin_initialization(l_pvt_data);
    PyGILState_Release(l_gil_state);
    return 0;

}

static int s_dap_chain_plugins_unload(dap_plugin_manifest_t * a_manifest, void * a_pvt_data, char ** a_error_str )
{
    log_it(L_NOTICE, "Unloading plugins");
    dap_plugin_manifest_t *l_manifest = a_manifest;
    void *l_pvt_data = a_pvt_data;
    if (l_manifest == NULL)
        return -100;
    if (*l_manifest->name == 0){
        log_it(L_ERROR, "Can't load a plugin, file not found");
        return -101;
    }
    PyGILState_STATE l_gil_state;
    l_gil_state = PyGILState_Ensure();
    s_plugins_load_plugin_uninitialization(l_pvt_data);
    PyGILState_Release(l_gil_state);
    return 0;
}


void* dap_chain_plugins_load_plugin_importing(const char *a_dir_path, const char *a_name) {
    log_it(L_NOTICE, "Import \"%s\" module from \"%s\" directory", a_name, a_dir_path);

    char *l_plugin_path = s_prepare_plugin_path(a_dir_path);
    if (!l_plugin_path)
        l_plugin_path = s_path_ensure_trailing_sep(dap_strdup(a_dir_path));

    PyObject *l_obj_dir_path = PyUnicode_FromString(a_dir_path);
    PyList_Append(s_sys_path, l_obj_dir_path);
    Py_XDECREF(l_obj_dir_path);

    if (PyRun_SimpleString("import importlib; importlib.invalidate_caches()") != 0) {
        python_error_in_log_it(LOG_TAG);
        PyErr_Clear();
    }

    PyObject *l_module_name = PyUnicode_FromString(a_name);

    // Trying to get an already loaded module
    PyObject *l_ext_module = PyImport_GetModule(l_module_name);
    PyObject *l_module = NULL;

    if (l_ext_module) {
        // The module has already been imported, drop it and its submodules from sys.modules
        log_it(L_NOTICE, "Module \"%s\" is already imported, dropping cached entries...", a_name);
        s_modules_purge_cache(a_name, l_plugin_path);
        Py_DECREF(l_ext_module);

        if (PyRun_SimpleString("import importlib; importlib.invalidate_caches()") != 0) {
            python_error_in_log_it(LOG_TAG);
            PyErr_Clear();
        }
    }

    // Import the module
    l_module = PyImport_ImportModule(a_name);
    if (!l_module) {
        python_error_in_log_it(LOG_TAG);
        log_it(L_ERROR, "Failed to import module \"%s\"", a_name);
    }

    Py_DECREF(l_module_name);

    if (!l_module) {
        DAP_DELETE(l_plugin_path);
        return NULL;
    }

    _dap_chain_plugins_module_t *module = DAP_NEW(_dap_chain_plugins_module_t);
    if (!module) {
        python_error_in_log_it(LOG_TAG);
        PyErr_Clear();
        DAP_DELETE(l_plugin_path);
        return NULL;
    }
    module->module = l_module;
    module->name = dap_strdup(a_name);
    module->path = l_plugin_path;
    return module;
}

static void s_plugins_load_plugin_initialization(void* a_module){
    if (!a_module)
        return;
    _dap_chain_plugins_module_t  *l_container = (_dap_chain_plugins_module_t  *)a_module;
    PyObject *l_func_init = PyObject_GetAttrString(l_container->module, "init");
    PyObject *l_func_deinit = PyObject_GetAttrString(l_container->module, "deinit");
    PyObject *l_res_int = NULL;
    PyErr_Clear();
    if (PyCallable_Check(l_func_init)) {
        PyObject *l_void_tuple = PyTuple_New(0);
        PyGILState_STATE l_gil_state;
        l_gil_state = PyGILState_Ensure();
        l_res_int = PyObject_CallObject(l_func_init, l_void_tuple);
        PyGILState_Release(l_gil_state);
        if (l_res_int && PyLong_Check(l_res_int)) {
            if (_PyLong_AsInt(l_res_int) != 0) {
                python_error_in_log_it(LOG_TAG);
                log_it(L_ERROR, "Can't initialize \"%s\" plugin. Code error: %i", l_container->name,
                       _PyLong_AsInt(l_res_int));
            }
        } else {
            python_error_in_log_it(LOG_TAG);
            log_it(L_ERROR, "The 'init' function of \"%s\" plugin didn't return an integer value", l_container->name);
        }
        Py_XDECREF(l_res_int);
        Py_XDECREF(l_void_tuple);
    } else {
        log_it(L_ERROR, "Can't find 'init' function of \"%s\" plugin", l_container->name);
    }
    if (l_func_deinit == NULL || !PyCallable_Check(l_func_deinit)){
        log_it(L_WARNING, "Can't find 'deinit' function of \"%s\" plugin", l_container->name);
    }
}

static void s_plugins_load_plugin_uninitialization(void* a_module){
    if (!a_module)
        return;
    _dap_chain_plugins_module_t  *l_container = (_dap_chain_plugins_module_t  *)a_module;
    PyGILState_STATE l_gil_state = PyGILState_Ensure();

    PyErr_Clear();
    PyObject *l_func_deinit = PyObject_GetAttrString(l_container->module, "deinit");
    if (l_func_deinit && PyCallable_Check(l_func_deinit)) {
        PyObject *l_res = PyObject_CallFunctionObjArgs(l_func_deinit, NULL);
        if (!l_res) {
            python_error_in_log_it(LOG_TAG);
            PyErr_Clear();
        } else {
            Py_DECREF(l_res);
        }
    } else {
        log_it(L_WARNING, "Can't find 'deinit' function of \"%s\" plugin", l_container->name);
    }
    Py_XDECREF(l_func_deinit);

    s_modules_purge_cache(l_container->name, l_container->path);

    if (PyRun_SimpleString("import importlib; importlib.invalidate_caches()") != 0) {
        python_error_in_log_it(LOG_TAG);
        PyErr_Clear();
    }

    Py_XDECREF(l_container->module);
    l_container->module = NULL;

    PyGILState_Release(l_gil_state);

    DAP_DELETE(l_container->name);
    DAP_DELETE(l_container->path);
    DAP_DELETE(l_container);
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
        PyObject *l_void_tuple = PyTuple_New(0);
        l_res_int = PyObject_CallObject(l_func_init, l_void_tuple);
        if (l_res_int && PyLong_Check(l_res_int)){
            if (_PyLong_AsInt(l_res_int) != 0){
                python_error_in_log_it(LOG_TAG);
                log_it(L_ERROR, "Can't initialize \"%s\" plugin. Code error: %i", a_name, _PyLong_AsInt(l_res_int));
            }
        } else {
            python_error_in_log_it(LOG_TAG);
            log_it(L_ERROR, "The 'init' function of \"%s\" plugin didn't return an integer value", a_name);
        }
        Py_XDECREF(l_res_int);
        Py_XDECREF(l_void_tuple);
    }else {
        log_it(L_ERROR, "Can't find 'init' function of \"%s\" plugin", a_name);
    }
    if (l_func_deinit == NULL || !PyCallable_Check(l_func_deinit)){
        log_it(L_WARNING, "Can't find 'deinit' function of \"%s\" plugin", a_name);
    }
}
