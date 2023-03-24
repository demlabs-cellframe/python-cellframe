#include "wrapping_dap_chain_datum_decree.h"
#include "libdap-python.h"
#include "datetime.h"
#include "wrapping_dap_sign.h"
#include "libdap_chain_net_python.h"
#include "wrapping_cert.h"

#define PVT(a)((PyDapChainDatumDecreeObject*)a)

PyObject* wrapping_dap_chain_datum_decree_get_ts_created(PyObject *self, void* closure) {
    (void)closure;
    PyDateTime_IMPORT;
    PyObject *obj_ts_float = PyLong_FromLong(PVT(self)->decree->header.ts_created);
    PyObject *obj_ts = Py_BuildValue("(O)", obj_ts_float);
    PyObject *obj_dt = PyDateTime_FromTimestamp(obj_ts);
    return obj_dt;
}
PyObject* wrapping_dap_chain_datum_decree_get_type(PyObject *self, void* closure){
    (void)closure;
    return Py_BuildValue("H", PVT(self)->decree->header.type);
}
PyObject* wrapping_dap_chain_datum_decree_get_type_str(PyObject *self, void* closure){
    (void)closure;
    switch(PVT(self)->decree->header.type){
        case DAP_CHAIN_DATUM_DECREE_TYPE_COMMON:
            return Py_BuildValue("s", "DAP_CHAIN_DATUM_DECREE_TYPE_COMMON");
        case DAP_CHAIN_DATUM_DECREE_TYPE_SERVICE:
            return Py_BuildValue("s", "DAP_CHAIN_DATUM_DECREE_TYPE_SERVICE");
    }
    return Py_BuildValue("s", "UNKNOWN");
}
PyObject* wrapping_dap_chain_datum_decree_get_subtype(PyObject *self, void* closure){
    (void)closure;
    return Py_BuildValue("H", PVT(self)->decree->header.sub_type);
}
PyObject* wrapping_dap_chain_datum_decree_get_subtype_str(PyObject *self, void* closure){
    (void)closure;
    switch(PVT(self)->decree->header.sub_type) {
        case DAP_CHAIN_DATUM_DECREE_COMMON_SUBTYPE_FEE:
            return Py_BuildValue("s", "DAP_CHAIN_DATUM_DECREE_COMMON_SUBTYPE_FEE");
        case DAP_CHAIN_DATUM_DECREE_COMMON_SUBTYPE_OWNERS:
            return Py_BuildValue("s", "DAP_CHAIN_DATUM_DECREE_COMMON_SUBTYPE_OWNERS");
        case DAP_CHAIN_DATUM_DECREE_COMMON_SUBTYPE_OWNERS_MIN:
            return Py_BuildValue("s", "DAP_CHAIN_DATUM_DECREE_COMMON_SUBTYPE_OWNERS_MIN");
        case DAP_CHAIN_DATUM_DECREE_COMMON_SUBTYPE_TON_SIGNERS_MIN:
            return Py_BuildValue("s", "DAP_CHAIN_DATUM_DECREE_COMMON_SUBTYPE_TON_SIGNERS_MIN");
        case DAP_CHAIN_DATUM_DECREE_COMMON_SUBTYPE_STAKE_APPROVE:
            return Py_BuildValue("s", "DAP_CHAIN_DATUM_DECREE_COMMON_SUBTYPE_STAKE_APPROVE");
        case DAP_CHAIN_DATUM_DECREE_COMMON_SUBTYPE_STAKE_INVALIDATE:
            return Py_BuildValue("s", "DAP_CHAIN_DATUM_DECREE_COMMON_SUBTYPE_STAKE_INVALIDATE");
        case DAP_CHAIN_DATUM_DECREE_COMMON_SUBTYPE_STAKE_MIN_VALUE:
            return Py_BuildValue("s", "DAP_CHAIN_DATUM_DECREE_COMMON_SUBTYPE_STAKE_MIN_VALUE");
        case DAP_CHAIN_DATUM_DECREE_COMMON_SUBTYPE_STAKE_MIN_VALIDATORS_COUNT:
            return Py_BuildValue("s", "DAP_CHAIN_DATUM_DECREE_COMMON_SUBTYPE_STAKE_MIN_VALIDATORS_COUNT");
    }
    return Py_BuildValue("s", "UNKNOWN");
}

