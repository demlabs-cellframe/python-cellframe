from libCellFrame import *
import os
import sys

def create_config_file(app_name):
    f = open(app_name+".cfg", "w")
    f.write("[server]\nlisten_address=0.0.0.0\n")
    f.close()

print("Start main test")
app_name = "testAPP"
print("Create config file")
create_config_file(app_name)


json_string = """{
    "modules": "",
    "DAP": {
       "config_dir": \""""+os.getcwd()+"""\",
       "log_level": "L_DEBUG",
       "application_name": \""""+app_name+"""\",
       "file_name_log": \""""+app_name+""".text\"
    }
    }"""

init(json_string)
logItInfo("Initialization of the DAP done")
setLogLevel(DEBUG)
logItInfo("Level logging ""DEBUG"" done")
logItInfo( "Test. Outputting a string using the log_it function in the libdap library")
logItInfo("Outputting a string using the log_it function done")
res1 = configGetItem("server", "listen_address")
logItInfo("Output [server] 'listen_address' = "+res1+"\n")
res2 = configGetItemDefault("server1", "listen_address", "8.8.8.8")
logItInfo("Output default value '8.8.8.8' [server1] 'listen_address' = "+res2+"\n")
logItInfo( "TEST. Get default config done")
deinit()
logItInfo("Deinitialization done")

os.remove(app_name+".cfg")
logItInfo( "Main test done");

sys.exit(0)
