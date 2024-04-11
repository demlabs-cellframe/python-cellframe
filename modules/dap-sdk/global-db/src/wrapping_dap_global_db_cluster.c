#include "libdap-python.h"
#include "wrapping_guuid.h"
#include "wrapping_dap_global_db_cluster.h"
#include "wrapping_dap_global_db_instance.h"
#include "wrapping_dap_global_db_role.h"
#include "wrapping_dap_cluster_member.h"


static PyMethodDef DapGlobalDBClusterMethods[] = {
    {"byGroup", (PyCFunction)wrapping_dap_global_db_cluster_by_group, METH_VARARGS | METH_STATIC, ""},
    {"memberAdd", (PyCFunction)wrapping_dap_global_db_cluster_member_add, METH_VARARGS, ""},
    {NULL, NULL, 0, NULL}
};

static PyGetSetDef DapGlobalDBCluseterGetSets[] = {
    {}
};

int DapGlobalDBCluster_init(PyGlobalDBClusterObject *self, PyObject *argv, PyObject *kwds){
    const char **kwlist = {
        "dbi",
        "goup_name_or_mnemonium",
        "GUID",
        "group_mask"
        "TTL",
        "owner_root_access",
        "defaultRole",
        "clusterRole",
        NULL
    };
    PyObject *dbi;
    const char *gn_or_mnemonuim;
    PyObject *guid = NULL;
    const char *group_mask;
    uint32_t ttl;
    bool owner_root_access;
    PyObject *defaultRole;
    PyObject *clusterRole;
    if (!PyArg_ParseTupleAndKeywords(argv, kwds, "Os|OsIpOO", kwlist, &dbi, &gn_or_mnemonuim, &guid, &group_mask, &ttl, &owner_root_access, &defaultRole, &clusterRole))
        return -1;
    if (!PyObject_TypeCheck(dbi, &DapGlobalDBInstanceObjectType)){
        PyErr_SetString(PyExc_Exception, "The first argument is incorrect, it should be an instance of the DAP.GlobalDB.Instance class.");
        return -1;
    }
    if (!PyObject_TypeCheck(guid, &PyCryptoGUUIDObjectType)){
        PyErr_SetString(PyExc_Exception, "The third argument is incorrect, it should be an instance of the DAP.Crypto.GUUID class.");
        return -1;
    }
    if (getuid == NULL) {
        self->cluster = dap_global_db_cluster_by_group(((PyDapGlobalDBInstanceObject*)dbi)->instance, gn_or_mnemonuim);
        return 0;
    } else {
        if (!PyObject_TypeCheck(defaultRole, &DapGlobalDBRoleObjectType)) {
            PyErr_SetString(PyExc_Exception, "The seventh argument is incorrect, it should be an instance of the DAP.GlobalDB.Role class. To get it, use the DAP.GlobalDB.Roles enumeration.");
            return -1;
        }
        if (!PyObject_TypeCheck(clusterRole, &DapClusterRoleObjectType)) {
            PyErr_SetString(PyExc_Exception, "The eighth argument is incorrect, it should be an instance of the DAP.Network.ClusterRole class. To obtain it, use the DAP.Network.ClusterRoles enumeration.");
            return -1;
        }
        self->cluster = dap_global_db_cluster_add(((PyDapGlobalDBInstanceObject*)dbi)->instance, gn_or_mnemonuim, 
                                                  ((PyCryptoGUUIDObject*)guid)->guuid, group_mask, ttl, owner_root_access,
                                                  ((PyGlobalDBRoleObject*)defaultRole)->role, ((PyDapClusterRoleObject*)clusterRole)->role);
    }
    return 0;
}

PyObject *wrapping_dap_global_db_cluster_by_group(PyObject *self, PyObject *argv){
    PyObject *obj_instance;
    const char group;
    if (!PyArg_ParseTuple(argv, "Os", &obj_instance, &group))
        return NULL;
    dap_global_db_instance_t *l_instance = ((PyDapGlobalDBInstanceObject*)self)->instance;
    return NULL;
}

PyObject *wrapping_dap_global_db_cluster_member_add(PyObject *self, PyObject *argv) {
    dap_global_db_cluster_t *l_cluster = ((PyGlobalDBClusterObject*)self)->cluster;
    PyObject *obj_node_addr, *obj_role;
    if (!PyArg_ParseTuple(argv, "OO", obj_node_addr, obj_role)) {
        return NULL;
    }
    if (!PyObject_TypeCheck(obj_node_addr, &DapStreamNodeAddrObject)) {
        PyErr_SetString(PyExc_Exception, "The first argument must be an instance of the DAP.Network.StreamNodeAddr object");
        return NULL;
    }
    if (!PyObject_TypeCheck(obj_role, &DapGlobalDBRoleObjectType)) {
        PyErr_SetString(PyExc_Exception, "The second argument must be an instance of the DAP.GlobalDB.ClusterRole object.");
        return NULL;
    }
    dap_cluster_member_t *l_member = dap_global_db_cluster_member_add(l_cluster, 
                                    &((PyDapStreamNodeAddrObject*)obj_node_addr)->addr, 
                                    ((PyGlobalDBRoleObject*)obj_role)->role);
    if (!l_member) {
        Py_RETURN_NONE;
    }
    PyDapClusterMemberObject *obj_member = PyObject_New(PyDapClusterMemberObject, &DapClusterMemberObjectType);
    obj_member->member = l_member;
    return (PyObject*)obj_member;
}

PyTypeObject PyDaoGlobalDBClusterObjectType = DAP_PY_TYPE_OBJECT(
        "DAP.GlobalDB.Cluster", sizeof(PyGlobalDBClusterObject),
        "GlobalDB.Cluster object",
        .tp_methods = DapGlobalDBClusterMethods,
        .tp_init = (initproc)DapGlobalDBCluster_init
);