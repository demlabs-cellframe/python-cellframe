#pragma once
#include <Python.h>
#include "dap_guuid.h"

typedef struct PyCryptoGUUID{
    PyObject_HEAD
    dap_guuid_t guuid;
} PyCryptoGUUIDObject;

extern PyTypeObject PyCryptoGUUIDObjectType;