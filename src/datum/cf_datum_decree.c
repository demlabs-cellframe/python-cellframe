#include "cf_datum_internal.h"
#include "dap_chain_datum_decree.h"
#include "dap_json.h"
#include "dap_pkey.h"
#include "dap_sign.h"

static PyObject *cf_datum_make_rc_bytes_tuple(int rc, const void *data, size_t data_size) {
    PyObject *tuple = PyTuple_New(2);
    if (!tuple) {
        return NULL;
    }
    PyTuple_SET_ITEM(tuple, 0, PyLong_FromLong(rc));
    if (rc != 0) {
        Py_INCREF(Py_None);
        PyTuple_SET_ITEM(tuple, 1, Py_None);
        return tuple;
    }
    PyObject *bytes_obj = PyBytes_FromStringAndSize((const char *)data, (Py_ssize_t)data_size);
    if (!bytes_obj) {
        Py_DECREF(tuple);
        return NULL;
    }
    PyTuple_SET_ITEM(tuple, 1, bytes_obj);
    return tuple;
}
PyObject *dap_chain_datum_decree_certs_dump_json_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *json_obj = NULL;
    const char *signs = NULL;
    Py_ssize_t signs_size = 0;
    const char *hash_out_type = NULL;
    int version = 0;

    if (!PyArg_ParseTuple(args, "Oy#si", &json_obj, &signs, &signs_size, &hash_out_type, &version)) {
        return NULL;
    }

    dap_json_t *json = (dap_json_t *)cf_datum_get_capsule(
        json_obj, "dap_json_t", "First argument must be a JSON capsule"
    );
    if (!json) {
        return NULL;
    }

    dap_chain_datum_decree_certs_dump_json(json, (byte_t *)signs, (size_t)signs_size, hash_out_type, version);
    Py_RETURN_NONE;
}

PyObject *dap_chain_datum_decree_dump_json_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *json_obj = NULL;
    PyObject *decree_obj = NULL;
    Py_ssize_t decree_size = 0;
    const char *hash_out_type = NULL;
    int version = 0;

    if (!PyArg_ParseTuple(args, "OOnsi", &json_obj, &decree_obj, &decree_size, &hash_out_type, &version)) {
        return NULL;
    }
    if (decree_size < 0) {
        PyErr_SetString(PyExc_ValueError, "decree_size must be non-negative");
        return NULL;
    }

    dap_json_t *json = (dap_json_t *)cf_datum_get_capsule(
        json_obj, "dap_json_t", "First argument must be a JSON capsule"
    );
    if (!json) {
        return NULL;
    }

    dap_chain_datum_decree_t *decree = (dap_chain_datum_decree_t *)cf_datum_get_capsule(
        decree_obj, "dap_chain_datum_decree_t", "Second argument must be a decree capsule"
    );
    if (!decree) {
        return NULL;
    }

    dap_chain_datum_decree_dump_json(json, decree, (size_t)decree_size, hash_out_type, version);
    Py_RETURN_NONE;
}

PyObject *dap_chain_datum_decree_get_action_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *decree_obj = NULL;

    if (!PyArg_ParseTuple(args, "O", &decree_obj)) {
        return NULL;
    }

    dap_chain_datum_decree_t *decree = (dap_chain_datum_decree_t *)cf_datum_get_capsule(
        decree_obj, "dap_chain_datum_decree_t", "First argument must be a decree capsule"
    );
    if (!decree) {
        return NULL;
    }

    uint8_t action = 0;
    int rc = dap_chain_datum_decree_get_action(decree, &action);

    PyObject *tuple = PyTuple_New(2);
    if (!tuple) {
        return NULL;
    }
    PyTuple_SET_ITEM(tuple, 0, PyLong_FromLong(rc));
    if (rc != 0) {
        Py_INCREF(Py_None);
        PyTuple_SET_ITEM(tuple, 1, Py_None);
        return tuple;
    }
    PyTuple_SET_ITEM(tuple, 1, PyLong_FromUnsignedLong((unsigned long)action));
    return tuple;
}

