#include "cf_datum_internal.h"
#include "dap_chain_datum_decree.h"
#include "dap_chain_decree_registry.h"
#include "../common/cf_callbacks_registry.h"

#include <pthread.h>

#define LOG_TAG "python_cellframe_datum"

typedef struct {
    uint16_t decree_type;
    uint16_t decree_subtype;
    PyObject *callback;
    char *handler_name;
} python_decree_handler_ctx_t;

static python_decree_handler_ctx_t **s_decree_handlers = NULL;
static size_t s_decree_handlers_count = 0;
static pthread_mutex_t s_decree_handlers_mutex = PTHREAD_MUTEX_INITIALIZER;

static python_decree_handler_ctx_t *cf_datum_decree_handler_find(uint16_t type, uint16_t subtype, size_t *index_out) {
    for (size_t i = 0; i < s_decree_handlers_count; i++) {
        python_decree_handler_ctx_t *ctx = s_decree_handlers[i];
        if (ctx && ctx->decree_type == type && ctx->decree_subtype == subtype) {
            if (index_out) {
                *index_out = i;
            }
            return ctx;
        }
    }
    return NULL;
}

static PyObject *cf_datum_decree_handler_get_callback(uint16_t type, uint16_t subtype) {
    PyObject *callback = NULL;
    pthread_mutex_lock(&s_decree_handlers_mutex);
    python_decree_handler_ctx_t *ctx = cf_datum_decree_handler_find(type, subtype, NULL);
    if (ctx && ctx->callback) {
        callback = ctx->callback;
        Py_INCREF(callback);
    }
    pthread_mutex_unlock(&s_decree_handlers_mutex);
    return callback;
}

static void cf_datum_decree_handler_free(python_decree_handler_ctx_t *ctx) {
    if (!ctx) {
        return;
    }
    cf_callbacks_registry_remove(ctx);
    DAP_DELETE(ctx->handler_name);
    DAP_DELETE(ctx);
}

static void cf_datum_decree_handlers_clear(void) {
    pthread_mutex_lock(&s_decree_handlers_mutex);
    for (size_t i = 0; i < s_decree_handlers_count; i++) {
        cf_datum_decree_handler_free(s_decree_handlers[i]);
    }
    DAP_DELETE(s_decree_handlers);
    s_decree_handlers = NULL;
    s_decree_handlers_count = 0;
    pthread_mutex_unlock(&s_decree_handlers_mutex);
}

static int s_decree_registry_handler_wrapper(dap_chain_datum_decree_t *a_decree, dap_chain_net_t *a_net,
                                             bool a_apply, bool a_anchored) {
    if (!a_decree || !a_net) {
        log_it(L_ERROR, "Invalid decree registry handler arguments");
        return -1;
    }

    PyObject *callback = cf_datum_decree_handler_get_callback(a_decree->header.type, a_decree->header.sub_type);
    if (!callback) {
        log_it(L_ERROR, "No Python decree handler registered for type=%u subtype=%u",
               a_decree->header.type, a_decree->header.sub_type);
        return -404;
    }

    PyGILState_STATE gstate = PyGILState_Ensure();

    PyObject *decree_capsule = PyCapsule_New(a_decree, "dap_chain_datum_decree_t", NULL);
    PyObject *net_capsule = PyCapsule_New(a_net, "dap_chain_net_t", NULL);
    PyObject *apply_obj = PyBool_FromLong(a_apply ? 1 : 0);
    PyObject *anchored_obj = PyBool_FromLong(a_anchored ? 1 : 0);

    if (!decree_capsule || !net_capsule || !apply_obj || !anchored_obj) {
        Py_XDECREF(decree_capsule);
        Py_XDECREF(net_capsule);
        Py_XDECREF(apply_obj);
        Py_XDECREF(anchored_obj);
        Py_DECREF(callback);
        PyGILState_Release(gstate);
        return -1;
    }

    PyObject *result = PyObject_CallFunctionObjArgs(
        callback, decree_capsule, net_capsule, apply_obj, anchored_obj, NULL
    );

    Py_DECREF(decree_capsule);
    Py_DECREF(net_capsule);
    Py_DECREF(apply_obj);
    Py_DECREF(anchored_obj);
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
PyObject *dap_chain_decree_registry_deinit_py(PyObject *self, PyObject *args) {
    (void)self;
    (void)args;

    dap_chain_decree_registry_deinit();
    cf_datum_decree_handlers_clear();
    Py_RETURN_NONE;
}

PyObject *dap_chain_decree_registry_has_handler_py(PyObject *self, PyObject *args) {
    (void)self;
    unsigned int decree_type = 0;
    unsigned int decree_subtype = 0;

    if (!PyArg_ParseTuple(args, "II", &decree_type, &decree_subtype)) {
        return NULL;
    }

    bool has = dap_chain_decree_registry_has_handler((uint16_t)decree_type, (uint16_t)decree_subtype);
    return PyBool_FromLong(has ? 1 : 0);
}

PyObject *dap_chain_decree_registry_init_py(PyObject *self, PyObject *args) {
    (void)self;
    (void)args;

    int rc = dap_chain_decree_registry_init();
    return PyLong_FromLong(rc);
}

PyObject *dap_chain_decree_registry_process_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *decree_obj = NULL;
    PyObject *net_obj = NULL;
    int apply = 0;
    int anchored = 0;

    if (!PyArg_ParseTuple(args, "OOpp", &decree_obj, &net_obj, &apply, &anchored)) {
        return NULL;
    }

    dap_chain_datum_decree_t *decree = (dap_chain_datum_decree_t *)cf_datum_get_capsule(
        decree_obj, "dap_chain_datum_decree_t", "First argument must be a decree capsule"
    );
    if (!decree) {
        return NULL;
    }

    dap_chain_net_t *net = (dap_chain_net_t *)cf_datum_get_capsule(
        net_obj, "dap_chain_net_t", "Second argument must be a net capsule"
    );
    if (!net) {
        return NULL;
    }

    int rc = dap_chain_decree_registry_process(decree, net, apply != 0, anchored != 0);
    return PyLong_FromLong(rc);
}

