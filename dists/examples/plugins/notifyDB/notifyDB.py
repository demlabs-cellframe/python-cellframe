from API_CellFrame import logItNotice, ChainNet

def notify_net_db(op_code, group, key, value):
    logItNotice("Notificator op_code: "+op_code+"\n\tgroup: "+group+"\n\tkey:"+key+"\n\tvalue:"+str(value))

def init():
    net = ChainNet.byName("subzero")
    net.addNotify(notify_net_db)
    return 0

