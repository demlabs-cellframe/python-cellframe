# pycfhelpers.node.http package

## Submodules

## pycfhelpers.node.http.simple module

### *class* pycfhelpers.node.http.simple.CFSimpleHTTPRequest(http: DAP.Network.HttpSimple)

The simple HTTP request.


#### Attributes:

##### http

The HTTP request object.

* **Type:** *HttpSimple*

\_\_init_\_(http: DAP.Network.HttpSimple)

Initializate a CFSimpleHTTPRequest object.

* **Parameters:**
  **http** (*HttpSimple*) – Adress of the CellFrame HTTP request

##### *property* body

Return the body of the HTTP request.

* **Returns:** The body of the request.
* **Return type:** *bytes*

##### *property* client_address

Return the client IP address of the HTTP request.
* **Returns:** The client IP address.
* **Return type:** *str*

##### *property* method

Return the HTTP method of the request.
* **Returns:** The HTTP method.
* **Return type:** *str*

##### *property* query

Return the query parameters of the HTTP request.
* **Returns:** The query parameters.
* **Return type:** *dict*

##### *property* url

Return the URL path of the HTTP request.
* **Returns:** The URL path.
* **Return type:** *str*

### *class* pycfhelpers.node.http.simple.CFSimpleHTTPRequestHandler(methods: list[str], handler: Callable[[[CFSimpleHTTPRequest](#pycfhelpers.node.http.simple.CFSimpleHTTPRequest)], tuple[bytes, int]])

The handler for simple HTTP requests.


#### Attributes:

##### methods

The supported HTTP methods.
* **Type:** *list[str]*

handler

The handler function for processing requests.

* **Type:** *Callable[[[CFSimpleHTTPRequest](#pycfhelpers.node.http.simple.CFSimpleHTTPRequest)], tuple[bytes, int]]*

##### \_\_call_\_(http: DAP.Network.HttpSimple, response_code: DAP.Network.HttpCode)

Handle the HTTP request.

* **Parameters:**
  * **http** (*HttpSimple*) – The HTTP request object.
  * **response_code** (*HttpCode*) – The HTTP response code object.

#### Methods:

##### \_\_init_\_(methods: list[str], handler: Callable[[[CFSimpleHTTPRequest](#pycfhelpers.node.http.simple.CFSimpleHTTPRequest)], tuple[bytes, int]])

Initializate a CFSimpleHTTPRequestHandler object.

* **Parameters:**
  * **methods** (*list* *[**str* *]*) – The supported HTTP methods
  * **handler** (*Callable* *[* *[*[*CFSimpleHTTPRequest*](#pycfhelpers.node.http.simple.CFSimpleHTTPRequest) *]* *,* *tuple* *[**bytes* *,* *int* *]* *]*) – The handler function for processing requests.

### *class* pycfhelpers.node.http.simple.CFSimpleHTTPServer

The simple HTTP server.


#### Attributes:

#### Methods:

##### \_\_init_\_()

Initializate a CFSimpleHTTPServer object.

*static* handler(uri: str, methods: list[str] = ('GET',), \*\*kwargs)

Decorator to register a URI handler.

* **Parameters:**
  * **uri** (*str*) – The URI to handle.
  * **methods** (*list* *[**str* *]*) – The HTTP methods supported by the handler.
* **Returns:** A wrapper function to register the URI handler.
* **Return type:** *Callable*

##### register_uri_handler(uri: str, handler: [CFSimpleHTTPRequestHandler](#pycfhelpers.node.http.simple.CFSimpleHTTPRequestHandler))

Register a URI handler for the server.
* **Parameters:**
  * **uri** (*str*) – The URI to handle.
  * **handler** ([*CFSimpleHTTPRequestHandler*](#pycfhelpers.node.http.simple.CFSimpleHTTPRequestHandler)) – The handler for processing the URI requests.

## Module contents
