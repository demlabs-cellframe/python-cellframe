import libdapConnector

json_string = """{
    "modules": "libdap",
    "dap": {
       "config_dir": "/opt/dap/etc/",
       "log_level": "Debug",
       "application_name": "TestAPP"
    }
    }"""
print("Standard Configuration \n"+json_string)
daptest = libdapConnector.Dap(json_string)
print("Initialization of the DAP")
daptest.setLogLevel("DEBUG")
print("Level logging ""DEBUG"" done")
