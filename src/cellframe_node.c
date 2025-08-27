#include "python_cellframe.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// External error object from main module
extern PyObject *CellframeError;

// Include DAP SDK node headers
#include "dap_chain_node.h"
#include "dap_chain_node_cli.h"

// Node type implementation
PyTypeObject PyCellframeNodeType = {0};

PyObject* PyCellframeNode_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
    PyCellframeNode *self;
    self = (PyCellframeNode *)type->tp_alloc(type, 0);
    if (self != NULL) {
        self->node_ptr = NULL;
        self->node_addr = NULL;
        self->node_port = 0;
        self->is_initialized = false;
    }
    return (PyObject *)self;
}

int PyCellframeNode_init(PyCellframeNode *self, PyObject *args, PyObject *kwds) {
    const char *node_addr = "0.0.0.0";
    int node_port = 8079;
    
    static char *kwlist[] = {"addr", "port", NULL};
    
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "|si", kwlist, &node_addr, &node_port)) {
        return -1;
    }
    
    if (node_addr) {
        self->node_addr = strdup(node_addr);
        if (!self->node_addr) {
            PyErr_SetString(CellframeError, "Failed to allocate memory for node address");
            return -1;
        }
    }
    
    self->node_port = (uint16_t)node_port;
    self->is_initialized = true;
    
    return 0;
}

void PyCellframeNode_dealloc(PyCellframeNode *self) {
    if (self) {
        if (self->node_addr) {
            free(self->node_addr);
            self->node_addr = NULL;
        }
        // Note: node_ptr is managed by DAP SDK, don't free it
        Py_TYPE(self)->tp_free((PyObject *)self);
    }
}

PyObject* PyCellframeNode_start(PyCellframeNode *self, PyObject *args) {
    if (!self || !self->is_initialized) {
        PyErr_SetString(CellframeError, "Node not initialized");
        return NULL;
    }
    
    // For now, just mark as started (node management is complex)
    // In real implementation, this would start the node services
    
    PyObject *result_dict = PyDict_New();
    if (!result_dict) {
        return NULL;
    }
    
    PyObject *status_obj = PyUnicode_FromString("started");
    if (status_obj) {
        PyDict_SetItemString(result_dict, "status", status_obj);
        Py_DECREF(status_obj);
    }
    
    PyObject *addr_obj = PyUnicode_FromString(self->node_addr ? self->node_addr : "unknown");
    if (addr_obj) {
        PyDict_SetItemString(result_dict, "address", addr_obj);
        Py_DECREF(addr_obj);
    }
    
    PyObject *port_obj = PyLong_FromLong(self->node_port);
    if (port_obj) {
        PyDict_SetItemString(result_dict, "port", port_obj);
        Py_DECREF(port_obj);
    }
    
    return result_dict;
}

PyObject* PyCellframeNode_stop(PyCellframeNode *self, PyObject *args) {
    if (!self || !self->is_initialized) {
        PyErr_SetString(CellframeError, "Node not initialized");
        return NULL;
    }
    
    // For now, just mark as stopped
    // In real implementation, this would stop the node services
    
    PyObject *result_dict = PyDict_New();
    if (!result_dict) {
        return NULL;
    }
    
    PyObject *status_obj = PyUnicode_FromString("stopped");
    if (status_obj) {
        PyDict_SetItemString(result_dict, "status", status_obj);
        Py_DECREF(status_obj);
    }
    
    return result_dict;
}

PyObject* PyCellframeNode_get_status(PyCellframeNode *self, PyObject *args) {
    if (!self) {
        PyErr_SetString(CellframeError, "Invalid node object");
        return NULL;
    }
    
    PyObject *status_dict = PyDict_New();
    if (!status_dict) {
        return NULL;
    }
    
    // Add initialization status
    PyObject *init_obj = PyBool_FromLong(self->is_initialized ? 1 : 0);
    if (init_obj) {
        PyDict_SetItemString(status_dict, "initialized", init_obj);
        Py_DECREF(init_obj);
    }
    
    // Add node address
    PyObject *addr_obj = PyUnicode_FromString(self->node_addr ? self->node_addr : "not_set");
    if (addr_obj) {
        PyDict_SetItemString(status_dict, "address", addr_obj);
        Py_DECREF(addr_obj);
    }
    
    // Add node port
    PyObject *port_obj = PyLong_FromLong(self->node_port);
    if (port_obj) {
        PyDict_SetItemString(status_dict, "port", port_obj);
        Py_DECREF(port_obj);
    }
    
    // Add node state (simplified for now)
    PyObject *state_obj = PyUnicode_FromString(self->is_initialized ? "ready" : "not_ready");
    if (state_obj) {
        PyDict_SetItemString(status_dict, "state", state_obj);
        Py_DECREF(state_obj);
    }
    
    return status_dict;
} 