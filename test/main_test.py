from libCellFrame import *
import pickle
import os
import sys

def create_config_file(app_name):
    f = open(app_name+".cfg", "w")
    f.write("[server]\nlisten_address=0.0.0.0\n")
    f.close()

print("Start main test")
app_name = "testAPP"
print("Create config file")
create_config_file(app_name)

json_string = """{
    "modules": ["crypto"],
    "DAP": {
       "config_dir": \""""+os.getcwd()+"""\",
       "log_level": "L_DEBUG",
       "application_name": \""""+app_name+"""\",
       "file_name_log": \""""+app_name+""".text\"
    }
    }"""


print("init start")
init(json_string)
logItInfo("Initialization of the DAP done")
setLogLevel(DEBUG)
logItInfo("Level logging ""DEBUG"" done")
logItInfo( "Test. Outputting a string using the log_it function in the libdap library")
logItInfo("Outputting a string using the log_it function done")
res1 = configGetItem("server", "listen_address")
logItInfo("Output [server] 'listen_address' = "+res1+"\n")
res2 = configGetItemDefault("server1", "listen_address", "8.8.8.8")
logItInfo("Output default value '8.8.8.8' [server1] 'listen_address' = "+res2+"\n")
logItInfo( "TEST. Get default config done")

logItInfo ("Create KEY")
key = Crypto.newKey(CryptoKeyType.DAP_ENC_KEY_TYPE_IAES())
del key
logItInfo("Create KEY TWO")
key2 = Crypto.newKey(CryptoKeyType.DAP_ENC_KEY_TYPE_OAES())
logItInfo ("Dellete key")
del key2

logItInfo("TEST BASE58. START...")
s = """Test! I will crush Base58!"""
base_in = pickle.dumps(s)
crypt = Crypto.encodeBase58(base_in)
decrypt = Crypto.decodeBase58(crypt)
out_data = pickle.loads(decrypt)
if s == out_data:
    logItInfo ("TEST 1. Encode/Decode base58 done")
else:
    logItInfo ("TEST 1. Encode/Decode base58 faild")
    sys.exit(1)
logItInfo("TEST. BASE64 START...")
s = "Test! I will crush Base64!"
crypt = Crypto.encodeBase64(bytes(s, "utf-8"), CryptoDataType.DAP_ENC_DATA_TYPE_B64())
decrypt = Crypto.decodeBase64(crypt, CryptoDataType.DAP_ENC_DATA_TYPE_B64())
if bytes(s, "utf-8") == decrypt:
    logItInfo ("TEST 1. Encode/Decode base64 done")
else:
    logItInfo ("TEST 1. Encode/Decode base64 faild")
    sys.exit(1)
logItInfo ("TEST.BASE64 URLSAFE START...")
u = "http://kelvin.foundation/"
crypt_u = Crypto.encodeBase64(bytes(u, "utf-8"), CryptoDataType.DAP_ENC_DATA_TYPE_B64_URLSAFE())
decrypt_u = Crypto.decodeBase64(crypt_u, CryptoDataType.DAP_ENC_DATA_TYPE_B64_URLSAFE())
if bytes(u, "utf-8") == decrypt_u:
     logItInfo ("TEST 2. Encode/Decode base64 urlsafe done")
else:
     logItInfo ("TEST 2. Encode/Decode base64 urlsafe faild")
     sys.exit(2)

logItInfo ("TEST. IAES256 CBC START...")
s = "Test! I will crush iaes256!"
kex_buff = bytes("123", "utf-8")
size_kex_buff = len(kex_buff)
seed = bytes(112771128)
seed_size = len(seed)
key_n = Crypto.generateNewKey(CryptoKeyType.DAP_ENC_KEY_TYPE_IAES(), kex_buff, size_kex_buff, seed, seed_size, 0)
source = bytes(s, "utf-8")
enc = Crypto.encryptIAES256CBCFast(key_n, source, len(source), 2048)
decrypt = Crypto.decryptIAES256CBCFast(key_n, enc, len(enc), 2048)
if bytes(s, "utf-8") == decrypt:
    logItInfo ("TEST 1. Encode/Decode IAES256 CBC FAST done")
else:
    logItInfo ("TEST 1. Encode/Decode IAES256 CBC FAST faild")
    sys.exit(1)

logItInfo ("TEST. OAES START...")
s = "Test! I will crush OAES!"
kex_buff = bytes("114151400014314485131FGXVGHcJFIH", "utf-8")
size_kex_buff = len(kex_buff)
seed = bytes(112771128)
seed_size = len(seed)
key_id = Crypto.generateNewKey(CryptoKeyType.DAP_ENC_KEY_TYPE_OAES(), kex_buff, size_kex_buff, seed, seed_size, 32)
source = bytes(s, "utf-8")
enc = Crypto.encryptOAESFast(key_id, source, len(source), 2048)
decrypt = Crypto.decryptOAESFast(key_id, enc, len(enc), 2048)
if bytes(s, "utf-8") == decrypt:
    logItInfo ("TEST 1. Encode/Decode OAES FAST done")
else:
    logItInfo ("TEST 1. Encode/Decode OAES CBC FAST faild")
    sys.exit(1)



deinit()
logItInfo("Deinitialization done")

os.remove(app_name+".cfg")
logItInfo( "Main test done");

sys.exit(0)
