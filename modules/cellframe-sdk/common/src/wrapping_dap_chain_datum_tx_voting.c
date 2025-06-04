#include "wrapping_dap_chain_datum_tx_voting.h"
#include "libdap-python.h"
#include "wrapping_dap_hash.h"
#include "datetime.h"

#define LOG_TAG "wrapping_voting"

//Voting
#define PVT_VOTING(a) ((PyDapChainTXVotingObject*)a)->voting

PyGetSetDef PyDapChainTxVotingGetSetDef[] = {
        {"question", wrapping_dap_chain_tx_voting_get_question, NULL, NULL, NULL},
        {"answers", wrapping_dap_chain_tx_voting_get_answers, NULL, NULL, NULL},
        {"maxCountVote", wrapping_dap_chain_tx_voting_get_max_count, NULL, NULL, NULL},
        {"expire", wrapping_dap_chain_tx_voting_get_expire, NULL, NULL, NULL},
        {"isDelegateKeyRequired", wrapping_dap_chain_tx_voting_get_is_delegate_key_required, NULL, NULL, NULL},
        {"isVoteChangingAllowed", wrapping_dap_chain_tx_voting_get_is_vote_changing_allowed, NULL, NULL, NULL},
        {}
};

PyObject *wrapping_dap_chain_tx_voting_get_question(PyObject *self, void *closure){
    (void)closure;
    return Py_BuildValue("s", PVT_VOTING(self)->voting_question);
}
PyObject *wrapping_dap_chain_tx_voting_get_answers(PyObject *self, void *closure){
    (void)closure;
    PyObject *obj_list = PyList_New(PVT_VOTING(self)->answers_count);
    for (uint64_t i = PVT_VOTING(self)->answers_count; --i;) {
        char *l_data = dap_list_nth_data(PVT_VOTING(self)->answers_list, i);
        PyObject *obj_str = Py_BuildValue("s", l_data);
        PyList_SetItem(obj_list, i, obj_str);
    }
    return obj_list;
}
PyObject *wrapping_dap_chain_tx_voting_get_max_count(PyObject *self, void *closure){
    (void)closure;
    return Py_BuildValue("k", PVT_VOTING(self)->votes_max_count);
}
PyObject *wrapping_dap_chain_tx_voting_get_expire(PyObject *self, void *closure){
    (void)closure;
    if (PVT_VOTING(self)->voting_expire) {
        PyDateTime_IMPORT;
        PyObject *obj_ts_long =  Py_BuildValue("(k)", PVT_VOTING(self)->voting_expire);
        PyObject *obj_ts = PyDateTime_FromTimestamp(obj_ts_long);
        return obj_ts;
    }
    Py_RETURN_NONE;
}
PyObject *wrapping_dap_chain_tx_voting_get_is_delegate_key_required(PyObject *self, void *closure){
    (void)closure;
    if (PVT_VOTING(self)->delegate_key_required)
        Py_RETURN_TRUE;
    else
        Py_RETURN_FALSE;
}
PyObject *wrapping_dap_chain_tx_voting_get_is_vote_changing_allowed(PyObject *self, void *closure){
    (void)closure;
    if (PVT_VOTING(self)->vote_changing_allowed)
        Py_RETURN_TRUE;
    else
        Py_RETURN_FALSE;
}

void PyDapChainTxVoting_dealloc(PyDapChainTXVotingObject *self) {
    PyTypeObject *tp = Py_TYPE(self);
    if (self->voting) {
        // Free voting question
        if (self->voting->voting_question) {
            DAP_DELETE(self->voting->voting_question);
        }
        
        // Free answers list and answer strings
        if (self->voting->answers_list) {
            dap_list_t *l_temp = self->voting->answers_list;
            while (l_temp) {
                if (l_temp->data) {
                    DAP_DELETE(l_temp->data);
                }
                l_temp = l_temp->next;
            }
            dap_list_free(self->voting->answers_list);
        }
        
        // Free the main voting structure
        DAP_DELETE(self->voting);
    }
    tp->tp_free(self);
}

PyTypeObject PyDapChainTxVotingObjectType = DAP_PY_TYPE_OBJECT(
        "CellFrame.Common.TxVoting",
        sizeof(PyDapChainTXVotingObject),
        "Wrapping item voting for transaction",
        .tp_dealloc = (destructor)PyDapChainTxVoting_dealloc,
        .tp_getset = PyDapChainTxVotingGetSetDef);

//Vote

PyGetSetDef PyDapChainTxVoteGetSetDef[] = {
        {"hash", (getter) wrapping_dap_chain_tx_vote_get_hash, NULL, NULL, NULL},
        {"answerIdx", (getter) wrapping_dap_chain_tx_vote_get_answer_idx, NULL, NULL, NULL},
        {}
};

PyObject *wrapping_dap_chain_tx_vote_get_hash(PyObject *self, void *closure){
    (void)closure;
    PyDapHashFastObject *obj_hf = PyObject_New(PyDapHashFastObject, &DapHashFastObjectType);
    if (!obj_hf) {
        log_it(L_CRITICAL, "Failed to create PyDapHashFastObject");
        return NULL;
    }
    
    obj_hf->hash_fast = DAP_NEW(dap_hash_fast_t);
    if (!obj_hf->hash_fast) {
        log_it(L_CRITICAL, "Memory allocation error for vote hash");
        Py_DECREF(obj_hf);
        return NULL;
    }
    
    memcpy(obj_hf->hash_fast, &((PyDapChainTXVoteObject*)self)->vote->voting_hash, sizeof(dap_hash_fast_t));
    obj_hf->origin = true;
    return (PyObject*)obj_hf;
}

PyObject *wrapping_dap_chain_tx_vote_get_answer_idx(PyObject *self, void *closure){
    (void)closure;
    return Py_BuildValue("k", ((PyDapChainTXVoteObject*)self)->vote->answer_idx);
}

void PyDapChainTxVote_dealloc(PyDapChainTXVoteObject *self) {
    // Note: vote data is typically owned by the transaction, so we don't free it here
    // unless it's explicitly marked as owned by this object
    Py_TYPE(self)->tp_free((PyObject*)self);
}

PyTypeObject PyDapChainTXVoteObjectType = DAP_PY_TYPE_OBJECT(
        "CellFrame.Chain.TxVote",
        sizeof(PyDapChainTXVoteObject), "Wrapping item vote for transaction",
        .tp_dealloc = (destructor)PyDapChainTxVote_dealloc,
        .tp_getset = PyDapChainTxVoteGetSetDef);