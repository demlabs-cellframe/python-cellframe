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
