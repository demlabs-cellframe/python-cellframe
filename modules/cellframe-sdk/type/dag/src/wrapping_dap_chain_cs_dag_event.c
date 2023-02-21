#include "wrapping_dap_chain_cs_dag_event.h"

PyMethodDef PyDapChainCsDagEventMethodsDef[] = {
        {"fromAtom", (PyCFunction)wrapping_dap_chain_cs_dag_event_from_atom, METH_VARARGS | METH_STATIC, ""},
        {NULL, NULL, 0, NULL}
};

PyGetSetDef PyDapChainCsDagEventGetsSetsDef[] = {
        {"hash", (getter)wrapping_dap_chain_cs_dag_event_get_hash, NULL, NULL, NULL},
        {"version", (getter)wrapping_dap_chain_cs_dag_event_get_version, NULL, NULL, NULL},
        {"roundId", (getter)wrapping_dap_chain_cs_dag_event_get_round_id, NULL, NULL, NULL},
        {"created", (getter)wrapping_dap_chain_cs_dag_event_get_ts_created, NULL, NULL, NULL},
        {"chainId", (getter)wrapping_dap_chain_cs_dag_event_get_chain_id, NULL, NULL, NULL},
        {"cellId", (getter)wrapping_dap_chain_cs_dag_event_get_cell_id, NULL, NULL, NULL},
        {"hashCount", (getter)wrapping_dap_chain_cs_dag_event_get_hash_count, NULL, NULL, NULL},
        {"signsCount", (getter)wrapping_dap_chain_cs_dag_event_get_signs_count, NULL, NULL, NULL},
        {"links", (getter)wrapping_dap_chain_cs_dag_event_get_links, NULL, NULL, NULL},
        {"datum", (getter)wrapping_dap_chain_cs_dag_event_get_datum, NULL, NULL, NULL},
        {"signs", (getter)wrapping_dap_chain_cs_dag_event_get_signs, NULL, NULL, NULL},
        {NULL}
};

PyTypeObject DapChainCsDagEventType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "CellFrame.ChainCsDagEvent",                                            /* tp_name */
        sizeof(PyDapChainCsDagEventObject),                                     /* tp_basicsize */
        0,                                                            /* tp_itemsize */
        0,                         /* tp_dealloc */
        0,                                                            /* tp_print */
        0,                                                            /* tp_getattr */
        0,                                                            /* tp_setattr */
        0,                                                            /* tp_reserved */
        0,                                                            /* tp_repr */
        0,                                                            /* tp_as_number */
        0,                                                            /* tp_as_sequence */
        0,                                                            /* tp_as_mapping */
        0,                                                            /* tp_hash  */
        0,                                                            /* tp_call */
        0,                                                            /* tp_str */
        0,                                                            /* tp_getattro */
        0,                                                            /* tp_setattro */
        0,                                                            /* tp_as_buffer */
        Py_TPFLAGS_DEFAULT |
        Py_TPFLAGS_BASETYPE,                                      /* tp_flags */
        "Chain cs dag event objects",                                              /* tp_doc */
        0,		                                                      /* tp_traverse */
        0,		                                                      /* tp_clear */
        0,		                                                      /* tp_richcompare */
        0,		                                                      /* tp_weaklistoffset */
        0,		                                                      /* tp_iter */
        0,		                                                      /* tp_iternext */
        PyDapChainCsDagEventMethodsDef,                             /* tp_methods */
        0,                                                            /* tp_members */
        PyDapChainCsDagEventGetsSetsDef,                             /* tp_getset */
        0,                                                            /* tp_base */
        0,                                                            /* tp_dict */
        0,                                                            /* tp_descr_get */
        0,                                                            /* tp_descr_set */
        0,                                                            /* tp_dictoffset */
        0,                                                            /* tp_init */
        0,                                                            /* tp_alloc */
        PyType_GenericNew,                                            /* tp_new */
};

PyObject *wrapping_dap_chain_cs_dag_event_from_atom(PyObject *self, PyObject *args){
    (void)self;
    PyObject *obj_atom_ptr;
    size_t l_atom_size;
    if (!PyArg_ParseTuple(args, "On", &obj_atom_ptr, &l_atom_size)){
        return NULL;
    }
    PyDapChainCsDagEventObject *obj_dag = PyObject_New(PyDapChainCsDagEventObject, &DapChainCsDagEventType);
    obj_dag->event = (dap_chain_cs_dag_event_t *)((PyChainAtomPtrObject*)obj_atom_ptr)->ptr;
    obj_dag->event_size = l_atom_size;
    return (PyObject*)obj_dag;
}

PyObject *wrapping_dap_chain_cs_dag_event_get_hash(PyObject *self, void *closure){
    (void)closure;
    PyDapHashFastObject *obj_hf = PyObject_New(PyDapHashFastObject, &DapChainHashFastObjectType);
    obj_hf->hash_fast = DAP_NEW(dap_chain_hash_fast_t);
    dap_hash_fast(
            ((PyDapChainCsDagEventObject*)self)->event,
            ((PyDapChainCsDagEventObject*)self)->event_size,
            obj_hf->hash_fast);
    obj_hf->origin = true;
    return (PyObject*)obj_hf;
}

