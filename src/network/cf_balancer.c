#include "cellframe.h"
#include "dap_chain_net_balancer.h"
#include "dap_chain_net_node_list.h"
#include "dap_chain_net.h"
#include "dap_http_status_code.h"

#define LOG_TAG "python_cellframe_net_balancer"

// =============================================================================
// BALANCER FUNCTIONS
// =============================================================================

/**
 * @brief Handshake with balancer
 * @param a_self Python self object (unused)
 * @param a_args Arguments (node_info capsule, net capsule)
 * @return Integer result
 */
PyObject* dap_chain_net_balancer_handshake_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_node_info_obj, *l_net_obj;
    
    if (!PyArg_ParseTuple(a_args, "OO", &l_node_info_obj, &l_net_obj)) {
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_node_info_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a node info capsule");
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_net_obj)) {
        PyErr_SetString(PyExc_TypeError, "Second argument must be a network capsule");
        return NULL;
    }
    
    dap_chain_node_info_t *l_node_info = (dap_chain_node_info_t *)PyCapsule_GetPointer(l_node_info_obj, "dap_chain_node_info_t");
    dap_chain_net_t *l_net = (dap_chain_net_t *)PyCapsule_GetPointer(l_net_obj, "dap_chain_net_t");
    
    if (!l_node_info || !l_net) {
        PyErr_SetString(PyExc_ValueError, "Invalid arguments");
        return NULL;
    }
    
    int l_res = dap_chain_net_balancer_handshake(l_node_info, l_net);
    return PyLong_FromLong(l_res);
}

/**
 * @brief Convert balancer type to string
 * @param a_self Python self object (unused)
 * @param a_args Arguments (type int)
 * @return String representation
 */
PyObject* dap_chain_net_balancer_type_to_str_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    int l_type = 0;

    if (!PyArg_ParseTuple(a_args, "i", &l_type)) {
        return NULL;
    }

    const char *l_str = dap_chain_net_balancer_type_to_str((dap_balancer_type_t)l_type);
    return PyUnicode_FromString(l_str ? l_str : "UNDEFINED");
}

/**
 * @brief Deinitialize balancer subsystem
 * @param a_self Python self object (unused)
 * @param a_args No arguments
 * @return None
 */
PyObject* dap_chain_net_balancer_deinit_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;

    dap_chain_net_balancer_deinit();
    Py_RETURN_NONE;
}

/**
 * @brief Handle HTTP balancer request
 * @param a_self Python self object (unused)
 * @param a_args Arguments (http_simple capsule, optional status capsule)
 * @return HTTP status code
 */
PyObject* dap_chain_net_balancer_http_issue_link_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_http_obj = NULL;
    PyObject *l_status_obj = Py_None;

    if (!PyArg_ParseTuple(a_args, "O|O", &l_http_obj, &l_status_obj)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_http_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a dap_http_simple_t capsule");
        return NULL;
    }

    dap_http_simple_t *l_http = (dap_http_simple_t *)PyCapsule_GetPointer(l_http_obj, "dap_http_simple_t");
    if (!l_http) {
        PyErr_SetString(PyExc_ValueError, "Invalid dap_http_simple_t capsule");
        return NULL;
    }

    dap_http_status_code_t l_status = DAP_HTTP_STATUS_OK;
    dap_http_status_code_t *l_status_ptr = &l_status;

    if (l_status_obj && l_status_obj != Py_None) {
        if (!PyCapsule_CheckExact(l_status_obj)) {
            PyErr_SetString(PyExc_TypeError, "Status must be a dap_http_status_code_t capsule or None");
            return NULL;
        }
        l_status_ptr = (dap_http_status_code_t *)PyCapsule_GetPointer(l_status_obj, "dap_http_status_code_t");
        if (!l_status_ptr) {
            PyErr_SetString(PyExc_ValueError, "Invalid dap_http_status_code_t capsule");
            return NULL;
        }
    }

    dap_chain_net_balancer_http_issue_link(l_http, l_status_ptr);
    return PyLong_FromLong((long)*l_status_ptr);
}

