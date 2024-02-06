#include "wrapping_dap_chain_net_srv_vote_info.h"
#include "wrapping_dap_hash.h"

#define PVT(a) ((PyDapChainNetSrvVoteInfoObject*)a)->info
#define PVT_OPTION(a) ((PyDapChainNetSrvVoteInfoOptionObject*)a)->option

static PyGetSetDef DapChainNetSrvVoteInfoGetSet[] = {
        {"hash", wrapping_dap_chain_net_srv_vote_get_hash, NULL, NULL, NULL},
        {"question", wrapping_dap_chain_net_srv_vote_get_question, NULL, NULL, NULL},
        {"options", wrapping_dap_chain_net_srv_vote_get_options, NULL, NULL, NULL},
        {"expire", wrapping_dap_chain_net_srv_vote_get_expire_datetime, NULL, NULL, NULL},
        {"isDelegateKeyRequired", wrapping_dap_chain_net_srv_vote_get_is_delegate_key_required, NULL, NULL, NULL},
        {"isVoteChangingAllowed", wrapping_dap_chain_net_srv_vote_get_is_vote_changing_allowed, NULL, NULL, NULL},
        {}
};

void DapChainNetSrvVoteInfo_dealloc(PyDapChainNetSrvVoteInfoObject *self) {
    PyTypeObject *tp = Py_TYPE(self);
    DAP_DELETE(PVT(self)->options.options);
    tp->tp_free(self);
    Py_DECREF(self);
}

PyObject *wrapping_dap_chain_net_srv_vote_get_hash(PyObject *self, void *closure) {
    (void)closure;
    PyDapHashFastObject *obj = PyObject_New(PyDapHashFastObject, &DapHashFastObjectType);
    obj->hash_fast = DAP_NEW(dap_hash_fast_t);
    memcpy(obj->hash_fast, &PVT(self)->hash, sizeof(dap_hash_fast_t));
    return (PyObject*)obj;
}
PyObject *wrapping_dap_chain_net_srv_vote_get_question(PyObject *self, void *closure) {
    (void)closure;
    return Py_BuildValue("s", PVT(self)->question);
}
PyObject *wrapping_dap_chain_net_srv_vote_get_options(PyObject *self, void *closure) {
    (void)closure;
    if (PVT(self)->options.count_option == 0) {
        Py_RETURN_NONE;
    }
    PyObject *obj_list = PyList_New(PVT(self)->options.count_option);
    for (size_t i = PVT(self)->options.count_option; --i;) {
        PyDapChainNetSrvVoteInfoOptionObject *option  = PyObject_New(PyDapChainNetSrvVoteInfoOptionObject,
                                                                     &DapChainNetSrvVoteInfoObjectType);
        option->option = PVT(self)->options.options[i];
        PyList_SetItem(obj_list, i, (PyObject*)option);
    }
    return obj_list;
}
PyObject *wrapping_dap_chain_net_srv_vote_get_expire_datetime(PyObject *self, void *closure) {
    (void)closure;
    PyDateTime_IMPORT;
    PyObject *obj_ts_long =  Py_BuildValue("(k)",((PyDapChainDatumObject*)self)->datum->header.ts_create);
    PyObject *obj_ts = PyDateTime_FromTimestamp(obj_ts_long);
    return obj_ts;
}
PyObject *wrapping_dap_chain_net_srv_vote_get_is_delegate_key_required(PyObject *self, void *closure) {
    (void)closure;
    bool delegate_key = PVT(self)->is_delegate_key_required;
    if (delegate_key)
        Py_RETURN_TRUE;
    else
        Py_RETURN_FALSE;
}
PyObject *wrapping_dap_chain_net_srv_vote_get_is_vote_changing_allowed(PyObject *self, void *closure) {
    (void)closure;
    bool vote_changing_allowed = PVT(self)->is_changing_allowed;
    if (vote_changing_allowed)
        Py_RETURN_TRUE;
    else
        Py_RETURN_FALSE;
}

PyTypeObject DapChainNetSrvVoteInfoObjectType = DAP_PY_TYPE_OBJECT(
        "CellFrame.Services.VoteInfo",
        sizeof(PyDapChainNetSrvVoteInfoObject),
        "CellFrame.Services.VoteInfo",
        .tp_dealloc = (destructor)DapChainNetSrvVoteInfo_dealloc,
        .tp_getset = DapChainNetSrvVoteInfoGetSet);

static PyGetSetDef DapChainNetSrvVoteInfoOptionGetSet[] = {
        {"description", wrapping_dap_chain_net_srv_vote_option_get_description, NULL, NULL, NULL},
        {"votes", wrapping_dap_chain_net_srv_vote_option_get_votes, NULL, NULL, NULL},
        {"weights", wrapping_dap_chain_net_srv_vote_option_get_weights, NULL, NULL, NULL},
        {}
};

PyObject *wrapping_dap_chain_net_srv_vote_option_get_description(PyObject *self, void *closure) {
    (void)closure;
    return Py_BuildValue("s", PVT_OPTION(self).description);
}
PyObject *wrapping_dap_chain_net_srv_vote_option_get_votes(PyObject *self, void *closure) {
    (void)closure;
    return Py_BuildValue("k", PVT_OPTION(self).votes_count);
}
PyObject *wrapping_dap_chain_net_srv_vote_option_get_weights(PyObject *self, void *closure) {
    (void)closure;
    DapMathObject *obj_weights = PyObject_New(DapMathObject, &DapMathObjectType);
    obj_weights->value = PVT_OPTION(self).weight;
    return (PyObject*)obj_weights;
}

PyTypeObject DapChainNetSrvVoteInfoOptionObjectType = DAP_PY_TYPE_OBJECT(
        "CellFrame.Services.VoteInfoOption",
        sizeof(PyDapChainNetSrvVoteInfoOptionObject),
        "CellFrame.Services.VoteInfoOption",
        .tp_getset = DapChainNetSrvVoteInfoOptionGetSet);
