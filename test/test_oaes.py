import libdap_crypto_python_module as crypto
import sys

print ("Start test crypto OAES")
s = "Test! I will crush OAES!"
print ("Input data: "+s)
kex_buff = bytes("114151400014314485131FGXVGHcJFIH", "utf-8")
size_kex_buff = len(kex_buff)
seed = bytes(112771128)
seed_size = len(seed)
crypto.init()
key_id = crypto.generateNewKey(1, kex_buff, size_kex_buff, seed, seed_size, 32)
source = bytes(s, "utf-8")
enc = crypto.encryptOAESFast(key_id, source, len(source), 2048)
decrypt = crypto.decryptOAESFast(key_id, enc, len(enc), 2048)
print (decrypt)


if bytes(s, "utf-8") == decrypt:
    print ("TEST 1. Encode/Decode OAES FAST done")
else:
    print ("TEST 1. Encode/Decode OAES CBC FAST faild")

#print ("Test Base64 URLSAFE")
#u = "http://kelvin.foundation/"
#crypt_u = crypto.encodeBase64(bytes(u, "utf-8"), 2)
#decrypt_u = crypto.decodeBase64(crypt_u)
#if bytes(u, "utf-8") == decrypt_u:
#     print ("TEST 2. Encode/Decode base64 urlsafe done")
#else:
#     print ("TEST 2. Encode/Decode base64 urlsafe faild")
#
sys.exit(0)

