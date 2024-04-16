#include "node_address.h"
#include "libdap-python.h"

PyObject* PyDapNodeAddrObject_str(PyObject* self) {
    char *ret = dap_strdup_printf(NODE_ADDR_FP_STR, NODE_ADDR_FP_ARGS_S(((PyDapNodeAddrObject*) self)->addr));
    PyObject *l_obj = Py_BuildValue("s", ret);
    DAP_FREE(ret);
    return l_obj;
}

PyTypeObject DapNodeAddrObjectType = DAP_PY_TYPE_OBJECT("DAP.Core.NodeAddr", sizeof(PyDapNodeAddrObject),
                                                          "Stream node addr object", 
                                                          .tp_str = PyDapNodeAddrObject_str);