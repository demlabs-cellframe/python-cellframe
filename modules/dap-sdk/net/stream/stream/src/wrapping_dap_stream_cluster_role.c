#include "wrapping_dap_stream_cluster_role.h"
#include "libdap-python.h"

#define PVT(a) ((PyDapClusterRoleObject*)a)->role

PyObject *DapClusterBRoleObject_compare(PyObject *self, PyObject *other, int op) {
    if (!PyObject_TypeCheck(other, &DapClusterRoleObjectType)) {
        return Py_NotImplemented;
    }
    switch (op) {
        case Py_EQ:
            if (PVT(self) == PVT(other))
                Py_RETURN_TRUE;
            else
                Py_RETURN_FALSE;
            break;
        case Py_NE:
            if (PVT(self) != PVT(other))
                Py_RETURN_TRUE;
            else
                Py_RETURN_FALSE;
            break;
        default:
            return Py_NotImplemented;
    }
}

PyTypeObject DapClusterRoleObjectType = DAP_PY_TYPE_OBJECT("DAP.Network.ClusterRole", sizeof(PyDapClusterRoleObject),
                                                           "Class container for role cluster",
                                                           .tp_richcompare = DapClusterBRoleObject_compare);

static PyGetSetDef DapClusterRolesGetsSets[] = {
    {"INVALID", (getter)WR_CLUSTER_ROLE_INVALID, NULL, "", NULL},
    {"EMBEDDED", (getter)WR_CLUSTER_ROLE_EMBEDDED, NULL, "", NULL},
    {"AUTONOMIC", (getter)WR_CLUSTER_ROLE_AUTONOMIC, NULL, "", NULL},
    {"ISOLATED", (getter)WR_CLUSTER_ROLE_ISOLATED, NULL, "", NULL},
    {"VIRTUAL", (getter)WR_CLUSTER_ROLE_VIRTUAL, NULL, "", NULL},
    {}
};

PyObject *WR_CLUSTER_ROLE_INVALID(PyObject *self, void *closure){
    PyDapClusterRoleObject *obj = PyObject_New(PyDapClusterRoleObject, &DapClusterRoleObjectType);
    obj->role = DAP_CLUSTER_ROLE_INVALID;
    return (PyObject*)obj;
}
PyObject *WR_CLUSTER_ROLE_EMBEDDED(PyObject *self, void *closure){
    PyDapClusterRoleObject *obj = PyObject_New(PyDapClusterRoleObject, &DapClusterRoleObjectType);
    obj->role = DAP_CLUSTER_ROLE_EMBEDDED;
    return (PyObject*)obj;
}
PyObject *WR_CLUSTER_ROLE_AUTONOMIC(PyObject *self, void *closure){
    PyDapClusterRoleObject *obj = PyObject_New(PyDapClusterRoleObject, &DapClusterRoleObjectType);
    obj->role = DAP_CLUSTER_ROLE_AUTONOMIC;
    return (PyObject*)obj;
}
PyObject *WR_CLUSTER_ROLE_ISOLATED(PyObject *self, void *closure){
    PyDapClusterRoleObject *obj = PyObject_New(PyDapClusterRoleObject, &DapClusterRoleObjectType);
    obj->role = DAP_CLUSTER_ROLE_ISOLATED;
    return (PyObject*)obj;
}
PyObject *WR_CLUSTER_ROLE_VIRTUAL(PyObject *self, void *closure){
    PyDapClusterRoleObject *obj = PyObject_New(PyDapClusterRoleObject, &DapClusterRoleObjectType);
    obj->role = DAP_CLUSTER_ROLE_VIRTUAL;
    return (PyObject*)obj;
}

PyTypeObject DapClusterRolesObjectType = DAP_PY_TYPE_OBJECT("DAP.Netrowk.ClusterRoles", sizeof(DapClusterRolesGetsSets),
                                                            "Enum roles for cluster",
                                                            .tp_getset = DapClusterRolesGetsSets);