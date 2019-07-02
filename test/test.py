import libdap_crypto_python_module as crypto
import unittest

class TestLibdapCryptoPythonModule(unittest.TestCase):
    def test_ini(self):
        self.assertTrue(crypto.init() == 0, "Failed init libdap crypto python")
    def test_deinit(self):
        self.assertTrue(crypto.deinit() == 0, "Failed deinit libdap crypto python")
    def test_b58(self):
        s = "REvgshguqwt76thuioh1`lwi0-8i-d0hjwpeocnpgh89efty7ug"
        crypt = crypto.encode_base58(s)
        decrypt = crypto.decode_base58(crypt)
        self.assertTrue(s == decrypt, "Encoding and decoded information using the base58 algorithm does not match the original")
    def test_b64(self):
        s = "LindyfekrngFHJFGR23356fer"
        crypt = crypto.encode_base64(s)
        decrypt = crypto.decode_base64(crypt, len(s))
        self.assertTrue(s == decrypt, "Encoding and decoded information using the base64 algorithm does not match the original")

if __name__ == '__main__':
    unittest.main()
