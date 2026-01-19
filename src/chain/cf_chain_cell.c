#include "cf_chain_internal.h"
#include "dap_chain_cell.h"

#define LOG_TAG "python_cellframe_chain"

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
    
    PyObject *l_capsule = PyCapsule_New(l_cell, "dap_chain_cell_t", NULL);
    if (!l_capsule) {
        dap_chain_cell_remit(l_chain);
        return NULL;
    }
    
    return l_capsule;
}



PyObject* dap_chain_cell_remit_py(PyObject *a_self, PyObject *a_args) {
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
    
    dap_chain_cell_remit(l_chain);
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
    if (l_atom_map) {
        DAP_DELETE(l_atom_map);
    }
    
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
