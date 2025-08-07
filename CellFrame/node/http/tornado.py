import asyncio
import sys
import json
import time
import urllib
import threading

from CellFrame.legacy.pycfhelpers.logger import log
from typing import Optional, Callable, Tuple

try:
    import tornado
    from tornado.web import RequestHandler, Application
    from tornado.httpserver import HTTPServer
except ImportError:
    log.warning("[TORNADO] Tornado not installed, external HTTP server will be unavailable")

MAX_CONCURRENT_REQUESTS = 10
REQUEST_TIMEOUT_SECONDS = 180

http_loop = None
http_thread = None
http_server: HTTPServer | None = None

class Http:
    """
    Cellframe-compatible request wrapper.
    """

    def __init__(self, trnd: RequestHandler):
        self.trnd = trnd
        self._body_data: Optional[dict] = None
        log.notice(f"[TORNADO] Http wrapper initialized for request: {trnd.request.method} {trnd.request.uri}")

    @property
    def query(self) -> str:
        query = self.trnd.request.query
        log.notice(f"[TORNADO] Raw query string: {query}")
        return query

    @property
    def body(self) -> dict:
        if self._body_data is None:
            try:
                self._body_data = json.loads(self.trnd.request.body.decode())
                log.notice(f"[TORNADO] Parsed JSON body: {self._body_data}")
            except Exception as e:
                log.warning(f"[TORNADO] Failed to parse JSON body: {e}")
                self._body_data = {}
        return self._body_data

    def replyAdd(self, s: str):
        self.trnd.write(s)


class HttpCode:
    def __init__(self):
        self.code = 200
        log.notice("[TORNADO] HttpCode initialized with default 200")

    def set(self, v: int):
        self.code = v
        log.notice(f"[TORNADO] HttpCode set to {v}")

    def get(self) -> int:
        return self.code


def build_main_handler(
    handler_fn: Callable[[Http, HttpCode], None],
    max_concurrent_requests: int = MAX_CONCURRENT_REQUESTS,
    request_timeout_seconds: int = REQUEST_TIMEOUT_SECONDS,
):
    """
    Builds a Tornado RequestHandler class that wraps `handler_fn` with
    concurrency and timeout control.

    Args:
        handler_fn: function or coroutine taking (Http, HttpCode).
        max_concurrent_requests: max number of simultaneous requests.
        request_timeout_seconds: per-request timeout in seconds.
    """
    semaphore = asyncio.Semaphore(max_concurrent_requests)

    class MainHandler(RequestHandler):
        async def prepare(self):
            self.start_time = time.monotonic()
            self.set_header("Content-Type", "application/json")
            self.set_header("Access-Control-Allow-Origin", "*")
            self.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization")
            self.set_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS")
            log.notice(f"[TORNADO] Received {self.request.method} {self.request.uri}")

        async def get(self):
            await self._handle()

        async def post(self):
            await self._handle()

        async def options(self):
            self.set_status(204)
            self.finish()

        async def _handle(self):
            http = Http(self)
            code = HttpCode()

            try:
                async with semaphore:
                    log.notice("[TORNADO] Request entered semaphore queue")
                    await asyncio.wait_for(
                        self._run_handler(http, code),
                        timeout=request_timeout_seconds,
                    )
            except asyncio.TimeoutError:
                log.warning("[TORNADO] Request timed out")
                self.set_status(504)
                self.finish(json.dumps({"error": "Request timeout", "code": 504}))
                return
            except Exception as e:
                log.error(f"[TORNADO] Handler error: {e}")
                self.set_status(500)
                self.finish(json.dumps({"error": str(e)}))
                return

            status = code.get()
            log.notice(f"[TORNADO] Final HTTP status: {status}")
            self.set_status(status)
            self.finish()

        async def _run_handler(self, http_obj, code_obj):
            if asyncio.iscoroutinefunction(handler_fn):
                await handler_fn(http_obj, code_obj)
            else:
                loop = asyncio.get_event_loop()
                await loop.run_in_executor(None, handler_fn, http_obj, code_obj)

        def on_finish(self):
            duration = time.monotonic() - self.start_time
            log.notice(f"[TORNADO] {self.request.method} {self.request.uri} -> {self.get_status()} in {duration:.3f}s")

    return MainHandler

def init_tornado_http(
    routes: list[Tuple[str, Callable[[Http, HttpCode], None]]],
    port: int,
    max_concurrent_requests: int = MAX_CONCURRENT_REQUESTS,
    request_timeout_seconds: int = REQUEST_TIMEOUT_SECONDS,
):
    """
    Launch Tornado server in a separate thread.

    Args:
        routes: list of (url_pattern, handler_fn) tuples.
        port: TCP port to listen on.
        max_concurrent_requests: overrides default concurrency.
        request_timeout_seconds: overrides default timeout.
    """
    if "tornado" not in sys.modules:
        raise BaseException("[TORNADO] Tornado not installed")

    global http_loop, http_thread

    async def eventloop():
        global http_server
        handlers = [
            (rf"{url}.*", build_main_handler(
                fn,
                max_concurrent_requests=max_concurrent_requests,
                request_timeout_seconds=request_timeout_seconds,
            ))
            for url, fn in routes
        ]
        app = Application(handlers)
        http_server = HTTPServer(app)
        http_server.listen(port)
        log.notice(f"[TORNADO] Listening on port {port}")
        await asyncio.Event().wait()

    def run():
        global http_loop
        try:
            http_loop = asyncio.new_event_loop()
            asyncio.set_event_loop(http_loop)
            http_loop.run_until_complete(eventloop())
        except Exception as e:
            log.error(f"[TORNADO] HTTP server thread crashed: {e}")

    log.notice(f"[TORNADO] Launching server on port {port} for {len(routes)} routes")
    http_thread = threading.Thread(target=run, daemon=True)
    http_thread.start()


def shutdown_http_server():
    global http_loop, http_server

    if http_server:
        log.warning("[TORNADO] Stopping HTTPServer (closing sockets)...")
        http_server.stop()
        try:
            for sock in getattr(http_server, "_sockets", {}).values():
                sock.close()
            http_server._sockets.clear()
        except Exception as e:
            log.error(f"[TORNADO] Error closing sockets: {e}")
        http_server = None
    else:
        log.warning("[TORNADO] No HTTPServer instance to stop")

    if http_loop and http_loop.is_running():
        log.warning("[TORNADO] Stopping I/O loop...")
        http_loop.call_soon_threadsafe(http_loop.stop)
    else:
        log.warning("[TORNADO] No running I/O loop to stop")
