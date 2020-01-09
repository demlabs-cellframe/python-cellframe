from CellFrame import *
import os

app_name = "example-node"
tmp_dir = os.getcwd() + "/testdir"
var_dir = os.getcwd() + "/var_dir"

json_string = """{
    "modules": ["Crypto", "ServerCore", "Http", "HttpFolder", "GlobalDB", "Client", "HttpClientSimple", "Mempool",
     "Chain", "Wallet", "ChainCSDag", "ChainCSDagPoa", "ChainCSDagPos", "GDB", "Net", "ChainNetSrv", "EncHttp",
     "Stream", "StreamCtl", "HttpSimple", "StreamChChain", "StreamChChainNet", "StreamChChainNetSrv"],
    "DAP": {
       "config_dir": \""""+tmp_dir+"""\",
       "log_level": "L_DEBUG",
       "application_name": \""""+app_name+"""\",
       "file_name_log": \""""+app_name+""".text\"
    },
    "Configuration" : {
        "general": {
            "debug_mode": false,
            "debug_dump_stream_headers": false,
            "wallets_default": "default"
        },
        "server": {
            "enabled": false,
            "listen_address": "0.0.0.0",
            "listen_port_tcp": 8079
        },
        "mempool": {
             "accept": false
        },
        "conserver": {
            "enabled": true,
            "listen_unix_socket_path": \""""+var_dir+"""/run/node_cli\"
        },
        "resources": {
            "threads_cnt": 0,
            "pid_path": \""""+var_dir+"""/run/cellframe-node.pid\",
            "log_file": \""""+var_dir+"""/log/cellframe-node.log\",
            "wallets_path": \""""+var_dir+"""/lib/wallet\",
            "ca_folders": [
                \""""+var_dir+"""/lib/ca\",
                \""""+tmp_dir+"""/share/ca\"
            ],
            "dap_global_db_path": \""""+var_dir+"""/lib/global_db\",
            "dap_global_db_driver": "cdb"
        },
        "networks":{
            "private": {
                "general":{
                    "id": "0xFF00000000000001",
                    "name": "private",
                    "type": "testing",
                    "node-role": "full",
                    "gdb_groups_prefix": "private",
                    "node-addr-expired": 168,
                    "seed_nodes_ipv4": ["153.256.133.160", "62.216.90.227"],
                    "seed_nodes_port": [8079, 8079],
                    "seed_nodes_aliases": ["kelvin.testnet.root.0", "kelvin.testnet.root.1"],
                    "seed_nodes_addrs": ["ffff::0000::0000::0001","ffff::0000::0000::0002"]
                },
                "name_cfg_files": ["chain-gdb"],
                "conf_files":{
                    "chain-gdb": {
                        "general": {
                            "id": "0xf00000000000000f",
                            "name": "gdb",
                            "consensus": "gdb",
                            "class": "gdb",
                            "datum_types": ["token", "emission", "shard", "ca", "transaction"]
                        },
                        "gdb":{
                            "celled": false
                        }
                    }
                }
            }
        }
    },
    "Stream" : {
        "DebugDumpStreamHeaders": false
    },
    "ServerCore" : {
        "thread_cnt": 0,
        "conn": 0
    }
    }"""

init(json_string)
setLogLevel(DEBUG)

server_host_name = configGetItem("server", "listen_address")
server_port = int(configGetItem("server", "listen_port_tcp"))
sr = ServerCore.listen("0.0.0.0", 3307, 0)
Http.new(sr, app_name)
EncHttp.addProc(sr, "/enc_http")
Stream.addProcHttp(sr, "/stream")
StreamCtl.addProcHttp(sr, "/stream_ctl")

if (configGetItem("mempool", "accept") == "true"):
    Mempool.addProc(sr, "/mempool")

ev = Events()
ev.start()

rc = ServerCore.loop(sr)



deinit()
