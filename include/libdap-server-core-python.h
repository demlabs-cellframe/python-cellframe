#define PY_SSIZE_T_CLEAN
#include "Python.h"
#include "dap_server.h"
#include "dap_client_remote.h"
#include "dap_events.h"
#include "dap_common.h"
#include "dap_events_python.h"
#include "dap_events_socket_python.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct PyDapServer {
    PyObject_HEAD
    dap_server_t *t_server;
}PyDapServerObject;

PyTypeObject dapServer_dapServerType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "libDapServerCore.DapServer",             /* tp_name */
    sizeof(PyDapServerObject), /* tp_basicsize */
    0,                         /* tp_itemsize */
    0,                         /* tp_dealloc */
    0,                         /* tp_print */
    0,                         /* tp_getattr */
    0,                         /* tp_setattr */
    0,                         /* tp_compare */
    0,                         /* tp_repr */
    0,                         /* tp_as_number */
    0,                         /* tp_as_sequence */
    0,                         /* tp_as_mapping */
    0,                         /* tp_hash */
    0,                         /* tp_call */
    0,                         /* tp_str */
    0,                         /* tp_getattro */
    0,                         /* tp_setattro */
    0,                         /* tp_as_buffer */
    Py_TPFLAGS_DEFAULT,        /* tp_flags */
    "DapServer objects",           /* tp_doc */
};

#define LOG_TAG "libdap-server-core-python"

static PyObject *dap_server_core_init(PyObject *self, PyObject *args);
static PyObject *dap_server_core_deinit();

static PyObject *dap_server_core_loop(PyObject *self, PyObject *args);
static PyObject *dap_server_core_listen(PyObject *self, PyObject *args);

static PyMethodDef DapServerCorePythonMethods[] = {
        {"init", dap_server_core_init, METH_VARARGS, "Initialization of the DAP (Deus Applicaions Prototypes) server core library"},
        {"deinit", dap_server_core_deinit, METH_NOARGS, "Deinitialization of the DAP (Deus Applicaions Prototypes) server core library"},
        {"loop", dap_server_core_loop, METH_VARARGS, ""},
        {"listen", dap_server_core_listen, METH_VARARGS, ""},
        /* Wrapping Events */
        {"eventsInit", dap_events_init_py, METH_VARARGS, ""},
        {"eventsDeinit", dap_events_deinit_py, METH_NOARGS, ""},
        {"eventsNew", dap_events_new_py, METH_NOARGS, ""},
        {"eventsDellete", dap_events_delete_py, METH_VARARGS, ""},
        {"eventsStart", dap_events_start_py, METH_VARARGS, ""},
        {"eventsWait", dap_events_wait_py, METH_VARARGS, ""},
        /* Events Socket */
        {"eventsSocketInit", dap_events_socket_init_py, METH_NOARGS, ""},
        {"eventsSocketDeinit", dap_events_socket_deinit_py, METH_NOARGS, ""},
        {"eventsSocketCreateAfter", dap_events_socket_create_after_py, METH_VARARGS, ""},
        {"eventsSocketFind", dap_events_socket_find_py, METH_VARARGS, ""},
        {"eventsSocketIsReadyToRead", dap_events_socket_is_ready_to_read_py, METH_VARARGS, ""},
        {"eventsSocketIsReadyToWrite", dap_events_socket_is_ready_to_write_py, METH_VARARGS, ""},
        {"eventsSocketSetReadable", dap_events_socket_set_readable_py, METH_VARARGS, ""},
        {"eventsSocketSetWritable", dap_events_socket_set_writable_py, METH_VARARGS, ""},
        {"eventsSocketWrite", dap_events_socket_write_py, METH_VARARGS, ""},
        {"eventsSocketWriteF", dap_events_socket_write_f_py, METH_VARARGS, ""},
        {"eventsSocketRead", dap_events_socket_read_py, METH_VARARGS, ""},
        {"eventsSocketDelete", dap_events_socket_delete_py, METH_VARARGS, ""},
        {"eventsSocketShrinkBufIn", dap_events_socket_shrink_buf_in_py, METH_VARARGS, ""},
        {NULL, NULL, 0, NULL}
};

static struct PyModuleDef dapservercorepythonmodule = {
        PyModuleDef_HEAD_INIT,
        "libDapServerCore",   /* name of module */
        NULL, /* module documentation, may be NULL */
        -1,       /* size of per-interpreter state of the module,
                 or -1 if the module keeps state in global variables. */
        DapServerCorePythonMethods
};

PyMODINIT_FUNC PyInit_libDapServerCore(void);


#ifdef __cplusplus
}
#endif
