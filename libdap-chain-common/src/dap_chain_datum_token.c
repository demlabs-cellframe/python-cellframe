/*
 * Authors:
 * Dmitriy A. Gearasimov <kahovski@gmail.com>
 * DeM Labs Inc.   https://demlabs.net
 * DeM Labs Open source community https://github.com/demlabsinc
 * Copyright  (c) 2017-2018
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
#include <stdio.h>
#include <string.h>
#include "dap_chain_datum_token.h"

const char *c_dap_chain_datum_token_emission_type_str[]={
    [DAP_CHAIN_DATUM_TOKEN_EMISSION_TYPE_UNDEFINED] = "DAP_CHAIN_DATUM_TOKEN_EMISSION_TYPE_UNDEFINED",
    [DAP_CHAIN_DATUM_TOKEN_EMISSION_TYPE_AUTH] = "DAP_CHAIN_DATUM_TOKEN_EMISSION_TYPE_AUTH",
    [DAP_CHAIN_DATUM_TOKEN_EMISSION_TYPE_ALGO] = "DAP_CHAIN_DATUM_TOKEN_EMISSION_TYPE_ALGO",
    [DAP_CHAIN_DATUM_TOKEN_EMISSION_TYPE_ATOM_OWNER] = "DAP_CHAIN_DATUM_TOKEN_EMISSION_TYPE_ATOM_OWNER",
    [DAP_CHAIN_DATUM_TOKEN_EMISSION_TYPE_SMART_CONTRACT] = "DAP_CHAIN_DATUM_TOKEN_EMISSION_TYPE_SMART_CONTRACT",
};
