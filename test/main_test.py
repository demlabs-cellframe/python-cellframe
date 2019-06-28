import libdapConnector
import os
import sys

def output_info(string):
    libdapConnector.logIt(libdapConnector.LogLevel.L_INFO, string)
def output_info_failed(string):
    libdapConnector.logIt(libdapConnector.LogLevel.L_CRITICAL, string)
def create_config_file(app_name):
    f = open(app_name+".cfg", "w")
    f.write("[server]\nlisten_address=0.0.0.0\n")
    f.close()

output_info("Start main test")
app_name = "TestAPP"
output_info("Create config file")
create_config_file(app_name)


json_string = """{
    "modules": "libdap",
    "dap": {
       "config_dir": \""""+os.getcwd()+"""\",
       "log_level": \""""+libdapConnector.LogLevel.L_DEBUG.name+"""\",
       "application_name": \""""+app_name+"""\"
    }
    }"""
libdapConnector.init(json_string)
output_info("Initialization of the DAP done")
libdapConnector.setLogLevel(libdapConnector.LogLevel.L_DEBUG)
output_info("Level logging ""DEBUG"" done")
libdapConnector.logIt(libdapConnector.LogLevel.L_DEBUG, "Test. Outputting a string using the log_it function in the libdap library")
output_info("Outputting a string using the log_it function done")
res1 = libdapConnector.configGetItem("server", "listen_address")
output_info("Output [server] 'listen_address' = "+res1+"\n")
res2 = libdapConnector.configGetItemDefault("server1", "listen_address", "8.8.8.8")
output_info("Output default value '8.8.8.8' [server1] 'listen_address' = "+res2+"\n")
output_info("TEST. Get default config done")
libdapConnector.deInit()
output_info("Deinitialization done")

os.remove(app_name+".cfg")
output_info("Dellete config file")
output_info("Main test done");

sys.exit(0)
