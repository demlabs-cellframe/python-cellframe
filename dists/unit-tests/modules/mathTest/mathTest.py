from DAP.Core import logIt
from DAP.Core import Math

def summTest():
    res = Math("115792089237316195423570985008687907853269984665640564039457584007913129639935")
    a = Math("115792089237316195423570985008687907853269984665640564039457584007913129639934")
    b = 1
    c = a + b
    if res == c:
        a = Math("115792089237316195423570985008687907853269984665640564000457584007913129639930")
        b = 10000005
        c = a + b
        res = Math("115792089237316195423570985008687907853269984665640564000457584007913139639935")
        if res == c:
            return True
    return False

def subTest():
    res = Math("115792089237316195423570985008687907853269984665640564039457584007913129639934")
    a = Math("115792089237316195423570985008687907853269984665640564039457584007913129639935")
    b = a - 1
    if res == b:
        res=Math("110000000000000000000000000000000000000000000000000000000000000000000000000000")
        a = Math("115792089237316195423570985008687907853269984665640564039457584007913129639935")
        b = Math("5792089237316195423570985008687907853269984665640564039457584007913129639935")
        c = a - b
        if res == c:
            return True
    return False

def multiTest():
    a = Math("6000")
    b = a * 2
    if b == 12000:
        a = Math("501000000000")
        a *= a
        if a == Math("251001000000000000000000"):
            return True
    return False

def divTest():
    a = Math("25100100000000000000000000000000000000000000000000000000000000000000")
    b = a / 2
    if b == Math("12550050000000000000000000000000000000000000000000000000000000000000"):
        a /= a
        if a == 1:
            return True
    return False

def compareTest():
    a = Math("3")
    b = 3
    ret = a != b and a < b and b > a
    if ret is False:
        a = Math("115792089237316195423570985008687907853269984665640564039457584007913129639935")
        if a == a:
            b = a - 1
            if a != b and a > b and b < a:
                return True
    return False

def run_unit_test():
    s_false = "| [FALSE] |"
    s_ok =    "| [ OK ]  |"
    lret = True
    logIt.notice("====================================")
    logIt.notice("| Run test 256 - bit math from SDK |")
    logIt.notice("+----------------------------------+")
    logIt.notice("| Name test              |  Result |")
    logIt.notice("+------------------------+---------+")
    try:
        ret = summTest()
    except:
        ret = False
    if ret is True:
        logIt.notice("| Summation              "+s_ok)
    else:
        logIt.notice("| Summation              " + s_false)
        lret = False
    logIt.notice("+------------------------+---------+")
    try:
        ret = subTest()
    except:
        ret = False
    if ret is True:
        logIt.notice("| Subtraction            "+s_ok)
    else:
        logIt.notice("| Subtraction            " + s_false)
        lret = False
    logIt.notice("+------------------------+---------+")
    try:
        ret = multiTest()
    except:
        ret = False
    if ret is True:
        logIt.notice("| Multiplication         "+ s_ok)
    else:
        logIt.notice("| Multiplication         " + s_false)
        lret = False
    logIt.notice("+------------------------+---------+")
    try:
        ret = divTest()
    except:
        ret = False
    if ret is True:
        logIt.notice("| Integer division       " + s_ok)
    else:
        logIt.notice("| Integer division       " + s_false)
        lret = False
    logIt.notice("+------------------------+---------+")
    try:
        ret = compareTest()
    except:
        ret = False
    if ret is True:
        logIt.notice("| Comparison             " + s_ok)
    else:
        logIt.notice("| Comparison             " + s_false)
        lret = False
    logIt.notice("====================================")
    return lret

def init():
    logIt.notice("Start test Math plugin")
    # Max value in 256-bit 115792089237316195423570985008687907853269984665640564039457584007913129639935
    test_res = run_unit_test()
    if test_res is True:
        logIt.notice("256-bit math test completed successfully.")
    else:
        logIt.warning("256-bit math test failed.")
    return 0