PyObject *dap_chain_decree_registry_register_handler_py(PyObject *self, PyObject *args) {
    (void)self;
    unsigned int decree_type = 0;
    unsigned int decree_subtype = 0;
    PyObject *callback = NULL;
    const char *handler_name = NULL;

    if (!PyArg_ParseTuple(args, "IIOs", &decree_type, &decree_subtype, &callback, &handler_name)) {
        return NULL;
    }

    if (!PyCallable_Check(callback)) {
        PyErr_SetString(PyExc_TypeError, "Handler must be callable");
        return NULL;
    }

    python_decree_handler_ctx_t *ctx = DAP_NEW_Z(python_decree_handler_ctx_t);
    if (!ctx) {
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate handler context");
        return NULL;
    }

    ctx->decree_type = (uint16_t)decree_type;
    ctx->decree_subtype = (uint16_t)decree_subtype;
    ctx->callback = callback;
    ctx->handler_name = dap_strdup(handler_name);
    if (!ctx->handler_name) {
        DAP_DELETE(ctx);
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate handler name");
        return NULL;
    }

    int rc = dap_chain_decree_registry_register_handler(
        ctx->decree_type, ctx->decree_subtype, s_decree_registry_handler_wrapper, ctx->handler_name
    );
    if (rc != 0) {
        DAP_DELETE(ctx->handler_name);
        DAP_DELETE(ctx);
        return PyLong_FromLong(rc);
    }

    pthread_mutex_lock(&s_decree_handlers_mutex);
    python_decree_handler_ctx_t **tmp = DAP_REALLOC(
        s_decree_handlers, (s_decree_handlers_count + 1) * sizeof(python_decree_handler_ctx_t *)
    );
    if (!tmp) {
        pthread_mutex_unlock(&s_decree_handlers_mutex);
        dap_chain_decree_registry_unregister_handler(ctx->decree_type, ctx->decree_subtype);
        DAP_DELETE(ctx->handler_name);
        DAP_DELETE(ctx);
        PyErr_SetString(PyExc_MemoryError, "Failed to grow handler registry");
        return NULL;
    }
    s_decree_handlers = tmp;
    s_decree_handlers[s_decree_handlers_count++] = ctx;
    pthread_mutex_unlock(&s_decree_handlers_mutex);

    if (cf_callbacks_registry_add(CF_CALLBACK_TYPE_DECREE_REGISTRY, callback, NULL, ctx, NULL) != 0) {
        log_it(L_WARNING, "Failed to register decree handler in callbacks registry");
    }

    return PyLong_FromLong(rc);
}

PyObject *dap_chain_decree_registry_unregister_handler_py(PyObject *self, PyObject *args) {
    (void)self;
    unsigned int decree_type = 0;
    unsigned int decree_subtype = 0;

    if (!PyArg_ParseTuple(args, "II", &decree_type, &decree_subtype)) {
        return NULL;
    }

    int rc = dap_chain_decree_registry_unregister_handler((uint16_t)decree_type, (uint16_t)decree_subtype);
    if (rc != 0) {
        return PyLong_FromLong(rc);
    }

    python_decree_handler_ctx_t *ctx = NULL;
    pthread_mutex_lock(&s_decree_handlers_mutex);
    size_t index = 0;
    ctx = cf_datum_decree_handler_find((uint16_t)decree_type, (uint16_t)decree_subtype, &index);
    if (ctx) {
        for (size_t i = index + 1; i < s_decree_handlers_count; i++) {
            s_decree_handlers[i - 1] = s_decree_handlers[i];
        }
        s_decree_handlers_count--;
        if (s_decree_handlers_count == 0) {
            DAP_DELETE(s_decree_handlers);
            s_decree_handlers = NULL;
        } else {
            python_decree_handler_ctx_t **tmp = DAP_REALLOC(
                s_decree_handlers, s_decree_handlers_count * sizeof(python_decree_handler_ctx_t *)
            );
            if (tmp) {
                s_decree_handlers = tmp;
            }
        }
    }
    pthread_mutex_unlock(&s_decree_handlers_mutex);

    if (ctx) {
        cf_datum_decree_handler_free(ctx);
    }

    return PyLong_FromLong(rc);
}
