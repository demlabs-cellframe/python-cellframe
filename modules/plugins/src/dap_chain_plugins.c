/*
 * Authors:
 * Copyright (c) DeM Labs Inc.
 * License: GNU General Public License
 */

#include "dap_config.h"
#include "dap_plugin.h"
#include "dap_plugin_manifest.h"
#include "dap_common.h"
#include "dap_file_utils.h"
#include "python-cellframe.h"
#include "dap_chain_plugins.h"
#include <dlfcn.h>
#include <pthread.h>
#include <patchlevel.h>
#include "uthash.h"

#undef LOG_TAG
#define LOG_TAG "dap_chain_plugins"

// Fallback definition for linter - CMake will override this
#ifndef PYTHON_VERSION
#define PYTHON_VERSION "python3.10"
#endif

static bool s_debug_more = false;

PyObject *s_sys_path = NULL;
const char *s_plugins_root_path = NULL;

PyThreadState *s_thread_state = NULL;
static bool s_python_initialized = false;

// Two-phase initialization state
typedef enum {
    PHASE_NONE,
    PHASE_EARLY,
    PHASE_LATE
} dap_chain_plugins_phase_t;

static dap_chain_plugins_phase_t s_current_phase = PHASE_NONE;
static bool s_early_init_done = false;

typedef struct _dap_chain_plugins_module{
    PyObject *module;
    char *name;
    bool preload_called;
    bool init_called;
    struct _dap_chain_plugins_module *next;
}_dap_chain_plugins_module_t;

_dap_chain_plugins_module_t *_s_modules = NULL;

static int s_dap_chain_plugins_load(dap_plugin_manifest_t * a_manifest, void ** a_pvt_data, char ** a_error_str );
static int s_dap_chain_plugins_unload(dap_plugin_manifest_t * a_manifest, void * a_pvt_data, char ** a_error_str );
static void s_plugins_load_plugin_initialization(void* a_module);
static void s_plugins_load_plugin_preload(void* a_module);
static void s_plugins_load_plugin_uninitialization(void* a_module);
static int s_python_interpreter_init(dap_config_t *a_config);
static void s_load_all_python_plugins();

const char *pycfhelpers_path = "/opt/cellframe-node/python/lib/"PYTHON_VERSION"/site-packages/pycfhelpers";
const char *pycftools_path = "/opt/cellframe-node/python/lib/"PYTHON_VERSION"/site-packages/pycftools";
const char *plugins_path = "/opt/cellframe-node/var/lib/plugins/";
char *strings[]={"DAP", "CellFrame", NULL};

wchar_t *s_get_full_path(const char *a_prefix, const char *a_path)
{
    char *l_full_char_path = dap_strjoin("", a_prefix, "/", a_path, NULL);
    size_t l_len = strlen(l_full_char_path) + 1;
    wchar_t *l_ret = DAP_NEW_Z_SIZE(wchar_t, l_len * sizeof(wchar_t));
    mbstowcs(l_ret, l_full_char_path, l_len);
    DAP_DELETE(l_full_char_path);
    return l_ret;
}

/**
 * @brief Early phase Python plugin initialization
 * @details Initializes Python interpreter and loads plugins, calling preload() method
 * @param a_config Configuration object
 * @return 0 on success, negative on error
 */
int dap_chain_plugins_early_init(dap_config_t *a_config)
{
    log_it(L_NOTICE, "=== PYTHON PLUGINS EARLY INIT START ===");
    
    if (!dap_config_get_item_bool_default(a_config, "plugins", "py_load", false)) {
        log_it(L_NOTICE, "Python plugins disabled in config - skipping early init");
        return 0; // Not an error, just disabled
    }

    if (s_early_init_done) {
        log_it(L_WARNING, "Python plugins early init already done");
        return 0;
    }

    s_current_phase = PHASE_EARLY;
    
    // Initialize Python interpreter
    int l_ret = s_python_interpreter_init(a_config);
    if (l_ret != 0) {
        log_it(L_ERROR, "Failed to initialize Python interpreter in early phase");
        return l_ret;
    }

    // Load all Python plugins and call preload() method
    s_load_all_python_plugins();

    s_early_init_done = true;
    log_it(L_NOTICE, "=== PYTHON PLUGINS EARLY INIT COMPLETED ===");
    return 0;
}

/**
 * @brief Late phase Python plugin initialization  
 * @details Calls init() method on already loaded plugins and handles app context
 * @param a_config Configuration object
 * @return 0 on success, negative on error
 */
