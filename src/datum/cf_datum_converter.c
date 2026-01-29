#include "cf_datum_internal.h"
#include "dap_chain_datum_converter.h"

PyObject *dap_chain_datum_from_tx_py(PyObject *self, PyObject *args) {
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

    dap_chain_datum_t *datum = dap_chain_datum_from_tx(tx);
    if (!datum) {
        Py_RETURN_NONE;
    }
    return PyCapsule_New(datum, "dap_chain_datum_t", NULL);
}
