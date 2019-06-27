import libdapConnector

json_string = """{
    "modules": "libdap",
    "dap": {
       "config_dir": "/opt/dap/etc",
       "log_level": "Debug",
       "application_name": "TestAPP"
    }
    }"""
print("Standard Configuration \n"+json_string)
daptest = libdapConnector.Dap(json_string)
print("Initialization of the DAP")
daptest.setLogLevel("DEBUG")
print("Level logging ""DEBUG"" done")
daptest.logIt("""{
    "level": "DEBUG",
    "data": "Test. Outputting a string using the log_it function in the libdap library"
}""")
print("Outputting a string using the log_it function done")
res1 = daptest.configGetItem("server", "listen_address")
print("Output [server] 'listen_address' = "+res1+"\n")
res2 = daptest.configGetItemDefault("server1", "listen_address", "8.8.8.8")
print("Output default value '8.8.8.8' \n [server1] 'listen_address' = "+res2+"\n")
print("TEST. Get default config done")