int dap_chain_plugins_late_init(dap_config_t *a_config)
{
    log_it(L_NOTICE, "=== PYTHON PLUGINS LATE INIT START ===");
    
    if (!dap_config_get_item_bool_default(a_config, "plugins", "py_load", false)) {
        log_it(L_NOTICE, "Python plugins disabled in config - skipping late init");
        return 0;
    }

    if (!s_early_init_done) {
        log_it(L_WARNING, "Python plugins early init not done, calling late init will do full init");
        return dap_chain_plugins_init(a_config);
    }

    s_current_phase = PHASE_LATE;
    
    // Call init() method on all loaded modules
    _dap_chain_plugins_module_t *l_module = _s_modules;
    while (l_module) {
        if (l_module->preload_called && !l_module->init_called) {
            log_it(L_NOTICE, "Calling init() for plugin: %s", l_module->name);
            s_plugins_load_plugin_initialization(l_module);
            l_module->init_called = true;
        }
        l_module = l_module->next;
    }

    log_it(L_NOTICE, "=== PYTHON PLUGINS LATE INIT COMPLETED ===");
    return 0;
}

/**
 * @brief Extract Python interpreter initialization logic
 * @param a_config Configuration object
 * @return 0 on success, negative on error
 */
static int s_python_interpreter_init(dap_config_t *a_config)
{
    if (s_python_initialized) {
        log_it(L_DEBUG, "Python interpreter already initialized");
        return 0;
    }

    log_it(L_NOTICE, "Python plugins enabled, proceeding with initialization...");

    dap_plugin_type_callbacks_t l_callbacks={};
    l_callbacks.load = s_dap_chain_plugins_load;
    l_callbacks.unload = s_dap_chain_plugins_unload;
    dap_plugin_type_create("python",&l_callbacks);

    s_debug_more = dap_config_get_item_bool_default(a_config, "plugins", "debug_more", s_debug_more);
    log_it(L_NOTICE, "Debug mode: %s", s_debug_more ? "enabled" : "disabled");

    const char *l_default_path_plugins = dap_strjoin(NULL, g_sys_dir_path, "/var/plugins/", NULL);
    const char *l_plugins_root_path = dap_config_get_item_str_path_default(a_config, "plugins", "py_path",
                                                        l_default_path_plugins);
    s_plugins_root_path = dap_strjoin("", l_plugins_root_path, "/", NULL);
    DAP_DELETE(l_default_path_plugins);

    log_it(L_NOTICE, "Plugins root path: %s", s_plugins_root_path);
    log_it(L_INFO, "Start initialization of python (%s) plugins. Path plugins: %s", PYTHON_VERSION, s_plugins_root_path);
    
    if (!dap_dir_test(s_plugins_root_path)){
        log_it(L_ERROR, "Can't find \"%s\" directory", s_plugins_root_path);
        return -1;
    }
    
    log_it(L_NOTICE, "Adding Python modules to import table...");
    // Add module initialization before Python setup
    PyImport_AppendInittab("DAP", PyInit_libDAP);
    log_it(L_NOTICE, "Added DAP module to import table");
    PyImport_AppendInittab("CellFrame", PyInit_libCellFrame);
    log_it(L_NOTICE, "Added CellFrame module to import table");

    log_it(L_NOTICE, "Starting Python interpreter initialization...");
    PyStatus l_status;
    PyPreConfig l_preconfig;
    PyConfig l_config;
    
#ifdef DAP_BUILD_WITH_PYTHON_ENV
    PyPreConfig_InitIsolatedConfig(&l_preconfig);
    l_preconfig.utf8_mode = 1;

    #if DAP_OS_DARWIN
    char *pypath  = "/Applications/CellframeNode.app/Contents/Frameworks/";
    char *py_framework_path = "Python.framework/Versions/Current/";
    #else
    char *pypath  = g_sys_dir_path;
    char *py_framework_path = "python/";
    #endif

    PyConfig_InitIsolatedConfig(&l_config);
    l_config.module_search_paths_set = 1;
    
    char *py_lib_path_template = dap_strjoin("", py_framework_path, "lib/" PYTHON_VERSION, NULL);
    wchar_t *l_path = s_get_full_path(pypath, py_lib_path_template);
    l_status = PyWideStringList_Append(&l_config.module_search_paths, l_path);
    DAP_DELETE(l_path);
    DAP_FREE(py_lib_path_template);
    if (PyStatus_Exception(l_status))
        goto excpt;
        
    py_lib_path_template = dap_strjoin("", py_framework_path, "lib/" PYTHON_VERSION "/lib-dynload", NULL);
    l_path = s_get_full_path(pypath, py_lib_path_template);
    l_status = PyWideStringList_Append(&l_config.module_search_paths, l_path);
    DAP_DELETE(l_path);
    DAP_FREE(py_lib_path_template);
    if (PyStatus_Exception(l_status))
        goto excpt;
        
    py_lib_path_template = dap_strjoin("", py_framework_path, "lib/" PYTHON_VERSION "/site-packages", NULL);
    l_path = s_get_full_path(pypath, py_lib_path_template);
    l_status = PyWideStringList_Append(&l_config.module_search_paths, l_path);
    DAP_DELETE(l_path);
    DAP_FREE(py_lib_path_template);
    if (PyStatus_Exception(l_status))
        goto excpt;

    l_path = s_get_full_path(pypath, py_framework_path);
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

    py_lib_path_template = dap_strjoin("", py_framework_path, "bin/" PYTHON_VERSION, NULL);
    l_path = s_get_full_path(pypath, py_lib_path_template);
    l_status = PyConfig_SetString(&l_config, &l_config.executable, l_path);
    DAP_DELETE(l_path);
    DAP_FREE(py_lib_path_template);
    if (PyStatus_Exception(l_status))
        goto excpt;
        
    l_status = Py_PreInitialize(&l_preconfig);
    if (PyStatus_Exception(l_status))
        Py_ExitStatusException(l_status);

#ifdef DAP_OS_WINDOWS
    wchar_t l_program_name[] = L"python";
#else
    wchar_t l_program_name[] = L"python3";
#endif
    l_status = PyConfig_SetString(&l_config, &l_config.program_name, l_program_name);
    if (PyStatus_Exception(l_status))
        goto excpt;

    l_status = Py_InitializeFromConfig(&l_config);
    if (PyStatus_Exception(l_status))
        goto excpt;
        
#else
    PyPreConfig_InitPythonConfig(&l_preconfig);
    PyConfig_InitPythonConfig(&l_config);
    
    l_status = Py_PreInitialize(&l_preconfig);
    if (PyStatus_Exception(l_status))
        Py_ExitStatusException(l_status);

#ifdef DAP_OS_WINDOWS
    wchar_t l_program_name[] = L"python";
#else
    wchar_t l_program_name[] = L"python3";
#endif
    l_status = PyConfig_SetString(&l_config, &l_config.program_name, l_program_name);
    if (PyStatus_Exception(l_status))
        goto excpt;

    l_status = Py_InitializeFromConfig(&l_config);
    if (PyStatus_Exception(l_status))
        goto excpt;
#endif

    s_python_initialized = true;
    
    // Initialize threading support - CRITICAL for multi-threaded environments
    if (!PyEval_ThreadsInitialized()) {
        log_it(L_DEBUG, "[GIL-DEBUG] Initializing Python thread support");  
        PyEval_InitThreads();
        log_it(L_DEBUG, "[GIL-DEBUG] Python thread support initialized");
    } else {
        log_it(L_DEBUG, "[GIL-DEBUG] Python thread support already initialized");
    }
    
    log_it(L_DEBUG, "[GIL-DEBUG] Init complete thread=%lu py_thread=%lu gil=%s", 
           (unsigned long)pthread_self(), PyThread_get_thread_ident(), 
           PyGILState_Check() ? "HELD" : "NOT_HELD");

    char py_path[1024];
    wchar_t *py_wpath = Py_GetPythonHome();
    if (!py_wpath)
        py_wpath = Py_GetPrefix();
    if (!py_wpath)
        py_wpath = Py_GetExecPrefix();
    if (py_wpath) {
        wcstombs(py_path, py_wpath, 1024);
        log_it(L_NOTICE, "Python interpreter initialized from %s with version %s",
                                    py_path, Py_GetVersion());
    } else {
        log_it(L_NOTICE, "Python interpreter initialized with version %s", Py_GetVersion());
    }

    PyObject *l_sys_module = PyImport_ImportModule("sys");
    if (!l_sys_module) {
        log_it(L_ERROR, "Failed to import sys module");
        python_error_in_log_it(LOG_TAG);
        PyConfig_Clear(&l_config);
        return -1;
    }
    
    s_sys_path = PyObject_GetAttrString(l_sys_module, "path");
    Py_DECREF(l_sys_module);
    
    if (!s_sys_path) {
        log_it(L_ERROR, "Failed to get sys.path");
        python_error_in_log_it(LOG_TAG);
        PyConfig_Clear(&l_config);
        return -1;
    }

    if (s_debug_more) {
        PyRun_SimpleString("import tracemalloc\n"
                           "tracemalloc.start(10)\n");
        PyRun_SimpleString("import sys\n"
                           "for attr in dir(sys):\n"
                               "\tprint(\"sys.%s = %r\" % (attr, getattr(sys, attr)))\n");
    }

    log_it(L_DEBUG, "[GIL-DEBUG] Python initialization complete, GIL held by main thread");

    // CRITICAL FIX: Release GIL after initialization to allow other threads to acquire it
    PyThreadState *main_thread_state = PyEval_SaveThread();
    log_it(L_DEBUG, "[GIL-DEBUG] Main thread RELEASED GIL thread=%lu state=%p", 
           (unsigned long)pthread_self(), main_thread_state);
    
    // Store main thread state for later use if needed
    // static PyThreadState *s_main_thread_state = NULL;
    // s_main_thread_state = main_thread_state;

    PyConfig_Clear(&l_config);
    return 0;

excpt:
    PyConfig_Clear(&l_config);
    if (PyStatus_Exception(l_status)) {
        log_it(L_ERROR, "Python initialization failed: %s", l_status.err_msg ? l_status.err_msg : "Unknown error");
        Py_ExitStatusException(l_status);
    }
    return -1;
}

