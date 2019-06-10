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
            raise DapIniException("Failed to set the logging level, perhaps you did correctly specify the name of the level")

