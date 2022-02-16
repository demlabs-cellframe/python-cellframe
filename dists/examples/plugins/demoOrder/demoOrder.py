from API_CellFrame import logItNotice, ChainNetSrvOrder, ChainNet, ChainNetSrvUID, ChainNetSrvOrderDirection, ChainNetSrvPriceUnitUID
from API_CellFrame import Crypto, CryptoKeyType, AppCliServer, ChainHashFast
import time
from datetime import datetime, timedelta

def infoOrder(order):
#    hash = ChainHashFast.strToHashFast(hash_str)
#    order = ChainNetSrvOrder.find(net, hash_str)
    reply = "Order: \n"
    reply += "\tVersion:" + str(order.version)+"\n"
    reply += "\tUID: " + str(order.uid) + "\n"
    reply += "\tDirection: " + str(order.direction) + "\n"
    reply += "\tNode addr: " + str(order.nodeAddr) + "\n"
    reply += "\tCond hash: " + str(order.condHash) + "\n"
    reply += "\tPrice unit: " + str(order.priceUnit) + "\n"
    created = datetime.fromtimestamp(order.tsCreated)
    reply += "\tCreated: " + str(created) + "\n"
    expires = datetime.fromtimestamp(order.tsExpires)
    reply += "\tExpires: " + str(expires) + "\n"
    reply += "\tSrv price: " + str(order.srvPrice) + "\n"
    reply += "\tSrv price ticker: " + order.srvPriceTicker + "\n"
    reply += "\text size:" + str(order.extSize) + "\n"
    reply += "\text:" + str(order.ext) + "\n"
    return reply

def pwoCMD(argv, indexStrReply):
    print(indexStrReply)
    if (len(argv) == 5):
        net = ChainNet.byName(argv[2])
        if argv[3] == "find":
            order = ChainNetSrvOrder.find(net, argv[4])
            reply = ""
            if order is None:
                reply = "Order with hash "+argv[4]+" not fined."
            else:
                reply = "Order: \n"
                reply += "\tVersion:" + str(order.version)+"\n"
                reply += "\tUID: " + str(order.uid) + "\n"
                reply += "\tDirection: " + str(order.direction) + "\n"
                reply += "\tNode addr: " + str(order.nodeAddr) + "\n"
#                reply += "\tCond hash: " + str(order.condHash) + "\n"
                reply += "\tPrice unit: " + str(order.priceUnit) + "\n"
                created = datetime.fromtimestamp(order.tsCreated)
                reply += "\tCreated: " + str(created) + "\n"
                expires = datetime.fromtimestamp(order.tsExpires)
                reply += "\tExpires: " + str(expires) + "\n"
                reply += "\tSrv price: " + str(order.srvPrice) + "\n"
                reply += "\tSrv price ticker: " + order.srvPriceTicker + "\n"
                reply += "\text size:" + str(order.extSize) + "\n"
                reply += "\text:" + str(order.ext) + "\n"
            AppCliServer.setReplyText(reply, indexStrReply)
        elif argv[3] == "del":
            res = ChainNetSrvOrder.delete(net, argv[4])
            AppCliServer.setReplyText("Result: "+str(res)+".", indexStrReply)
        else:
            AppCliServer.setReplyText("An unknown action is specified to be performerd on the order.", indexStrReply)
    else:
        AppCliServer.setReplyText("This command takes only four arguments.", indexStrReply)



def infoCMD(argv, indexStrReply):
    if (len(argv) == 2):
        hash = ChainHashFast.strToHashFast(argv[1])
        order = ChainNetSrvOrder.find(hash)
        reply = "Order: \n"
        reply += "\tVersion:" + str(order.version)+"\n"
        reply += "\tUID: " + str(order.uid) + "\n"
        reply += "\tDirection: " + str(order.direction) + "\n"
        reply += "\tNode addr: " + str(order.nodeAddr) + "\n"
        reply += "\tCond hash: " + str(order.condHash) + "\n"
        reply += "\tPrice unit: " + str(order.priceUnit) + "\n"
        created = datetime.fromtimestamp(order.tsCreated)
        reply += "\tCreated: " + str(created) + "\n"
        expires = datetime.fromtimestamp(order.tsExpires)
        reply += "\tExpires: " + str(expires) + "\n"
        reply += "\tSrv price: " + str(order.srvPrice) + "\n"
        reply += "\tSrv price ticker: " + order.srvPriceTicker + "\n"
        reply += "\text size:" + str(order.extSize) + "\n"
        reply += "\text:" + str(order.ext) + "\n"
        AppCliServer.setReplyText(reply, indexStrReply)
    else:
        AppCliServer.setReplyText("Order hash not set", indexStrReply)

"""

"""

def create_key():
    return Crypto.generateNewKey(CryptoKeyType.DAP_ENC_KEY_TYPE_SIG_BLISS(), "SARON00151454_VDGTKHJFY", 512, " I SIMPKLE SEED STRING ")

def order_notify(op_code, group, key, order):
    print("Notify: \n")
    print("op_code: "+op_code+"\n")
    print("group: "+group+"\n")
    print("key: "+key+"\n")
    if order is not None:
        print(infoOrder(order))
    else:
        print("No order.")

def init():
    logItNotice("Running plugin order")
    net = ChainNet.byName("kelvin-testnet")
#Getting info
    ChainNetSrvOrder.addNotify(net, order_notify)
    gdb_group = ChainNetSrvOrder.getGdbGroup(net)
    logItNotice("Ordr in group: "+gdb_group)
    node_list_group = ChainNetSrvOrder.getNodelistGroup(net)
    logItNotice("Node list group: "+node_list_group)
#Create order
    print("net: "+ str(net))
    net_srv = ChainNetSrvUID.fromStr("0x0a")
    direction = ChainNetSrvOrderDirection.getDirSell()
    print("direction" + str(direction))
    node_addr = net.getCurAddr()
    tx_cond = "TXCOND"
    price = 531
    priceUnitUid = ChainNetSrvPriceUnitUID.mb()
    print ("Price Unit UID:" + str(priceUnitUid))
    key = create_key()
    #Expires
    dt_exp = datetime.now()
    dt_delta = timedelta(days = 30)
    dt_exp += dt_delta
    print (dt_exp)
    print (dt_exp.timestamp())
    ts_expires = dt_exp.timestamp()
    print(type(ts_expires))
    order = ChainNetSrvOrder(net, direction, net_srv, node_addr, None, 100, priceUnitUid, "tCELL", ts_expires, "http://myvi.com/?QBS".encode('utf-8'), key)
    hash = order.save(net)
    logItNotice("HASH order:"+str(hash))
    AppCliServer.cmdItemCreate("pwo", pwoCMD, "Example command for working with orders through the plugin",
"""
    Arguments for command:
    net <net> find <hash order>
    net <net> del <hash order>
""")
    return 0

#def deinit():
#    logItNotice("Plugin client deinit")