PyObject *dap_chain_datum_decree_get_atom_num_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *decree_obj = NULL;

    if (!PyArg_ParseTuple(args, "O", &decree_obj)) {
        return NULL;
    }

    dap_chain_datum_decree_t *decree = (dap_chain_datum_decree_t *)cf_datum_get_capsule(
        decree_obj, "dap_chain_datum_decree_t", "First argument must be a decree capsule"
    );
    if (!decree) {
        return NULL;
    }

    uint64_t atom_num = 0;
    int rc = dap_chain_datum_decree_get_atom_num(decree, &atom_num);

    PyObject *tuple = PyTuple_New(2);
    if (!tuple) {
        return NULL;
    }
    PyTuple_SET_ITEM(tuple, 0, PyLong_FromLong(rc));
    if (rc != 0) {
        Py_INCREF(Py_None);
        PyTuple_SET_ITEM(tuple, 1, Py_None);
        return tuple;
    }
    PyTuple_SET_ITEM(tuple, 1, PyLong_FromUnsignedLongLong(atom_num));
    return tuple;
}

PyObject *dap_chain_datum_decree_get_ban_addr_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *decree_obj = NULL;

    if (!PyArg_ParseTuple(args, "O", &decree_obj)) {
        return NULL;
    }

    dap_chain_datum_decree_t *decree = (dap_chain_datum_decree_t *)cf_datum_get_capsule(
        decree_obj, "dap_chain_datum_decree_t", "First argument must be a decree capsule"
    );
    if (!decree) {
        return NULL;
    }

    const char *addr = NULL;
    int rc = dap_chain_datum_decree_get_ban_addr(decree, &addr);

    PyObject *tuple = PyTuple_New(2);
    if (!tuple) {
        return NULL;
    }
    PyTuple_SET_ITEM(tuple, 0, PyLong_FromLong(rc));
    if (rc != 0 || !addr) {
        Py_INCREF(Py_None);
        PyTuple_SET_ITEM(tuple, 1, Py_None);
        return tuple;
    }
    PyTuple_SET_ITEM(tuple, 1, PyUnicode_FromString(addr));
    return tuple;
}

PyObject *dap_chain_datum_decree_get_empty_block_every_times_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *decree_obj = NULL;

    if (!PyArg_ParseTuple(args, "O", &decree_obj)) {
        return NULL;
    }

    dap_chain_datum_decree_t *decree = (dap_chain_datum_decree_t *)cf_datum_get_capsule(
        decree_obj, "dap_chain_datum_decree_t", "First argument must be a decree capsule"
    );
    if (!decree) {
        return NULL;
    }

    uint16_t blockgen_period = 0;
    int rc = dap_chain_datum_decree_get_empty_block_every_times(decree, &blockgen_period);

    PyObject *tuple = PyTuple_New(2);
    if (!tuple) {
        return NULL;
    }
    PyTuple_SET_ITEM(tuple, 0, PyLong_FromLong(rc));
    if (rc != 0) {
        Py_INCREF(Py_None);
        PyTuple_SET_ITEM(tuple, 1, Py_None);
        return tuple;
    }
    PyTuple_SET_ITEM(tuple, 1, PyLong_FromUnsignedLong((unsigned long)blockgen_period));
    return tuple;
}

PyObject *dap_chain_datum_decree_get_fee_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *decree_obj = NULL;

    if (!PyArg_ParseTuple(args, "O", &decree_obj)) {
        return NULL;
    }

    dap_chain_datum_decree_t *decree = (dap_chain_datum_decree_t *)cf_datum_get_capsule(
        decree_obj, "dap_chain_datum_decree_t", "First argument must be a decree capsule"
    );
    if (!decree) {
        return NULL;
    }

    uint256_t fee_value = {0};
    int rc = dap_chain_datum_decree_get_fee(decree, &fee_value);
    return cf_datum_make_rc_bytes_tuple(rc, &fee_value, sizeof(uint256_t));
}

