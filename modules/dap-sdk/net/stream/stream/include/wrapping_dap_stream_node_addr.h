#pragma once
#include <Python.h>
#include "dap_common.h"

typedef struct PyDapStreamNodeAddr
{
    PyObject_HEAD
    dap_stream_node_addr_t addr;
}PyDapStreamNodeAddrObject;

extern PyTypeObject DapStreamNodeAddrObject;
