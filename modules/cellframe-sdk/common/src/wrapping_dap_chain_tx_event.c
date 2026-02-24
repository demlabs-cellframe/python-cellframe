#include "wrapping_dap_chain_tx_event.h"
#include "dap_chain_datum_tx_event.h"
#include "dap_chain_net_srv_stake_ext.h"

#define LOG_TAG "wrapping_dap_chain_tx_event"

static PyGetSetDef DapChainTxEventGetsSetsDef[] = {
    {"type", (getter)wrapping_dap_chain_tx_event_get_type, NULL, NULL, NULL},
    {"version", (getter)wrapping_dap_chain_tx_event_get_version, NULL, NULL, NULL},
    {"group_name", (getter)wrapping_dap_chain_tx_event_get_group_name, NULL, NULL, NULL},
    {"group_size", (getter)wrapping_dap_chain_tx_event_get_group_size, NULL, NULL, NULL},
    {"event_type", (getter)wrapping_dap_chain_tx_event_get_event_type, NULL, NULL, NULL},
    {"timestamp", (getter)wrapping_dap_chain_tx_event_get_timestamp, NULL, NULL, NULL},
    {"srv_uid", (getter)wrapping_dap_chain_tx_event_get_srv_uid, NULL, NULL, NULL},
    {}
};

static PyMethodDef PyDapChainTxEventObjectMethods[] ={
    {"STAKE_EXT_STARTED", (PyCFunction)TX_EVENT_TYPE_STAKE_EXT_STARTED_PY, METH_NOARGS | METH_STATIC, ""},
    {"STAKE_EXT_BID_PLACED", (PyCFunction)TX_EVENT_TYPE_STAKE_EXT_BID_PLACED_PY, METH_NOARGS | METH_STATIC, ""},
    {"STAKE_EXT_ENDED", (PyCFunction)TX_EVENT_TYPE_STAKE_EXT_ENDED_PY, METH_NOARGS | METH_STATIC, ""},
    {"STAKE_EXT_CANCELLED", (PyCFunction)TX_EVENT_TYPE_STAKE_EXT_CANCELLED_PY, METH_NOARGS | METH_STATIC, ""},
    {}
};

PyTypeObject DapChainTxEventObjectType = DAP_PY_TYPE_OBJECT(
    "CellFrame.ChainTxEvent", sizeof(PyDapChainTxEventObject),
    "Chain tx event object",
    .tp_getset = DapChainTxEventGetsSetsDef,
    .tp_methods = PyDapChainTxEventObjectMethods,
    .tp_init = (initproc)PyDapChainTxEvent_init
);

int PyDapChainTxEvent_init(PyObject *self, PyObject *args, PyObject *kwds) {
    const char *kwlist[] = {
        "group_name",
        "event_type",
        "srv_uid",
        NULL
    };
    char *group_name;
    uint16_t event_type;
    uint64_t srv_uid;
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "sHH", (char **)kwlist, &group_name, &event_type, &srv_uid))
        return -1;
    ((PyDapChainTxEventObject*)self)->tx_event = dap_chain_datum_tx_event_create((dap_chain_net_srv_uid_t){.uint64 = srv_uid}, group_name, event_type);
    if (!((PyDapChainTxEventObject*)self)->tx_event)
        return -1;
    return 0;
}

PyObject *wrapping_dap_chain_tx_event_get_type(PyObject *self, void *closure) {
    (void)closure;
    return Py_BuildValue("b", ((PyDapChainTxEventObject*)self)->tx_event->type);
}

PyObject *wrapping_dap_chain_tx_event_get_version(PyObject *self, void *closure) {
    (void)closure;
    return Py_BuildValue("b", ((PyDapChainTxEventObject*)self)->tx_event->version);
}

PyObject *wrapping_dap_chain_tx_event_get_group_name(PyObject *self, void *closure) {
    (void)closure;
    return PyUnicode_FromString((char *)((PyDapChainTxEventObject*)self)->tx_event->group_name);
}

PyObject *wrapping_dap_chain_tx_event_get_group_size(PyObject *self, void *closure) {
    (void)closure;
    return Py_BuildValue("H", ((PyDapChainTxEventObject*)self)->tx_event->group_name_size);
}

PyObject *wrapping_dap_chain_tx_event_get_event_type(PyObject *self, void *closure) {
    (void)closure;
    return Py_BuildValue("H", ((PyDapChainTxEventObject*)self)->tx_event->event_type);
}

PyObject *wrapping_dap_chain_tx_event_get_timestamp(PyObject *self, void *closure) {
    (void)closure;
    return Py_BuildValue("L", ((PyDapChainTxEventObject*)self)->tx_event->timestamp);
}

PyObject *wrapping_dap_chain_tx_event_get_srv_uid(PyObject *self, void *closure) {
    (void)closure;
    return Py_BuildValue("k", ((PyDapChainTxEventObject*)self)->tx_event->srv_uid.uint64);
}

PyObject *TX_EVENT_TYPE_STAKE_EXT_STARTED_PY(PyObject *self, PyObject *args) {
    (void)self;
    (void)args;
    return PyLong_FromLong(DAP_CHAIN_TX_EVENT_TYPE_STAKE_EXT_STARTED);
}

PyObject *TX_EVENT_TYPE_STAKE_EXT_BID_PLACED_PY(PyObject *self, PyObject *args) {
    (void)self;
    (void)args;
    return PyLong_FromLong(DAP_CHAIN_TX_EVENT_TYPE_STAKE_EXT_LOCK_PLACED);
}

PyObject *TX_EVENT_TYPE_STAKE_EXT_ENDED_PY(PyObject *self, PyObject *args) {
    (void)self;
    (void)args;
    return PyLong_FromLong(DAP_CHAIN_TX_EVENT_TYPE_STAKE_EXT_ENDED);
}

PyObject *TX_EVENT_TYPE_STAKE_EXT_CANCELLED_PY(PyObject *self, PyObject *args) {
    (void)self;
    (void)args;
    return PyLong_FromLong(DAP_CHAIN_TX_EVENT_TYPE_STAKE_EXT_CANCELLED);
} 