PyObject *dap_chain_datum_decree_get_fee_addr_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *decree_obj = NULL;

    if (!PyArg_ParseTuple(args, "O", &decree_obj)) {
        return NULL;
    }

    dap_chain_datum_decree_t *decree = (dap_chain_datum_decree_t *)cf_datum_get_capsule(
        decree_obj, "dap_chain_datum_decree_t", "First argument must be a decree capsule"
    );
    if (!decree) {
        return NULL;
    }

    dap_chain_addr_t fee_addr = {0};
    int rc = dap_chain_datum_decree_get_fee_addr(decree, &fee_addr);

    PyObject *tuple = PyTuple_New(2);
    if (!tuple) {
        return NULL;
    }
    PyTuple_SET_ITEM(tuple, 0, PyLong_FromLong(rc));
    if (rc != 0) {
        Py_INCREF(Py_None);
        PyTuple_SET_ITEM(tuple, 1, Py_None);
        return tuple;
    }

    dap_chain_addr_t *addr_copy = DAP_NEW_Z(dap_chain_addr_t);
    if (!addr_copy) {
        Py_DECREF(tuple);
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate address");
        return NULL;
    }
    *addr_copy = fee_addr;

    PyObject *capsule = PyCapsule_New(addr_copy, "dap_chain_addr_t", NULL);
    if (!capsule) {
        DAP_DELETE(addr_copy);
        Py_DECREF(tuple);
        return NULL;
    }
    PyTuple_SET_ITEM(tuple, 1, capsule);
    return tuple;
}

PyObject *dap_chain_datum_decree_get_hash_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *decree_obj = NULL;

    if (!PyArg_ParseTuple(args, "O", &decree_obj)) {
        return NULL;
    }

    dap_chain_datum_decree_t *decree = (dap_chain_datum_decree_t *)cf_datum_get_capsule(
        decree_obj, "dap_chain_datum_decree_t", "First argument must be a decree capsule"
    );
    if (!decree) {
        return NULL;
    }

    dap_hash_fast_t hash = {0};
    int rc = dap_chain_datum_decree_get_hash(decree, &hash);
    return cf_datum_make_rc_bytes_tuple(rc, &hash, sizeof(dap_hash_fast_t));
}

PyObject *dap_chain_datum_decree_get_min_owners_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *decree_obj = NULL;

    if (!PyArg_ParseTuple(args, "O", &decree_obj)) {
        return NULL;
    }

    dap_chain_datum_decree_t *decree = (dap_chain_datum_decree_t *)cf_datum_get_capsule(
        decree_obj, "dap_chain_datum_decree_t", "First argument must be a decree capsule"
    );
    if (!decree) {
        return NULL;
    }

    uint256_t min_owners = {0};
    int rc = dap_chain_datum_decree_get_min_owners(decree, &min_owners);
    return cf_datum_make_rc_bytes_tuple(rc, &min_owners, sizeof(uint256_t));
}

PyObject *dap_chain_datum_decree_get_node_addr_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *decree_obj = NULL;

    if (!PyArg_ParseTuple(args, "O", &decree_obj)) {
        return NULL;
    }

    dap_chain_datum_decree_t *decree = (dap_chain_datum_decree_t *)cf_datum_get_capsule(
        decree_obj, "dap_chain_datum_decree_t", "First argument must be a decree capsule"
    );
    if (!decree) {
        return NULL;
    }

    dap_chain_node_addr_t node_addr = {0};
    int rc = dap_chain_datum_decree_get_node_addr(decree, &node_addr);

    PyObject *tuple = PyTuple_New(2);
    if (!tuple) {
        return NULL;
    }
    PyTuple_SET_ITEM(tuple, 0, PyLong_FromLong(rc));
    if (rc != 0) {
        Py_INCREF(Py_None);
        PyTuple_SET_ITEM(tuple, 1, Py_None);
        return tuple;
    }

    dap_chain_node_addr_t *addr_copy = DAP_NEW_Z(dap_chain_node_addr_t);
    if (!addr_copy) {
        Py_DECREF(tuple);
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate node address");
        return NULL;
    }
    *addr_copy = node_addr;

    PyObject *capsule = PyCapsule_New(addr_copy, "dap_chain_node_addr_t", NULL);
    if (!capsule) {
        DAP_DELETE(addr_copy);
        Py_DECREF(tuple);
        return NULL;
    }
    PyTuple_SET_ITEM(tuple, 1, capsule);
    return tuple;
}

