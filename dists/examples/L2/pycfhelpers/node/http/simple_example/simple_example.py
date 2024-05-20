from DAP.Core import logIt
from pycfhelpers.node.http.simple import (CFSimpleHTTPRequestHandler,
                                          CFSimpleHTTPServer,
                                          CFSimpleHTTPRequest,
                                          CFSimpleHTTPResponse)
from pycfhelpers.node.logging import CFLog


from urllib.parse import urlencode, unquote
from urllib.request import Request, urlopen

# You have to set up a configuration settings
# if you want the Cellframe Node to accept requests.

# Go to the server part and set enabled=true.
# To send requests you also have to set up
# listen_port_tcp and listen_address (from [notify_server]) values.

URI_1 = "/example_1"
URI_2 = "/example_2"
URI_3 = "/example_3"
LISTEN_PORT_TCP = "8079"  # default value from config file.
LISTEN_ADDRESS = "127.0.0.1"  # default value from config file.

# Create an instance of CFLog to see the result.

log = CFLog()


# Create function to send request with method POST and GET
# Retrieve a decoded response and output the response.

def post_request(url, data, headers=None):
    """ Send a POST request.

    Sends data to the specified addres logs the response.
    
    Args:
        url (str): the URL for sending the request.
        data (dict):  data to send.
        headers (dict, optional): additional headers to include in the request.
    """

    # Convert the data dictionary to a URL encoding string
    data = urlencode(data)

    # Encode into a byte object using UTF-8 encoding.
    data = data.encode("utf-8")

    # Create a request object to send a POST request
    # to the specified URL with encoded data.
    # It is possible to send requests with your custom headers.
    request = Request(url, data, method="POST", headers=headers)

    # Open a connection to the server,
    # send a request, and receive a response.
    with urlopen(request) as response:
        response_body = response.read().decode('utf-8')
        response_code = response.getcode()
        response_headers = response.info()
        request_headers = request.headers

        # Output the response:
        log.notice(response_body)
        log.notice(f"Status code: {str(response_code)}")

        log.notice("Request Headers:")
        for header, value in request_headers.items():
            log.notice(f"{header}: {value}")

        log.notice("Response Headers:")
        for header, value in response_headers.items():
            log.notice(f"{header}: {value}")


def get_request(url):
    """Send a GET request to the specified address and log the response.

    Args:
        url (str): The URL for sending the request.
    """

    # Open a connection to the server by using
    # the specified URL and receive a response.
    with urlopen(url) as response:
        response_body = response.read().decode('utf-8')
        response_code = response.getcode()
        response_headers = response.info()

        # Output the response:
        log.notice(response_body)
        log.notice(f"Status code: {str(response_code)}")

        log.notice("Response Headers:")
        for header, value in response_headers.items():
            log.notice(f"{header}: {value}")

# Create handler functions.
# It is possible to return a custom answer containing
# tuple[bytes, int] where "bytes" is a decoded custom body of answer
# and "int" - http status code. Or you can send a predefined
# response in the form of a CFSimpleHTTPResponse instance.


def handler_example_1(request: CFSimpleHTTPRequest) -> CFSimpleHTTPResponse:
    """Handler function for example 1.
    Processes the incoming HTTP request and returns a predefined response.

    Args:
        request (CFSimpleHTTPRequest): The incoming request.

    Returns:
        CFSimpleHTTPResponse: The response containing the body and HTTP status code.
    """
    # Define the response body
    body = (b"The request has been processed by handler "
            b"registered via register_uri_handler function")

    # Create and return the HTTP response with status code 200 (OK)
    return CFSimpleHTTPResponse(body, 200)


