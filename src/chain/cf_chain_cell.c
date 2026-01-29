#include "cf_chain_internal.h"
#include "dap_chain_cell.h"

#define LOG_TAG "python_cellframe_chain"

typedef struct cf_chain_cell_capture_ctx {
    dap_chain_t *chain;
    bool released;
    struct cf_chain_cell_capture_ctx *next;
} cf_chain_cell_capture_ctx_t;

static pthread_mutex_t s_cell_capture_lock = PTHREAD_MUTEX_INITIALIZER;
static cf_chain_cell_capture_ctx_t *s_cell_capture_ctxs = NULL;

static void s_cell_capture_ctx_add(cf_chain_cell_capture_ctx_t *ctx) {
    pthread_mutex_lock(&s_cell_capture_lock);
    ctx->next = s_cell_capture_ctxs;
    s_cell_capture_ctxs = ctx;
    pthread_mutex_unlock(&s_cell_capture_lock);
}

static void s_cell_capture_ctx_remove(cf_chain_cell_capture_ctx_t *ctx) {
    pthread_mutex_lock(&s_cell_capture_lock);
    cf_chain_cell_capture_ctx_t *prev = NULL;
    cf_chain_cell_capture_ctx_t *cur = s_cell_capture_ctxs;
    while (cur) {
        if (cur == ctx) {
            if (prev) {
                prev->next = cur->next;
            } else {
                s_cell_capture_ctxs = cur->next;
            }
            break;
        }
        prev = cur;
        cur = cur->next;
    }
    pthread_mutex_unlock(&s_cell_capture_lock);
}

static size_t s_cell_capture_ctx_mark_released(dap_chain_t *chain) {
    size_t count = 0;
    pthread_mutex_lock(&s_cell_capture_lock);
    for (cf_chain_cell_capture_ctx_t *cur = s_cell_capture_ctxs; cur; cur = cur->next) {
        if (cur->chain == chain && !cur->released) {
            cur->released = true;
            count++;
        }
    }
    pthread_mutex_unlock(&s_cell_capture_lock);
    return count;
}

size_t cf_chain_release_cell_captures(dap_chain_t *a_chain) {
    if (!a_chain) {
        return 0;
    }
    size_t l_count = s_cell_capture_ctx_mark_released(a_chain);
    for (size_t i = 0; i < l_count; ++i) {
        dap_chain_cell_remit(a_chain);
    }
    return l_count;
}

static void s_chain_cell_capsule_destructor(PyObject *capsule) {
    cf_chain_cell_capture_ctx_t *ctx = (cf_chain_cell_capture_ctx_t *)PyCapsule_GetContext(capsule);
    if (!ctx) {
        return;
    }
    if (!ctx->released) {
        dap_chain_cell_remit(ctx->chain);
        ctx->released = true;
    }
    s_cell_capture_ctx_remove(ctx);
    DAP_DELETE(ctx);
    PyCapsule_SetContext(capsule, NULL);
}

// =========================================
// CHAIN CELL OPERATIONS
// =========================================

PyObject* dap_chain_cell_init_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
    int l_result = dap_chain_cell_init();
    return PyLong_FromLong(l_result);
}



PyObject* dap_chain_cell_open_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_capsule = NULL;
    PyObject *l_cell_id_obj = Py_None;
    const char *l_mode_str = NULL;
    
    if (!PyArg_ParseTuple(a_args, "OOs", &l_chain_capsule, &l_cell_id_obj, &l_mode_str)) {
        return NULL;
    }
    
    if (!l_mode_str || strlen(l_mode_str) != 1) {
        PyErr_SetString(PyExc_ValueError, "Mode must be a single character");
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_chain_capsule)) {
        PyErr_SetString(PyExc_TypeError, "Expected chain capsule");
        return NULL;
    }
    
    dap_chain_t *l_chain = (dap_chain_t*)PyCapsule_GetPointer(l_chain_capsule, "dap_chain_t");
    if (!l_chain) {
        PyErr_SetString(PyExc_ValueError, "Invalid chain capsule");
        return NULL;
    }

    dap_chain_cell_id_t l_cell_id = {0};
    if (cf_chain_parse_cell_id(l_cell_id_obj, &l_cell_id) != 0) {
        return NULL;
    }

    if (!cf_chain_cell_is_configured(l_chain)) {
        return PyLong_FromLong(-1);
    }
    
    int l_result = dap_chain_cell_open(l_chain, l_cell_id, l_mode_str[0]);
    return PyLong_FromLong(l_result);
}



