#ifndef _LIBDAP_CHAIN_GLOBAL_DB_PYTHON_
#define _LIBDAP_CHAIN_GLOBAL_DB_PYTHON_

#define PY_SSIZE_T_CLEAN

#include <Python.h>
#include "dap_chain_global_db.h"
#include "dap_config.h"
#include "dap_store_obj_python.h"

#ifdef __cplusplus
extern "C"{
#endif

typedef struct PyDapChainGlobalDB{
    PyObject_HEAD
    dap_global_db_obj_t *db_object;
    dap_global_db_obj_callback_notify_t callback_notify;
    bool obj_clear;
}PyDapChainGlobalDBObject;

static PyObject *binding_callback_notify = NULL;

int dap_chain_global_db_init_py(dap_config_t *config);
void dap_chain_global_db_deinit_py();

PyObject *PyDapChainGlobalDB_create(PyTypeObject *type_object, PyObject *args, PyObject *kwds);

void dap_chain_global_db_obj_delete_py(PyDapChainGlobalDBObject *bj);
PyObject *dap_chain_global_db_flush_py();
PyObject *dap_chain_global_db_objs_delete_py(PyObject *self, PyObject *args);

PyObject *dap_chain_global_db_add_history_group_prefix_py(PyObject *self, PyObject *args);
PyObject *dap_chain_global_db_add_history_callback_notify_py(PyObject *self, PyObject *args); //WRN!

//PyObject *dap_chain_global_db_obj_get_py(PyObject *self,PyObject *args);
PyObject *dap_chain_global_db_gr_get_py(PyObject *self,PyObject *args);
PyObject *dap_chain_global_db_gr_set_py(PyObject *self,PyObject *args);
PyObject *dap_chain_global_db_gr_del_py(PyObject *self,PyObject *args);

//Addded wrapping
//PyObject *dap_chain_global_db_obj_clean_py(PyObject *self,PyObject *args); -
PyObject *dap_chain_global_db_get_history_group_by_group_name_py(PyObject *self,PyObject *args); //!
PyObject *dap_chain_global_db_add_history_extra_group_py(PyObject *self,PyObject *args); // ?! Check
PyObject *dap_chain_global_db_add_history_extra_group_callback_notify_py(PyObject *self,PyObject *args); //WRN
PyObject *dap_chain_global_db_obj_gr_get_py(PyObject *self,PyObject *args); // WRN
PyObject *dap_chain_global_db_get_py(PyObject *self,PyObject *args);

PyObject *dap_chain_global_db_set_py(PyObject *self,PyObject *args);

PyObject *dap_chain_global_db_del_py(PyObject *self,PyObject *args);

PyObject *global_db_gr_del_get_timestamp_py(PyObject *self,PyObject *args);


/**
 * Read the entire database into an array of size bytes
 *
 * @param data_size[out] size of output array
 * @return array (note:not Null-terminated string) on NULL in case of an error
 */
PyObject *dap_chain_global_db_get_last_py(PyObject *self,PyObject *args);
PyObject *dap_chain_global_db_cond_load_py(PyObject *self,PyObject *args);
PyObject *dap_chain_global_db_gr_load_py(PyObject *self,PyObject *args);
PyObject *dap_chain_global_db_load_py(PyObject *self,PyObject *args);

/**
 * Write to the database from an array of data_size bytes
 *
 * @return
 */
PyObject *dap_chain_global_db_obj_save_py(PyObject *self,PyObject *args);
PyObject *dap_chain_global_db_gr_save_py(PyObject *self,PyObject *args);
PyObject *dap_chain_global_db_save_py(PyObject *self,PyObject *args);


PyObject *dap_chain_global_db_hash_py(PyObject *self,PyObject *args);
PyObject *dap_chain_global_db_hash_fast_py(PyObject *self,PyObject *args);

PyObject *dap_db_log_pack_py(PyObject *self,PyObject *args);
PyObject *dap_db_log_unpack_py(PyObject *self,PyObject *args);

// Get last id in log
PyObject *dap_db_log_get_group_history_last_id_py(PyObject *self,PyObject *args);
PyObject *dap_db_log_get_last_id_py(PyObject *self,PyObject *args);
// Get log diff as list
PyObject *dap_db_log_get_list_py(PyObject *self,PyObject *args);
// Free list getting from dap_db_log_get_list()
PyObject *dap_db_log_del_list_py(PyObject *self,PyObject *args);
// Get log diff as string
PyObject *dap_db_log_get_diff_py(PyObject *self,PyObject *args);

static PyMethodDef DapChainGlobalDbMethods[] = {
        {"flush", dap_chain_global_db_flush_py, METH_NOARGS | METH_STATIC, "Flushing database to disk"},
        {"addHistoryGroupPrefix", dap_chain_global_db_add_history_group_prefix_py, METH_VARARGS, ""},
        {"addHistoryCallbackNotify", dap_chain_global_db_add_history_callback_notify_py, METH_VARARGS, ""},
        {"grGet", dap_chain_global_db_gr_get_py, METH_VARARGS, ""},
        {"grSet", dap_chain_global_db_gr_set_py, METH_VARARGS, ""},
        {"grDel", dap_chain_global_db_gr_del_py, METH_VARARGS, ""},
        {"getHistoryGroupByGroupName", dap_chain_global_db_get_history_group_by_group_name_py, METH_VARARGS | METH_STATIC, ""},
        {"addHistoryExtraGroup", dap_chain_global_db_add_history_extra_group_py, METH_VARARGS | METH_STATIC, ""},
        {"objGrGet", dap_chain_global_db_obj_gr_get_py, METH_VARARGS | METH_STATIC, ""},
        {NULL, NULL, 0, NULL}
};

static PyTypeObject dapChainObject_dapChainType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "CellFrame.ChainGlobalDB",                                            /* tp_name */
        sizeof(PyDapChainGlobalDBObject),                                     /* tp_basicsize */
        0,                                                            /* tp_itemsize */
        (destructor)dap_chain_global_db_obj_delete_py,                         /* tp_dealloc */
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
        "Chain global db object",                                              /* tp_doc */
        0,		                                                      /* tp_traverse */
        0,		                                                      /* tp_clear */
        0,		                                                      /* tp_richcompare */
        0,		                                                      /* tp_weaklistoffset */
        0,		                                                      /* tp_iter */
        0,		                                                      /* tp_iternext */
        DapChainGlobalDbMethods,                                              /* tp_methods */
        0,                                                            /* tp_members */
        0,                                                            /* tp_getset */
        0,                                                            /* tp_base */
        0,                                                            /* tp_dict */
        0,                                                            /* tp_descr_get */
        0,                                                            /* tp_descr_set */
        0,                                                            /* tp_dictoffset */
        0,                                                            /* tp_init */
        0,                                                            /* tp_alloc */
        PyType_GenericNew,                                            /* tp_new */

};


#ifdef __cplusplus
}
#endif

#endif //_LIBDAP_CHAIN_GLOBAL_DB_PYTHON_
