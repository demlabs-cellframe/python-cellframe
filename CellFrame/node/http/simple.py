from DAP.Core import AppContext
from DAP.Network import Server, HttpSimple, HttpCode, HttpHeader
from pycfhelpers.logger import log

from typing import Callable, Union

HTTP_REPLY_SIZE_MAX = 10 * 1024 * 1024


class CFSimpleHTTPRequest:
    """
    Represents a simple HTTP request

    Attributes:
        http (HttpSimple): The instance of HttpSimple.
    """
    def __init__(self, http: HttpSimple):
        """Initialize a CFSimpleHTTPRequest object.

        Args:
            http (HttpSimple): Adress of the CellFrame HTTP request
        """
        self.http = http

    @property
    def query(self) -> str:
        """
        Return the query parameters of the HTTP request.

        Returns:
            str: The query parameters.
        """
        return self.http.query

    @property
    def url(self) -> str:
        """
        Return the URL path of the HTTP request.

        Returns:
            str: The URL path.
        """
        return self.http.urlPath

    @property
    def method(self) -> str:
        """
        Return the HTTP method of the request.

        Returns:
            str: The HTTP method.
        """
        return self.http.action

    @property
    def body(self) -> bytes:
        """
        Return the body of the HTTP request.

        Returns:
            bytes: The body of the request.
        """
        return self.http.request

    @property
    def client_address(self) -> str:
        """
        Return the client IP address of the HTTP request.

        Returns:
            str: The client IP address.
        """
        return self.http.ipClient

    @property
    def headers(self) -> dict[str, str]:
        """Return the headers of the HTTP request.

        Returns:
            dict: The headers of the request.
        """
        headers = {}
        for header_object in self.http.requestHeader:
            header_name = header_object.name.strip()
            header_value = header_object.value.strip()
            headers[header_name] = header_value
        return headers


class CFSimpleHTTPResponse:
    """Represents a simple HTTP response."""

    def __init__(self, body: bytes = b"", code: int = 200, headers: dict = None):
        """Initialize a CFSimpleHTTPResponse object.

        Args:
            body (bytes): The body of the response.
            code (int): The HTTP status code of the response.
            headers (dict, optional): The headers of the response. Defaults to None.
        """
        self._body = body
        self._code = code
        self._headers = headers or {}

    @property
    def body(self) -> bytes:
        """Get the response body."""
        return self._body

    @body.setter
    def body(self, value: bytes):
        """Set the response body."""
        if not isinstance(value, bytes):
            raise ValueError("Body must be of type bytes")
        self._body = value

    @property
    def code(self) -> int:
        """Get the HTTP status code."""
        return self._code

    @code.setter
    def code(self, value: int):
        """Set the HTTP status code."""
        if not isinstance(value, int) or not (100 <= value <= 599):
            raise ValueError("Code must be an integer between 100 and 599")
        self._code = value

    @property
    def headers(self) -> dict:
        """Get the response headers."""
        return self._headers

    @headers.setter
    def headers(self, value: dict):
        """Set the response headers."""
        if not isinstance(value, dict):
            raise ValueError("Headers must be a dictionary")
        self._headers.update(value)


class CFSimpleHTTPRequestHandler:
    """Represents a handler for simple HTTP requests."""

    def __init__(self, methods: list[str], handler: Callable[[CFSimpleHTTPRequest], tuple[bytes, int] | CFSimpleHTTPResponse]):
        """Initialize a CFSimpleHTTPRequestHandler object.

        Args:
            methods (list[str]): The supported HTTP methods.
            handler (Callable[[CFSimpleHTTPRequest], tuple[bytes, int] | CFSimpleHTTPResponse]]): The handler function for processing requests.
        """
        self.handler = handler
        self.methods = methods

    def __call__(self, http: HttpSimple, response_code: HttpCode):
        """Handle the HTTP request.

        Args:
            http (HttpSimple): The HTTP request object.
            response_code (HttpCode): The HTTP response code object.
        """
        if http.action not in self.methods:
            log.notice(f"Method {http.action} not supported for {http.urlPath}")
            response_code.set(405)
            return

        result = self.handler(CFSimpleHTTPRequest(http))
        if not isinstance(result, CFSimpleHTTPResponse):
            body, code = result
            http.replyAdd(body)
            response_code.set(code)
            return

        response = result
        http.replyAdd(response.body)

        existing_headers = http.getResponseHeader()
        for header, value in response.headers.items():
            if not existing_headers:
                existing_headers = HttpHeader(header, value)
                continue

            existing_headers.append(header, value)

        if existing_headers is not None:
            http.setResponseHeader(existing_headers)

        response_code.set(response.code)


class CFSimpleHTTPServer:
    """Represents a simple HTTP server."""

    def __init__(self):
        """Initializate a CFSimpleHTTPServer instanse."""
        self._sc = Server()
        AppContext.getServer(self._sc)

    def register_uri_handler(self, uri: str, handler: CFSimpleHTTPRequestHandler):
        """
        Register a URI handler for the server.

        Args:
            uri (str): The URI to handle.
            handler (CFSimpleHTTPRequestHandler): The handler for processing the URI requests.
        """
        HttpSimple.addProc(self._sc, uri, HTTP_REPLY_SIZE_MAX, handler)

    @staticmethod
    def handler(uri: str, methods: list[str] = ("GET",), **kwargs):
        """
        Decorator to register a URI handler.

        Args:
            uri (str): The URI to handle.
            methods (list[str], optional): The HTTP methods supported by the handler. Defaults to ["GET"].

        Returns:
            Callable: A wrapper function to register the URI handler.
        """
        def wrapper(func):
            CFSimpleHTTPServer().register_uri_handler(uri, CFSimpleHTTPRequestHandler(methods=methods, handler=func))

        return wrapper