PyObject* dap_chain_cell_create_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_capsule = NULL;
    PyObject *l_cell_id_obj = Py_None;
    
    if (!PyArg_ParseTuple(a_args, "OO", &l_chain_capsule, &l_cell_id_obj)) {
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_chain_capsule)) {
        PyErr_SetString(PyExc_TypeError, "Expected chain capsule");
        return NULL;
    }
    
    dap_chain_t *l_chain = (dap_chain_t*)PyCapsule_GetPointer(l_chain_capsule, "dap_chain_t");
    if (!l_chain) {
        PyErr_SetString(PyExc_ValueError, "Invalid chain capsule");
        return NULL;
    }

    dap_chain_cell_id_t l_cell_id = {0};
    if (cf_chain_parse_cell_id(l_cell_id_obj, &l_cell_id) != 0) {
        return NULL;
    }

    if (!cf_chain_cell_is_configured(l_chain)) {
        return PyLong_FromLong(0);
    }
    
    int l_result = dap_chain_cell_create(l_chain, l_cell_id);
    return PyLong_FromLong(l_result);
}



PyObject* dap_chain_cell_find_by_id_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_capsule = NULL;
    PyObject *l_cell_id_obj = Py_None;
    
    if (!PyArg_ParseTuple(a_args, "OO", &l_chain_capsule, &l_cell_id_obj)) {
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_chain_capsule)) {
        PyErr_SetString(PyExc_TypeError, "Expected chain capsule");
        return NULL;
    }
    
    dap_chain_t *l_chain = (dap_chain_t*)PyCapsule_GetPointer(l_chain_capsule, "dap_chain_t");
    if (!l_chain) {
        PyErr_SetString(PyExc_ValueError, "Invalid chain capsule");
        return NULL;
    }
    
    dap_chain_cell_id_t l_cell_id = {0};
    if (cf_chain_parse_cell_id(l_cell_id_obj, &l_cell_id) != 0) {
        return NULL;
    }
    
    dap_chain_cell_t *l_cell = dap_chain_cell_find_by_id(l_chain, l_cell_id);
    if (!l_cell) {
        Py_RETURN_NONE;
    }
    
    return PyCapsule_New(l_cell, "dap_chain_cell_t", NULL);
}



PyObject* dap_chain_cell_capture_by_id_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_capsule = NULL;
    PyObject *l_cell_id_obj = Py_None;
    
    if (!PyArg_ParseTuple(a_args, "OO", &l_chain_capsule, &l_cell_id_obj)) {
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_chain_capsule)) {
        PyErr_SetString(PyExc_TypeError, "Expected chain capsule");
        return NULL;
    }
    
    dap_chain_t *l_chain = (dap_chain_t*)PyCapsule_GetPointer(l_chain_capsule, "dap_chain_t");
    if (!l_chain) {
        PyErr_SetString(PyExc_ValueError, "Invalid chain capsule");
        return NULL;
    }
    
    dap_chain_cell_id_t l_cell_id = {0};
    if (cf_chain_parse_cell_id(l_cell_id_obj, &l_cell_id) != 0) {
        return NULL;
    }
    
    dap_chain_cell_t *l_cell = dap_chain_cell_capture_by_id(l_chain, l_cell_id);
    if (!l_cell) {
        dap_chain_cell_remit(l_chain);
        Py_RETURN_NONE;
    }

    cf_chain_cell_capture_ctx_t *l_ctx = DAP_NEW_Z(cf_chain_cell_capture_ctx_t);
    if (!l_ctx) {
        dap_chain_cell_remit(l_chain);
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate cell capture context");
        return NULL;
    }
    l_ctx->chain = l_chain;
    l_ctx->released = false;
    s_cell_capture_ctx_add(l_ctx);
    
    PyObject *l_capsule = PyCapsule_New(l_cell, "dap_chain_cell_t", s_chain_cell_capsule_destructor);
    if (!l_capsule) {
        s_cell_capture_ctx_remove(l_ctx);
        DAP_DELETE(l_ctx);
        dap_chain_cell_remit(l_chain);
        return NULL;
    }
    if (PyCapsule_SetContext(l_capsule, l_ctx) != 0) {
        s_cell_capture_ctx_remove(l_ctx);
        DAP_DELETE(l_ctx);
        dap_chain_cell_remit(l_chain);
        Py_DECREF(l_capsule);
        return NULL;
    }
    
    return l_capsule;
}



