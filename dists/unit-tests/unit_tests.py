from DAP.Core import logIt
from DAP.Crypto import HashFast
from CellFrame.Chain import ChainAddr
import sys
import modules.demoCustomCMD as customCMD


class UnitTest:
    def __init__(self, name, func, result):
        self.name = name
        self.func = func
        self.result = result

    def run(self):
        try:
            result = self.func()
        except Exception as e:
            return False, repr(e)
        if result == self.result:
            return True, None
        else:
            return False, "The function was supposed to return a value of "+str(self.result)+", but returned " \
                                                                                             ""+str(result)+"."

class UnitTests:
    def __init__(self):
        self.test = []
    def registration(self, test):
        self.test.append(test)
    def registration(self, name, func, result):
        test = UnitTest(name, func, result)
        self.test.append(test)

def init():
    logIt.notice("Launching the unit testing plugin.")
    unittest = UnitTests()
    logIt.notice("Registering features to be tested.")
    unittest.registration("Test pass", test_pass, 1)
    unittest.registration("Test for creating a custom CLI command", customCMD.init, 0)
    unittest.registration(
        "Test for getting a HashFast object from a string",
        test_get_hash_fast,
        "0x795327F8B194E24BC6067F5C95DB5B31A74C7CAF28AD74D0A805C960192BD57E")
    unittest.registration(
        "Test for getting an instance of an object of type ChainAddr from a string",
        test_get_chain_addr,
        "mWNv7A43YnqRHCWVJCb2oR1ZaBwaiDQAgZzf3UjXGercDkFwre8z7ShMpsWRRRjRJfdJfQbs8EUqDzKG232a4fpahKBDjfV8ru1LeWTP"
    )
    unittest.registration("Test FAIL", test_pass, 0)
    len_tests = len(unittest.test)
    logIt.notice("Start testing "+str(len_tests)+" modules (functions).")
    i = 0
    for test in unittest.test:
        i += 1
        logIt.info("["+str(i)+"/"+str(len_tests)+"] "+test.name)
        res = test.run()
        if res[0] == False:
            logIt.error("Test FAILED! \n Err: "+res[1])
            sys.exit(-1)
        else:
            logIt.notice("Test PASS!")
    return 0

def test_pass():
    return 1

def test_get_hash_fast():
    ch_hf = HashFast.fromString("abc")
    if ch_hf is not None:
        return False
    ch_hf = HashFast.fromString("0x03FKEN02E0932JF")
    if ch_hf is not None:
        return False
    ch_hf = HashFast.fromString("0x795327F8B194E24BC6067F5C95DB5B31A74C7CAF28AD74D0A805C960192BD57E")
    return str(ch_hf)

def test_get_chain_addr():
    addr = ChainAddr.fromStr("mFJJEF2312AAGIWJIEc1885841")
    if addr is not None:
        return False
    addr = ChainAddr.fromStr("mWNv7A43YnqRHCWVJCb2oR1ZaBwaiDQAgZzf3UjXGercDkFwre8z7ShMpsWRRRjRJfdJfQbs8EUqDzKG232a4fpahKBDjfV8ru1LeWTP")
    return str(addr)