PyObject *dap_chain_datum_decree_get_owners_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *decree_obj = NULL;

    if (!PyArg_ParseTuple(args, "O", &decree_obj)) {
        return NULL;
    }

    dap_chain_datum_decree_t *decree = (dap_chain_datum_decree_t *)cf_datum_get_capsule(
        decree_obj, "dap_chain_datum_decree_t", "First argument must be a decree capsule"
    );
    if (!decree) {
        return NULL;
    }

    uint16_t owners_num = 0;
    dap_list_t *owners = dap_chain_datum_decree_get_owners(decree, &owners_num);

    PyObject *tuple = PyTuple_New(2);
    if (!tuple) {
        return NULL;
    }

    if (!owners) {
        Py_INCREF(Py_None);
        PyTuple_SET_ITEM(tuple, 0, Py_None);
    } else {
        PyObject *capsule = PyCapsule_New(owners, "dap_list_t", NULL);
        if (!capsule) {
            Py_DECREF(tuple);
            return NULL;
        }
        PyTuple_SET_ITEM(tuple, 0, capsule);
    }
    PyTuple_SET_ITEM(tuple, 1, PyLong_FromUnsignedLong((unsigned long)owners_num));
    return tuple;
}

PyObject *dap_chain_datum_decree_get_pkey_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *decree_obj = NULL;

    if (!PyArg_ParseTuple(args, "O", &decree_obj)) {
        return NULL;
    }

    dap_chain_datum_decree_t *decree = (dap_chain_datum_decree_t *)cf_datum_get_capsule(
        decree_obj, "dap_chain_datum_decree_t", "First argument must be a decree capsule"
    );
    if (!decree) {
        return NULL;
    }

    dap_pkey_t *pkey = dap_chain_datum_decree_get_pkey(decree);
    if (!pkey) {
        Py_RETURN_NONE;
    }
    return PyCapsule_New(pkey, "dap_pkey_t", NULL);
}

PyObject *dap_chain_datum_decree_get_policy_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *decree_obj = NULL;

    if (!PyArg_ParseTuple(args, "O", &decree_obj)) {
        return NULL;
    }

    dap_chain_datum_decree_t *decree = (dap_chain_datum_decree_t *)cf_datum_get_capsule(
        decree_obj, "dap_chain_datum_decree_t", "First argument must be a decree capsule"
    );
    if (!decree) {
        return NULL;
    }

    dap_chain_policy_t *policy = dap_chain_datum_decree_get_policy(decree);
    if (!policy) {
        Py_RETURN_NONE;
    }
    return PyCapsule_New(policy, "dap_chain_policy_t", NULL);
}

PyObject *dap_chain_datum_decree_get_signature_type_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *decree_obj = NULL;

    if (!PyArg_ParseTuple(args, "O", &decree_obj)) {
        return NULL;
    }

    dap_chain_datum_decree_t *decree = (dap_chain_datum_decree_t *)cf_datum_get_capsule(
        decree_obj, "dap_chain_datum_decree_t", "First argument must be a decree capsule"
    );
    if (!decree) {
        return NULL;
    }

    uint32_t signature_type = 0;
    int rc = dap_chain_datum_decree_get_signature_type(decree, &signature_type);

    PyObject *tuple = PyTuple_New(2);
    if (!tuple) {
        return NULL;
    }
    PyTuple_SET_ITEM(tuple, 0, PyLong_FromLong(rc));
    if (rc != 0) {
        Py_INCREF(Py_None);
        PyTuple_SET_ITEM(tuple, 1, Py_None);
        return tuple;
    }
    PyTuple_SET_ITEM(tuple, 1, PyLong_FromUnsignedLong((unsigned long)signature_type));
    return tuple;
}

