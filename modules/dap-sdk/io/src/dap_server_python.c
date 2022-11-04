#include "libdap-python.h"
#include "dap_server_python.h"

PyTypeObject dapServer_dapServerType = DAP_PY_TYPE_OBJECT(
        "DAP.IO.Server", sizeof(PyDapServerObject),
        "DapServer objects");
