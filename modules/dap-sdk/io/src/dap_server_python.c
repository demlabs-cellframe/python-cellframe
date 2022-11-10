#include "libdap-python.h"
#include "dap_server_python.h"

PyTypeObject DapServerObjectType = DAP_PY_TYPE_OBJECT(
        "DAP.IO.Server", sizeof(PyDapServerObject),
        "DapServer objects");
