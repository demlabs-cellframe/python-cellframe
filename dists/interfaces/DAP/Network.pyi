from typing import Any, Protocol, Callable
from enum import Enum


class ServerType(Enum):
    """
    Enumeration of server types.
    """
    SERVER_TCP = 0
    SERVER_UDP = 1
    SERVER_LOCAL = 2


# PyDapServerObject

class Server(Protocol):
    """
    A server that can listen for incoming connections.
    """
    @staticmethod
    def listen(addr: str, port: int, type: int, /) -> 'Server':
        """
        Start listening for incoming connections.

        This method initializes the server to listen for incoming connections 
        on the specified address and port. It creates a new server instance 
        and returns it to the caller.

        Args:
            addr (str): The address on which the server will listen for connections.
            port (int): The port on which the server will listen for connections.
            type (int): The type of server (usually 0 for TCP either 1 for UDP or 2 for local).

        Returns:
            Server: An instance of the Server that is configured to listen on 
                    the specified address and port.

        Raises:
            ValueError: If the type of server is invalid (not 0 or 1).
        """
        pass


# DapEventsObjectType
class Events(Protocol):
    """
    The Events class provides an interface to manage and interact with
    event-driven socket operations in the DAP system. It allows starting
    and waiting for events, as well as removing and deleting event sockets.
    """

    def start(self) -> int:
        """
        Start the event system.

        Returns:
            int: The result of starting the event system, usually 0 for success.
        """
        pass

    def wait(self) -> int:
        """
        Wait for events to occur.

        Returns:
            int: The result of waiting for events, usually 0 for success.
        """
        pass

    def killSocket(self, socket: 'EventsSocket') -> int:
        """
        Kill a specific event socket.

        Args:
            socket (EventsSocket): The event socket to be killed.

        Returns:
            int: The result of the operation, usually 0 for success.
        """
        pass

    def removeAndDeleteSocket(self, socket: 'EventsSocket', preserve_inheritor: Optional[bool] = True) -> int:
        """
        Remove and delete a specific event socket.

        Args:
            socket (EventsSocket): The event socket to be removed and deleted.
            preserve_inheritor (Optional[bool]): Whether to preserve inheritors of the socket. Defaults to True.

        Returns:
            int: The result of the operation, usually 0 for success.
        """
        pass

    def __del__(self) -> None:
        """
        Deallocate and clean up resources held by the Events object.
        """
        pass


# DapEventsSocketObjectType
class EventsSocket(Protocol):
    pass

class EventsSocket(Protocol):
    """
    This class represents an event-driven socket in the CellFrame system. It provides methods to interact with the 
    event socket, including creating, reading, writing, and deleting the socket, as well as managing its state.
    """

    def createAfter(self) -> int:
        """
        Adds the event socket to the worker for processing after its creation.

        Returns:
            int: Status code indicating the success (0) or failure of the operation.
        """
        pass

    def setReadable(self, is_ready: bool) -> int:
        """
        Sets the event socket as readable or not readable.

        Args:
            is_ready (bool): True if the socket should be marked as readable, False otherwise.

        Returns:
            int: Status code indicating the success (0) or failure of the operation.
        """
        pass

    def setWritable(self, is_ready: bool) -> int:
        """
        Sets the event socket as writable or not writable.

        Args:
            is_ready (bool): True if the socket should be marked as writable, False otherwise.

        Returns:
            int: Status code indicating the success (0) or failure of the operation.
        """
        pass

    def write(self, data: bytes, size: int) -> int:
        """
        Writes data to the event socket.

        Args:
            data (bytes): The data to write to the socket.
            size (int): The number of bytes to write.

        Returns:
            int: The number of bytes written to the socket.
        """
        pass

    def writeF(self, format: str, *args) -> int:
        """
        Writes formatted data to the event socket.

        Args:
            format (str): The format string.
            *args: Arguments for the format string.

        Returns:
            int: The number of bytes written to the socket.
        """
        pass

    def read(self, size: int) -> tuple[int, bytes]:
        """
        Reads data from the event socket.

        Args:
            size (int): The number of bytes to read.

        Returns:
            tuple[int, bytes]: A tuple containing the number of bytes read and the read data.
        """
        pass

    def delete(self, preserve_inheritor: bool) -> int:
        """
        Deletes the event socket and optionally preserves the inheritor.

        Args:
            preserve_inheritor (bool): Whether to preserve the inheritor.

        Returns:
            int: Status code indicating the success (0) or failure of the operation.
        """
        pass

    def shrinkBufIn(self, size: int) -> int:
        """
        Shrinks the input buffer of the event socket.

        Args:
            size (int): The number of bytes to shrink the buffer by.

        Returns:
            int: Status code indicating the success (0) or failure of the operation.
        """
        pass


