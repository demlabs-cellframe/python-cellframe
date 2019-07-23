import libdap_crypto_python_module as crypto
import pickle

print ("Start test crypto base58")
s = """Test! I will crush Base58!"""
#s = """dll"""
base_in = pickle.dumps(s)
#base_in = bytes(s, "utf-8")
print ("Input data: "+s)
print (base_in)
crypt = crypto.encodeBase58(base_in)
print ("Encrypted data: \t")
print(crypt)
decrypt = crypto.decodeBase58(crypt)
print ("Decoded data: \t")
print(decrypt)
out_data = pickle.loads(decrypt)
if s == out_data:
    print ("TEST 1. Encode/Decode base64 done")
else:
    print ("TEST 1. Encode/Decode base64 faild")