PyObject *dap_chain_datum_decree_get_signs_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *decree_obj = NULL;

    if (!PyArg_ParseTuple(args, "O", &decree_obj)) {
        return NULL;
    }

    dap_chain_datum_decree_t *decree = (dap_chain_datum_decree_t *)cf_datum_get_capsule(
        decree_obj, "dap_chain_datum_decree_t", "First argument must be a decree capsule"
    );
    if (!decree) {
        return NULL;
    }

    size_t signs_size = 0;
    dap_sign_t *signs = dap_chain_datum_decree_get_signs(decree, &signs_size);

    PyObject *tuple = PyTuple_New(2);
    if (!tuple) {
        return NULL;
    }

    PyObject *signs_obj = PyBytes_FromStringAndSize((const char *)signs, (Py_ssize_t)signs_size);
    if (!signs_obj) {
        Py_DECREF(tuple);
        return NULL;
    }

    PyTuple_SET_ITEM(tuple, 0, signs_obj);
    PyTuple_SET_ITEM(tuple, 1, PyLong_FromSize_t(signs_size));
    return tuple;
}

PyObject *dap_chain_datum_decree_get_size_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *decree_obj = NULL;

    if (!PyArg_ParseTuple(args, "O", &decree_obj)) {
        return NULL;
    }

    dap_chain_datum_decree_t *decree = (dap_chain_datum_decree_t *)cf_datum_get_capsule(
        decree_obj, "dap_chain_datum_decree_t", "First argument must be a decree capsule"
    );
    if (!decree) {
        return NULL;
    }

    size_t size = dap_chain_datum_decree_get_size(decree);
    return PyLong_FromUnsignedLongLong((unsigned long long)size);
}

PyObject *dap_chain_datum_decree_get_stake_min_signers_count_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *decree_obj = NULL;

    if (!PyArg_ParseTuple(args, "O", &decree_obj)) {
        return NULL;
    }

    dap_chain_datum_decree_t *decree = (dap_chain_datum_decree_t *)cf_datum_get_capsule(
        decree_obj, "dap_chain_datum_decree_t", "First argument must be a decree capsule"
    );
    if (!decree) {
        return NULL;
    }

    uint256_t min_signers = {0};
    int rc = dap_chain_datum_decree_get_stake_min_signers_count(decree, &min_signers);
    return cf_datum_make_rc_bytes_tuple(rc, &min_signers, sizeof(uint256_t));
}

PyObject *dap_chain_datum_decree_get_stake_min_value_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *decree_obj = NULL;

    if (!PyArg_ParseTuple(args, "O", &decree_obj)) {
        return NULL;
    }

    dap_chain_datum_decree_t *decree = (dap_chain_datum_decree_t *)cf_datum_get_capsule(
        decree_obj, "dap_chain_datum_decree_t", "First argument must be a decree capsule"
    );
    if (!decree) {
        return NULL;
    }

    uint256_t min_value = {0};
    int rc = dap_chain_datum_decree_get_stake_min_value(decree, &min_value);
    return cf_datum_make_rc_bytes_tuple(rc, &min_value, sizeof(uint256_t));
}

