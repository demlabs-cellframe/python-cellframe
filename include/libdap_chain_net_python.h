#define PY_SSIZE_T_CLEAN
#include "Python.h"
#include "dap_chain_net.h"

#ifdef __cplusplus
extern "C"{
#endif

typedef struct PyDapChainNet{
    PyObject_HEAD
}PyDapChainNetObject;

int dap_chain_net_init_py(void);
void dap_chain_net_deinit_py(void);

#ifdef __cplusplus
}
#endif
