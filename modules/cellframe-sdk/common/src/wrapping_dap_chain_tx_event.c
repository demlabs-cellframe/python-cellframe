#include "wrapping_dap_chain_tx_event.h"
#include "dap_chain_datum_tx_event.h"

#define LOG_TAG "wrapping_dap_chain_tx_event"

static PyGetSetDef DapChainTxEventGetsSetsDef[] = {
    {"type", (getter)wrapping_dap_chain_tx_event_get_type, NULL, NULL, NULL},
    {"version", (getter)wrapping_dap_chain_tx_event_get_version, NULL, NULL, NULL},
    {"group_name", (getter)wrapping_dap_chain_tx_event_get_group_name, NULL, NULL, NULL},
    {"group_size", (getter)wrapping_dap_chain_tx_event_get_group_size, NULL, NULL, NULL},
    {"event_type", (getter)wrapping_dap_chain_tx_event_get_event_type, NULL, NULL, NULL},
    {"timestamp", (getter)wrapping_dap_chain_tx_event_get_timestamp, NULL, NULL, NULL},
    {}
};

static PyObject *TX_EVENT_DATA_AUCTION_STARTED_CREATE_PY(PyObject *self, PyObject *args)
{
    (void)self;
    unsigned int multiplier;
    unsigned long long duration; // dap_time_t
    unsigned int time_unit; // dap_chain_tx_event_data_time_unit_t
    unsigned int calculation_rule_id;
    PyObject *py_projects = NULL;
    if (!PyArg_ParseTuple(args, "IKII|O", &multiplier, &duration, &time_unit, &calculation_rule_id, &py_projects))
        return NULL;

    uint8_t projects_cnt = 0;
    uint32_t *project_ids = NULL;

    if (py_projects && py_projects != Py_None) {
        if (!PyList_Check(py_projects) && !PyTuple_Check(py_projects)) {
            PyErr_SetString(PyExc_TypeError, "project_ids must be a list or tuple of integers");
            return NULL;
        }
        Py_ssize_t n = PySequence_Size(py_projects);
        if (n < 0)
            return NULL;
        if (n > 255) {
            PyErr_SetString(PyExc_ValueError, "project_ids length must be <= 255");
            return NULL;
        }
        projects_cnt = (uint8_t)n;
        if (projects_cnt) {
            project_ids = (uint32_t *)DAP_NEW_Z_COUNT(uint32_t, projects_cnt);
            if (!project_ids) {
                PyErr_SetString(PyExc_MemoryError, "Allocation failed");
                return NULL;
            }
            for (Py_ssize_t i = 0; i < n; i++) {
                PyObject *item = PySequence_GetItem(py_projects, i);
                if (!item) { DAP_DELETE(project_ids); return NULL; }
                unsigned long val = PyLong_AsUnsignedLong(item);
                Py_DECREF(item);
                if (PyErr_Occurred()) { DAP_DELETE(project_ids); return NULL; }
                project_ids[i] = (uint32_t)val;
            }
        }
    }

    size_t data_size = 0;
    byte_t *data = dap_chain_tx_event_data_auction_started_create(
        &data_size, (uint32_t)multiplier, (dap_time_t)duration, (dap_chain_tx_event_data_time_unit_t)time_unit,
        (uint32_t)calculation_rule_id, projects_cnt, (uint32_t *)project_ids);

    if (project_ids)
        DAP_DELETE(project_ids);

    if (!data) {
        Py_RETURN_NONE;
    }

    PyObject *py_bytes = PyBytes_FromStringAndSize((const char *)data, (Py_ssize_t)data_size);
    DAP_DELETE(data);
    return py_bytes;
}

static PyMethodDef PyDapChainTxEventObjectMethods[] ={
    {"AUCTION_STARTED", (PyCFunction)TX_EVENT_TYPE_AUCTION_STARTED_PY, METH_NOARGS | METH_STATIC, ""},
    {"AUCTION_BID_PLACED", (PyCFunction)TX_EVENT_TYPE_AUCTION_BID_PLACED_PY, METH_NOARGS | METH_STATIC, ""},
    {"AUCTION_ENDED", (PyCFunction)TX_EVENT_TYPE_AUCTION_ENDED_PY, METH_NOARGS | METH_STATIC, ""},
    {"AUCTION_CANCELLED", (PyCFunction)TX_EVENT_TYPE_AUCTION_CANCELLED_PY, METH_NOARGS | METH_STATIC, ""},
    {"auction_started_create", (PyCFunction)TX_EVENT_DATA_AUCTION_STARTED_CREATE_PY, METH_VARARGS | METH_STATIC, "Create auction_started event data bytes"},
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
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "sH", (char **)kwlist, &group_name, &event_type))
        return -1;
    ((PyDapChainTxEventObject*)self)->tx_event = dap_chain_datum_tx_event_create(group_name, event_type);
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

PyObject *TX_EVENT_TYPE_AUCTION_ENDED_PY(PyObject *self, PyObject *args) {
    (void)self;
    (void)args;
    return PyLong_FromLong(DAP_CHAIN_TX_EVENT_TYPE_AUCTION_ENDED);
}

PyObject *TX_EVENT_TYPE_AUCTION_CANCELLED_PY(PyObject *self, PyObject *args) {
    (void)self;
    (void)args;
    return PyLong_FromLong(DAP_CHAIN_TX_EVENT_TYPE_AUCTION_CANCELLED);
} 