PyObject* wrapping_dap_chain_datum_decree_get_tsd(PyObject *self, void* closure) {
    (void)closure;
    size_t l_tsd_total_size = PVT(self)->decree->header.data_size;
    if (l_tsd_total_size == 0)
        Py_RETURN_NONE;
    PyObject *obj_list = PyList_New(0);
    for (size_t l_offset = 0; l_offset < l_tsd_total_size;) {
        dap_tsd_t *l_tsd = (dap_tsd_t*)(((byte_t*)PVT(self)->decree->data_n_signs) + l_offset);
        size_t l_tsd_size = dap_tsd_size(l_tsd);
        if (l_tsd_size + l_offset > l_tsd_total_size)
            break;
        l_offset += l_tsd_size;
        PyObject *obj_type = PyLong_FromLong(l_tsd->type);
        PyObject *obj_value = NULL;
        Py_ssize_t py_tsd_size = l_tsd_size;
        PyBytes_AsStringAndSize(obj_value, (char **)&l_tsd->data, &py_tsd_size);
        PyObject *obj_dict = PyDict_New();
        PyDict_SetItemString(obj_dict, "type", obj_type);
        PyDict_SetItemString(obj_dict, "value", obj_value);
        PyList_Append(obj_list, obj_dict);
    }
    return (PyObject*)obj_list;
}

PyObject* wrapping_dap_chain_datum_decree_get_signs(PyObject *self, void* closure) {
    (void)closure;
    size_t l_signs_size = 0;
    dap_sign_t *l_signs = dap_chain_datum_decree_get_signs(PVT(self)->decree, &l_signs_size);
    if (l_signs_size == 0)
        Py_RETURN_NONE;
    PyObject *obj_list = PyList_New(0);
    for (size_t l_offset = 0; l_offset < l_signs_size; ) {
        size_t l_sign_size = dap_sign_get_size(l_signs + l_offset);
        PyDapSignObject *obj_sign = PyObject_New(PyDapSignObject, &DapCryptoSignObjectType);
        obj_sign->sign = l_signs + l_offset;
        l_offset += l_sign_size;
    }
    return obj_list;
}

PyGetSetDef  DapChainDatumDecreeGetSet[] = {
        {"tsCreated", (getter)wrapping_dap_chain_datum_decree_get_ts_created, NULL, NULL, NULL},
        {"type", (getter)wrapping_dap_chain_datum_decree_get_type, NULL, NULL, NULL},
        {"typeStr", (getter)wrapping_dap_chain_datum_decree_get_type_str, NULL, NULL, NULL},
        {"subtype", (getter)wrapping_dap_chain_datum_decree_get_subtype, NULL, NULL, NULL},
        {"subtypeStr", (getter)wrapping_dap_chain_datum_decree_get_subtype_str, NULL, NULL, NULL},
        {"signs", (getter)wrapping_dap_chain_datum_decree_get_signs, NULL, NULL, NULL},
        {"TSD", (getter)wrapping_dap_chain_datum_decree_get_tsd, NULL, NULL, NULL},
        {}
};

