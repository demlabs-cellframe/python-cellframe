import libdap_crypto_python_module as crypto

print ("Start test crypto b64")
s = "Test! I will crush Base64!"
print ("Input data: "+s)
crypt = crypto.encodeBase64(bytes(s, "utf-8"),1)
print ("Encrypted data: \t")
print(crypt)
decrypt = crypto.decodeBase64(crypt, 1)
print ("Decoded data: \t")
print(decrypt)
if bytes(s, "utf-8") == decrypt:
    print ("TEST 1. Encode/Decode base64 done")
else:
    print ("TEST 1. Encode/Decode base64 faild")

print ("Test Base64 URLSAFE")
u = "http://kelvin.foundation/"
crypt_u = crypto.encodeBase64(bytes(u, "utf-8"), 2)
decrypt_u = crypto.decodeBase64(crypt_u)
if bytes(u, "utf-8") == decrypt_u:
     print ("TEST 2. Encode/Decode base64 urlsafe done")
else:
     print ("TEST 2. Encode/Decode base64 urlsafe faild")
