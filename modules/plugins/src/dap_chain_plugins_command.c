#include "dap_chain_node_cli.h"
#include "dap_chain_node_cli_cmd.h"
#include "dap_chain_plugins_manifest.h"
#include "dap_chain_plugins.h"
#include "utlist.h"
#include "dap_chain_plugins_command.h"

static bool s_l_restart_plugins = false;

#undef LOG_TAG
#define LOG_TAG "dap_chain_plugins_command"

void dap_chain_plugins_command_create(void){
    if (!s_l_restart_plugins){
        dap_cli_server_cmd_add("plugins", dap_chain_plugins_command_handler,
                                           "Commands for working with plugins.", dap_chain_node_cli_cmd_id_from_str("plugins"),
                                           "plugins list \t- show list plugins \n"
                                           "plugins show --name <name_plugin> \t-show information for plugin \n"
                                           "plugins restart \t-Restart all plugins \n"
                                           "plugins reload --name <name_plugin> \t-Restart plugin \n\n");
        s_l_restart_plugins = true;
    }
}

int dap_chain_plugins_command_handler(int a_argc, char **a_argv, dap_json_t *a_json_arr_reply, int UNUSED_ARG a_version)
{   
    enum {
        CMD_NONE, CMD_LIST, CMD_SHOW_NAME, CMD_RESTART, CMD_RELOAD_NAME
    };
    int l_arg_index = 1;
    int l_cmd_name = CMD_NONE;
    const char *l_name_plugin = NULL;
    dap_chain_plugins_list_manifest_t *l_element = NULL;
    if (dap_cli_server_cmd_find_option_val(a_argv,l_arg_index, a_argc, "list", NULL))
        l_cmd_name = CMD_LIST;
    if (dap_cli_server_cmd_find_option_val(a_argv,l_arg_index, a_argc, "show", NULL))
        l_cmd_name = CMD_SHOW_NAME;
    if (dap_cli_server_cmd_find_option_val(a_argv,l_arg_index, a_argc, "restart", NULL))
        l_cmd_name = CMD_RESTART;
    if (dap_cli_server_cmd_find_option_val(a_argv,l_arg_index, a_argc, "reload", NULL))
        l_cmd_name = CMD_RELOAD_NAME;
    char *l_str = NULL;
    switch (l_cmd_name) {
    case CMD_LIST:
        l_str = dap_strdup("|\tName plugin\t|\tVersion\t|\tAuthor(s)\t|\n");
        LL_FOREACH(dap_chain_plugins_manifests_get_list(), l_element){
            l_str = dap_strjoin(NULL,
                              l_str, "|\t",l_element->name, "\t|\t", l_element->version, "\t|\t", l_element->author, "\t|\n", NULL);

        }
        dap_json_rpc_error_add(a_json_arr_reply, 0, l_str);
        DAP_DELETE(l_str);
        break;
    case CMD_SHOW_NAME:
        dap_cli_server_cmd_find_option_val(a_argv, l_arg_index, a_argc, "--name", &l_name_plugin);
        LL_SEARCH(dap_chain_plugins_manifests_get_list(), l_element, l_name_plugin, dap_chain_plugins_manifest_name_cmp);
        if (l_element != NULL){
            char *l_dep = dap_chain_plugins_manifests_get_list_dependencyes(l_element);
            if (l_dep != NULL){
                char *l_reply_str = dap_strdup_printf(" Name: %s\n Version: %s\n Author: %s\n"
                                                      " Description: %s\n Dependencies: %s \n\n",
                                                      l_element->name, l_element->version, l_element->author,
                                                      l_element->description, l_dep);
                dap_json_rpc_error_add(a_json_arr_reply, 0, l_reply_str);
                DAP_DELETE(l_reply_str);
                DAP_FREE(l_dep);
            } else {
                char *l_reply_str = dap_strdup_printf(" Name: %s\n Version: %s\n Author: %s\n"
                                                      " Description: %s\n\n",
                                                      l_element->name, l_element->version, l_element->author, l_element->description);
                dap_json_rpc_error_add(a_json_arr_reply, 0, l_reply_str);
                DAP_DELETE(l_reply_str);
            }
        } else {
            char *l_reply_str = dap_strdup_printf("Can't find a plugin named %s", l_name_plugin);
            dap_json_rpc_error_add(a_json_arr_reply, -1, l_reply_str);
            DAP_DELETE(l_reply_str);
        }
        break;
    case CMD_RESTART:
        log_it(L_NOTICE, "Restart python plugin module");
        dap_chain_plugins_deinit();
        dap_chain_plugins_init(g_config);
        log_it(L_NOTICE, "Restart is completed");
        dap_json_rpc_error_add(a_json_arr_reply, 0, "Restart is completed.");
        break;
    case CMD_RELOAD_NAME:
        dap_cli_server_cmd_find_option_val(a_argv, l_arg_index, a_argc, "--name", &l_name_plugin);
        int l_result = dap_chain_plugins_reload_plugin(l_name_plugin);
        switch (l_result) {
        case 0:
            char *l_reply_str = dap_strdup_printf("Restart \"%s\" plugin is completed successfully.", l_name_plugin);
            dap_json_rpc_error_add(a_json_arr_reply, 0, l_reply_str);
            DAP_DELETE(l_reply_str);
            break;
        case -2:
            char *l_reply_str = dap_strdup_printf("\"%s\" plugin has unresolved dependencies. Restart all plugins.", l_name_plugin);
            dap_json_rpc_error_add(a_json_arr_reply, -1, l_reply_str);
            DAP_DELETE(l_reply_str);
            break;
        case -3:
            char *l_reply_str = dap_strdup_printf("Registration manifest for \"%s\" plugin is failed.", l_name_plugin);
            dap_json_rpc_error_add(a_json_arr_reply, -1, l_reply_str);
            DAP_DELETE(l_reply_str);
            break;
        case -4:
            char *l_reply_str = dap_strdup_printf("A plugin named \"%s\" was not found.", l_name_plugin);
            dap_json_rpc_error_add(a_json_arr_reply, -1, l_reply_str);
            DAP_DELETE(l_reply_str);
            break;
        default:
            dap_json_rpc_error_add(a_json_arr_reply, l_result, "An unforeseen error has occurred.");
            break;
        }
        break;
    default:
        dap_json_rpc_error_add(a_json_arr_reply, -1, "Arguments are incorrect.");
        break;

    }
    return 0;
}