PyObject *wrapping_dap_chain_datum_decree_add_sign(PyObject *self, PyObject *args) {
    PyObject *obj_cert;
    if (!PyArg_ParseTuple(args, "O", &obj_cert)) {
        return NULL;
    }
    if (!PyCryptoKeyObject_check(obj_cert)) {
        PyErr_SetString(PyExc_AttributeError, "The first argument was not passed to the function "
                                              "correctly. The second argument must be of type DAP.Crypto.Cert.");
        return NULL;
    }

    size_t l_cur_sign_offset = PVT(self)->decree->header.data_size + PVT(self)->decree->header.signs_size;
    size_t l_total_signs_size = PVT(self)->decree->header.signs_size;

    dap_sign_t *l_sign = dap_cert_sign(((PyCryptoCertObject*)obj_cert)->cert, PVT(self)->decree,
                                       sizeof(dap_chain_datum_decree_t) + PVT(self)->decree->header.data_size, 0);
    if (l_sign) {
        size_t l_sign_size = dap_sign_get_size(l_sign);
        PVT(self)->decree = DAP_REALLOC(PVT(self)->decree, sizeof(dap_chain_datum_decree_t) + l_cur_sign_offset + l_sign_size);
        memcpy((byte_t*)PVT(self)->decree->data_n_signs + l_cur_sign_offset, l_sign, l_sign_size);
        l_total_signs_size += l_sign_size;
        PVT(self)->decree->header.signs_size = l_total_signs_size;
        DAP_DELETE(l_sign);
    } else {
        PyErr_SetString(PyExc_RuntimeError, "Decree signing failed");
        return NULL;
    }
    Py_RETURN_NONE;
}

PyMethodDef DapChainDatumDecreeMethods[] = {
        {
                "addSign",
                wrapping_dap_chain_datum_decree_add_sign,
                METH_VARARGS,
                "Adding a signature for the decree datum."
            },
        {}
};

