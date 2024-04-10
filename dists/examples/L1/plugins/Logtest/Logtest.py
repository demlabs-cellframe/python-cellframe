from pycfhelpers.node.logging import CFLog


def init():
    log = CFLog()
    log.debug("debug message")
    log.info("info message")
    log.notice("notice message")
    log.message("simple message")
    log.dap("dap message")
    log.warning("warning message")
    log.att("attention message")
    log.error("error message")
    log.critical("critical message")
    return 0