void dap_chain_plugins_save_thread(dap_config_t *a_config)
{
    // Thread state management simplified - main thread keeps GIL throughout execution
    if (s_python_initialized && dap_config_get_item_bool_default(a_config, "plugins", "py_load", false)) {
        log_it(L_DEBUG, "[GIL-DEBUG] save_thread thread=%lu py_thread=%lu gil=%s", 
               (unsigned long)pthread_self(), PyThread_get_thread_ident(), 
               PyGILState_Check() ? "HELD" : "NOT_HELD");
    }
}

static int s_dap_chain_plugins_load(dap_plugin_manifest_t * a_manifest, void ** a_pvt_data, char ** a_error_str ){
    log_it(L_NOTICE, "=== LOADING PLUGIN: %s ===", a_manifest->name ? a_manifest->name : "UNKNOWN");
    log_it(L_NOTICE, "Plugin path: %s", a_manifest->path ? a_manifest->path : "UNKNOWN");
    
    dap_plugin_manifest_t *l_manifest = a_manifest;
    void *l_pvt_data = NULL;
    if (l_manifest == NULL) {
        log_it(L_ERROR, "Plugin manifest is NULL");
        return -100;
    }
    if (*l_manifest->name == 0){
        log_it(L_ERROR, "Can't load a plugin, file not found");
        return -101;
    }
    log_it(L_NOTICE, "Check dependencies for plugin: %s", l_manifest->name);

    log_it(L_NOTICE, "About to call dap_chain_plugins_load_plugin_importing...");
    // Check Python state before the call
    log_it(L_CRITICAL, "EMERGENCY: Python initialized: %s, sys_path: %p, GIL check: %s", 
           s_python_initialized ? "YES" : "NO", s_sys_path, PyGILState_Check() ? "HELD" : "NOT_HELD");
           
    // CRITICAL FIX: Threading issue - ensure we're in the right thread context
    log_it(L_CRITICAL, "EMERGENCY: Thread safety check - current thread: %lu", 
           (unsigned long)pthread_self());
           
    // CRITICAL FIX: Ensure Python GIL is properly held for this thread
    PyGILState_STATE gstate = PyGILState_Ensure();
    log_it(L_CRITICAL, "EMERGENCY: Python GIL state acquired for thread safety");
    
    // CRITICAL FIX: Add memory barrier to prevent corruption
    __sync_synchronize();
    log_it(L_CRITICAL, "EMERGENCY: Memory barrier established, about to call function");
    
    // Use function pointer to ensure proper calling convention
    void* (*safe_import_func)(const char*, const char*) = dap_chain_plugins_load_plugin_importing;
    log_it(L_CRITICAL, "EMERGENCY: Function pointer set up: %p", safe_import_func);
    
    // CRITICAL FIX: Call with proper thread safety
    l_pvt_data = safe_import_func(l_manifest->path, l_manifest->name);
    log_it(L_CRITICAL, "EMERGENCY: Function call completed, result: %p", l_pvt_data);
    
    // Release Python GIL
    PyGILState_Release(gstate);
    log_it(L_CRITICAL, "EMERGENCY: Python GIL state released");
    
    if (!l_pvt_data){
        log_it(L_ERROR, "Plugin importing failed for %s", l_manifest->name);
        return -102;
    }
    
    log_it(L_NOTICE, "Plugin importing successful, initializing plugin: %s", l_manifest->name);
    *a_pvt_data = l_pvt_data;
    
    // Handle two-phase initialization
    _dap_chain_plugins_module_t *l_container = (_dap_chain_plugins_module_t *)l_pvt_data;
    if (s_current_phase == PHASE_EARLY) {
        // During early phase, call preload() method
        s_plugins_load_plugin_preload(l_pvt_data);
        l_container->preload_called = true;
        l_container->init_called = false;
    } else if (s_current_phase == PHASE_LATE || !s_early_init_done) {
        // During late phase or legacy mode, call init() method
        if (!l_container->init_called) {
            s_plugins_load_plugin_initialization(l_pvt_data);
            l_container->init_called = true;
        }
    }
    
    log_it(L_NOTICE, "=== PLUGIN LOADED SUCCESSFULLY: %s ===", l_manifest->name);
    return 0;
}