PyObject *PyDapChainDatumDecreeObject_new(PyTypeObject *type_object, PyObject *args, PyObject *kwds) {
    (void)type_object;
    (void)kwds;
    PyObject *obj_net;
    PyObject *obj_tsd_list;
    const char *l_subtype_str;
    PyObject *obj_cert;
    if (!PyArg_ParseTuple(args, "OOsO", &obj_net, &obj_cert, &l_subtype_str, &obj_tsd_list)) {
        return NULL;
    }
    if (!PyObject_TypeCheck(obj_net, &DapChainNetObjectType)) {
        PyErr_SetString(PyExc_AttributeError, "The first argument to the constructor was not correctly "
                                              "passed, the first argument must be CellFrame.NetworkChain");
        return NULL;
    }
    if (!PyCryptoKeyObject_check(obj_cert)) {
        PyErr_SetString(PyExc_AttributeError, "The second argument to the constructor was not correctly "
                                              "passed, the second argument must be DAP.Crypto.Cert");
        return NULL;
    }
    if (!PyList_Check(obj_tsd_list)) {
        PyErr_SetString(PyExc_AttributeError, "The third argument to the constructor was not correctly "
                                              "passed, the third argument must be a list.");
        return NULL;
    }
    //Get subtype
    uint16_t l_subtype = 0;
    if (strcmp(l_subtype_str, "DAP_CHAIN_DATUM_DECREE_COMMON_SUBTYPE_FEE") == 0 ) {
        l_subtype = DAP_CHAIN_DATUM_DECREE_COMMON_SUBTYPE_FEE;
    } else if (strcmp(l_subtype_str, "DAP_CHAIN_DATUM_DECREE_COMMON_SUBTYPE_OWNERS") == 0) {
        l_subtype = DAP_CHAIN_DATUM_DECREE_COMMON_SUBTYPE_OWNERS;
    } else if (strcmp(l_subtype_str, "DAP_CHAIN_DATUM_DECREE_COMMON_SUBTYPE_OWNERS_MIN") == 0) {
        l_subtype = DAP_CHAIN_DATUM_DECREE_COMMON_SUBTYPE_OWNERS_MIN;
    } else if (strcmp(l_subtype_str, "DAP_CHAIN_DATUM_DECREE_COMMON_SUBTYPE_TON_SIGNERS_MIN") == 0) {
        l_subtype = DAP_CHAIN_DATUM_DECREE_COMMON_SUBTYPE_TON_SIGNERS_MIN;
    } else if (strcmp(l_subtype_str, "DAP_CHAIN_DATUM_DECREE_COMMON_SUBTYPE_STAKE_APPROVE") == 0) {
        l_subtype = DAP_CHAIN_DATUM_DECREE_COMMON_SUBTYPE_STAKE_APPROVE;
    } else if (strcmp(l_subtype_str, "DAP_CHAIN_DATUM_DECREE_COMMON_SUBTYPE_STAKE_INVALIDATE") == 0) {
        l_subtype = DAP_CHAIN_DATUM_DECREE_COMMON_SUBTYPE_STAKE_INVALIDATE;
    } else if (strcmp(l_subtype_str, "DAP_CHAIN_DATUM_DECREE_COMMON_SUBTYPE_STAKE_MIN_VALUE") == 0) {
        l_subtype = DAP_CHAIN_DATUM_DECREE_COMMON_SUBTYPE_STAKE_MIN_VALUE;
    } else {
        char *l_err_str = dap_strdup_printf("An unsupported %s subtype was passed as the third argument. Supported delegation token subtypes:\n"
                                            "\tDAP_CHAIN_DATUM_DECREE_COMMON_SUBTYPE_FEE\n"
                                            "\tDAP_CHAIN_DATUM_DECREE_COMMON_SUBTYPE_OWNERS\n"
                                            "\tDAP_CHAIN_DATUM_DECREE_COMMON_SUBTYPE_OWNERS_MIN\n"
                                            "\tDAP_CHAIN_DATUM_DECREE_COMMON_SUBTYPE_TON_SIGNERS_MIN\n"
                                            "\tDAP_CHAIN_DATUM_DECREE_COMMON_SUBTYPE_STAKE_APPROVE\n"
                                            "\tDAP_CHAIN_DATUM_DECREE_COMMON_SUBTYPE_STAKE_INVALIDATE\n"
                                            "\tDAP_CHAIN_DATUM_DECREE_COMMON_SUBTYPE_STAKE_MIN_VALUE\n"
                                            "\tDAP_CHAIN_DATUM_DECREE_COMMON_SUBTYPE_STAKE_MIN_VALIDATORS_COUNT", l_subtype_str);
        PyErr_SetString(PyExc_AttributeError, l_err_str);
        DAP_DELETE(l_err_str);
        return NULL;
    }
    //Parsing TSD List
    dap_list_t *l_tsd_list = NULL;
    PyObject* l_iter = PyObject_GetIter(obj_tsd_list);
    if (l_iter == NULL) {
        PyErr_SetString(PyExc_AttributeError, "Passed as the fourth argument, the list contains no elements.");
        return NULL;
    }
    PyObject *l_item = NULL;
    size_t l_el_count = 0;
    size_t l_total_tsd_size = 0;
    while((l_item = PyIter_Next(l_iter)) != NULL){
        l_el_count++;
        if (!PyDict_Check(l_item)) {
            char *l_err_str = dap_strdup_printf("Leaf parsing error, element number: %zu is not a dictionary.", l_el_count);
            PyErr_SetString(PyExc_AttributeError, l_err_str);
            DAP_DELETE(l_err_str);
            return NULL;
        }
        PyObject *l_obj_type = PyDict_GetItemString(l_item, "type");
        if (!PyLong_Check(l_obj_type)) {
            char *l_err_str = dap_strdup_printf("The dictionary obtained from the list under the %zu element "
                                                "does not contain an object with the key type or this object is not "
                                                "a number.", l_el_count);
            PyErr_SetString(PyExc_AttributeError, l_err_str);
            DAP_DELETE(l_err_str);
            return NULL;
        }
        PyObject *l_obj_value = PyDict_GetItemString(l_item, "value");
        if (!PyBytes_Check(l_obj_value)) {
            char *l_err_str = dap_strdup_printf("The dictionary obtained from the list under the %zu element "
                                                "does not contain an object with the key type or this object is not "
                                                "a bytes.", l_el_count);
            PyErr_SetString(PyExc_AttributeError, l_err_str);
            DAP_DELETE(l_err_str);
            return NULL;
        }
        unsigned long l_type = PyLong_AsUnsignedLong(l_obj_type);
        size_t l_value_size = PyBytes_Size(l_obj_value);
        void *l_value = PyBytes_AsString(l_obj_value);
        dap_tsd_t *l_tsd = dap_tsd_create(l_type, l_value, l_value_size);
        l_total_tsd_size += dap_tsd_size(l_tsd);
        l_tsd_list = dap_list_append(l_tsd_list, l_tsd);
    }
    //Creating datum decree
    dap_chain_datum_decree_t *l_decree = DAP_NEW_Z_SIZE(dap_chain_datum_decree_t, sizeof(dap_chain_datum_decree_t) + l_total_tsd_size);
    l_decree->decree_version = DAP_CHAIN_DATUM_DECREE_VERSION;
    l_decree->header.ts_created = dap_time_now();
    l_decree->header.type = DAP_CHAIN_DATUM_DECREE_TYPE_COMMON;
    l_decree->header.common_decree_params.net_id = ((PyDapChainNetObject*)obj_net)->chain_net->pub.id;
    l_decree->header.common_decree_params.chain_id = dap_chain_net_get_default_chain_by_chain_type(
            ((PyDapChainNetObject*)obj_net)->chain_net, CHAIN_TYPE_DECREE)->id;
    l_decree->header.common_decree_params.cell_id = *dap_chain_net_get_cur_cell(((PyDapChainNetObject*)obj_net)->chain_net);
    l_decree->header.sub_type = l_subtype;
    l_decree->header.data_size = l_total_tsd_size;
    l_decree->header.signs_size = 0;

    size_t l_data_tsd_offset = 0;
    for ( dap_list_t* l_iter=dap_list_first(l_tsd_list); l_iter; l_iter=l_iter->next){
        dap_tsd_t * l_b_tsd = (dap_tsd_t *) l_iter->data;
        size_t l_tsd_size = dap_tsd_size(l_b_tsd);
        memcpy((byte_t*)l_decree->data_n_signs + l_data_tsd_offset, l_b_tsd, l_tsd_size);
        l_data_tsd_offset += l_tsd_size;
    }
    dap_list_free_full(l_tsd_list, NULL);

    dap_sign_t * l_sign = dap_cert_sign(((PyCryptoCertObject*)obj_cert)->cert,  l_decree,
                                        sizeof(dap_chain_datum_decree_t) + l_decree->header.data_size, 0);

    if (l_sign) {
        size_t l_sign_size = dap_sign_get_size(l_sign);
        l_decree = DAP_REALLOC(l_decree, sizeof(dap_chain_datum_decree_t) + l_total_tsd_size + l_sign_size);
        memcpy((byte_t*)l_decree->data_n_signs + l_total_tsd_size, l_sign, l_sign_size);
        l_decree->header.signs_size = l_sign_size;
        DAP_DELETE(l_sign);
    } else {
        DAP_DELETE(l_decree);
        PyErr_SetString(PyExc_RuntimeError, "Decree signing failed");
        return NULL;
    }

    PyDapChainDatumDecreeObject *obj_self = PyObject_New(PyDapChainDatumDecreeObject, &DapChainDatumDecreeObjectType);
    obj_self->decree = l_decree;
    return (PyObject*)obj_self;
}

PyTypeObject DapChainDatumDecreeObjectType = {
        .ob_base = PyVarObject_HEAD_INIT(NULL, 0)
        .tp_name = "CellFrame.Common.DatumDecree",
        .tp_basicsize = sizeof(PyDapChainDatumDecreeObject),
        .tp_doc = "Chain datum type decree object",
        .tp_getset = DapChainDatumDecreeGetSet,
        .tp_methods = DapChainDatumDecreeMethods,
        .tp_new = PyDapChainDatumDecreeObject_new
};