/**
 * @brief Issue DNS balancer link request
 * @param a_self Python self object (unused)
 * @param a_args Arguments (net_name string)
 * @return Dict with link info or None
 */
PyObject* dap_chain_net_balancer_dns_issue_link_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    const char *l_net_name = NULL;

    if (!PyArg_ParseTuple(a_args, "s", &l_net_name)) {
        return NULL;
    }

    dap_link_info_t *l_info = dap_chain_net_balancer_dns_issue_link(l_net_name);
    if (!l_info) {
        Py_RETURN_NONE;
    }

    PyObject *l_dict = Py_BuildValue("{s:K,s:s,s:H}",
                                     "node_addr", l_info->node_addr.uint64,
                                     "uplink_addr", l_info->uplink_addr,
                                     "uplink_port", l_info->uplink_port);
    DAP_DELETE(l_info);
    return l_dict;
}

/**
 * @brief Get balancer node list JSON string
 * @param a_self Python self object (unused)
 * @param a_args Arguments (net capsule)
 * @return JSON string or None
 */
PyObject* dap_chain_net_balancer_get_node_str_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_obj = NULL;

    if (!PyArg_ParseTuple(a_args, "O", &l_net_obj)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_net_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a network capsule");
        return NULL;
    }

    dap_chain_net_t *l_net = (dap_chain_net_t *)PyCapsule_GetPointer(l_net_obj, "dap_chain_net_t");
    if (!l_net) {
        PyErr_SetString(PyExc_ValueError, "Invalid network capsule");
        return NULL;
    }

    dap_json_t *l_json = dap_chain_net_balancer_get_node_str(l_net);
    if (!l_json) {
        Py_RETURN_NONE;
    }

    char *l_json_str = dap_json_to_string(l_json);
    dap_json_object_free(l_json);
    if (!l_json_str) {
        Py_RETURN_NONE;
    }

    PyObject *l_result = PyUnicode_FromString(l_json_str);
    DAP_DELETE(l_json_str);
    return l_result;
}

/**
 * @brief Issue balancer request for links
 * @param a_self Python self object (unused)
 * @param a_args Arguments (net capsule, host string, port int, type int)
 * @return None
 */
PyObject* dap_chain_net_balancer_request_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_obj = NULL;
    const char *l_host = NULL;
    int l_port = 0;
    int l_type = 0;

    if (!PyArg_ParseTuple(a_args, "Osii", &l_net_obj, &l_host, &l_port, &l_type)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_net_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a network capsule");
        return NULL;
    }

    dap_chain_net_t *l_net = (dap_chain_net_t *)PyCapsule_GetPointer(l_net_obj, "dap_chain_net_t");
    if (!l_net) {
        PyErr_SetString(PyExc_ValueError, "Invalid network capsule");
        return NULL;
    }

    if (l_port < 0 || l_port > 65535) {
        PyErr_SetString(PyExc_ValueError, "port must be between 0 and 65535");
        return NULL;
    }

    size_t l_host_len = l_host ? strlen(l_host) : 0;
    size_t l_alloc_size = sizeof(dap_balancer_link_request_t) + l_host_len + 1;
    dap_balancer_link_request_t *l_req = DAP_NEW_Z_SIZE(dap_balancer_link_request_t, l_alloc_size);
    if (!l_req) {
        PyErr_SetString(PyExc_MemoryError, "Failed to allocate balancer request");
        return NULL;
    }

    l_req->net = l_net;
    l_req->host_port = (uint16_t)l_port;
    l_req->type = (dap_balancer_type_t)l_type;
    if (l_host_len > 0) {
        char *l_host_buf = (char *)l_req + sizeof(dap_balancer_link_request_t);
        memcpy(l_host_buf, l_host, l_host_len + 1);
        l_req->host_addr = l_host_buf;
    }

    dap_chain_net_balancer_request(l_req);
    Py_RETURN_NONE;
}

// =============================================================================
// NODE LIST FUNCTIONS
// =============================================================================

