from CellFrame.libCellFrame import *

def cmdUser(*argv):
    reply="user managment command:\n"
    for arg in argv:
        reply+=" arg"+str(arg)+"=\""+arg+"\"\n"
    setReplyText(reply)

def init():
    AppCliServer.cmdItemCreate("user",cmdUser,"User managment command","""
Print user list
    user list

Create user
    user create -u <user name> -p <password>
    """)
    logItNotice("Initalized custom command set")
