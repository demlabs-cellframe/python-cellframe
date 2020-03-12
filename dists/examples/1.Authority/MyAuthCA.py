#!/usr/bin/python3
from CellFrame.libCellFrame import *
from string import Template
import os
import sys
import MyAuthConf

tmp_dir = os.getcwd() + "/tmp"
var_dir = os.getcwd() + "/var"
app_name = "MyAuth"

jsonCfg=MyAuthConf.getJsonString(app_name,"CRITICAL")
try:
    init( jsonCfg )
except json.decoder.JSONDecodeError as jex:
    sys.stderr.write("load_json_config JSONdecode :%s" % jex)
    exit(-1)

counter=0

action = "help"
cmd_name = ""
action_arg = {}
for arg in sys.argv:
    counter+=1
    if counter == 1:
        cmd_name = arg
    if counter == 2:
        if arg == "init_root_ca":
            print( "Init CA for root nodes selected" )
            action = "init_root_ca"
    elif counter == 3:
        if arg == "create_ca":
            print( "Create custom CA action selected" )
            action="create_ca"
    elif counter >= 4:
        action_arg[counter-3]=arg

if action == "help":
    print("CA managment script usage: ")
    print("")
    print("To get this help")
    print("\t"+cmd_name+" [help]")
    print("")
    print("Generate <Root nodes number> certificates (5 by default) for selected <Algorythm> (\"sig_dil\" by default)")
    print("\t"+cmd_name+" init_root_ca [<Root nodes number>] [<Algorythm>]")
    print("")
elif action =="init_root_ca":
    Crypto.
