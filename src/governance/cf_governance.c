#include "cellframe.h"
#include "dap_chain_decree_callbacks.h"
#include "dap_chain_datum_decree.h"
#include "dap_chain_ledger.h"
#include "cf_callbacks_registry.h"

#include <pthread.h>

#define LOG_TAG "python_cellframe_governance"

typedef struct {
    uint16_t decree_type;
    uint16_t decree_subtype;
    PyObject *callback;
} python_governance_decree_handler_ctx_t;

static python_governance_decree_handler_ctx_t **s_governance_handlers = NULL;
static size_t s_governance_handlers_count = 0;
static pthread_mutex_t s_governance_handlers_mutex = PTHREAD_MUTEX_INITIALIZER;

static python_governance_decree_handler_ctx_t *cf_governance_handler_find(uint16_t type, uint16_t subtype, size_t *index_out) {
    for (size_t i = 0; i < s_governance_handlers_count; i++) {
        python_governance_decree_handler_ctx_t *ctx = s_governance_handlers[i];
        if (ctx && ctx->decree_type == type && ctx->decree_subtype == subtype) {
            if (index_out) {
                *index_out = i;
            }
            return ctx;
        }
    }
    return NULL;
}

static PyObject *cf_governance_handler_get_callback(uint16_t type, uint16_t subtype) {
    PyObject *callback = NULL;
    pthread_mutex_lock(&s_governance_handlers_mutex);
    python_governance_decree_handler_ctx_t *ctx = cf_governance_handler_find(type, subtype, NULL);
    if (ctx && ctx->callback) {
        callback = ctx->callback;
        Py_INCREF(callback);
    }
    pthread_mutex_unlock(&s_governance_handlers_mutex);
    return callback;
}

static void cf_governance_handler_remove_ctx(python_governance_decree_handler_ctx_t *ctx) {
    if (!ctx) {
        return;
    }

    pthread_mutex_lock(&s_governance_handlers_mutex);
    size_t index = 0;
    for (; index < s_governance_handlers_count; index++) {
        if (s_governance_handlers[index] == ctx) {
            break;
        }
    }
    if (index < s_governance_handlers_count) {
        for (size_t i = index + 1; i < s_governance_handlers_count; i++) {
            s_governance_handlers[i - 1] = s_governance_handlers[i];
        }
        s_governance_handlers_count--;
        if (s_governance_handlers_count == 0) {
            DAP_DELETE(s_governance_handlers);
            s_governance_handlers = NULL;
        } else {
            python_governance_decree_handler_ctx_t **tmp = DAP_REALLOC(
                s_governance_handlers, s_governance_handlers_count * sizeof(python_governance_decree_handler_ctx_t *)
            );
            if (tmp) {
                s_governance_handlers = tmp;
            }
        }
    }
    pthread_mutex_unlock(&s_governance_handlers_mutex);
}

static int s_governance_decree_handler_wrapper(dap_chain_datum_decree_t *a_decree, dap_ledger_t *a_ledger,
                                               dap_chain_t *a_chain, bool a_apply) {
    if (!a_decree || !a_ledger) {
        log_it(L_ERROR, "Invalid decree handler arguments");
        return -1;
    }

    PyObject *callback = cf_governance_handler_get_callback(a_decree->header.type, a_decree->header.sub_type);
    if (!callback) {
        log_it(L_ERROR, "No Python decree handler registered for type=%u subtype=%u",
               a_decree->header.type, a_decree->header.sub_type);
        return -404;
    }

    PyGILState_STATE gstate = PyGILState_Ensure();

    PyObject *decree_capsule = PyCapsule_New(a_decree, "dap_chain_datum_decree_t", NULL);
    PyObject *ledger_capsule = PyCapsule_New(a_ledger, "dap_ledger_t", NULL);
    PyObject *chain_capsule = a_chain ? PyCapsule_New(a_chain, "dap_chain_t", NULL) : Py_None;
    PyObject *apply_obj = PyBool_FromLong(a_apply ? 1 : 0);

    if (!decree_capsule || !ledger_capsule || !apply_obj || (a_chain && !chain_capsule)) {
        Py_XDECREF(decree_capsule);
        Py_XDECREF(ledger_capsule);
        if (a_chain) {
            Py_XDECREF(chain_capsule);
        }
        Py_XDECREF(apply_obj);
        Py_DECREF(callback);
        PyGILState_Release(gstate);
        return -1;
    }

    if (!a_chain) {
        Py_INCREF(Py_None);
        chain_capsule = Py_None;
    }

    PyObject *result = PyObject_CallFunctionObjArgs(
        callback, decree_capsule, ledger_capsule, chain_capsule, apply_obj, NULL
    );

    Py_DECREF(decree_capsule);
    Py_DECREF(ledger_capsule);
    Py_DECREF(chain_capsule);
    Py_DECREF(apply_obj);
    Py_DECREF(callback);

    int rc = -1;
    if (!result) {
        log_it(L_ERROR, "Python decree handler raised an exception");
        PyErr_Print();
    } else {
        if (PyLong_Check(result)) {
            rc = (int)PyLong_AsLong(result);
        } else {
            log_it(L_WARNING, "Decree handler returned non-integer result");
        }
        Py_DECREF(result);
    }

    PyGILState_Release(gstate);
    return rc;
}

