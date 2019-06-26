import json
import libdap_python_module

class DapIniException(Exception):
    pass

class Dap:
    def __init__(self, data):
        res = json.loads(data)
        self.modules=res['modules']
        self.config_dir=res['dap']['config_dir']
        self.log_level=res['dap']['log_level']
        self.application_name=res['dap']['application_name']
        res_init = libdap_python_module.init(res['dap']['config_dir']+"\n"+res['dap']['application_name']+"\n")
        if res_init == -1 or res_init == -2:
            raise DapIniException("Initialization erorr")
    def __del__(self):
        libdap_python_module.deinit()
    def setLogLevel(self, data):
        self.log_level=data
        res_setLogLevel = libdap_python_module.setLogLevel(data)
        if res_setLogLevel == -1:
            raise DapIniException("Failed to set the logging level, perhaps you did not correctly specify the name of the level")
    def logIt(self, data):
        parse_data = json.loads(data)
        res_log_it = libdap_python_module.logIt(parse_data['level']+"\n"+parse_data['data'])
        if res_log_it == -1:
            raise DapIniException("Could not execute log_it function. Perhaps you did not correctly specify the name of the logging level or did not leave the information that needs to be displayed")
    def configGetItem(self, section_path, item_name):
        res = libdap_python_module.configGetItem(section_path, item_name)
        return res
    def configGetItemDefault(self, section_path, item_name, default):
        return libdap_python_module.configGetItemDefault(section_path, item_name, default)



