#include "cf_datum_internal.h"
#include "dap_chain_datum_tx_voting.h"
#include "dap_json.h"
PyObject *dap_chain_datum_tx_item_vote_create_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *hash_obj = NULL;
    unsigned long long answer_idx = 0;

    if (!PyArg_ParseTuple(args, "OK", &hash_obj, &answer_idx)) {
        return NULL;
    }

    dap_hash_fast_t voting_hash = {0};
    if (cf_datum_parse_hash_fast(hash_obj, &voting_hash) != 0) {
        return NULL;
    }

    uint64_t answer_idx_value = (uint64_t)answer_idx;
    dap_chain_tx_vote_t *item = dap_chain_datum_tx_item_vote_create(&voting_hash, &answer_idx_value);
    if (!item) {
        Py_RETURN_NONE;
    }

    return PyCapsule_New(item, "dap_chain_tx_vote_t", NULL);
}

PyObject *dap_chain_datum_tx_item_vote_to_json_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *vote_obj = NULL;
    int version = 0;

    if (!PyArg_ParseTuple(args, "Oi", &vote_obj, &version)) {
        return NULL;
    }

    dap_chain_tx_vote_t *vote = (dap_chain_tx_vote_t *)cf_datum_get_capsule(
        vote_obj, "dap_chain_tx_vote_t", "First argument must be a tx vote capsule"
    );
    if (!vote) {
        return NULL;
    }

    dap_json_t *json = dap_chain_datum_tx_item_vote_to_json(vote, version);
    if (!json) {
        Py_RETURN_NONE;
    }

    return PyCapsule_New(json, "dap_json_t", NULL);
}

PyObject *dap_chain_datum_tx_item_voting_create_py(PyObject *self, PyObject *args) {
    (void)self;
    if (!PyArg_ParseTuple(args, "")) {
        return NULL;
    }

    dap_chain_tx_voting_t *item = dap_chain_datum_tx_item_voting_create();
    if (!item) {
        Py_RETURN_NONE;
    }

    return PyCapsule_New(item, "dap_chain_tx_voting_t", NULL);
}

PyObject *dap_chain_datum_tx_item_voting_tsd_to_json_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *tx_obj = NULL;
    int version = 0;

    if (!PyArg_ParseTuple(args, "Oi", &tx_obj, &version)) {
        return NULL;
    }

    dap_chain_datum_tx_t *tx = (dap_chain_datum_tx_t *)cf_datum_get_capsule(
        tx_obj, "dap_chain_datum_tx_t", "First argument must be a tx datum capsule"
    );
    if (!tx) {
        return NULL;
    }

    dap_json_t *json = dap_chain_datum_tx_item_voting_tsd_to_json(tx, version);
    if (!json) {
        Py_RETURN_NONE;
    }

    return PyCapsule_New(json, "dap_json_t", NULL);
}

PyObject *dap_chain_datum_tx_voting_get_answer_text_by_idx_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *tx_obj = NULL;
    unsigned long long answer_idx = 0;

    if (!PyArg_ParseTuple(args, "OK", &tx_obj, &answer_idx)) {
        return NULL;
    }

    dap_chain_datum_tx_t *tx = (dap_chain_datum_tx_t *)cf_datum_get_capsule(
        tx_obj, "dap_chain_datum_tx_t", "First argument must be a tx datum capsule"
    );
    if (!tx) {
        return NULL;
    }

    char *answer = dap_chain_datum_tx_voting_get_answer_text_by_idx(tx, (uint64_t)answer_idx);
    if (!answer) {
        Py_RETURN_NONE;
    }

    PyObject *answer_obj = PyUnicode_FromString(answer);
    DAP_DELETE(answer);
    return answer_obj;
}

PyObject *dap_chain_datum_tx_voting_params_delete_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *params_obj = NULL;

    if (!PyArg_ParseTuple(args, "O", &params_obj)) {
        return NULL;
    }

    dap_chain_datum_tx_voting_params_t *params = (dap_chain_datum_tx_voting_params_t *)cf_datum_get_capsule(
        params_obj, "dap_chain_datum_tx_voting_params_t", "First argument must be a voting params capsule"
    );
    if (!params) {
        return NULL;
    }

    dap_chain_datum_tx_voting_params_delete(params);
    Py_RETURN_NONE;
}

PyObject *dap_chain_datum_tx_voting_parse_tsd_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *tx_obj = NULL;

    if (!PyArg_ParseTuple(args, "O", &tx_obj)) {
        return NULL;
    }

    dap_chain_datum_tx_t *tx = (dap_chain_datum_tx_t *)cf_datum_get_capsule(
        tx_obj, "dap_chain_datum_tx_t", "First argument must be a tx datum capsule"
    );
    if (!tx) {
        return NULL;
    }

    dap_chain_datum_tx_voting_params_t *params = dap_chain_datum_tx_voting_parse_tsd(tx);
    if (!params) {
        Py_RETURN_NONE;
    }

    return PyCapsule_New(params, "dap_chain_datum_tx_voting_params_t", NULL);
}

PyObject *dap_chain_datum_voting_answer_tsd_create_py(PyObject *self, PyObject *args) {
    (void)self;
    const char *answer = NULL;
    Py_ssize_t answer_len = 0;

    if (!PyArg_ParseTuple(args, "s#", &answer, &answer_len)) {
        return NULL;
    }
    if (answer_len <= 0) {
        PyErr_SetString(PyExc_ValueError, "answer must be non-empty");
        return NULL;
    }

    dap_chain_tx_tsd_t *tsd = dap_chain_datum_voting_answer_tsd_create(answer, (size_t)answer_len);
    if (!tsd) {
        Py_RETURN_NONE;
    }

    return PyCapsule_New(tsd, "dap_chain_tx_tsd_t", NULL);
}

