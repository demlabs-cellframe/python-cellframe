#include "libdap-crypto-python.h"

int dap_crypto_init(void){
    if(dap_enc_init()!=0){
        log_it(L_CRITICAL,"Can't init encryption module");
        return -1;
    }
    if(dap_enc_key_init()!=0){
        log_it(L_CRITICAL,"Can't init encryption key module");
        return -2;
    }
    PyCryptoKeyObject_PyCryptoKeyType.tp_new = PyType_GenericNew;
    return 0;
}

void dap_crypto_deinit(void){
    dap_enc_key_deinit();
    dap_enc_deinit();
}