static int s_dap_chain_plugins_unload(dap_plugin_manifest_t * a_manifest, void * a_pvt_data, char ** a_error_str )
{
    log_it(L_NOTICE, "=== UNLOADING PLUGIN: %s ===", a_manifest->name ? a_manifest->name : "UNKNOWN");
    dap_plugin_manifest_t *l_manifest = a_manifest;
    void *l_pvt_data = a_pvt_data;
    if (l_manifest == NULL)
        return -100;
    if (*l_manifest->name == 0){
        log_it(L_ERROR, "Can't unload a plugin, file not found");
        return -101;
    }
    
    log_it(L_NOTICE, "Calling plugin uninitialization for: %s", l_manifest->name);
    // Main thread already holds the GIL, no need for PyGILState_Ensure/Release
    s_plugins_load_plugin_uninitialization(l_pvt_data);
    log_it(L_NOTICE, "=== PLUGIN UNLOADED: %s ===", l_manifest->name);
    return 0;
}

void* dap_chain_plugins_load_plugin_importing(const char *a_dir_path, const char *a_name) {
    // EMERGENCY: First logging to check if function is entered at all
    log_it(L_CRITICAL, "EMERGENCY: Function dap_chain_plugins_load_plugin_importing ENTERED with dir='%s', name='%s'", 
           a_dir_path ? a_dir_path : "NULL", a_name ? a_name : "NULL");
    
    log_it(L_NOTICE, "=== STARTING IMPORT OF MODULE '%s' FROM '%s' ===", a_name, a_dir_path);

    // Critical null pointer check to prevent crash
    if (!s_sys_path) {
        log_it(L_ERROR, "Python sys.path is not initialized, cannot import module \"%s\"", a_name);
        return NULL;
    }

    log_it(L_NOTICE, "Step 1: Creating Python string for directory path: %s", a_dir_path);
    PyObject *l_obj_dir_path = PyUnicode_FromString(a_dir_path);
    if (!l_obj_dir_path) {
        log_it(L_ERROR, "Step 1 FAILED: Cannot create Python string for directory path: %s", a_dir_path);
        python_error_in_log_it(LOG_TAG);
        return NULL;
    }
    log_it(L_NOTICE, "Step 1 SUCCESS: Python string created for directory path");
    
    log_it(L_NOTICE, "Step 2: Appending directory to sys.path");
    if (PyList_Append(s_sys_path, l_obj_dir_path) < 0) {
        log_it(L_ERROR, "Step 2 FAILED: Cannot append directory to sys.path: %s", a_dir_path);
        python_error_in_log_it(LOG_TAG);
        Py_DECREF(l_obj_dir_path);
        return NULL;
    }
    log_it(L_NOTICE, "Step 2 SUCCESS: Directory appended to sys.path");

    log_it(L_NOTICE, "Step 3: About to call PyImport_ImportModule(\"%s\") - THIS IS THE CRITICAL STEP", a_name);
    log_it(L_NOTICE, "Step 3: Python GIL state before import: %s", PyGILState_Check() ? "HELD" : "NOT HELD");
    
    // This is where the crash likely happens
    PyObject *l_obj_module = PyImport_ImportModule(a_name);
    
    if (!l_obj_module) {
        log_it(L_ERROR, "Step 3 FAILED: PyImport_ImportModule(\"%s\") returned NULL", a_name);
        python_error_in_log_it(LOG_TAG);
        Py_DECREF(l_obj_dir_path);
        return NULL;
    }
    
    log_it(L_NOTICE, "Step 3 SUCCESS: PyImport_ImportModule(\"%s\") completed successfully", a_name);
    log_it(L_NOTICE, "Step 4: Module imported successfully, checking for init function");

    // ... existing code ...

    log_it(L_NOTICE, "Creating module container...");
    _dap_chain_plugins_module_t *module = DAP_NEW_Z(_dap_chain_plugins_module_t);
    if (!module) {
        log_it(L_ERROR, "Failed to allocate memory for module container");
        python_error_in_log_it(LOG_TAG);
        PyErr_Clear();
        return NULL;
    }
    module->module = l_obj_module;
    module->name = dap_strdup(a_name);
    module->preload_called = false;
    module->init_called = false;
    module->next = _s_modules;
    _s_modules = module;
    Py_INCREF(l_obj_module);
    log_it(L_NOTICE, "Module container created successfully");
    log_it(L_NOTICE, "=== PLUGIN MODULE IMPORT COMPLETED ===");
    return module;
}

