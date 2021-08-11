#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include "dap_client.h"
#include "dap_events_python.h"
#include "uthash.h"
//#include "wrapping_dap_enc_key.h"
#include "wrapping_dap_client_stage.h"
#include "dap_events_socket_python.h"
#include "wrapping_cert.h"
#include "wrapping_dap_client_stage_status.h"
#include "wrapping_dap_stream.h"
#include "wrapping_dap_stream_ch.h"
#include "wrapping_dap_stream_worker.h"
#include "libdap_crypto_key_python.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct PyDapClient{
    PyObject_HEAD
    dap_client_t *client;
}PyDapClientObject;

struct dap_client_call_hash{
    dap_client_t *client;
    PyObject *call_func_stage_status;
    PyObject *call_func_stage_status_error;
    UT_hash_handle hh;
};

static struct dap_client_call_hash *g_client_hash_table = NULL;
static struct dap_client_call_hash *g_client_hash_table_go_stage_end = NULL;

struct dap_client_call_responce{
    dap_client_t *client;
    PyObject *call_func_response_proc;
    PyObject *call_func_response_error;
    UT_hash_handle hh;
};

static struct dap_client_call_responce *g_client_hash_table_response = NULL;

int dap_client_init_py();
void dap_client_deinit_py();

void _wrapping_callback_stage_status(dap_client_t *a_client, void *a_data);
void _wrapping_callback_stage_status_error(dap_client_t *a_client, void *a_data);

int dap_client_obj_init(PyDapClientObject *self, PyObject *args, PyObject *kwds);

PyObject *dap_client_set_uplink_unsafe_py(PyObject *self, PyObject *args);
PyObject *dap_client_get_uplink_addr_unsafe_py(PyObject *self, PyObject *args);
PyObject *dap_client_get_uplink_port_unsafe_py(PyObject *self, PyObject *args);

PyObject *dap_client_get_key_stream_py(PyObject *self, PyObject *args);
PyObject *dap_client_go_stage_py(PyObject *self, PyObject *args);

void dap_client_delete_mt_py(PyObject *self);
//PyObject *dap_client_delete_unsafe_py(dap_client_t * a_client);

//PyObject *dap_client_reset_py(PyObject *self, PyObject *args);
PyObject *dap_client_request_enc_unsafe_py(PyObject *self, PyObject *args);
PyObject *dap_client_request_unsafe_py(PyObject *self, PyObject *args);
//PyObject *dap_client_disconnect_py(PyObject *self, PyObject *args);

PyObject *dap_client_get_stage_str_py(PyObject *self, PyObject *args);
//PyObject *dap_client_stage_str_py(PyObject *self, PyObject *args); IN GET Value from object ClientStage

PyObject *dap_client_get_stage_status_str_py(PyObject *self, PyObject *args);
//PyObject *dap_client_stage_status_str_py(PyObject *self, PyObject *args); IN GET Value from object ClientStageStatus
//PyObject *dap_client_error_str_py(PyObject *self, PyObject *args);
PyObject *dap_client_get_error_str_py(PyObject *self, PyObject *args);

PyObject *dap_client_get_is_always_reconnect_py(PyObject *self, PyObject *args);
PyObject *dap_client_set_is_always_reconnect_py(PyObject *self, PyObject *args);

PyObject *dap_client_from_esocket_py(PyObject *self, PyObject *args);
PyObject *dap_client_get_auth_cookie_py(PyObject *self, PyObject *args);
PyObject *dap_client_get_stream_py(PyObject *self, PyObject *args);
PyObject *dap_client_get_stream_ch_unsafe_py(PyObject *self, PyObject *args);
PyObject *dap_client_get_stream_worker_py(PyObject *self, PyObject *args);
PyObject *dap_client_get_stream_id_py(PyObject *self, PyObject *args);
PyObject *dap_client_set_active_channels_unsafe_py(PyObject *self, PyObject *args);
PyObject *dap_client_set_auth_cert_unsafe_py(PyObject *self, PyObject *args);

PyObject *dap_client_get_stage_py(PyObject *self, PyObject *args);
PyObject *dap_client_get_stage_status_py(PyObject *self, PyObject *args);

