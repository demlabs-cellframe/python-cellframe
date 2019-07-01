from libdapConnector import *
import os
import sys

def create_config_file(app_name):
    f = open(app_name+".cfg", "w")
    f.write("[server]\nlisten_address=0.0.0.0\n")
    f.close()

logIt(LogLevel.L_INFO,"Start main test")
app_name = "TestAPP"
logIt(LogLevel.L_INFO, "Create config file")
create_config_file(app_name)


json_string = """{
    "modules": "libdap",
    "dap": {
       "config_dir": \""""+os.getcwd()+"""\",
       "log_level": \""""+LogLevel.L_DEBUG.name+"""\",
       "application_name": \""""+app_name+"""\"
    }
    }"""
init(json_string)
logIt(LogLevel.L_INFO, "Initialization of the DAP done")
setLogLevel(LogLevel.L_DEBUG)
logIt(LogLevel.L_INFO,"Level logging ""DEBUG"" done")
logIt(LogLevel.L_DEBUG, "Test. Outputting a string using the log_it function in the libdap library")
logIt(LogLevel.L_INFO,"Outputting a string using the log_it function done")
res1 = configGetItem("server", "listen_address")
logIt(LogLevel.L_INFO, "Output [server] 'listen_address' = "+res1+"\n")
res2 = configGetItemDefault("server1", "listen_address", "8.8.8.8")
logIt(LogLevel.L_INFO, "Output default value '8.8.8.8' [server1] 'listen_address' = "+res2+"\n")
logIt(LogLevel.L_INFO, "TEST. Get default config done")
deInit()
logIt(LogLevel.L_INFO, "Deinitialization done")

os.remove(app_name+".cfg")
logIt(LogLevel.L_INFO, "Dellete config file")
logIt(LogLevel.L_INFO, "Main test done");

sys.exit(0)
