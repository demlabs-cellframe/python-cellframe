cmake_minimum_required(VERSION 2.8)
project (0.Beginning)

FILE(COPY MyChains.py DESTINATION .)
#FILE(COPY MyChainsConf.py DESTINATION .)
FILE(COPY MyChainsCli.py DESTINATION .)
#ADD_TEST(NAME 0.Beginning COMMAND ${PYTHON_EXECUTABLE} MyChainsTest.sh )
