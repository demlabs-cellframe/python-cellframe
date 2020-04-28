#pragma once
/*
* Authors:
* Alexey V. Stratulat <alexey.stratulat@demlabs.net>
* DeM Labs Inc.   https://demlabs.net
* DeM Labs Open source community https://gitlab.demlabs.net/cellframe/libdap-plugins-python
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

#include "dap_chain_node_cli.h"
#include "dap_chain_node_cli_cmd.h"
#include "dap_chain_plugins_manifest.h"
#include "dap_chain_plugins.h"

#ifdef __cplusplus
extern "C" {
#endif

#undef LOG_TAG
#define LOG_TAG "dap_chain_plugins_command"

void dap_chain_plugins_command_create(void);
int dap_chain_plugins_command_handler(int a_argc, char **a_argv, void *a_arg_func, char **a_str_reply);
//char *dap_chain_plugins_command_table();

#ifdef __cplusplus
}
#endif
