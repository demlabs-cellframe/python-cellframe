#include "wrapping_dap_chain_net_srv_vote.h"
#include "dap_chain_net_voting.h"
#include "dap_chain_net.h"
#include "math_python.h"
#include "datetime.h"
#include "dap_chain_wallet_python.h"
#include "libdap_chain_net_python.h"
#include "wrapping_dap_chain_net_srv_vote_info.h"

PyObject *wrapping_dap_chain_net_srv_vote_list(PyObject *self, PyObject *args){
    PyObject *obj_net;
}

PyObject *wrapping_dap_chain_net_srv_vote_create(PyObject *self, PyObject *args) {
    const char *question;
    PyObject *obj_list_option;
    PyObject *obj_expire_option = NULL;
    unsigned long max_vote = 0;
    DapMathObject *fee;
    PyObject *obj_delegate_key_required = NULL;
    PyObject *obj_vote_changing_allowed = NULL;
    PyObject *obj_wallet;
    PyObject *obj_net;
    if (!PyArg_ParseTuple(args, "sOOOO|kOOO", &question, &obj_list_option, &fee, &obj_wallet, &obj_net, &max_vote,
                          &obj_expire_option, &obj_vote_changing_allowed, &obj_delegate_key_required))
        return NULL;
    if (!PyList_Check(obj_list_option)) {
        PyErr_SetString(DapChainNetSrvVoteError, "The second argument is incorrect. There should be a list of options.");
        return NULL;
    }
    if (!PyObject_TypeCheck(fee, &DapMathObjectType)) {
        PyErr_SetString(DapChainNetSrvVoteError, "The third argument is incorrect. There must be an instance of the "
                                                 "DapMath object.");
        return NULL;
    }
    if (!PyDapChainWalletObject_Check(obj_wallet)) {
        PyErr_SetString(DapChainNetSrvVoteError, "The fourth argument is incorrect. There must be an instance of the "
                                                 "Wallet object.");
        return NULL;
    }
    if (!PyDapChainNet_Check((PyDapChainNetObject*)obj_net)) {
        PyErr_SetString(DapChainNetSrvVoteError, "The fifth argument is incorrect. There must be an instance of the "
                                                 "CellFrame.Network.Net object.");
        return NULL;
    }
    bool l_obj_expire_option = false;
    dap_time_t l_time_expire_option = 0;
    if (obj_expire_option) {
        if (!PyDateTime_Check(obj_expire_option)) {
            PyErr_SetString(DapChainNetSrvVoteError,
                            "The sixth argument is incorrect. There must be an instance of the "
                            "DateTime object.");
            return NULL;
        }
        l_obj_expire_option = true;
        struct tm l_tm;
        l_tm.tm_sec = PyDateTime_DATE_GET_SECOND(obj_expire_option);
        l_tm.tm_min = PyDateTime_DATE_GET_MINUTE(obj_expire_option);
        l_tm.tm_hour = PyDateTime_DATE_GET_HOUR(obj_expire_option);
        l_tm.tm_mday = PyDateTime_GET_DAY(obj_expire_option);
        l_tm.tm_mon = PyDateTime_GET_MONTH(obj_expire_option);
        l_tm.tm_year = PyDateTime_GET_YEAR(obj_expire_option);
        time_t tmp = mktime(&l_tm);
        l_time_expire_option = (tmp <= 0) ? 0 : tmp;
    }
    if (obj_delegate_key_required) {
        if (PyBool_Check(obj_delegate_key_required)) {
            PyErr_SetString(DapChainNetSrvVoteError,
                            "The seventh argument is incorrect. There must be an instance of a "
                            "Boolean object.");
            return NULL;
        }
    }
    if (obj_vote_changing_allowed) {
        if (PyBool_Check(obj_vote_changing_allowed)) {
            PyErr_SetString(DapChainNetSrvVoteError, "The eighth argument is incorrect. There must be an instance of a "
                                                     "Boolean object.");
            return NULL;
        }
    }
    dap_list_t *l_option = NULL;
    for (int i = 0; i < PyList_Size(obj_list_option); i++) {
        PyObject *el = PyList_GetItem(obj_list_option, i);
        if (!PyUnicode_Check(el)) {
            dap_list_free(l_option);
            PyErr_SetString(DapChainNetSrvVoteError, "The list passed as the second argument does not contain a string.");
            return NULL;
        }
        char *value = PyUnicode_AsUTF8(el);
        l_option = dap_list_append(l_option, value);
    }
    dap_list_free(l_option);
    bool l_delegated_key_required = Py_IsTrue(obj_delegate_key_required) ? true : false;
    bool l_vote_changing_allowed = Py_IsTrue(obj_vote_changing_allowed) ? true : false;
    char *l_hash_ret;
    int res = dap_chain_net_vote_create(question, l_option, l_obj_expire_option ? &l_time_expire_option : NULL, &max_vote,
                              ((DapMathObject*)fee)->value, l_delegated_key_required, l_vote_changing_allowed,
                              ((PyDapChainWalletObject*)obj_wallet)->wallet, ((PyDapChainNetObject*)obj_net)->chain_net,
                              "hex", &l_hash_ret);
    switch (res) {
        case DAP_CHAIN_NET_VOTE_CREATE_OK: {
            return Py_BuildValue("s", l_hash_ret);
        } break;
        case DAP_CHAIN_NET_VOTE_CREATE_LENGTH_QUESTION_OVERSIZE_MAX: {
            char *l_ret = dap_strdup_printf("The question must contain no more than %d characters",
                                            DAP_CHAIN_DATUM_TX_VOTING_QUESTION_MAX_LENGTH);
            PyErr_SetString(DapChainNetSrvVoteError, l_ret);
            DAP_DELETE(l_ret);
            return NULL;
        } break;
        case DAP_CHAIN_NET_VOTE_CREATE_COUNT_OPTION_OVERSIZE_MAX: {
            char *l_ret = dap_strdup_printf("The voting can to contain no more than %d options",
                                            DAP_CHAIN_DATUM_TX_VOTING_OPTION_MAX_COUNT);
            PyErr_SetString(DapChainNetSrvVoteError, l_ret);
            DAP_DELETE(l_ret);
            return NULL;
        } break;
        case DAP_CHAIN_NET_VOTE_CREATE_FEE_IS_ZERO: {
            PyErr_SetString(DapChainNetSrvVoteError, "The commission amount must be greater than zero");
            return NULL;
        } break;
        case DAP_CHAIN_NET_VOTE_CREATE_SOURCE_ADDRESS_IS_INVALID: {
            PyErr_SetString(DapChainNetSrvVoteError, "Failed to get address from wallet.");
            return NULL;
        } break;
        case DAP_CHAIN_NET_VOTE_CREATE_NOT_ENOUGH_FUNDS_TO_TRANSFER: {
            PyErr_SetString(DapChainNetSrvVoteError, "Not enough funds to transfer");
            return NULL;
        } break;
        case DAP_CHAIN_NET_VOTE_CREATE_MAX_COUNT_OPTION_EXCEEDED: {
            char *l_ret = dap_strdup_printf("The option must contain no more than %d characters",
                                            DAP_CHAIN_DATUM_TX_VOTING_OPTION_MAX_LENGTH);
            PyErr_SetString(DapChainNetSrvVoteError, l_ret);
            DAP_DELETE(l_ret);
            return NULL;
        } break;
        case DAP_CHAIN_NET_VOTE_CREATE_CAN_NOT_OPTION_TSD_ITEM: {
            PyErr_SetString(DapChainNetSrvVoteError, "Can't create option tsd item.");
            return NULL;
        } break;
        case DAP_CHAIN_NET_VOTE_CREATE_INPUT_TIME_MORE_CURRENT_TIME: {
            PyErr_SetString(DapChainNetSrvVoteError, "Can't create voting with expired time");
            return NULL;
        } break;
        case DAP_CHAIN_NET_VOTE_CREATE_CAN_NOT_CREATE_TSD_EXPIRE_TIME: {
            PyErr_SetString(DapChainNetSrvVoteError, "Can't create expired tsd item.");
            return NULL;
        } break;
        case DAP_CHAIN_NET_VOTE_CREATE_CAN_NOT_CREATE_TSD_DELEGATE_KEY: {
            PyErr_SetString(DapChainNetSrvVoteError, "Can't create delegated key req tsd item.");
            return NULL;
        } break;
        case DAP_CHAIN_NET_VOTE_CREATE_CAN_NOT_ADD_NET_FEE_OUT: {
            PyErr_SetString(DapChainNetSrvVoteError, "Can't add net fee out.");
            return NULL;
        } break;
        case DAP_CHAIN_NET_VOTE_CREATE_CAN_NOT_ADD_OUT_WITH_VALUE_BACK: {
            PyErr_SetString(DapChainNetSrvVoteError, "Can't add out with value back");
            return NULL;
        } break;
        case DAP_CHAIN_NET_VOTE_CREATE_CAN_NOT_SIGNED_TX: {
            PyErr_SetString(DapChainNetSrvVoteError, "Can not sign transaction");
            return NULL;
        } break;
        case DAP_CHAIN_NET_VOTE_CREATE_CAN_NOT_POOL_DATUM_IN_MEMPOOL: {
            PyErr_SetString(DapChainNetSrvVoteError, "Can not pool transaction in mempool");
            return NULL;
        } break;
        default: {
            char *l_ret = dap_strdup_printf("Unknown error. Code: %d", res);
            PyErr_SetString(DapChainNetSrvVoteError, l_ret);
            DAP_DELETE(l_ret);
            return NULL;
        }
    }
}

PyObject *wrapping_dap_chain_net_srv_voting_list(PyObject *self, PyObject *argv) {
    PyObject *obj_net;
    if (!PyArg_ParseTuple(argv, "O", &obj_net)) {
        return NULL;
    }
    if (!PyDapChainNet_Check((PyDapChainNetObject*)obj_net)) {
        return NULL;
    }

    size_t l_list_count = 0;
    dap_chain_net_vote_info_t **l_list = dap_chain_net_vote_list(((PyDapChainNetObject*)obj_net)->chain_net, &l_list_count);
    if (!l_list_count) {
        Py_RETURN_NONE;
    }
    PyObject *obj_list = PyList_New(l_list_count);
    for (size_t i = l_list_count; --l_list_count;) {
        PyDapChainNetSrvVoteInfoObject *obj = PyObject_New(PyDapChainNetSrvVoteInfoObject, &DapChainNetSrvVoteInfoObjectType);
        obj->info = l_list[i];
        PyList_SetItem(obj_list, i, (PyObject*)obj);
    }
    return obj_list;
}
