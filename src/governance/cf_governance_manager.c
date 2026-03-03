/*
 * Authors:
 * Dmitriy Gerasimov <naeper@demlabs.net>
 * DeM Labs Inc.   https://demlabs.net
 * Copyright  (c) 2025
 * All rights reserved.

Governance Module - Python Bindings

Thin wrappers around C governance API:
- dap_chain_decree_handler_register() - register decree handlers
- dap_chain_decree_handler_call() - call handlers
- Decree datum type (already in cf_governance_decree.c)
*/

#include "cf_governance.h"
#include "dap_chain_datum_decree.h"
#include "dap_chain_decree_callbacks.h"
#include "dap_chain_ledger.h"
#include "dap_chain.h"
#include "dap_ht.h"
#include <Python.h>
#include <pthread.h>

#define LOG_TAG "cf_governance"

// GIL management
#define ENSURE_GIL() PyGILState_STATE gstate = PyGILState_Ensure()
#define RELEASE_GIL() PyGILState_Release(gstate)

// ============================================================================
// Python Callback Registry
// ============================================================================

typedef struct {
    uint32_t key;  // (type << 16) | subtype
    PyObject *callback;
    PyObject *user_data;
    dap_ht_handle_t hh;
} py_decree_handler_t;

static py_decree_handler_t *s_py_handlers = NULL;
static pthread_mutex_t s_handlers_mutex = PTHREAD_MUTEX_INITIALIZER;

// C wrapper that calls Python callback with GIL management
static int s_py_decree_handler_wrapper(dap_chain_datum_decree_t *a_decree,
                                       dap_ledger_t *a_ledger,
                                       dap_chain_t *a_chain,
                                       bool a_apply) {
    if (!a_decree) {
        return -1;
    }
    
    uint32_t key = ((uint32_t)a_decree->header.type << 16) | a_decree->header.sub_type;
    
    // Find Python handler
    py_decree_handler_t *handler = NULL;
    pthread_mutex_lock(&s_handlers_mutex);
    dap_ht_find_int(s_py_handlers, key, handler);
    pthread_mutex_unlock(&s_handlers_mutex);
    
    if (!handler || !handler->callback) {
        return -1;
    }
    
    // Acquire GIL
    ENSURE_GIL();
    
    // Wrap C objects for Python
    size_t decree_size = dap_chain_datum_decree_get_size(a_decree);
    PyObject *py_decree = PyDapDecree_from_decree(a_decree, decree_size, false);
    PyObject *py_ledger = PyCapsule_New(a_ledger, "dap_ledger_t", NULL);
    PyObject *py_chain = a_chain ? PyCapsule_New(a_chain, "dap_chain_t", NULL) : Py_None;
    if (py_chain != Py_None) Py_INCREF(py_chain);
    PyObject *py_apply = PyBool_FromLong(a_apply);
    
    // Call Python callback(decree, ledger, chain, apply, user_data)
    PyObject *result = PyObject_CallFunctionObjArgs(
        handler->callback,
        py_decree,
        py_ledger,
        py_chain,
        py_apply,
        handler->user_data ? handler->user_data : Py_None,
        NULL
    );
    
    int ret_code = 0;
    if (result == NULL) {
        PyErr_Print();
        ret_code = -1;
    } else {
        if (PyLong_Check(result)) {
            ret_code = (int)PyLong_AsLong(result);
        }
        Py_DECREF(result);
    }
    
    // Cleanup
    Py_XDECREF(py_decree);
    Py_XDECREF(py_ledger);
    if (py_chain != Py_None) Py_DECREF(py_chain);
    Py_DECREF(py_apply);
    
    RELEASE_GIL();
    return ret_code;
}

// ============================================================================
// Python API Functions
// ============================================================================

/*
 * Register Python decree handler
 * 
 * Args:
 *   type (int): Decree type (0x0001=COMMON, 0x0002=SERVICE)
 *   subtype (int): Decree subtype
 *   callback (callable): Python function(decree, ledger, chain, apply, user_data) -> int
 *   user_data (object): Optional user data
 * 
 * Returns:
 *   None
 */