PyObject *dap_chain_decree_handler_register_py(PyObject *self, PyObject *args) {
    (void)self;
    unsigned int decree_type = 0;
    unsigned int decree_subtype = 0;
    PyObject *callback = NULL;

    if (!PyArg_ParseTuple(args, "IIO", &decree_type, &decree_subtype, &callback)) {
        return NULL;
    }

    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, "Handler must be callable");
        return NULL;
    }

    python_governance_decree_handler_ctx_t *ctx = NULL;
    PyObject *old_callback = NULL;
    bool replaced = false;

    pthread_mutex_lock(&s_governance_handlers_mutex);
    ctx = cf_governance_handler_find((uint16_t)decree_type, (uint16_t)decree_subtype, NULL);
    if (!ctx) {
        ctx = DAP_NEW_Z(python_governance_decree_handler_ctx_t);
        if (!ctx) {
            pthread_mutex_unlock(&s_governance_handlers_mutex);
            PyErr_SetString(PyExc_MemoryError, "Failed to allocate handler context");
            return NULL;
        }
        ctx->decree_type = (uint16_t)decree_type;
        ctx->decree_subtype = (uint16_t)decree_subtype;

        python_governance_decree_handler_ctx_t **tmp = DAP_REALLOC(
            s_governance_handlers, (s_governance_handlers_count + 1) * sizeof(python_governance_decree_handler_ctx_t *)
        );
        if (!tmp) {
            pthread_mutex_unlock(&s_governance_handlers_mutex);
            DAP_DELETE(ctx);
            PyErr_SetString(PyExc_MemoryError, "Failed to grow handler registry");
            return NULL;
        }
        s_governance_handlers = tmp;
        s_governance_handlers[s_governance_handlers_count++] = ctx;
    } else {
        replaced = true;
        old_callback = ctx->callback;
    }
    ctx->callback = callback;
    pthread_mutex_unlock(&s_governance_handlers_mutex);

    int rc = dap_chain_decree_handler_register((uint16_t)decree_type, (uint16_t)decree_subtype,
                                               s_governance_decree_handler_wrapper);
    if (rc != 0) {
        if (replaced) {
            pthread_mutex_lock(&s_governance_handlers_mutex);
            ctx->callback = old_callback;
            pthread_mutex_unlock(&s_governance_handlers_mutex);
        } else {
            cf_governance_handler_remove_ctx(ctx);
            DAP_DELETE(ctx);
        }
        return PyLong_FromLong(rc);
    }

    if (replaced) {
        cf_callbacks_registry_remove(ctx);
    }

    if (cf_callbacks_registry_add(CF_CALLBACK_TYPE_DECREE_REGISTRY, callback, NULL, ctx, NULL) != 0) {
        log_it(L_WARNING, "Failed to register decree handler in callbacks registry");
    }

    return PyLong_FromLong(rc);
}

PyObject *dap_chain_decree_handler_call_py(PyObject *self, PyObject *args) {
    (void)self;
    unsigned int decree_type = 0;
    unsigned int decree_subtype = 0;
    PyObject *decree_obj = NULL;
    PyObject *ledger_obj = NULL;
    PyObject *chain_obj = NULL;
    int apply = 0;

    if (!PyArg_ParseTuple(args, "IIOOOp", &decree_type, &decree_subtype, &decree_obj, &ledger_obj, &chain_obj, &apply)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(decree_obj)) {
        PyErr_SetString(PyExc_TypeError, "Decree must be a dap_chain_datum_decree_t capsule");
        return NULL;
    }
    dap_chain_datum_decree_t *decree = (dap_chain_datum_decree_t *)PyCapsule_GetPointer(decree_obj, "dap_chain_datum_decree_t");
    if (!decree) {
        PyErr_SetString(PyExc_ValueError, "Invalid decree capsule");
        return NULL;
    }

    if (!PyCapsule_CheckExact(ledger_obj)) {
        PyErr_SetString(PyExc_TypeError, "Ledger must be a dap_ledger_t capsule");
        return NULL;
    }
    dap_ledger_t *ledger = (dap_ledger_t *)PyCapsule_GetPointer(ledger_obj, "dap_ledger_t");
    if (!ledger) {
        PyErr_SetString(PyExc_ValueError, "Invalid ledger capsule");
        return NULL;
    }

    dap_chain_t *chain = NULL;
    if (chain_obj != Py_None) {
        if (!PyCapsule_CheckExact(chain_obj)) {
            PyErr_SetString(PyExc_TypeError, "Chain must be a dap_chain_t capsule or None");
            return NULL;
        }
        chain = (dap_chain_t *)PyCapsule_GetPointer(chain_obj, "dap_chain_t");
        if (!chain) {
            PyErr_SetString(PyExc_ValueError, "Invalid chain capsule");
            return NULL;
        }
    }

    int rc = dap_chain_decree_handler_call((uint16_t)decree_type, (uint16_t)decree_subtype,
                                           decree, ledger, chain, apply != 0);
    return PyLong_FromLong(rc);
}

PyMethodDef *cellframe_governance_get_methods(void) {
    static PyMethodDef governance_methods[] = {
        {"dap_chain_decree_handler_register", (PyCFunction)dap_chain_decree_handler_register_py, METH_VARARGS,
         "Register decree handler callback\n"
         "Args:\n"
         "    decree_type (int): Decree type\n"
         "    decree_subtype (int): Decree subtype\n"
         "    handler (callable): Python callback (decree, ledger, chain, apply) -> int\n"
         "Returns:\n"
         "    int: 0 on success, negative on error"},
        {"dap_chain_decree_handler_call", (PyCFunction)dap_chain_decree_handler_call_py, METH_VARARGS,
         "Call registered decree handler\n"
         "Args:\n"
         "    decree_type (int): Decree type\n"
         "    decree_subtype (int): Decree subtype\n"
         "    decree (capsule): dap_chain_datum_decree_t capsule\n"
         "    ledger (capsule): dap_ledger_t capsule\n"
         "    chain (capsule|None): dap_chain_t capsule or None\n"
         "    apply (bool): apply changes\n"
         "Returns:\n"
         "    int: 0 on success, -1 if no handler"},
        {NULL, NULL, 0, NULL}
    };
    return governance_methods;
}