# DapHttpObjectType
class Http(Protocol):
    """
    The Http class represents an HTTP service within the CellFrame system. It provides methods to create
    and manage HTTP services associated with a specific server.
    """
    def new(self, name) -> int:
        """_summary_

        Returns:
            int: _description_

        Raises:
            SystemError: If it is not possible to add a handler to a non-existent server.
        """

    @staticmethod
    def new(server: 'Server', name: str) -> int:
        """
        Create a new HTTP service associated with the specified server.
        
        Args:
            server (Server): The server object to which the HTTP service will be attached.
            name (str): The name of the new HTTP service.
        
        Returns:
            int: Status code indicating the success (0) or failure of the operation.
        
        Raises:
            SystemError: If it is not possible to add a handler to a non-existent server.
        """
        pass

# DapHttpCodeObjectType
class HttpCode(Protocol):
    def set(self, http_status_code: int, /) -> None:
        """Set specified status code Http response

        Args:
            http_status_code (int): Status code to set.
        """
        pass

    def OK(self) -> None:
        """Set status code OK(200) for this object"""
        pass

    def BadRequest(self) -> None:
        """Set status code Bad request(400) for this object"""
        pass

# DapHttpHeaderObjectType
class HttpHeader(Protocol):
    """
    The HttpHeader class allows you to work with HTTP headers for HttpSimple.

    This class wraps HTTP header functionalities and provides methods to
    interact with HTTP header objects in a Pythonic way.
    """

    def __init__(self, name: str, value: str):
        """
        Initialize a new HttpHeader instance.

        Args:
            name (str): The name of the HTTP header.
            value (str): The value of the HTTP header.

        Raises:
            ValueError: If name or value are not provided.
        """
        pass

    @property
    def name(self) -> str:
        """
        Get the name of the HTTP header.

        Returns:
            str: The name of the HTTP header.
        """
        pass

    @property
    def value(self) -> str:
        """
        Get the value of the HTTP header.

        Returns:
            str: The value of the HTTP header.
        """
        pass

    def append(self, other: 'HttpHeader') -> None:
        """
        Append another HttpHeader to this header.

        Args:
            other (HttpHeader): Another HttpHeader object to append.
        
        Raises:
            TypeError: If other is not an instance of HttpHeader.
        """
        pass

    def __str__(self) -> str:
        """
        Get the string representation of the HTTP header.

        Returns:
            str: The string representation in the format "name:value".
        """
        pass

    def __iter__(self):
        """
        Return an iterator object to iterate over the HTTP headers.

        Returns:
            Iterator: An iterator over the HTTP header objects.
        """
        pass

    def __next__(self):
        """
        Return the next HTTP header in the iteration.

        Returns:
            HttpHeader: The next HTTP header object.

        Raises:
            StopIteration: If there are no more headers.
        """
        pass



# DapHttpSimpleObjectType
class HttpSimple(Protocol):
    # read-only
    # action: str
    # query: str
    # ipClient: str
    # urlPath: str
    # request: Any

    @property
    def action(self) -> str:
        pass

    @property
    def query(self) -> str:
        pass

    @property
    def ipClient(self) -> str:
        pass

    @property
    def urlPath(self) -> str:
        pass

    @property
    def request(self) -> Any:
        pass
    

    @property
    def requestHeader(self) -> list: ...

    def init(self):
        pass

    def deinit(self):
        pass

    @staticmethod
    def addProc(server, url, reply_size_max, callback):
        pass

    def setPassUnknownUserAgents(self):
        pass

    def setFlagGenerateDefaultHeader(self):
        pass

    def replyAdd(self, reply_message: bytes):
        pass

    def setResponseHeader(self):
        pass

    def getResponseHeader(self) -> HttpHeader:
        pass