static PyObject* py_decree_handler_register(PyObject *self, PyObject *args, PyObject *kwargs) {
    static char *kwlist[] = {"type", "subtype", "callback", "user_data", NULL};
    
    int type, subtype;
    PyObject *callback = NULL, *user_data = Py_None;
    
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "iiO|O:register_handler", kwlist,
                                     &type, &subtype, &callback, &user_data)) {
        return NULL;
    }
    
    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, "callback must be callable");
        return NULL;
    }
    
    uint32_t key = ((uint32_t)type << 16) | (uint32_t)subtype;
    
    pthread_mutex_lock(&s_handlers_mutex);
    
    // Check if already registered
    py_decree_handler_t *existing = NULL;
    dap_ht_find_int(s_py_handlers, key, existing);
    if (existing) {
        pthread_mutex_unlock(&s_handlers_mutex);
        PyErr_Format(PyExc_ValueError, "Handler already registered for type=0x%04x subtype=0x%04x", type, subtype);
        return NULL;
    }
    
    // Create Python handler entry
    py_decree_handler_t *handler = DAP_NEW_Z(py_decree_handler_t);
    handler->key = key;
    handler->callback = callback;
    Py_INCREF(callback);
    if (user_data != Py_None) {
        handler->user_data = user_data;
        Py_INCREF(user_data);
    }
    
    dap_ht_add_int(s_py_handlers, key, handler);
    pthread_mutex_unlock(&s_handlers_mutex);
    
    // Register C wrapper with governance system
    int ret = dap_chain_decree_handler_register((uint16_t)type, (uint16_t)subtype, s_py_decree_handler_wrapper);
    if (ret != 0) {
        // Rollback Python registry
        pthread_mutex_lock(&s_handlers_mutex);
        dap_ht_del(s_py_handlers, handler);
        pthread_mutex_unlock(&s_handlers_mutex);
        
        Py_DECREF(callback);
        if (user_data != Py_None) Py_DECREF(user_data);
        DAP_DELETE(handler);
        
        PyErr_Format(PyExc_RuntimeError, "Failed to register decree handler: error %d", ret);
        return NULL;
    }
    
    Py_RETURN_NONE;
}

/*
 * Unregister decree handler
 * 
 * Args:
 *   type (int): Decree type
 *   subtype (int): Decree subtype
 * 
 * Returns:
 *   None
 */
static PyObject* py_decree_handler_unregister(PyObject *self, PyObject *args) {
    int type, subtype;
    
    if (!PyArg_ParseTuple(args, "ii:unregister_handler", &type, &subtype)) {
        return NULL;
    }
    
    uint32_t key = ((uint32_t)type << 16) | (uint32_t)subtype;
    
    pthread_mutex_lock(&s_handlers_mutex);
    
    py_decree_handler_t *handler = NULL;
    dap_ht_find_int(s_py_handlers, key, handler);
    if (handler) {
        dap_ht_del(s_py_handlers, handler);
        Py_XDECREF(handler->callback);
        Py_XDECREF(handler->user_data);
        DAP_DELETE(handler);
    }
    
    pthread_mutex_unlock(&s_handlers_mutex);
    
    if (!handler) {
        PyErr_Format(PyExc_ValueError, "No handler registered for type=0x%04x subtype=0x%04x", type, subtype);
        return NULL;
    }
    
    Py_RETURN_NONE;
}

/*
 * Call registered decree handler (for testing/manual invocation)
 * 
 * Args:
 *   type (int): Decree type
 *   subtype (int): Decree subtype
 *   decree (Decree): Decree object
 *   ledger (capsule): Ledger capsule
 *   chain (capsule): Chain capsule or None
 *   apply (bool): Apply changes or verify only
 * 
 * Returns:
 *   int: Handler return code
 */