PyObject *dap_chain_datum_decree_get_stake_signing_addr_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *decree_obj = NULL;

    if (!PyArg_ParseTuple(args, "O", &decree_obj)) {
        return NULL;
    }

    dap_chain_datum_decree_t *decree = (dap_chain_datum_decree_t *)cf_datum_get_capsule(
        decree_obj, "dap_chain_datum_decree_t", "First argument must be a decree capsule"
    );
    if (!decree) {
        return NULL;
    }

    dap_chain_addr_t signing_addr = {0};
    int rc = dap_chain_datum_decree_get_stake_signing_addr(decree, &signing_addr);

    PyObject *tuple = PyTuple_New(2);
    if (!tuple) {
        return NULL;
    }
    PyTuple_SET_ITEM(tuple, 0, PyLong_FromLong(rc));
    if (rc != 0) {
        Py_INCREF(Py_None);
        PyTuple_SET_ITEM(tuple, 1, Py_None);
        return tuple;
    }

    dap_chain_addr_t *addr_copy = DAP_NEW_Z(dap_chain_addr_t);
    if (!addr_copy) {
        Py_DECREF(tuple);
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate address");
        return NULL;
    }
    *addr_copy = signing_addr;

    PyObject *capsule = PyCapsule_New(addr_copy, "dap_chain_addr_t", NULL);
    if (!capsule) {
        DAP_DELETE(addr_copy);
        Py_DECREF(tuple);
        return NULL;
    }
    PyTuple_SET_ITEM(tuple, 1, capsule);
    return tuple;
}

PyObject *dap_chain_datum_decree_get_stake_value_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *decree_obj = NULL;

    if (!PyArg_ParseTuple(args, "O", &decree_obj)) {
        return NULL;
    }

    dap_chain_datum_decree_t *decree = (dap_chain_datum_decree_t *)cf_datum_get_capsule(
        decree_obj, "dap_chain_datum_decree_t", "First argument must be a decree capsule"
    );
    if (!decree) {
        return NULL;
    }

    uint256_t stake_value = {0};
    int rc = dap_chain_datum_decree_get_stake_value(decree, &stake_value);
    return cf_datum_make_rc_bytes_tuple(rc, &stake_value, sizeof(uint256_t));
}

PyObject *dap_chain_datum_decree_get_value_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *decree_obj = NULL;

    if (!PyArg_ParseTuple(args, "O", &decree_obj)) {
        return NULL;
    }

    dap_chain_datum_decree_t *decree = (dap_chain_datum_decree_t *)cf_datum_get_capsule(
        decree_obj, "dap_chain_datum_decree_t", "First argument must be a decree capsule"
    );
    if (!decree) {
        return NULL;
    }

    uint256_t value = {0};
    int rc = dap_chain_datum_decree_get_value(decree, &value);
    return cf_datum_make_rc_bytes_tuple(rc, &value, sizeof(uint256_t));
}

PyObject *dap_chain_datum_decree_new_py(PyObject *self, PyObject *args) {
    (void)self;
    unsigned long long net_id = 0;
    unsigned long long chain_id = 0;
    unsigned long long cell_id = 0;
    unsigned long long total_tsd_size = 0;

    if (!PyArg_ParseTuple(args, "KKKK", &net_id, &chain_id, &cell_id, &total_tsd_size)) {
        return NULL;
    }

    dap_chain_net_id_t net_id_struct = {.uint64 = net_id};
    dap_chain_id_t chain_id_struct = {.uint64 = chain_id};
    dap_chain_cell_id_t cell_id_struct = {.uint64 = cell_id};

    dap_chain_datum_decree_t *decree = dap_chain_datum_decree_new(
        net_id_struct, chain_id_struct, cell_id_struct, (size_t)total_tsd_size
    );
    if (!decree) {
        PyErr_SetString(PyExc_RuntimeError, "Failed to create decree");
        return NULL;
    }

    return PyCapsule_New(decree, "dap_chain_datum_decree_t", NULL);
}