/**
 * @brief Get node list GDB group name
 * @param a_self Python self object (unused)
 * @param a_args Arguments (net capsule)
 * @return Group name string or None
 */
PyObject* dap_chain_net_node_list_get_gdb_group_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_obj = NULL;

    if (!PyArg_ParseTuple(a_args, "O", &l_net_obj)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_net_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a network capsule");
        return NULL;
    }

    dap_chain_net_t *l_net = (dap_chain_net_t *)PyCapsule_GetPointer(l_net_obj, "dap_chain_net_t");
    if (!l_net) {
        PyErr_SetString(PyExc_ValueError, "Invalid network capsule");
        return NULL;
    }

    char *l_group = dap_chain_net_node_list_get_gdb_group(l_net);
    if (!l_group) {
        Py_RETURN_NONE;
    }

    PyObject *l_str = PyUnicode_FromString(l_group);
    DAP_DELETE(l_group);
    return l_str;
}

/**
 * @brief Handle HTTP node list request
 * @param a_self Python self object (unused)
 * @param a_args Arguments (http_simple capsule, optional status capsule)
 * @return HTTP status code
 */
PyObject* dap_chain_net_node_check_http_issue_link_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_http_obj = NULL;
    PyObject *l_status_obj = Py_None;

    if (!PyArg_ParseTuple(a_args, "O|O", &l_http_obj, &l_status_obj)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_http_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a dap_http_simple_t capsule");
        return NULL;
    }

    dap_http_simple_t *l_http = (dap_http_simple_t *)PyCapsule_GetPointer(l_http_obj, "dap_http_simple_t");
    if (!l_http) {
        PyErr_SetString(PyExc_ValueError, "Invalid dap_http_simple_t capsule");
        return NULL;
    }

    dap_http_status_code_t l_status = DAP_HTTP_STATUS_OK;
    dap_http_status_code_t *l_status_ptr = &l_status;

    if (l_status_obj && l_status_obj != Py_None) {
        if (!PyCapsule_CheckExact(l_status_obj)) {
            PyErr_SetString(PyExc_TypeError, "Status must be a dap_http_status_code_t capsule or None");
            return NULL;
        }
        l_status_ptr = (dap_http_status_code_t *)PyCapsule_GetPointer(l_status_obj, "dap_http_status_code_t");
        if (!l_status_ptr) {
            PyErr_SetString(PyExc_ValueError, "Invalid dap_http_status_code_t capsule");
            return NULL;
        }
    }

    dap_chain_net_node_check_http_issue_link(l_http, l_status_ptr);
    return PyLong_FromLong((long)*l_status_ptr);
}

/**
 * @brief Check node list for duplicate IP
 * @param a_self Python self object (unused)
 * @param a_args Arguments (node_info capsule, net capsule)
 * @return Node info capsule or None
 */
PyObject* dap_chain_node_list_ip_check_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_node_info_obj = NULL;
    PyObject *l_net_obj = NULL;

    if (!PyArg_ParseTuple(a_args, "OO", &l_node_info_obj, &l_net_obj)) {
        return NULL;
    }

    if (!PyCapsule_CheckExact(l_node_info_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a node info capsule");
        return NULL;
    }
    if (!PyCapsule_CheckExact(l_net_obj)) {
        PyErr_SetString(PyExc_TypeError, "Second argument must be a network capsule");
        return NULL;
    }

    dap_chain_node_info_t *l_node_info = (dap_chain_node_info_t *)PyCapsule_GetPointer(l_node_info_obj, "dap_chain_node_info_t");
    dap_chain_net_t *l_net = (dap_chain_net_t *)PyCapsule_GetPointer(l_net_obj, "dap_chain_net_t");
    if (!l_node_info || !l_net) {
        PyErr_SetString(PyExc_ValueError, "Invalid arguments");
        return NULL;
    }

    dap_chain_node_info_t *l_info = dap_chain_node_list_ip_check(l_node_info, l_net);
    if (!l_info) {
        Py_RETURN_NONE;
    }

    return PyCapsule_New(l_info, "dap_chain_node_info_t", NULL);
}

