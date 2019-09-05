#include "Python.h"
#include "dap_server.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct PyDapServer {
    PyObject_HEAD
    dap_server_t *t_server;
}PyDapServerObject;

PyTypeObject dapServer_dapServerType = {
    PyVarObject_HEAD_INIT(NULL, 0)
    "libCellFrame.ServerCore.DapServer",             /* tp_name */
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

#ifdef __cplusplus
}
#endif
