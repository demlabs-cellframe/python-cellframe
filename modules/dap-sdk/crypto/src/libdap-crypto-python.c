/*
 * Authors:
 * Alexey Stratulat <alexey.stratulat@demlabs.net>
 * Dmitriy A. Gearasimov <gerasimov.dmitriy@demlabs.net>
 * DeM Labs Inc.   https://demlabs.net
 * CellFrame       https://cellframe.net
 * Sources         https://gitlab.demlabs.net/cellframe
 * Copyright  (c) 2017-2020
 * All rights reserved.

 This file is part of DAP (Deus Applications Prototypes) the open source project

    DAP (Deus Applicaions Prototypes) is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    DAP is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with any DAP based project.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "libdap-crypto-python.h"

#define LOG_TAG "dap_crypto_python"

int dap_crypto_init(void){
    if(dap_enc_init()!=0){
        log_it(L_CRITICAL,"Can't initialize \"encryption\" module");
        return -1;
    }
    if(dap_enc_key_init()!=0){
        log_it(L_CRITICAL,"Can't initialize \"encryption\" key module");
        return -2;
    }
    PyCryptoKeyObject_PyCryptoKeyType.tp_new = PyType_GenericNew;
    return 0;
}

void dap_crypto_deinit(void){
    dap_enc_key_deinit();
    dap_enc_deinit();
}
