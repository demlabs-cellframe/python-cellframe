#include "wrapping_dap_stream_node_addr.h"
#include "dap_common.h"
#include "libdap-python.h"

PyObject* PyDapStreamNodeAddrObject_str(PyObject* self) {
    char *ret = dap_strdup_printf(NODE_ADDR_FP_STR, NODE_ADDR_FP_ARGS_S(((PyDapStreamNodeAddrObject*) self)->addr));
    PyObject *l_obj = Py_BuildValue("s", ret);
    DAP_FREE(ret);
    return l_obj;
}

PyTypeObject DapStreamNodeAddrObject = DAP_PY_TYPE_OBJECT("DAP.Network.StreamNodeAddr", sizeof(DapStreamNodeAddrObject),
                                                          "Stream node addr object", 
                                                          .tp_str = PyDapStreamNodeAddrObject_str);