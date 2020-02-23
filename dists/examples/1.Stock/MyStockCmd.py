from CellFrame.libCellFrame import *

def cmdUser():
    pass

def init():
    cl = ChainNodeCLI()
    cl.cmdItemCreate("user",cmdUser,"User managment command","""
Print user list
    user list

Create user
    user create -u <user name> -p <password>
    """)
    logItNotice("Initalized custom command set")
