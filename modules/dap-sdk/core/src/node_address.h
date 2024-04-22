#pragma once
#include <Python.h>
#include "dap_common.h"

typedef struct PyDapNodeAddr
{
    PyObject_HEAD
    dap_stream_node_addr_t addr;
}PyDapNodeAddrObject;

extern PyTypeObject DapNodeAddrObjectType;