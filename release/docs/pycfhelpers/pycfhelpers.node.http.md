# pycfhelpers.node.http package

## Submodules

## pycfhelpers.node.http.simple module

### *class* pycfhelpers.node.http.simple.CFSimpleHTTPRequest(http: DAP.Network.HttpSimple)

Bases: `object`

Represents a simple HTTP request

#### http

The instance of HttpSimple representing the HTTP request.

* **Type:**
  HttpSimple

#### \_\_init_\_(http: DAP.Network.HttpSimple)

Initialize a CFSimpleHTTPRequest object.

* **Parameters:**
  **http** (*HttpSimple*) – Adress of the CellFrame HTTP request

#### *property* query *: str*

Return the query parameters of the HTTP request.

* **Returns:**
  The query parameters.
* **Return type:**
  str

#### *property* url *: str*

Return the URL path of the HTTP request.

* **Returns:**
  The URL path.
* **Return type:**
  str

#### *property* method *: str*

Return the HTTP method of the request.

* **Returns:**
  The HTTP method.
* **Return type:**
  str

#### *property* body *: bytes*

Return the body of the HTTP request.

* **Returns:**
  The body of the request.
* **Return type:**
  bytes

#### *property* client_address *: str*

Return the client IP address of the HTTP request.

* **Returns:**
  The client IP address.
* **Return type:**
  str

#### *property* header *: str*

Return the heaser of the HTTP request.

* **Returns:**
  The header.
* **Return type:**
  str

### *class* pycfhelpers.node.http.simple.CFSimpleHTTPRequestHandler(methods: list[str], handler: Callable[[[CFSimpleHTTPRequest](#pycfhelpers.node.http.simple.CFSimpleHTTPRequest)], tuple[bytes, int]])

Bases: `object`

Represents a handler for simple HTTP requests.

#### methods

The supported HTTP methods.

* **Type:**
  list[str]

#### handler

The handler function for processing requests.

* **Type:**
  Callable[[[CFSimpleHTTPRequest](#pycfhelpers.node.http.simple.CFSimpleHTTPRequest)], tuple[bytes, int]]

#### \_\_init_\_(methods: list[str], handler: Callable[[[CFSimpleHTTPRequest](#pycfhelpers.node.http.simple.CFSimpleHTTPRequest)], tuple[bytes, int]])

Initializate a CFSimpleHTTPRequestHandler object.

* **Parameters:**
  * **methods** (*list* *[**str* *]*) – The supported HTTP methods
  * **handler** (*Callable* *[* *[*[*CFSimpleHTTPRequest*](#pycfhelpers.node.http.simple.CFSimpleHTTPRequest) *]* *,* *tuple* *[**bytes* *,* *int* *]* *]*) – The handler function for processing requests.

#### \_\_call_\_(http: DAP.Network.HttpSimple, response_code: DAP.Network.HttpCode)

Handle the HTTP request.

* **Parameters:**
  * **http** (*HttpSimple*) – The HTTP request object.
  * **response_code** (*HttpCode*) – The HTTP response code object.

### *class* pycfhelpers.node.http.simple.CFSimpleHTTPServer

Bases: `object`

Represents a simple HTTP server.

#### \_\_init_\_()

Initializate a CFSimpleHTTPServer instanse.

#### register_uri_handler(uri: str, handler: [CFSimpleHTTPRequestHandler](#pycfhelpers.node.http.simple.CFSimpleHTTPRequestHandler))

Register a URI handler for the server.

* **Parameters:**
  * **uri** (*str*) – The URI to handle.
  * **handler** ([*CFSimpleHTTPRequestHandler*](#pycfhelpers.node.http.simple.CFSimpleHTTPRequestHandler)) – The handler for processing the URI requests.

#### *static* handler(uri: str, methods: list[str] = ('GET',), \*\*kwargs)

Decorator to register a URI handler.

* **Parameters:**
  * **uri** (*str*) – The URI to handle.
  * **methods** (*list* *[**str* *]* *,* *optional*) – The HTTP methods supported by the handler. Defaults to [“GET”].
* **Returns:**
  A wrapper function to register the URI handler.
* **Return type:**
  Callable

## Module contents