static void s_plugins_load_plugin_initialization(void* a_module){
    log_it(L_NOTICE, "=== PLUGIN INITIALIZATION START ===");
    
    if (!a_module) {
        log_it(L_ERROR, "Module pointer is NULL, cannot initialize");
        return;
    }
    
    // CRITICAL FIX: Acquire GIL before accessing ANY Python objects
    log_it(L_DEBUG, "[GIL-DEBUG] Plugin init acquire thread=%lu", (unsigned long)pthread_self());
    PyGILState_STATE gstate = PyGILState_Ensure();
    log_it(L_DEBUG, "[GIL-DEBUG] Plugin init acquired state=%d thread=%lu", gstate, (unsigned long)pthread_self());
    
    _dap_chain_plugins_module_t  *l_container = (_dap_chain_plugins_module_t  *)a_module;
    log_it(L_NOTICE, "Initializing plugin: %s", l_container->name ? l_container->name : "UNKNOWN");
    
    log_it(L_NOTICE, "Looking for 'init' function...");
    PyObject *l_func_init = PyObject_GetAttrString(l_container->module, "init");
    if (!l_func_init) {
        log_it(L_ERROR, "No 'init' attribute found for plugin %s", l_container->name);
        python_error_in_log_it(LOG_TAG);
        PyErr_Clear();
        log_it(L_DEBUG, "[GIL-DEBUG] Plugin init release (no init) thread=%lu", (unsigned long)pthread_self());
        PyGILState_Release(gstate);
        return;
    }
    
    log_it(L_NOTICE, "Looking for 'deinit' function...");
    PyObject *l_func_deinit = PyObject_GetAttrString(l_container->module, "deinit");
    PyObject *l_res_int = NULL;
    PyErr_Clear();
    
    if (PyCallable_Check(l_func_init)) {
        log_it(L_NOTICE, "Found callable 'init' function, preparing to call...");
        PyObject *l_void_tuple = PyTuple_New(0);
        if (!l_void_tuple) {
            log_it(L_ERROR, "Failed to create empty tuple for init call");
            python_error_in_log_it(LOG_TAG);
            Py_XDECREF(l_func_init);
            Py_XDECREF(l_func_deinit);
            log_it(L_DEBUG, "[GIL-DEBUG] Plugin init release (tuple error) thread=%lu", (unsigned long)pthread_self());
            PyGILState_Release(gstate);
            return;
        }
        
        log_it(L_NOTICE, "CALLING PLUGIN INIT FUNCTION - CRITICAL SECTION");
        // GIL is already acquired at function start
        
        l_res_int = PyObject_CallObject(l_func_init, l_void_tuple);
        
        if (l_res_int) {
            log_it(L_NOTICE, "Plugin init function returned successfully");
            if (PyLong_Check(l_res_int)) {
                int result = _PyLong_AsInt(l_res_int);
                log_it(L_NOTICE, "Init function returned: %d", result);
                if (result == 0) {
                    log_it(L_NOTICE, "Plugin initialization successful");
                    Py_INCREF(l_container->module);
                } else {
                    log_it(L_ERROR, "Plugin initialization failed with code: %d", result);
                    python_error_in_log_it(LOG_TAG);
                }
            } else {
                log_it(L_ERROR, "Init function returned non-integer value");
                python_error_in_log_it(LOG_TAG);
            }
        } else {
            log_it(L_ERROR, "PLUGIN INIT FUNCTION CALL FAILED - this may cause crashes");
            python_error_in_log_it(LOG_TAG);
        }
        
        Py_XDECREF(l_res_int);
        Py_XDECREF(l_void_tuple);
    } else {
        log_it(L_ERROR, "Found 'init' attribute but it's not callable for plugin %s", l_container->name);
    }
    
    Py_XDECREF(l_func_init);
    
    if (l_func_deinit == NULL || !PyCallable_Check(l_func_deinit)){
        log_it(L_WARNING, "Can't find callable 'deinit' function for plugin %s", l_container->name);
    } else {
        log_it(L_NOTICE, "Found callable 'deinit' function");
    }
    Py_XDECREF(l_func_deinit);
    
    log_it(L_DEBUG, "[GIL-DEBUG] Plugin init release (end) thread=%lu", (unsigned long)pthread_self());
    PyGILState_Release(gstate);
    
    log_it(L_NOTICE, "=== PLUGIN INITIALIZATION COMPLETED ===");
}

