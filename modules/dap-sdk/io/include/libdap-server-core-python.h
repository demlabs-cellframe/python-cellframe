#pragma once
#define PY_SSIZE_T_CLEAN
#include "Python.h"
#include "dap_server_python.h"
#include "dap_server.h"
#include "dap_events.h"
#include "dap_common.h"
#include "dap_events_python.h"
#include "dap_events_socket_python.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct SeverCore{
    PyObject_HEAD
}PyServerCoreObject;

int dap_io_init(uint32_t l_thread_cnt, size_t conn_t);
void dap_io_deinit(void);

PyObject *dap_io_loop(PyObject *self, PyObject *args);
PyObject *dap_io_listen(PyObject *self, PyObject *args);

extern PyTypeObject DapServerCoreObjectType;

#ifdef __cplusplus
}
#endif