class HttpSimple(Protocol):
    """
    Зrovides a simplified interface for handling HTTP requests and responses.
    It allows you to initialize the HTTP module, add URL processors, handle HTTP requests, and manage HTTP headers.
    """

    @property
    def action(self) -> str:
        """
        The HTTP method (action) of the request, such as 'GET', 'POST', etc.

        Returns:
            str: The HTTP method.
        """
        pass

    @property
    def query(self) -> str:
        """
        The query string of the request URL.

        Returns:
            str: The query string.
        """
        pass

    @property
    def ipClient(self) -> str:
        """
        The IP address of the client making the request.

        Returns:
            str: The IP address.
        """
        pass

    @property
    def urlPath(self) -> str:
        """
        The URL path of the request.

        Returns:
            str: The URL path.
        """
        pass

    @property
    def request(self) -> Any:
        """
        The body of the request in bytes.

        Returns:
            Any: The request body.
        """
        pass

    @property
    def requestHeader(self) -> list[HttpHeader]:
        """
        The headers included in the request.

        Returns:
            list[HttpHeader]: A list of request headers.
        """
        pass

    def init(self) -> int:
        """
        Initialize the HTTP Simple module.

        Returns:
            int: Status code indicating the result of the initialization.
        """
        pass

    def deinit(self) -> None:
        """
        Deinitialize the HTTP Simple module.
        """
        pass

    @staticmethod
    def addProc(server: Server, url: str, reply_size_max: int, callback: Callable[[Any, int], None]) -> None:
        """
        Add a URL processor to handle HTTP requests.

        Args:
            server (Server): The server to which the processor will be added.
            url (str): The URL path that the processor will handle.
            reply_size_max (int): The maximum size of the response.
            callback (Callable[[Any, int], None]): The function to call when handling requests to this URL.
        """
        pass

    def setPassUnknownUserAgents(self, pass_ua: bool) -> None:
        """
        Set whether to pass requests from unknown user agents.

        Args:
            pass_ua (bool): If True, requests from unknown user agents will be passed.
        """
        pass

    def setFlagGenerateDefaultHeader(self, flag: bool) -> None:
        """
        Set the flag to generate default HTTP headers.

        Args:
            flag (bool): If True, default headers will be generated.

        """
        pass

    def replyAdd(self, reply_message: bytes) -> int:
        """
        Add a reply message to the response.

        Args:
            reply_message (bytes): The message to include in the response.

        Returns:
            int: The size of the reply added.
        """
        pass

    def setResponseHeader(self, headers: HttpHeader) -> None:
        """
        Set the headers for the response.

        Args:
            headers (HttpHeader): The headers to set for the response.
        """
        pass

    def getResponseHeader(self) -> HttpHeader:
        """
        Get the headers set for the response.

        Returns:
            HttpHeader: The response headers.
        """
        pass


# DapEncServerObjectType
class EncHttp(Protocol):
    pass


# DapStreamObjectType
class Stream(Protocol):
    pass

# DapStreamCtlObjectType
class StreamCtl(Protocol):
    pass

# DapJsonRpcRequestObjectType
class JSONRPCRequest(Protocol):
    pass

# DapJsonRpcResponseobjectType
class JSONRPCResponse(Protocol):
    pass


class ClientHTTP(Protocol):
    """
    A class for creating and managing HTTP client requests in the DAP Network.

    Attributes:
        uplink_addr (str): The address of the uplink server.
        uplink_port (int): The port of the uplink server.
        method (str): The HTTP method to use for the request (e.g., 'GET', 'POST').
        request_content_type (str): The content type of the request.
        path (str): The URL path to request on the server.
        request (bytes): The body of the request.
        cookie (str): The cookie to include in the request.
        response_callback (Callable[[bytes, Any], None]): A callback function to handle the response.
        error_callback (Callable[[int, Any], None]): A callback function to handle errors.
        callback_args (Any): Additional arguments to pass to the callbacks.
        custom_headers (list): A list of custom headers to include in the request.
        over_ssl (bool): Whether to use SSL for the request.
    """

    def __init__(self, uplink_addr: str, uplink_port: int, method: str, request_content_type: str,
                 path: str, request: bytes, cookie: str, response_callback: Callable[[bytes, Any], None],
                 error_callback: Callable[[int, Any], None], callback_args: Any, custom_headers: list,
                 over_ssl: bool) -> None:
        """
        Initializes a new instance of the ClientHTTP class.

        Args:
            uplink_addr (str): The address of the uplink server.
            uplink_port (int): The port of the uplink server.
            method (str): The HTTP method to use for the request (e.g., 'GET', 'POST').
            request_content_type (str): The content type of the request.
            path (str): The URL path to request on the server.
            request (bytes): The body of the request.
            cookie (str): The cookie to include in the request.
            response_callback (Callable[[bytes, Any], None]): A callback function to handle the response.
            error_callback (Callable[[int, Any], None]): A callback function to handle errors.
            callback_args (Any): Additional arguments to pass to the callbacks.
            custom_headers (list): A list of custom headers to include in the request.
            over_ssl (bool): Whether to use SSL for the request.

        Example:
            >>> client = ClientHTTP(
            >>>     uplink_addr="127.0.0.1",
            >>>     uplink_port=8079,
            >>>     method="POST",
            >>>     request_content_type="application/json",
            >>>     path="/api/test",
            >>>     request=b'{"key": "value"}',
            >>>     cookie="sessionid=abc123",
            >>>     response_callback=my_response_handler,
            >>>     error_callback=my_error_handler,
            >>>     callback_args=None,
            >>>     custom_headers=["Header1: value1", "Header2: value2"],
            >>>     over_ssl=False
            >>> )
        """
        pass

    @staticmethod
    def getTimeout() -> int:
        """
        Gets the timeout setting for the HTTP client connection in milliseconds.

        Returns:
            int: The timeout in milliseconds.

        Example:
            >>> timeout = ClientHTTP.getTimeout()
            >>> print(f"Timeout is set to {timeout} ms")
        """
        pass

