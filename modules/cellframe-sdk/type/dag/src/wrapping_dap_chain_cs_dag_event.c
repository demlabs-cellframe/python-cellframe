#include "wrapping_dap_chain_cs_dag_event.h"

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
    PyDapChainIDObject *obj_chain_id = PyObject_New(PyDapChainIDObject, &DapChainIDObject_DapChainIDType);
    PyObject_Dir((PyObject*)obj_chain_id);
    obj_chain_id->chain_id = &((PyDapChainCsDagEventObject*)self)->event->header.chain_id;
    return (PyObject*)obj_chain_id;
}
PyObject *wrapping_dap_chain_cs_dag_event_get_cell_id(PyObject *self, void *closure){
    (void)closure;
    PyDapChainCellIDObject *obj_cell_id = PyObject_New(PyDapChainCellIDObject, &DapChainCellIDObject_DapChainCellIDType);
    PyObject_Dir((PyObject*)obj_cell_id);
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
        PyDapHashFastObject  *obj_hf = PyObject_New(PyDapHashFastObject, &DapHashFastObject_DapHashFastObjectType);
        PyObject_Dir((PyObject*)obj_hf);
        obj_hf->hash_fast =
                (dap_chain_hash_fast_t *) (((PyDapChainCsDagEventObject*)self)->event->hashes_n_datum_n_signs +
                                                                    i*sizeof (dap_chain_hash_fast_t));
        PyList_SetItem(obj_list, i, obj_hf);
    }
    return obj_list;
}
PyObject *wrapping_dap_chain_cs_dag_event_get_datum(PyObject *self, void *closure){
    (void)closure;
    size_t l_offset =  ((PyDapChainCsDagEventObject*)self)->event->header.hash_count*sizeof (dap_chain_hash_fast_t);
    PyDapChainDatumObject *datum = PyObject_New(PyDapChainDatumObject, &DapChainDatumObject_DapChainDatumObjectType);
    PyObject_Dir((PyObject*)datum);
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
        PyDapSignObject *obj_sign = PyObject_New(PyDapSignObject, &DapSignObject_DapSignObjectType);
        PyObject_Dir((PyObject*)obj_sign);
        obj_sign->sign = l_sign;
        PyList_Append(obj_list, (PyObject*)obj_sign);
        l_offset += l_sign_size;
    }
    return obj_list;
}