PyObject* dap_chain_cell_remit_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_obj = NULL;
    
    if (!PyArg_ParseTuple(a_args, "O", &l_obj)) {
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_obj)) {
        PyErr_SetString(PyExc_TypeError, "Expected chain or cell capsule");
        return NULL;
    }

    if (PyCapsule_IsValid(l_obj, "dap_chain_cell_t")) {
        cf_chain_cell_capture_ctx_t *l_ctx = (cf_chain_cell_capture_ctx_t *)PyCapsule_GetContext(l_obj);
        if (!l_ctx) {
            Py_RETURN_NONE;
        }
        if (!l_ctx->released) {
            dap_chain_cell_remit(l_ctx->chain);
            l_ctx->released = true;
        }
        s_cell_capture_ctx_remove(l_ctx);
        DAP_DELETE(l_ctx);
        PyCapsule_SetContext(l_obj, NULL);
        Py_RETURN_NONE;
    }

    if (!PyCapsule_IsValid(l_obj, "dap_chain_t")) {
        PyErr_SetString(PyExc_TypeError, "Expected chain or cell capsule");
        return NULL;
    }
    
    dap_chain_t *l_chain = (dap_chain_t*)PyCapsule_GetPointer(l_obj, "dap_chain_t");
    if (!l_chain) {
        PyErr_SetString(PyExc_ValueError, "Invalid chain capsule");
        return NULL;
    }
    
    size_t l_count = s_cell_capture_ctx_mark_released(l_chain);
    for (size_t i = 0; i < l_count; ++i) {
        dap_chain_cell_remit(l_chain);
    }
    Py_RETURN_NONE;
}



PyObject* dap_chain_cell_close_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_capsule = NULL;
    PyObject *l_cell_id_obj = Py_None;
    
    if (!PyArg_ParseTuple(a_args, "OO", &l_chain_capsule, &l_cell_id_obj)) {
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_chain_capsule)) {
        PyErr_SetString(PyExc_TypeError, "Expected chain capsule");
        return NULL;
    }
    
    dap_chain_t *l_chain = (dap_chain_t*)PyCapsule_GetPointer(l_chain_capsule, "dap_chain_t");
    if (!l_chain) {
        PyErr_SetString(PyExc_ValueError, "Invalid chain capsule");
        return NULL;
    }
    
    dap_chain_cell_id_t l_cell_id = {0};
    if (cf_chain_parse_cell_id(l_cell_id_obj, &l_cell_id) != 0) {
        return NULL;
    }
    
    dap_chain_cell_close(l_chain, l_cell_id);
    Py_RETURN_NONE;
}



PyObject* dap_chain_cell_close_all_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_capsule = NULL;
    
    if (!PyArg_ParseTuple(a_args, "O", &l_chain_capsule)) {
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_chain_capsule)) {
        PyErr_SetString(PyExc_TypeError, "Expected chain capsule");
        return NULL;
    }
    
    dap_chain_t *l_chain = (dap_chain_t*)PyCapsule_GetPointer(l_chain_capsule, "dap_chain_t");
    if (!l_chain) {
        PyErr_SetString(PyExc_ValueError, "Invalid chain capsule");
        return NULL;
    }
    
    dap_chain_cell_close_all(l_chain);
    Py_RETURN_NONE;
}



