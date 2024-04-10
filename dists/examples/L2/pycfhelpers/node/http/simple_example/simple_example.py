from DAP.Core import logIt
from pycfhelpers.node.http.simple import (CFSimpleHTTPRequestHandler,
                                          CFSimpleHTTPServer,
                                          CFSimpleHTTPRequest)
from pycfhelpers.node.logging import CFLog

from urllib.parse import urlencode
from urllib.request import Request, urlopen

# The http.simple module provides classes for implementing a
# simple HTTP server and handling HTTP requests.

# So that the CellFrame-node can accept requests,
# You need to set up a config file.

# Go to the server part and set enabled=true.
# To send requests you will also need
# listen_port_tcp and listen_address (from [notify_server]) values.

URI_1 = "/example"
URI_2 = "/another"
LISTEN_PORT_TCP = "8079"  # default value from config file.
LISTEN_ADDRESS = "127.0.0.1"  # default value from config file.


# Create function to send request with
# method POST and get a decoded response.

def post_request(url, data):
    """ Send a POST request.

    Sends data to the specified address.

    Args:
        url (str): the URL for sending the request.
        data (dict):  data to send.

    Returns:
       response_body (str): response to the received request.
    """
    # Convert the data dictionary to a URL encoding string
    data = urlencode(data)

    # Encode into a byte object using UTF-8 encoding.
    data = data.encode("utf-8")

    # Create a request object to send a POST request
    # to the specified URL with encoded data.
    request = Request(url, data, method="POST")

    # Open a connection to the server,
    # send a request, and receive a response.
    with urlopen(request) as response:
        response_body = response.read().decode('utf-8')
        return response_body


# Create function to send request with
# method GET and get a decoded response.

def get_request(url):
    """Send a GET request to the specified address.

    Args:
        url (str): the URL for sending the request.

    Returns:
        response_body (str): response to the received request.
    """

    # Open a connection to the server by
    # spesified URL and receive a response.
    with urlopen(url) as response:
        response_body = response.read().decode('utf-8')
        return response_body


# Create a handler function

def handler_example(request: CFSimpleHTTPRequest) -> tuple[bytes, int]:
    """The request handler function.

    Parses the request data and returns it in a diffrent format.

    Args:
        request (CFSimpleHTTPRequest): Request.

    Returns:
        tuple[bytes, int]: contains the response_body and http status code
    """
    # Get the necessary data from the request object.
    url = request.url
    method = request.method
    body = request.body
    client_address = request.client_address

    # Initialize the variable to form the response body.
    response_body = "The request has been processed by handler"
    response_body += " registered via register_uri_handler function"

    # Add the request data to the response body
    response_body += f"\nurl = {url}"
    response_body += f"\nmethod = {method}"
    response_body += f"\nbody = [{body}]"
    response_body += f"\nclient_address = {client_address}"
    response_body += f"\nquery = [{request.query}]"

    # encode the response body in bytes.
    response_body = response_body.encode("utf-8")

    # Return the response body and HTTP status code.
    return response_body, 200


# In http.simple module there are two options for registration
# handler functions and their binding to a specific URi.

# The first way to register hadler is to use a decorator
# CFSimpleHTTPServer.handler(). In this example it is registered
# only to the request with "GET" method, therefore it is not passed
# to the arguments. The "uri" is a required argument.
# Below, under the decorator, describe what the handler function will do.

@CFSimpleHTTPServer.handler(uri=URI_2)
def another_handler_example(request: CFSimpleHTTPRequest) -> tuple[bytes, int]:
    """Another request handler function.

    Args:
        request (CFSimpleHTTPRequest): Request.

    Returns:
        tuple[bytes, int]: contains the response_body and http status code
    """
    # Get the necessary data from the request object.
    method = request.method

    # Initialize the variable to form the response body.
    response_body = "The request has been processed by handler"
    response_body += " created via wrapper function"
    response_body += f"\nmethod = {method}"

    # encode the response body in bytes.
    response_body = response_body.encode("utf-8")

    # Return the response body and HTTP status code.
    return response_body, 200


def init():

    # The second way is to register via register_uri_handler method
    # of the CFSimpleHTTPServer instance.

    # First, create an instanse of the CFSimpleHTTPRequestHandler.
    # The "methods" is an optional argument that's default to "GET",
    # but in this example it is registered for tho methods.
    # In the argument "handler" pass the handler function.

    handler = CFSimpleHTTPRequestHandler(methods=["GET", "POST"],
                                         handler=handler_example)
    CFSimpleHTTPServer().register_uri_handler(uri=URI_1,
                                              handler=handler)

    # IMPORTANT!
    # If you register a handler function through a decorator,
    # do it outside the "init" function. If register via register_uri_handler
    # method, do it inside the "init" function.

    # Create an instanse of CFLog to see the result.

    log = CFLog()

    # Create the url paths.

    url_1 = "http://" + LISTEN_ADDRESS + ":" + LISTEN_PORT_TCP + URI_1
    url_2 = "http://" + LISTEN_ADDRESS + ":" + LISTEN_PORT_TCP + URI_2

    # Create some data.

    data = {'Hello,': 'world!'}

    # Get the responses from the functions that send requests.
    # Log their answers.

    response_get = get_request(url_1)
    log.notice(response_get)

    response_post = post_request(url_1, data)
    log.notice(response_post)

    another_response_get = get_request(url_2)
    log.notice(another_response_get)

    return 0

# Start or restart the CellFrame-node check the log.
