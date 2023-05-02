#include "Python.h"
#include "dap_stream_ch_chain_net.h"

typedef struct PyDapStreamChChainNetRND{
    PyObject_HEAD
    dap_stream_ch_chain_validator_test_t *rnd;
}PyDapStreamChChainNetRNDObject;

void PyDapStreamChChainNetRNDObject_free(PyDapStreamChChainNetRNDObject *self);

PyObject *wrapping_dap_stream_ch_chain_net_rnd_get_version(PyObject *self, void *closure);
PyObject *wrapping_dap_stream_ch_chain_net_rnd_get_flags(PyObject *self, void *closure);
PyObject *wrapping_dap_stream_ch_chain_net_rnd_get_sign_size(PyObject *self, void *closure);
PyObject *wrapping_dap_stream_ch_chain_net_rnd_get_sign_correct(PyObject *self, void *closure);
PyObject *wrapping_dap_stream_ch_chain_net_rnd_get_overall_correct(PyObject *self, void *closure);
PyObject *wrapping_dap_stream_ch_chain_net_rnd_get_signs(PyObject *self, void *closure);

extern PyTypeObject PyDapStreamChChainNetRNDObjectType;
