#!/usr/bin/python3
from CellFrame.libCellFrame import *
from string import Template
import os
import sys
import MyAuthConf

# App name
appName = "MyAuth"

# Generate config
jsonCfg=MyAuthConf.getJsonString(appName,"CRITICAL")

# Init SDK
try:
    init( jsonCfg )
except json.decoder.JSONDecodeError as jex:
    sys.stderr.write("load_json_config JSONdecode :%s" % jex)
    exit(-1)

# Prepare to parse input arguments
counter=0

action = "help"
cmdName = ""
action_arg = {}

# Parse input arguments
for arg in sys.argv:
    counter+=1
    # Extract command name
    if counter == 1:
        cmdName = arg

    # Extract subcommand
    if counter == 2:
        action = arg
    elif counter > 2:
        action_arg[counter-2]=arg
# Process actions
# Action Help
def help():
    print("CA managment script usage: ")
    print("")
    print("To get this help")
    print("\t"+cmdName+" [help]")
    print("")
    print("Generate <Root nodes number> certificates (5 by default) for selected <Algorythm> (\"sig_dil\" by default)")
    print("\t"+cmdName+" init_root_ca [<Root nodes number>] [<Algorythm>]")
    print("")
if action == "help":
    help()
# Action init root CAs
elif action == "init_root_ca":
    # Default params
    rootCaNumber = 5
    rootCaAlgoName = "sig_dil"

    # Set default algo Dilithium
    rootCaAlgo = CryptoKeyType.DAP_ENC_KEY_TYPE_SIG_DILITHIUM

    # Read action args
    if len(action_arg) > 0:
        rootCaNumber = action_arg[1]
    if len(action_arg) > 1:
        rootCaAlgoName = action_arg[2]


    # Parse algo name
    if rootCaAlgoName == "sig_bliss":
        rootCaAlgo = CryptoKeyType.DAP_ENC_KEY_TYPE_SIG_BLISS
    elif rootCaAlgoName == "sig_tesla":
        rootCaAlgo = CryptoKeyType.DAP_ENC_KEY_TYPE_SIG_TESLA
    elif rootCaAlgoName == "sig_picnic":
        rootCaAlgo = CryptoKeyType.DAP_ENC_KEY_TYPE_SIG_PICNIC
    elif rootCaAlgoName == "sig_dil":
        rootCaAlgo = CryptoKeyType.DAP_ENC_KEY_TYPE_SIG_DILITHIUM
    else:
        # Process error case
        print("(!) Wrong algo name \""+rootCaAlgoName+"\", possible names: sig_bliss, sig_tesla, sig_picnic, sig_dil")
        help()
        exit(-1)

    # Create certs
    print("Init root "+str(rootCaNumber)+" certificates with algo "+rootCaAlgoName)
    for cur in range(int(rootCaNumber)):
        certName = MyAuthConf.getNetworkName(appName,"_") +"_root_"+str(cur)
        cert=Cert.generate( certName, rootCaAlgo,"fasdfsdf" )