static void s_plugins_load_plugin_uninitialization(void* a_module){
    if (!a_module)
        return;
        
    log_it(L_DEBUG, "[GIL-DEBUG] Plugin deinit acquire thread=%lu", (unsigned long)pthread_self());
    PyGILState_STATE gstate = PyGILState_Ensure();
    log_it(L_DEBUG, "[GIL-DEBUG] Plugin deinit acquired state=%d thread=%lu", gstate, (unsigned long)pthread_self());
    
    _dap_chain_plugins_module_t  *l_container = (_dap_chain_plugins_module_t  *)a_module;
    PyObject *l_func_deinit = PyObject_GetAttrString(l_container->module, "deinit");
    PyObject *l_res_int = NULL;
    PyErr_Clear();
    if (PyCallable_Check(l_func_deinit)) {
        PyObject *l_void_tuple = PyTuple_New(0);
        // GIL is already held by caller, no need to acquire again
        l_res_int = PyObject_CallObject(l_func_deinit, l_void_tuple);
        if (l_res_int && PyLong_Check(l_res_int)) {
            if (_PyLong_AsInt(l_res_int) == 0) {
                //                dap_chain_plugins_list_add(l_container->module, l_container->name);
                Py_INCREF(l_container->module);
            } else {
                python_error_in_log_it(LOG_TAG);
                log_it(L_ERROR, "Can't deinitialize \"%s\" plugin. Code error: %i", l_container->name,
                       _PyLong_AsInt(l_res_int));
            }
        } else {
            python_error_in_log_it(LOG_TAG);
            log_it(L_ERROR, "The 'deinit' function of \"%s\" plugin didn't return an integer value", l_container->name);
        }
        Py_XDECREF(l_res_int);
        Py_XDECREF(l_void_tuple);
    } else {
        log_it(L_ERROR, "Can't find 'deinit' function of \"%s\" plugin", l_container->name);
    }
    Py_XDECREF(l_func_deinit);
    
    log_it(L_DEBUG, "[GIL-DEBUG] Plugin deinit release thread=%lu", (unsigned long)pthread_self());
    PyGILState_Release(gstate);
}

