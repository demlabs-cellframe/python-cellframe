import json
import libdap_python_module

class Dap:
    def __init__(self, data):
        res = json.loads(data)
        self.modules=res['modules']
        self.config_dir=res['dap']['config_dir']
        self.log_level=res['dap']['log_level']
        self.application_name=res['dap']['application_name']
#        dap.init('/opt/var/ler\njaryt\ndebug')
        libdap_python_module.init(res['dap']['config_dir']+"\n"+res['dap']['application_name']+"\n")
    def __del__(self):
        libdap_python_module.deinit()