/**
 * @brief Request node list
 * @param a_self Python self object (unused)
 * @param a_args Arguments (net capsule, port int, sync bool, cmd char)
 * @return Integer result
 */
PyObject* dap_chain_net_node_list_request_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    PyObject *l_net_obj;
    int l_port;
    int l_sync;
    const char *l_cmd_str;
    
    if (!PyArg_ParseTuple(a_args, "Oiis", &l_net_obj, &l_port, &l_sync, &l_cmd_str)) {
        return NULL;
    }
    
    if (!PyCapsule_CheckExact(l_net_obj)) {
        PyErr_SetString(PyExc_TypeError, "First argument must be a network capsule");
        return NULL;
    }
    
    dap_chain_net_t *l_net = (dap_chain_net_t *)PyCapsule_GetPointer(l_net_obj, "dap_chain_net_t");
    if (!l_net) {
        PyErr_SetString(PyExc_ValueError, "Invalid network capsule");
        return NULL;
    }
    
    char l_cmd = l_cmd_str[0]; // Take first char
    
    int l_res = dap_chain_net_node_list_request(l_net, (uint16_t)l_port, (bool)l_sync, l_cmd);
    return PyLong_FromLong(l_res);
}

/**
 * @brief Initialize node list module
 * @param a_self Python self object (unused)
 * @param a_args No arguments
 * @return Integer result
 */
PyObject* dap_chain_net_node_list_init_py(PyObject *a_self, PyObject *a_args) {
    (void)a_self;
    (void)a_args;
    
    int l_res = dap_chain_net_node_list_init();
    return PyLong_FromLong(l_res);
}

// =============================================================================
// MODULE INITIALIZATION
// =============================================================================

/**
 * @brief Get method definitions for Net Balancer module
 */
PyMethodDef* cellframe_net_balancer_get_methods(void) {
    static PyMethodDef balancer_methods[] = {
        {"net_balancer_type_to_str", (PyCFunction)dap_chain_net_balancer_type_to_str_py, METH_VARARGS,
         "Convert balancer type to string"},
        {"net_balancer_deinit", (PyCFunction)dap_chain_net_balancer_deinit_py, METH_NOARGS,
         "Deinitialize balancer subsystem"},
        {"net_balancer_http_issue_link", (PyCFunction)dap_chain_net_balancer_http_issue_link_py, METH_VARARGS,
         "Handle HTTP balancer request"},
        {"net_balancer_dns_issue_link", (PyCFunction)dap_chain_net_balancer_dns_issue_link_py, METH_VARARGS,
         "Issue DNS balancer link request"},
        {"net_balancer_handshake", (PyCFunction)dap_chain_net_balancer_handshake_py, METH_VARARGS,
         "Handshake with balancer"},
        {"net_balancer_get_node_str", (PyCFunction)dap_chain_net_balancer_get_node_str_py, METH_VARARGS,
         "Get balancer node list JSON string"},
        {"net_balancer_request", (PyCFunction)dap_chain_net_balancer_request_py, METH_VARARGS,
         "Issue balancer request for links"},
        {"net_node_list_get_gdb_group", (PyCFunction)dap_chain_net_node_list_get_gdb_group_py, METH_VARARGS,
         "Get node list GDB group name"},
        {"net_node_check_http_issue_link", (PyCFunction)dap_chain_net_node_check_http_issue_link_py, METH_VARARGS,
         "Handle HTTP node list request"},
        {"net_node_list_request", (PyCFunction)dap_chain_net_node_list_request_py, METH_VARARGS,
         "Request node list"},
        {"net_node_list_ip_check", (PyCFunction)dap_chain_node_list_ip_check_py, METH_VARARGS,
         "Check node list for duplicate IP"},
        {"net_node_list_init", (PyCFunction)dap_chain_net_node_list_init_py, METH_VARARGS,
         "Initialize node list module"},
        
        {NULL, NULL, 0, NULL}  // Sentinel
    };
    return balancer_methods;
}