void dap_chain_plugins_load_plugin(const char *a_dir_path, const char *a_name){
    log_it(L_NOTICE, "Loading \"%s\" plugin directory %s", a_name, a_dir_path);
    
    log_it(L_DEBUG, "[GIL-DEBUG] Plugin load acquire thread=%lu", (unsigned long)pthread_self());
    PyGILState_STATE gstate = PyGILState_Ensure();
    log_it(L_DEBUG, "[GIL-DEBUG] Plugin load acquired state=%d thread=%lu", gstate, (unsigned long)pthread_self());
    
    PyErr_Clear();

    PyObject *l_obj_dir_path = PyUnicode_FromString(a_dir_path);
    if (!l_obj_dir_path) {
        log_it(L_ERROR, "Failed to create Python string for path: %s", a_dir_path);
        python_error_in_log_it(LOG_TAG);
        return;
    }
    
    PyList_Append(s_sys_path, l_obj_dir_path);
    Py_DECREF(l_obj_dir_path);
    
    PyObject *l_module = PyImport_ImportModule(a_name);
    if (!l_module) {
        log_it(L_ERROR, "Failed to import module \"%s\"", a_name);
        python_error_in_log_it(LOG_TAG);
        return;
    }
    
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
    } else {
        log_it(L_ERROR, "Can't find 'init' function of \"%s\" plugin", a_name);
    }
    
    if (l_func_deinit == NULL || !PyCallable_Check(l_func_deinit)){
        log_it(L_WARNING, "Can't find 'deinit' function of \"%s\" plugin", a_name);
    }
    
    Py_XDECREF(l_func_init);
    Py_XDECREF(l_func_deinit);
    Py_DECREF(l_module);
    
    log_it(L_DEBUG, "[GIL-DEBUG] Plugin load release thread=%lu", (unsigned long)pthread_self());
    PyGILState_Release(gstate);
}

/**
 * @brief Clean up Python interpreter and threading
 * @details This function should be called on application shutdown
 */
void dap_chain_plugins_deinit(void)
{
    if (!s_python_initialized)
        return;
        
    log_it(L_DEBUG, "Shutting down Python interpreter");
    
    // Clean up Python objects
    Py_XDECREF(s_sys_path);
    s_sys_path = NULL;
    
    // Finalize Python interpreter
    if (Py_FinalizeEx() < 0) {
        log_it(L_WARNING, "Python interpreter finalization returned error");
    }
    
    s_python_initialized = false;
    log_it(L_DEBUG, "Python interpreter shutdown complete");
}

/**
 * @brief Load all Python plugins from already-loaded manifests and call preload() method
 */