static PyObject* py_decree_handler_call(PyObject *self, PyObject *args, PyObject *kwargs) {
    static char *kwlist[] = {"type", "subtype", "decree", "ledger", "chain", "apply", NULL};
    
    int type, subtype;
    PyObject *py_decree, *py_ledger, *py_chain = Py_None;
    int apply = 0;
    
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "iiOO|Oi:call_handler", kwlist,
                                     &type, &subtype, &py_decree, &py_ledger, &py_chain, &apply)) {
        return NULL;
    }
    
    // Extract C objects
    dap_chain_datum_decree_t *decree = ((PyDapDecreeObject*)py_decree)->decree;
    dap_ledger_t *ledger = PyCapsule_GetPointer(py_ledger, "dap_ledger_t");
    dap_chain_t *chain = (py_chain != Py_None) ? PyCapsule_GetPointer(py_chain, "dap_chain_t") : NULL;
    
    if (!decree || !ledger) {
        PyErr_SetString(PyExc_ValueError, "Invalid decree or ledger");
        return NULL;
    }
    
    // Call C API
    int ret = dap_chain_decree_handler_call((uint16_t)type, (uint16_t)subtype, decree, ledger, chain, (bool)apply);
    
    return PyLong_FromLong(ret);
}

// ============================================================================
// Module Methods
// ============================================================================

static PyMethodDef governance_methods[] = {
    {"register_handler", (PyCFunction)(void(*)(void))py_decree_handler_register, METH_VARARGS | METH_KEYWORDS,
     "Register decree handler: register_handler(type, subtype, callback, user_data=None)"},
    {"unregister_handler", py_decree_handler_unregister, METH_VARARGS,
     "Unregister decree handler: unregister_handler(type, subtype)"},
    {"call_handler", (PyCFunction)(void(*)(void))py_decree_handler_call, METH_VARARGS | METH_KEYWORDS,
     "Call decree handler: call_handler(type, subtype, decree, ledger, chain, apply)"},
    {NULL, NULL, 0, NULL}
};

// ============================================================================
// Module Initialization & Cleanup
// ============================================================================

int cellframe_governance_manager_init(PyObject *module) {
    // Add governance functions to module
    for (PyMethodDef *method = governance_methods; method->ml_name != NULL; method++) {
        PyObject *func = PyCFunction_New(method, NULL);
        if (!func) {
            return -1;
        }
        if (PyModule_AddObject(module, method->ml_name, func) < 0) {
            Py_DECREF(func);
            return -1;
        }
    }
    
    // Add decree type/subtype constants
    PyModule_AddIntConstant(module, "DECREE_TYPE_COMMON", 0x0001);
    PyModule_AddIntConstant(module, "DECREE_TYPE_SERVICE", 0x0002);
    
    PyModule_AddIntConstant(module, "DECREE_SUBTYPE_FEE", 0x0001);
    PyModule_AddIntConstant(module, "DECREE_SUBTYPE_OWNERS", 0x0002);
    PyModule_AddIntConstant(module, "DECREE_SUBTYPE_OWNERS_MIN", 0x0003);
    PyModule_AddIntConstant(module, "DECREE_SUBTYPE_STAKE_APPROVE", 0x0005);
    PyModule_AddIntConstant(module, "DECREE_SUBTYPE_STAKE_INVALIDATE", 0x0006);
    PyModule_AddIntConstant(module, "DECREE_SUBTYPE_STAKE_MIN_VALUE", 0x0007);
    PyModule_AddIntConstant(module, "DECREE_SUBTYPE_STAKE_MIN_VALIDATORS", 0x0008);
    PyModule_AddIntConstant(module, "DECREE_SUBTYPE_BAN", 0x0009);
    PyModule_AddIntConstant(module, "DECREE_SUBTYPE_UNBAN", 0x000A);
    PyModule_AddIntConstant(module, "DECREE_SUBTYPE_HARDFORK", 0x000F);
    PyModule_AddIntConstant(module, "DECREE_SUBTYPE_POLICY", 0x0012);
    
    return 0;
}

void cellframe_governance_manager_cleanup(void) {
    // Cleanup all registered Python handlers
    pthread_mutex_lock(&s_handlers_mutex);
    
    py_decree_handler_t *handler, *tmp;
    dap_ht_foreach(s_py_handlers, handler, tmp) {
        dap_ht_del(s_py_handlers, handler);
        Py_XDECREF(handler->callback);
        Py_XDECREF(handler->user_data);
        DAP_DELETE(handler);
    }
    
    pthread_mutex_unlock(&s_handlers_mutex);
}