def handler_example_2(request: CFSimpleHTTPRequest) -> tuple[bytes, int]:
    """Request handler function for example 2.

    Args:
        request (CFSimpleHTTPRequest): The incoming request.

    Returns:
        tuple[bytes, int]: The response body and HTTP status code.
    """

    # Get request details
    url = request.url
    method = request.method
    body = request.body
    request_headers = request.headers

    # Decode the body if it's not None, using unquote method
    decoded_body = unquote(body) if body is not None else 'None'

    # Create response body content
    response_parts = [
        ("The request has been processed by handler "
         "registered via register_uri_handler function"),
        f"URL = {url}",
        f"Method = {method}",
        f"Body = [{decoded_body}]",
        "Request Headers:"
    ]

    # Add request headers and their values to response body
    for header, value in request_headers.items():
        response_parts.append(f"{header}: {value}")

    # Join all parts into a single response body
    response_body = "\n".join(response_parts)

    # Encode the response body in bytes
    response_body = response_body.encode("utf-8")

    # Return the response body and HTTP status code
    return response_body, 200


# In http.simple module there are two options for registration
# handler functions and their binding to a specific URi.

# The first way to register hadler is to use a decorator
# CFSimpleHTTPServer.handler(). In this example it is registered
# only to the request with "GET" method, therefore it is not passed
# to the arguments. The "uri" is a required argument.
# Below, under the decorator, describe what the handler function will do.

@CFSimpleHTTPServer.handler(uri=URI_2, methods=["GET"])
def decorator_handler_example(request: CFSimpleHTTPRequest) -> CFSimpleHTTPResponse:
    """Handler function registered via decorator.
    Args:
        request (CFSimpleHTTPRequest): The incoming request.

    Returns:
        CFSimpleHTTPResponse: The response containing the body, HTTP status code and headers.
    """
    response_body = b"The request has been processed by handler"
    response_body += b" created via wrapper function"

    # If you want to add custom response headers, create
    custom_headers = {'Custom-Response-Header-1': 'Custom-Value-1',
                      'Custom-Response-Header-2': 'Custom-Value-2',
                      'Custom-Response-Header-3': 'Custom-Value-3'}

    return CFSimpleHTTPResponse(response_body, 200, custom_headers)


def init():

    # The second way is to register via register_uri_handler method
    # of the CFSimpleHTTPServer instance.

    # First, create an instance of the CFSimpleHTTPRequestHandler.
    # The "methods" is an optional argument that's default to "GET",
    # but in this example it is registered for two methods.
    # In the argument "handler" pass the handler function.

    handler_1 = CFSimpleHTTPRequestHandler(methods=["GET", "POST"],
                                           handler=handler_example_1)
    handler_2 = CFSimpleHTTPRequestHandler(methods=["POST"],
                                           handler=handler_example_2)

    server = CFSimpleHTTPServer()
    server.register_uri_handler(uri=URI_1, handler=handler_1)
    server.register_uri_handler(uri=URI_3, handler=handler_2)

    # IMPORTANT!
    # If you register a handler function through a decorator,
    # do it outside the "init" function. If register via register_uri_handler
    # method, do it inside the "init" function.

    # Create the URL paths.

    URL_1 = f"http://{LISTEN_ADDRESS}:{LISTEN_PORT_TCP}{URI_1}"
    URL_2 = f"http://{LISTEN_ADDRESS}:{LISTEN_PORT_TCP}{URI_2}"
    URL_3 = f"http://{LISTEN_ADDRESS}:{LISTEN_PORT_TCP}{URI_3}"

    # Create some data.
    data = {'Hello,': 'world!'}

    # Create custom headers.
    custom_headers = {'Custom-Request-Header-1': 'Custom-Value-1',
                      'Custom-Request-Header-2': 'Custom-Value-2'}
    # Get the responses from the functions that send requests.

    # This request will be processed by a handler_example_1 function.
    get_request(URL_1)

    # This request will be also processed by a handler_example_1 function,
    # But answer will contain specified data and request headers.
    post_request(URL_1, data, headers=custom_headers)

    # This request will be processed by a decorator_handler_example function.
    # The response will contain additional headers, wich is specified in
    # a hadler function.
    get_request(URL_2)

    # The last request will be processed by the handler_example_2 function.
    post_request(URL_3, data, headers=custom_headers)

    return 0

# Start or restart the Cellframe Node check the log.