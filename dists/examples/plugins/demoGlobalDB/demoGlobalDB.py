from DAP.Core import logIt
from CellFrame.Chain import GlobalDB

def init():
    logIt.notice("Running plugin client")
#   Creates an entry in globalDB 
    GlobalDB.set("test", "local.mygroup", "ENX".encode('utf-8'))
#   Reads the content from the newly created entry.
    term = GlobalDB.get("test", "local.mygroup")
#   Deletes the created entry. 
    GlobalDB.delete("test", "local.mygroup")
    logIt.notice("Data reading form group local.mygroup key test:  "+str(term))
    return 0

def deinit():
    logIt.notice("Plugin client deinit")
