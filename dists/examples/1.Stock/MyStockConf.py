import os

def getJsonString(app_name):
    tmp_dir = os.getcwd() + "/tmp"
    var_dir = os.getcwd() + "/var"
    return """{
        "modules": ["Crypto", "ServerCore", "Http", "HttpFolder", "GlobalDB", "Client", "HttpClientSimple", "Mempool",
         "Chain", "Wallet", "ChainCSDag", "ChainCSDagPoa", "ChainCSDagPos", "GDB", "Net", "AppCliServer", "ChainNetSrv", "EncHttp",
         "Stream", "StreamCtl", "HttpSimple", "StreamChChain", "StreamChChainNet", "StreamChChainNetSrv"],
         "DAP": {
           "config_dir": \""""+tmp_dir+"""\",
           "log_level": "L_DEBUG",
           "application_name": \""""+app_name+"""\",
           "file_name_log": \""""+var_dir+"/log/"+app_name+".log"+"""\"
         },
         "Stream" : {
            "DebugDumpStreamHeaders": false
         },
         "ServerCore" : {
            "thread_cnt": 0,
            "conn": 0
         },
        "Configuration" : {
            "general": {
                "debug_mode": false,
                "debug_dump_stream_headers": false,
                "wallets_default": "default"
            },
            "server": {
                "enabled": true,
                "listen_address": "0.0.0.0",
                "listen_port_tcp": 8079
            },
            "conserver": {
                "enabled": true,
                "listen_unix_socket_path": \""""+tmp_dir+"/node_cli"+"""\"
            },
            "resources": {
                "threads_cnt": 0,
                "pid_path": \""""+ var_dir+"/run/"+ app_name+".pid"+"""\" ,
                "log_file": \""""+var_dir+"/log/"+ app_name+".log"+"""\",
                "wallets_path": \""""+var_dir+"/lib/wallet" + """\",
                "ca_folders": [ \""""+var_dir+"""/lib/ca\" ],
                "dap_global_db_path": \""""+var_dir+"/lib/global_db"+"""\",
                "dap_global_db_driver": "cdb"
            },
            "networks":{
                "mychains-dev": {
                    "general":{
                        "id": "0xFF00000000000003",
                        "name": "mychains-dev",
                        "type": "development",
                        "node-role": "root",
                        "gdb_groups_prefix": "devnet"
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
        }
        }"""