PyObject* dap_chain_cell_file_append_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_capsule = NULL;
    PyObject *l_cell_id_obj = Py_None;
    const char *l_atom_bytes = NULL;
    Py_ssize_t l_atom_size = 0;
    
    if (!PyArg_ParseTuple(a_args, "OOy#", &l_chain_capsule, &l_cell_id_obj, &l_atom_bytes, &l_atom_size)) {
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_chain_capsule)) {
        PyErr_SetString(PyExc_TypeError, "Expected chain capsule");
        return NULL;
    }
    
    dap_chain_t *l_chain = (dap_chain_t*)PyCapsule_GetPointer(l_chain_capsule, "dap_chain_t");
    if (!l_chain) {
        PyErr_SetString(PyExc_ValueError, "Invalid chain capsule");
        return NULL;
    }
    
    dap_chain_cell_id_t l_cell_id = {0};
    if (cf_chain_parse_cell_id(l_cell_id_obj, &l_cell_id) != 0) {
        return NULL;
    }
    
    char *l_atom_map = NULL;
    int l_result = dap_chain_cell_file_append(l_chain, l_cell_id, l_atom_bytes, (size_t)l_atom_size, &l_atom_map);
    
    return PyLong_FromLong(l_result);
}



PyObject* dap_chain_cell_remove_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_capsule = NULL;
    PyObject *l_cell_id_obj = Py_None;
    int l_archivate = 0;
    
    if (!PyArg_ParseTuple(a_args, "OOp", &l_chain_capsule, &l_cell_id_obj, &l_archivate)) {
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_chain_capsule)) {
        PyErr_SetString(PyExc_TypeError, "Expected chain capsule");
        return NULL;
    }
    
    dap_chain_t *l_chain = (dap_chain_t*)PyCapsule_GetPointer(l_chain_capsule, "dap_chain_t");
    if (!l_chain) {
        PyErr_SetString(PyExc_ValueError, "Invalid chain capsule");
        return NULL;
    }
    
    dap_chain_cell_id_t l_cell_id = {0};
    if (cf_chain_parse_cell_id(l_cell_id_obj, &l_cell_id) != 0) {
        return NULL;
    }
    
    int l_result = dap_chain_cell_remove(l_chain, l_cell_id, l_archivate != 0);
    return PyLong_FromLong(l_result);
}



PyObject* dap_chain_cell_truncate_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_chain_capsule = NULL;
    PyObject *l_cell_id_obj = Py_None;
    Py_ssize_t l_delta = 0;
    
    if (!PyArg_ParseTuple(a_args, "OOn", &l_chain_capsule, &l_cell_id_obj, &l_delta)) {
        return NULL;
    }
    
    if (l_delta < 0) {
        PyErr_SetString(PyExc_ValueError, "Delta must be non-negative");
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_chain_capsule)) {
        PyErr_SetString(PyExc_TypeError, "Expected chain capsule");
        return NULL;
    }
    
    dap_chain_t *l_chain = (dap_chain_t*)PyCapsule_GetPointer(l_chain_capsule, "dap_chain_t");
    if (!l_chain) {
        PyErr_SetString(PyExc_ValueError, "Invalid chain capsule");
        return NULL;
    }
    
    dap_chain_cell_id_t l_cell_id = {0};
    if (cf_chain_parse_cell_id(l_cell_id_obj, &l_cell_id) != 0) {
        return NULL;
    }
    
    int l_result = dap_chain_cell_truncate(l_chain, l_cell_id, (size_t)l_delta);
    return PyLong_FromLong(l_result);
}



PyObject* dap_chain_cell_set_load_skip_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
    dap_chain_cell_set_load_skip();
    Py_RETURN_NONE;
}

// =========================================
// CHAIN CH OPERATIONS
// =========================================
