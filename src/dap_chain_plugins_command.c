#include "dap_chain_plugins_command.h"

static bool restart_plugins = false;

void dap_chain_plugins_command_create(void){
    if (!restart_plugins){
        dap_chain_node_cli_cmd_item_create("plugins", dap_chain_plugins_command_handler, NULL,
                                           "Commands for working with plugins.",
                                           "plugins list \t- show list plugins \n"
                                           "plugins show --name <name_plugin> \t-show information for plugin \n"
                                           "plugins restart \t-Restart all plugins \n"
                                           "plugins reload --name <name_plugin> \t-Restart plugin \n\n");
        restart_plugins = true;
    }
}
int dap_chain_plugins_command_handler(int a_argc, char **a_argv, void *a_arg_func, char **a_str_reply){
    (void)a_arg_func;
    log_it(L_NOTICE, "Handler cmd");
    enum {
        CMD_NONE, CMD_LIST, CMD_SHOW_NAME, CMD_RESTART, CMD_RELOAD_NAME
    };
    int arg_index = 1;
    int cmd_name = CMD_NONE;
    const char *name_plugin = NULL;
    dap_chain_plugins_list_manifest_t *element = NULL;
    if (dap_chain_node_cli_find_option_val(a_argv,arg_index, a_argc, "list", NULL))
        cmd_name = CMD_LIST;
    if (dap_chain_node_cli_find_option_val(a_argv,arg_index, a_argc, "show", NULL))
        cmd_name = CMD_SHOW_NAME;
    if (dap_chain_node_cli_find_option_val(a_argv,arg_index, a_argc, "restart", NULL))
        cmd_name = CMD_RESTART;
    if (dap_chain_node_cli_find_option_val(a_argv,arg_index, a_argc, "reload", NULL))
        cmd_name = CMD_RELOAD_NAME;
    char *str = NULL;
    switch (cmd_name) {
    case CMD_LIST:
        str = dap_strdup("|\tName plugin\t|\tVersion\t|\tAuthor(s)\t|\n");
        LL_FOREACH(dap_chain_plugins_manifests_get_list(), element){
            str = dap_strjoin(NULL,
                              str, "|\t",element->name, "\t|\t", element->version, "\t|\t", element->author, "\t|\n", NULL);

        }
        dap_chain_node_cli_set_reply_text(a_str_reply, str);
        break;
    case CMD_SHOW_NAME:
        dap_chain_node_cli_find_option_val(a_argv, arg_index, a_argc, "--name", &name_plugin);
        log_it(L_NOTICE, "name plugin: %s", name_plugin);
        LL_SEARCH(dap_chain_plugins_manifests_get_list(), element, name_plugin, dap_chain_plugins_manifest_name_cmp);
        if (element != NULL){
            char *dep = dap_chain_plugins_manifests_get_list_dependencyes(element);
            if (dep != NULL){
                dap_chain_node_cli_set_reply_text(a_str_reply, " Name: %s\n Version: %s\n Author: %s\n"
                                                               " Description: %s\n Dependencys: %s \n\n",
                                                  element->name, element->version, element->author, element->description, dep);
                DAP_FREE(dep);
            } else {
                dap_chain_node_cli_set_reply_text(a_str_reply, " Name: %s\n Version: %s\n Author: %s\n"
                                                               " Description: %s\n\n",
                                                  element->name, element->version, element->author, element->description);
            }
        } else {
            dap_chain_node_cli_set_reply_text(a_str_reply, "Can't searching plugin with name %s", name_plugin);
        }
        break;
    case CMD_RESTART:
        log_it(L_NOTICE, "Start procedure restart python plugins module");
        dap_chain_plugins_deinit();
        dap_chain_plugins_init(g_config);
        log_it(L_NOTICE, "Done procedure restart python plugins module");
        dap_chain_node_cli_set_reply_text(a_str_reply, "Done procedure restart python plugins module.");
        break;
    case CMD_RELOAD_NAME:
        dap_chain_node_cli_find_option_val(a_argv, arg_index, a_argc, "--name", &name_plugin);
        int result = dap_chain_plugins_reload_plugin(name_plugin);
        switch (result) {
        case 0:
            dap_chain_node_cli_set_reply_text(a_str_reply, "Restarting the plugin %s completed successfully.", name_plugin);
            break;
        case -2:
            dap_chain_node_cli_set_reply_text(a_str_reply,
                                              "%s plugin has unresolved dependencys, restart all plagins",
                                              name_plugin);
            break;
        case -3:
            dap_chain_node_cli_set_reply_text(a_str_reply, "Registration %s manifest for %s plugin fail", name_plugin);
            break;
        case -4:
            dap_chain_node_cli_set_reply_text(a_str_reply, "A plugin named %s will not find", name_plugin);
            break;
        default:
            dap_chain_node_cli_set_reply_text(a_str_reply, "An unforeseen error has occurred.");
            break;
        }
        break;
    default:
        dap_chain_node_cli_set_reply_text(a_str_reply, "Not validation parameters");
        break;

    }
    return 0;
}
