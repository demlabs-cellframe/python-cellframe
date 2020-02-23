#!/usr/bin/python3
# CellFrame SDK.Python v0.9-4 Example Application #0
from CellFrame.libCellFrame import *
import os
from MyStockConf import getJsonString
app_name = "MyChains"

init( getJsonString(app_name) )
setLogLevel(DEBUG)

server_host_name = configGetItem("server", "listen_address")
server_port = int(configGetItem("server", "listen_port_tcp"))
sr = ServerCore.listen(server_host_name, server_port, 0)
Http.new(sr, app_name)
EncHttp.addProc(sr, "/enc_http")
Stream.addProcHttp(sr, "/stream")
StreamCtl.addProcHttp(sr, "/stream_ctl")

ev = Events()
ev.start()

logItNotice(app_name+" v0.1 runned on port "+str(server_port))
rc = ServerCore.loop(sr)

deinit()
