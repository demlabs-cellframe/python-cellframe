#pragma once
#include <Python.h>
#include "dap_http_client.h"

typedef struct PyDapHttpClient{
    PyObject_HEAD
    dap_http_client_t *client;
}PyDapHttpClientObject;

extern PyTypeObject DapHttpClientObjectType;