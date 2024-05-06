from pycfhelpers.node.logging import CFLog


log = CFLog()


def init():
    log.debug("Debug message")
    log.info("Info message")
    log.notice("Notice message")
    log.message("Message message")
    log.dap("DAP message")
    log.warning("Warning message")
    log.error("Error message")
    log.critical("Critical message")
    return 0