PyObject *dap_chain_datum_decree_sign_in_cycle_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *certs_obj = NULL;
    PyObject *decree_obj = NULL;

    if (!PyArg_ParseTuple(args, "OO", &certs_obj, &decree_obj)) {
        return NULL;
    }

    dap_chain_datum_decree_t *decree = (dap_chain_datum_decree_t *)cf_datum_get_capsule(
        decree_obj, "dap_chain_datum_decree_t", "Second argument must be a decree capsule"
    );
    if (!decree) {
        return NULL;
    }

    if (!PyList_Check(certs_obj) && !PyTuple_Check(certs_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a list or tuple of certificates");
        return NULL;
    }

    Py_ssize_t certs_count = PySequence_Size(certs_obj);
    if (certs_count < 0) {
        return NULL;
    }

    dap_cert_t **certs = NULL;
    if (certs_count > 0) {
        certs = DAP_NEW_Z_COUNT(dap_cert_t *, (size_t)certs_count);
        if (!certs) {
            PyErr_SetString(PyExc_MemoryError, "Failed to allocate certificates array");
            return NULL;
        }
    }

    for (Py_ssize_t i = 0; i < certs_count; i++) {
        PyObject *item = PySequence_GetItem(certs_obj, i);
        if (!item) {
            DAP_DELETE(certs);
            return NULL;
        }
        if (!PyCapsule_CheckExact(item)) {
            Py_DECREF(item);
            DAP_DELETE(certs);
            PyErr_SetString(PyExc_TypeError, "Certificate must be a dap_cert_t capsule");
            return NULL;
        }
        certs[i] = (dap_cert_t *)PyCapsule_GetPointer(item, "dap_cert_t");
        Py_DECREF(item);
        if (!certs[i]) {
            DAP_DELETE(certs);
            PyErr_SetString(PyExc_ValueError, "Invalid certificate capsule");
            return NULL;
        }
    }

    size_t total_sign_count = 0;
    dap_chain_datum_decree_t *result = dap_chain_datum_decree_sign_in_cycle(
        certs, decree, (size_t)certs_count, &total_sign_count
    );

    DAP_DELETE(certs);

    PyObject *tuple = PyTuple_New(2);
    if (!tuple) {
        return NULL;
    }

    if (!result) {
        Py_INCREF(Py_None);
        PyTuple_SET_ITEM(tuple, 0, Py_None);
        PyTuple_SET_ITEM(tuple, 1, PyLong_FromSize_t(total_sign_count));
        return tuple;
    }

    if (result != decree) {
        PyCapsule_SetPointer(decree_obj, result);
    }

    PyObject *capsule = PyCapsule_New(result, "dap_chain_datum_decree_t", NULL);
    if (!capsule) {
        Py_DECREF(tuple);
        return NULL;
    }

    PyTuple_SET_ITEM(tuple, 0, capsule);
    PyTuple_SET_ITEM(tuple, 1, PyLong_FromSize_t(total_sign_count));
    return tuple;
}

PyObject *dap_chain_datum_decree_subtype_to_str_py(PyObject *self, PyObject *args) {
    (void)self;
    unsigned int subtype = 0;

    if (!PyArg_ParseTuple(args, "I", &subtype)) {
        return NULL;
    }

    const char *str = dap_chain_datum_decree_subtype_to_str((uint16_t)subtype);
    return PyUnicode_FromString(str ? str : "DECREE_SUBTYPE_UNKNOWN");
}

PyObject *dap_chain_datum_decree_tsd_type_to_str_py(PyObject *self, PyObject *args) {
    (void)self;
    unsigned int tsd_type = 0;

    if (!PyArg_ParseTuple(args, "I", &tsd_type)) {
        return NULL;
    }

    const char *str = dap_chain_datum_decree_tsd_type_to_str((uint16_t)tsd_type);
    return PyUnicode_FromString(str ? str : "DECREE_TSD_TYPE_UNKNOWN");
}

PyObject *dap_chain_datum_decree_type_from_str_py(PyObject *self, PyObject *args) {
    (void)self;
    const char *type_str = NULL;

    if (!PyArg_ParseTuple(args, "s", &type_str)) {
        return NULL;
    }

    uint16_t type = dap_chain_datum_decree_type_from_str(type_str);
    return PyLong_FromUnsignedLong((unsigned long)type);
}
