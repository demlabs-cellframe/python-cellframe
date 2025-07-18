#include "wrapping_dap_chain_tx_event.h"

#define LOG_TAG "wrapping_dap_chain_tx_event"

static PyGetSetDef DapChainTxEventGetsSetsDef[] = {
    {"type", (getter)wrapping_dap_chain_tx_event_get_type, NULL, NULL, NULL},
    {"version", (getter)wrapping_dap_chain_tx_event_get_version, NULL, NULL, NULL},
    {"group_name", (getter)wrapping_dap_chain_tx_event_get_group_name, NULL, NULL, NULL},
    {"group_size", (getter)wrapping_dap_chain_tx_event_get_group_size, NULL, NULL, NULL},
    {"event_type", (getter)wrapping_dap_chain_tx_event_get_event_type, NULL, NULL, NULL},
    {}
};

static PyMethodDef PyDapChainTxEventObjectMethods[] ={
    {"AUCTION_STARTED", (PyCFunction)TX_EVENT_TYPE_AUCTION_STARTED_PY, METH_NOARGS | METH_STATIC, ""},
    {"AUCTION_BID_PLACED", (PyCFunction)TX_EVENT_TYPE_AUCTION_BID_PLACED_PY, METH_NOARGS | METH_STATIC, ""},
    {"AUCTION_WON", (PyCFunction)TX_EVENT_TYPE_AUCTION_WON_PY, METH_NOARGS | METH_STATIC, ""},
    {"AUCTION_LOST", (PyCFunction)TX_EVENT_TYPE_AUCTION_LOST_PY, METH_NOARGS | METH_STATIC, ""},
    {"AUCTION_CANCELLED", (PyCFunction)TX_EVENT_TYPE_AUCTION_CANCELLED_PY, METH_NOARGS | METH_STATIC, ""},
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
        NULL
    };
    char *group_name;
    uint16_t event_type;
    
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "sH", (char **)kwlist, &group_name, &event_type)) {
        return -1;
    }
    
    ((PyDapChainTxEventObject*)self)->tx_event = dap_chain_datum_tx_event_create(group_name, event_type);
    if (!((PyDapChainTxEventObject*)self)->tx_event) {
        return -1;
    }
    
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
    return Py_BuildValue("H", ((PyDapChainTxEventObject*)self)->tx_event->group_size);
}

PyObject *wrapping_dap_chain_tx_event_get_event_type(PyObject *self, void *closure) {
    (void)closure;
    return Py_BuildValue("H", ((PyDapChainTxEventObject*)self)->tx_event->event_type);
}

PyObject *TX_EVENT_TYPE_AUCTION_STARTED_PY(PyObject *self, PyObject *args) {
    (void)self;
    (void)args;
    return PyLong_FromLong(DAP_CHAIN_TX_EVENT_TYPE_AUCTION_STARTED);
}

PyObject *TX_EVENT_TYPE_AUCTION_BID_PLACED_PY(PyObject *self, PyObject *args) {
    (void)self;
    (void)args;
    return PyLong_FromLong(DAP_CHAIN_TX_EVENT_TYPE_AUCTION_BID_PLACED);
}

PyObject *TX_EVENT_TYPE_AUCTION_WON_PY(PyObject *self, PyObject *args) {
    (void)self;
    (void)args;
    return PyLong_FromLong(DAP_CHAIN_TX_EVENT_TYPE_AUCTION_WON);
}

PyObject *TX_EVENT_TYPE_AUCTION_LOST_PY(PyObject *self, PyObject *args) {
    (void)self;
    (void)args;
    return PyLong_FromLong(DAP_CHAIN_TX_EVENT_TYPE_AUCTION_LOST);
}

PyObject *TX_EVENT_TYPE_AUCTION_CANCELLED_PY(PyObject *self, PyObject *args) {
    (void)self;
    (void)args;
    return PyLong_FromLong(DAP_CHAIN_TX_EVENT_TYPE_AUCTION_CANCELLED);
} 