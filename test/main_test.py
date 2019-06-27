import libdapConnector

json_string = """{
    "modules": "libdap",
    "dap": {
       "config_dir": "/opt/dap/etc",
       "log_level": \""""+libdapConnector.LogLevel.L_DEBUG.name+"""\",
       "application_name": "TestAPP"
    }
    }"""
print("Standard Configuration \n"+json_string)
libdapConnector.init(json_string)
print("Initialization of the DAP")
libdapConnector.setLogLevel(libdapConnector.LogLevel.L_DEBUG)
print("Level logging ""DEBUG"" done")
libdapConnector.logIt(libdapConnector.LogLevel.L_DEBUG, "Test. Outputting a string using the log_it function in the libdap library")
print("Outputting a string using the log_it function done")
res1 = libdapConnector.configGetItem("server", "listen_address")
print("Output [server] 'listen_address' = "+res1+"\n")
res2 = libdapConnector.configGetItemDefault("server1", "listen_address", "8.8.8.8")
print("Output default value '8.8.8.8' \n [server1] 'listen_address' = "+res2+"\n")
print("TEST. Get default config done")
libdapConnector.deInit()
print("Deinitialization done")