static void s_load_all_python_plugins()
{
    log_it(L_NOTICE, "Loading Python plugins from manifest system");
    
    // Use the existing manifest system that already parsed all plugins
    dap_plugin_manifest_t *l_manifest, *l_tmp;
    int l_total_plugins = 0;
    int l_python_plugins = 0;
    
    HASH_ITER(hh, dap_plugin_manifest_all(), l_manifest, l_tmp) {
        l_total_plugins++;
        log_it(L_DEBUG, "Found plugin: %s (type: %s) at path: %s", 
               l_manifest->name, l_manifest->type ? l_manifest->type : "NULL", l_manifest->path);
        
        // Check if it's a Python plugin
        if (l_manifest->type && strcmp(l_manifest->type, "python") == 0) {
            l_python_plugins++;
            log_it(L_NOTICE, "Loading Python plugin: %s (module: %s) from path: %s", 
                   l_manifest->name, l_manifest->name, l_manifest->path);
            
            PyGILState_STATE gstate = PyGILState_Ensure();
            // Use manifest name (the actual Python module name) and manifest path
            void *l_module = dap_chain_plugins_load_plugin_importing(l_manifest->path, l_manifest->name);
            if (l_module) {
                // Call preload() method during early phase
                s_plugins_load_plugin_preload(l_module);
                
                // Add to our module list
                _dap_chain_plugins_module_t *l_container = (_dap_chain_plugins_module_t *)l_module;
                l_container->preload_called = true;
                l_container->init_called = false;
            } else {
                log_it(L_ERROR, "Failed to load Python plugin: %s", l_manifest->name);
            }
            PyGILState_Release(gstate);
        }
    }
    
    log_it(L_NOTICE, "Plugin discovery complete: found %d total plugins, %d Python plugins", 
           l_total_plugins, l_python_plugins);
}

/**
 * @brief Call preload() method on Python plugin module
 * @param a_module Plugin module container
 */
static void s_plugins_load_plugin_preload(void* a_module)
{
    log_it(L_NOTICE, "=== PLUGIN PRELOAD START ===");
    
    if (!a_module) {
        log_it(L_ERROR, "Module pointer is NULL, cannot preload");
        return;
    }
    
    _dap_chain_plugins_module_t *l_container = (_dap_chain_plugins_module_t *)a_module;
    log_it(L_NOTICE, "Preloading plugin: %s", l_container->name ? l_container->name : "UNKNOWN");
    
    PyObject *l_func_preload = PyObject_GetAttrString(l_container->module, "preload");
    if (!l_func_preload) {
        log_it(L_INFO, "No 'preload' method found for plugin %s, skipping", l_container->name);
        PyErr_Clear();
        return;
    }
    
    if (PyCallable_Check(l_func_preload)) {
        log_it(L_NOTICE, "Found callable 'preload' function, calling...");
        PyObject *l_void_tuple = PyTuple_New(0);
        if (!l_void_tuple) {
            log_it(L_ERROR, "Failed to create empty tuple for preload call");
            python_error_in_log_it(LOG_TAG);
            Py_XDECREF(l_func_preload);
            return;
        }
        
        PyObject *l_res_int = PyObject_CallObject(l_func_preload, l_void_tuple);
        
        if (l_res_int) {
            log_it(L_NOTICE, "Plugin preload function returned successfully");
            if (PyLong_Check(l_res_int)) {
                int result = _PyLong_AsInt(l_res_int);
                log_it(L_NOTICE, "Preload function returned: %d", result);
                if (result == 0) {
                    log_it(L_NOTICE, "Plugin preload successful - services can now be registered");
                } else {
                    log_it(L_ERROR, "Plugin preload failed with code: %d", result);
                    python_error_in_log_it(LOG_TAG);
                }
            } else {
                log_it(L_ERROR, "Preload function returned non-integer value");
                python_error_in_log_it(LOG_TAG);
            }
        } else {
            log_it(L_ERROR, "Plugin preload function call failed");
            python_error_in_log_it(LOG_TAG);
        }
        
        Py_XDECREF(l_res_int);
        Py_XDECREF(l_void_tuple);
    } else {
        log_it(L_ERROR, "Found 'preload' attribute but it's not callable for plugin %s", l_container->name);
    }
    
    Py_XDECREF(l_func_preload);
    log_it(L_NOTICE, "=== PLUGIN PRELOAD COMPLETED ===");
}

/**
 * @brief Legacy function - redirects to late initialization
 * @param a_config Configuration object  
 * @return 0 on success, negative on error
 */
int dap_chain_plugins_init(dap_config_t *a_config)
{
    log_it(L_NOTICE, "=== PYTHON PLUGINS INIT (LEGACY) ===");
    
    if (!dap_config_get_item_bool_default(a_config, "plugins", "py_load", false)) {
        log_it(L_NOTICE, "Python plugins disabled in config - exiting cleanly");
        return -1;
    }

    // If early init was not done, do full initialization
    if (!s_early_init_done) {
        log_it(L_NOTICE, "Early init not done, performing full initialization");
        
        s_current_phase = PHASE_LATE;
        
        // Initialize Python interpreter
        int l_ret = s_python_interpreter_init(a_config);
        if (l_ret != 0) {
            return l_ret;
        }
        
        return 0; // Let the plugin system handle loading via callbacks
    } else {
        // Early init was done, just do late init
        return dap_chain_plugins_late_init(a_config);
    }
}