static PyMethodDef DapClientMethods[] = {
        {"setUplinkUnsafe", (PyCFunction)dap_client_set_uplink_unsafe_py, METH_VARARGS, "Setting uplink for client"},
        {"getUplinkAddrUnsafe", (PyCFunction)dap_client_get_uplink_addr_unsafe_py, METH_NOARGS, "Get uplink addr for client"},
        {"getUplinkPortUnsafe", (PyCFunction)dap_client_get_uplink_port_unsafe_py, METH_NOARGS, "Get uplink addr for client"},
        {"getKeyStream", (PyCFunction)dap_client_get_key_stream_py, METH_NOARGS, "Get key from client stream"},
        {"goStage", (PyCFunction)dap_client_go_stage_py, METH_VARARGS, ""},
        {"requestEncUnsafe", (PyCFunction)dap_client_request_enc_unsafe_py, METH_VARARGS, ""},
        {"requestUnsafe", (PyCFunction)dap_client_request_unsafe_py, METH_VARARGS, ""},
        {"getStageStr", (PyCFunction)dap_client_get_stage_str_py, METH_NOARGS, ""},
        {"getStageStatusStr", (PyCFunction)dap_client_get_stage_status_str_py, METH_NOARGS, ""},
        {"getErrorStr", (PyCFunction)dap_client_get_error_str_py, METH_NOARGS, ""},
        {"getIsAlwaysReconnect", (PyCFunction)dap_client_get_is_always_reconnect_py, METH_NOARGS, ""},
        {"setIsAlwaysReconnect", (PyCFunction)dap_client_set_is_always_reconnect_py, METH_VARARGS, ""},
        {"fromEsocket", (PyCFunction)dap_client_from_esocket_py, METH_VARARGS | METH_STATIC, ""},
        {"authCookie", (PyCFunction)dap_client_get_auth_cookie_py, METH_NOARGS, ""},
        {"getStream", (PyCFunction)dap_client_get_stream_py, METH_NOARGS, ""},
        {"getStreamChUnsafe", (PyCFunction)dap_client_get_stream_ch_unsafe_py, METH_VARARGS, ""},
        {"getStreamWorker", (PyCFunction)dap_client_get_stream_worker_py, METH_NOARGS, ""},
        {"getStreamId", (PyCFunction)dap_client_get_stream_id_py, METH_VARARGS, ""},
        {"setActiveChannelsUnsafe", (PyCFunction)dap_client_set_active_channels_unsafe_py, METH_VARARGS, ""},
        {"setAuthCertUnsafe", (PyCFunction)dap_client_set_auth_cert_unsafe_py, METH_VARARGS, ""},
        {"getStage", (PyCFunction)dap_client_get_stage_py, METH_NOARGS, ""},
        {"getStageStatus", (PyCFunction)dap_client_get_stage_status_py, METH_NOARGS, ""},
        {NULL, NULL, 0, NULL}
};

static PyTypeObject dapClientObject_dapClientType = {
        PyVarObject_HEAD_INIT(NULL, 0)
        "CellFrame.Client",                                      /* tp_name */
        sizeof(PyDapClientObject),                               /* tp_basicsize */
        0,                                                            /* tp_itemsize */
        (destructor)dap_client_delete_mt_py,                          /* tp_dealloc */
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
        "Client objects",                                              /* tp_doc */
        0,		                                                      /* tp_traverse */
        0,		                                                      /* tp_clear */
        0,		                                                      /* tp_richcompare */
        0,		                                                      /* tp_weaklistoffset */
        0,		                                                      /* tp_iter */
        0,		                                                      /* tp_iternext */
        DapClientMethods,                                              /* tp_methods */
        0,                                                            /* tp_members */
        0,                                                            /* tp_getset */
        0,                                                            /* tp_base */
        0,                                                            /* tp_dict */
        0,                                                            /* tp_descr_get */
        0,                                                            /* tp_descr_set */
        0,                                                            /* tp_dictoffset */
        (initproc)dap_client_obj_init,                                /* tp_init */
        0,                                                            /* tp_alloc */
        PyType_GenericNew,                                            /* tp_new */

};

#ifdef __cplusplus
}
#endif

