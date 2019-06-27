import json
import libdap_python_module

class DapIniException(Exception):
    pass

def init(data):
    res = json.loads(data)
    modules=res['modules']
    config_dir=res['dap']['config_dir']
    log_level=res['dap']['log_level']
    application_name=res['dap']['application_name']
    res_init = libdap_python_module.init(config_dir, application_name)
    if res_init == -1 or res_init == -2:
        raise DapIniException("Initialization erorr")
    if log_level != "NOTICE":
        setLogLevel(log_level)
def deInit():
    libdap_python_module.deinit()
def setLogLevel(data):
    log_level=data
    res_setLogLevel = libdap_python_module.setLogLevel(data)
    if res_setLogLevel == -1:
       raise DapIniException("Failed to set the logging level, perhaps you did not correctly specify the name of the level")
def logIt(data):
    parse_data = json.loads(data)
    res_log_it = libdap_python_module.logIt(parse_data['level'], parse_data['data'])
    if res_log_it == -1:
       raise DapIniException("Could not execute log_it function. Perhaps you did not correctly specify the name of the logging level or did not leave the information that needs to be displayed")
def configGetItem(section_path, item_name):
    res = libdap_python_module.configGetItem(section_path, item_name)
    return res
def configGetItemDefault(section_path, item_name, default):
    return libdap_python_module.configGetItemDefault(section_path, item_name, default)



