#include "include/cf_ledger_internal.h"
#include "dap_chain_datum.h"
#include "dap_json.h"

/**
 * @brief Dump datum to JSON (Python binding)
 * @param a_self Python self object (unused)
 * @param a_args Arguments (json_arr capsule or None, json_out capsule, datum capsule, hash_out_type, net_id, verbose, version)
 * @return None
 */
PyObject* dap_chain_datum_dump_json_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_json_arr_obj = NULL;
    PyObject *l_json_out_obj = NULL;
    PyObject *l_datum_obj = NULL;
    const char *l_hash_out_type = NULL;
    unsigned long long l_net_id = 0;
    int l_verbose = 0;
    int l_version = 0;

    if (!PyArg_ParseTuple(a_args, "OOOsKpi", &l_json_arr_obj, &l_json_out_obj, &l_datum_obj,
                          &l_hash_out_type, &l_net_id, &l_verbose, &l_version)) {
        return NULL;
    }

    dap_json_t *l_json_arr = NULL;
    if (l_json_arr_obj && l_json_arr_obj != Py_None) {
        if (!PyCapsule_CheckExact(l_json_arr_obj)) {
            PyErr_SetString(PyExc_TypeError, "First argument must be a JSON capsule or None");
            return NULL;
        }
        l_json_arr = (dap_json_t *)PyCapsule_GetPointer(l_json_arr_obj, "dap_json_t");
        if (!l_json_arr) {
            PyErr_SetString(PyExc_ValueError, "Invalid JSON capsule");
            return NULL;
        }
    }

    if (!PyCapsule_CheckExact(l_json_out_obj)) {
        PyErr_SetString(PyExc_TypeError, "Second argument must be a JSON capsule");
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_datum_obj)) {
        PyErr_SetString(PyExc_TypeError, "Third argument must be a datum capsule");
        return NULL;
    }

    dap_json_t *l_json_out = (dap_json_t *)PyCapsule_GetPointer(l_json_out_obj, "dap_json_t");
    dap_chain_datum_t *l_datum = (dap_chain_datum_t *)PyCapsule_GetPointer(l_datum_obj, "dap_chain_datum_t");
    if (!l_json_out || !l_datum) {
        PyErr_SetString(PyExc_ValueError, "Invalid JSON or datum capsule");
        return NULL;
    }

    dap_chain_net_id_t l_net_id_struct = {.uint64 = l_net_id};
    dap_chain_datum_dump_json(l_json_arr, l_json_out, l_datum, l_hash_out_type,
                              l_net_id_struct, l_verbose != 0, l_version);
    Py_RETURN_NONE;
}

/**
 * @brief Get list of TX outputs from JSON
 * @param a_self Python self object (unused)
 * @param a_args Arguments (json capsule, outputs_count, value_need bytes, need_all_outputs bool)
 * @return PyCapsule wrapping dap_list_t* or None
 */
PyObject* dap_ledger_get_list_tx_outs_from_json_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_json_obj;
    int l_outputs_count;
    const char *l_value_need_bytes;
    Py_ssize_t l_value_need_size;
    int l_need_all_outputs;

    if (!PyArg_ParseTuple(a_args, "Ois#p", &l_json_obj, &l_outputs_count,
                          &l_value_need_bytes, &l_value_need_size, &l_need_all_outputs)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_json_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a JSON capsule");
        return NULL;
    }

    if ((size_t)l_value_need_size != sizeof(uint256_t)) {
        PyErr_Format(PyExc_ValueError, "value_need must be exactly %zu bytes", sizeof(uint256_t));
        return NULL;
    }

    dap_json_t *l_json = (dap_json_t *)PyCapsule_GetPointer(l_json_obj, "dap_json_t");
    if (!l_json) {
        PyErr_SetString(PyExc_ValueError, "Invalid JSON capsule");
        return NULL;
    }

    uint256_t l_value_need = *(uint256_t *)l_value_need_bytes;
    uint256_t l_value_transfer = uint256_0;

    dap_list_t *l_list = dap_ledger_get_list_tx_outs_from_json(l_json, l_outputs_count,
                                                                l_value_need, &l_value_transfer,
                                                                (bool)l_need_all_outputs);
    if (!l_list) {
        log_it(L_DEBUG, "No TX outs from JSON");
        Py_RETURN_NONE;
    }

    log_it(L_DEBUG, "Got TX outs list from JSON (need_all=%d)", l_need_all_outputs);
    return PyCapsule_New(l_list, "dap_list_t", NULL);
}

PyMethodDef* cellframe_ledger_json_get_methods(void) {
    static PyMethodDef methods[] = {
        {"dap_chain_datum_dump_json", (PyCFunction)dap_chain_datum_dump_json_py, METH_VARARGS,
         "Dump datum to JSON using ledger context"},
        {"ledger_get_list_tx_outs_from_json", (PyCFunction)dap_ledger_get_list_tx_outs_from_json_py, METH_VARARGS,
         "Get list of TX outputs from JSON"},
        {NULL, NULL, 0, NULL}
    };
    return methods;
}
