# Import necessary modules
from pycfhelpers.node.logging import CFLog
from pycfhelpers.common.parsers import parse_cf_v1_address
from pycfhelpers.node.net import CFNet, CFNetID

# Output the basic information in the logs.
log = CFLog()

# CF v1 address example
address = "jrmnGqeeds4Dp67Ace1RavHGPwUUkcqYmLzdDxR6FhwfxTq7uDWP2rQKZpKCoUaTCQnRcACyUwgfCtPsZfZiRNmvq1YWmiyRFZ1meVD7"


def init():
    # Parse the CF v1 address to get its components:
    version, net_id, sign_id, public_hash, summary_hash, control_hash = parse_cf_v1_address(address)

    # Get the name of the network linked to the wallet by 
    # encoding the network ID into bytes in hexadecimal format:
    net_id_str = f"0x{net_id:016X}"
    # Create a CFNetID instance using the network ID:
    net_id_obj = CFNetID(f'{net_id_str}')
    # Create a CFNet instance using the CFNetID object to retrieve network name:
    net_obj = CFNet(net_id_obj)

    # Log various components extracted from the CF v1 address.
    # version, net_id, and sign_id are presented in the "int" format.
    log.notice(f"Version: {version}")
    log.notice(f"Network: {net_obj.name}")
    log.notice(f"Signature ID: {sign_id}") 

    # Hashes are transmitted in byte format.
    # Therefore, if you need to get them in
    # hexadecimal format, use the hex() method:
    log.notice(f"Public hash: {public_hash.hex()}")
    log.notice(f"Summary hash: {summary_hash.hex()}")
    log.notice(f"Control hash: {control_hash.hex()}")
    return 0