PyObject *dap_chain_datum_voting_cancel_tsd_create_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *hash_obj = NULL;

    if (!PyArg_ParseTuple(args, "O", &hash_obj)) {
        return NULL;
    }

    dap_hash_fast_t voting_hash = {0};
    if (cf_datum_parse_hash_fast(hash_obj, &voting_hash) != 0) {
        return NULL;
    }

    dap_chain_tx_tsd_t *tsd = dap_chain_datum_voting_cancel_tsd_create(voting_hash);
    if (!tsd) {
        Py_RETURN_NONE;
    }

    return PyCapsule_New(tsd, "dap_chain_tx_tsd_t", NULL);
}

PyObject *dap_chain_datum_voting_delegated_key_required_tsd_create_py(PyObject *self, PyObject *args) {
    (void)self;
    int delegated_required = 0;

    if (!PyArg_ParseTuple(args, "p", &delegated_required)) {
        return NULL;
    }

    dap_chain_tx_tsd_t *tsd = dap_chain_datum_voting_delegated_key_required_tsd_create(
        delegated_required ? true : false
    );
    if (!tsd) {
        Py_RETURN_NONE;
    }

    return PyCapsule_New(tsd, "dap_chain_tx_tsd_t", NULL);
}

PyObject *dap_chain_datum_voting_expire_tsd_create_py(PyObject *self, PyObject *args) {
    (void)self;
    unsigned long long expire = 0;

    if (!PyArg_ParseTuple(args, "K", &expire)) {
        return NULL;
    }

    dap_chain_tx_tsd_t *tsd = dap_chain_datum_voting_expire_tsd_create((dap_time_t)expire);
    if (!tsd) {
        Py_RETURN_NONE;
    }

    return PyCapsule_New(tsd, "dap_chain_tx_tsd_t", NULL);
}

PyObject *dap_chain_datum_voting_max_votes_count_tsd_create_py(PyObject *self, PyObject *args) {
    (void)self;
    unsigned long long max_count = 0;

    if (!PyArg_ParseTuple(args, "K", &max_count)) {
        return NULL;
    }

    dap_chain_tx_tsd_t *tsd = dap_chain_datum_voting_max_votes_count_tsd_create((uint64_t)max_count);
    if (!tsd) {
        Py_RETURN_NONE;
    }

    return PyCapsule_New(tsd, "dap_chain_tx_tsd_t", NULL);
}

PyObject *dap_chain_datum_voting_question_tsd_create_py(PyObject *self, PyObject *args) {
    (void)self;
    const char *question = NULL;
    Py_ssize_t question_len = 0;

    if (!PyArg_ParseTuple(args, "s#", &question, &question_len)) {
        return NULL;
    }
    if (question_len <= 0) {
        PyErr_SetString(PyExc_ValueError, "question must be non-empty");
        return NULL;
    }

    dap_chain_tx_tsd_t *tsd = dap_chain_datum_voting_question_tsd_create(question, (size_t)question_len);
    if (!tsd) {
        Py_RETURN_NONE;
    }

    return PyCapsule_New(tsd, "dap_chain_tx_tsd_t", NULL);
}

PyObject *dap_chain_datum_voting_token_tsd_create_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *token_obj = NULL;

    if (!PyArg_ParseTuple(args, "O", &token_obj)) {
        return NULL;
    }

    const char *token = cf_datum_get_cstring(token_obj, "First argument must be a string or bytes");
    if (!token) {
        return NULL;
    }

    dap_chain_tx_tsd_t *tsd = dap_chain_datum_voting_token_tsd_create(token);
    if (!tsd) {
        Py_RETURN_NONE;
    }

    return PyCapsule_New(tsd, "dap_chain_tx_tsd_t", NULL);
}

PyObject *dap_chain_datum_voting_vote_changing_allowed_tsd_create_py(PyObject *self, PyObject *args) {
    (void)self;
    int changing_allowed = 0;

    if (!PyArg_ParseTuple(args, "p", &changing_allowed)) {
        return NULL;
    }

    dap_chain_tx_tsd_t *tsd = dap_chain_datum_voting_vote_changing_allowed_tsd_create(
        changing_allowed ? true : false
    );
    if (!tsd) {
        Py_RETURN_NONE;
    }

    return PyCapsule_New(tsd, "dap_chain_tx_tsd_t", NULL);
}

PyObject *dap_chain_datum_voting_vote_tx_cond_tsd_create_py(PyObject *self, PyObject *args) {
    (void)self;
    PyObject *hash_obj = NULL;
    int out_idx = 0;

    if (!PyArg_ParseTuple(args, "Oi", &hash_obj, &out_idx)) {
        return NULL;
    }

    dap_hash_fast_t tx_hash = {0};
    if (cf_datum_parse_hash_fast(hash_obj, &tx_hash) != 0) {
        return NULL;
    }

    dap_chain_tx_tsd_t *tsd = dap_chain_datum_voting_vote_tx_cond_tsd_create(tx_hash, out_idx);
    if (!tsd) {
        Py_RETURN_NONE;
    }

    return PyCapsule_New(tsd, "dap_chain_tx_tsd_t", NULL);
}
