#ifndef _DAP_SERVER_PYTHON_
#define _DAP_SERVER_PYTHON_
#include "Python.h"
#include "dap_server.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct PyDapServer {
    PyObject_HEAD
    dap_server_t *t_server;
}PyDapServerObject;

extern PyTypeObject dapServer_dapServerType;

#ifdef __cplusplus
}
#endif
#endif //_DAP_SERVER_PYTHON_
