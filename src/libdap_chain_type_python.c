#include "libdap_chain_type_python.h"

PyObject* CHAIN_TYPE_FIRST_PY(){
    return PyLong_FromLong(CHAIN_TYPE_FIRST);
}
PyObject* CHAIN_TYPE_TOKEN_PY(){
    return PyLong_FromLong(CHAIN_TYPE_TOKEN);
}
PyObject* CHAIN_TYPE_EMISSION_PY(){
    return PyLong_FromLong(CHAIN_TYPE_EMISSION);
}
PyObject* CHAIN_TYPE_TX_PY(){
    return PyLong_FromLong(CHAIN_TYPE_TX);
}
PyObject* CHAIN_TYPE_LAST_PY(){
    return PyLong_FromLong(CHAIN_TYPE_LAST);
}
