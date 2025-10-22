#include "wrapping_dap_notify_srv.h"
#include "python-cellframe_common.h"
#include "libdap-python.h"

#define LOG_TAG "wrapping_dap_notify_srv"

static PyMethodDef PyDapNotifySrvMethods[] = {
    {"send", (PyCFunction)wrapping_dap_notify_srv_send, METH_VARARGS | METH_STATIC, "Send string to notify socket"},
    {"sendf", (PyCFunction)wrapping_dap_notify_srv_sendf, METH_VARARGS | METH_STATIC, "Send formatted string to notify socket"},
    {}
};

PyTypeObject DapNotifySrvObjectType = DAP_PY_TYPE_OBJECT(
        "DAP.Network.Notify", sizeof(PyDapNotifySrvObject),
        "DAP notify service object",
        .tp_methods = PyDapNotifySrvMethods
);

PyObject *wrapping_dap_notify_srv_send(PyObject *self, PyObject *args)
{
    (void)self;
    const char *data = NULL;
    if (!PyArg_ParseTuple(args, "s", &data))
        return NULL;
    if (!data)
        Py_RETURN_NONE;
    int rc = dap_notify_server_send_mt(data);
    return PyLong_FromLong(rc);
}

PyObject *wrapping_dap_notify_srv_sendf(PyObject *self, PyObject *args)
{
    (void)self;
    const char *fmt = NULL;
    PyObject *tupleArgs = NULL;
    if (!PyArg_ParseTuple(args, "sO", &fmt, &tupleArgs))
        return NULL;
    if (!fmt || !PyTuple_Check(tupleArgs)) {
        PyErr_SetString(PyExc_TypeError, "Expected (str, tuple) arguments");
        return NULL;
    }
    // Build formatted string using Python to avoid vsnprintf with va_list from Python
    PyObject *pyFmt = PyUnicode_FromString(fmt);
    if (!pyFmt) return NULL;
    PyObject *formatted = PyUnicode_Format(pyFmt, tupleArgs);
    Py_DECREF(pyFmt);
    if (!formatted) return NULL;
    PyObject *bytes = PyUnicode_AsUTF8String(formatted);
    Py_DECREF(formatted);
    if (!bytes) return NULL;
    const char *cstr = PyBytes_AsString(bytes);
    int rc = dap_notify_server_send_mt(cstr);
    Py_DECREF(bytes);
    return PyLong_FromLong(rc);
}