PyObject *wrapping_dap_chain_cs_dag_event_get_version(PyObject *self, void *closure){
    (void)closure;
    return Py_BuildValue("H", ((PyDapChainCsDagEventObject*)self)->event->header.version);
}
PyObject *wrapping_dap_chain_cs_dag_event_get_round_id(PyObject *self, void *closure){
    (void)closure;
    return Py_BuildValue("k", ((PyDapChainCsDagEventObject*)self)->event->header.round_id);
}
PyObject *wrapping_dap_chain_cs_dag_event_get_ts_created(PyObject *self, void *closure){
    (void)closure;
    PyDateTime_IMPORT;
    PyObject *obj_ts_float = PyLong_FromLong(((PyDapChainCsDagEventObject*)self)->event->header.ts_created);
    PyObject *obj_ts = Py_BuildValue("(O)", obj_ts_float);
    PyDateTime_IMPORT;
    PyObject *obj_dt = PyDateTime_FromTimestamp(obj_ts);
    return obj_dt;
}
PyObject *wrapping_dap_chain_cs_dag_event_get_chain_id(PyObject *self, void *closure){
    (void)closure;
    PyDapChainIDObject *obj_chain_id = PyObject_New(PyDapChainIDObject, &DapChainIdObjectType);
    obj_chain_id->chain_id = &((PyDapChainCsDagEventObject*)self)->event->header.chain_id;
    return (PyObject*)obj_chain_id;
}
PyObject *wrapping_dap_chain_cs_dag_event_get_cell_id(PyObject *self, void *closure){
    (void)closure;
    PyDapChainCellIDObject *obj_cell_id = PyObject_New(PyDapChainCellIDObject, &DapChainCellIdObjectType);
    obj_cell_id->cell_id = ((PyDapChainCsDagEventObject*)self)->event->header.cell_id;
    return (PyObject*)obj_cell_id;
}
PyObject *wrapping_dap_chain_cs_dag_event_get_hash_count(PyObject *self, void *closure){
    (void)closure;
    return Py_BuildValue("H", ((PyDapChainCsDagEventObject*)self)->event->header.hash_count);
}
PyObject *wrapping_dap_chain_cs_dag_event_get_signs_count(PyObject *self, void *closure){
    (void)closure;
    return Py_BuildValue("H", ((PyDapChainCsDagEventObject*)self)->event->header.signs_count);
}
PyObject *wrapping_dap_chain_cs_dag_event_get_links(PyObject *self, void *closure){
    (void)closure;
    PyObject *obj_list = PyList_New(((PyDapChainCsDagEventObject*)self)->event->header.hash_count);
    for (uint16_t i=0; i < ((PyDapChainCsDagEventObject*)self)->event->header.hash_count; i++){
        PyDapHashFastObject  *obj_hf = PyObject_New(PyDapHashFastObject, &DapChainHashFastObjectType);
        obj_hf->hash_fast =
                (dap_chain_hash_fast_t *) (((PyDapChainCsDagEventObject*)self)->event->hashes_n_datum_n_signs +
                                                                    i*sizeof (dap_chain_hash_fast_t));
        obj_hf->origin = false;
        PyList_SetItem(obj_list, i, (PyObject *)obj_hf);
    }
    return obj_list;
}
PyObject *wrapping_dap_chain_cs_dag_event_get_datum(PyObject *self, void *closure){
    (void)closure;
    size_t l_offset =  ((PyDapChainCsDagEventObject*)self)->event->header.hash_count*sizeof (dap_chain_hash_fast_t);
    PyDapChainDatumObject *datum = PyObject_New(PyDapChainDatumObject, &DapChainDatumObjectType);
    datum->origin = false;
    datum->datum = (dap_chain_datum_t*) (((PyDapChainCsDagEventObject*)self)->event->hashes_n_datum_n_signs + l_offset);
    return (PyObject*)datum;
}
PyObject *wrapping_dap_chain_cs_dag_event_get_signs(PyObject *self, void *closure){
    size_t l_offset =  ((PyDapChainCsDagEventObject*)self)->event->header.hash_count*sizeof (dap_chain_hash_fast_t);
    dap_chain_datum_t  *l_datum = (dap_chain_datum_t*) (((PyDapChainCsDagEventObject*)self)->event->hashes_n_datum_n_signs + l_offset);
    l_offset += dap_chain_datum_size(l_datum);
    PyObject *obj_list = PyList_New(0);
    while (l_offset + sizeof(((PyDapChainCsDagEventObject*)self)->event->header) < ((PyDapChainCsDagEventObject*)self)->event_size){
        dap_sign_t * l_sign =(dap_sign_t *) (((PyDapChainCsDagEventObject*)self)->event->hashes_n_datum_n_signs +l_offset);
        size_t l_sign_size = dap_sign_get_size(l_sign);
        if (l_sign_size == 0){
            break;
        }
        PyDapSignObject *obj_sign = PyObject_New(PyDapSignObject, &DapCryptoSignObjectType);
        obj_sign->sign = l_sign;
        PyList_Append(obj_list, (PyObject*)obj_sign);
        l_offset += l_sign_size;
    }
    return obj_list;
}
