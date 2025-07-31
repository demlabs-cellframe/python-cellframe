# 🔌 План интеграции веб-интерфейса с SLC Agent

## 📋 Обзор интеграции

Веб-интерфейс интегрируется с SLC Agent через **HTTP REST API** и **WebSocket** соединения. Для этого в агенте нужно создать новый компонент - **Web Interface Server**.

## 🏗️ Архитектура интеграции

```
Web Interface (React)  ←→  Node.js Backend  ←→  SLC Agent HTTP Server
     ↓                          ↓                        ↓
- Dashboard             - Express Routes        - FastAPI/Flask Server  
- Chat                  - Socket.io            - WebSocket Handler
- Minds                 - API Proxy            - Agent API Endpoints
- Analytics             - Authentication       - Real-time Events
```

## 🔧 Компоненты для создания в SLC Agent

### 1. Web Interface Server (`src/api/web_interface_server.py`)

```python
"""
Web Interface Server - HTTP API для веб-интерфейса
"""

from fastapi import FastAPI, WebSocket, HTTPException
from fastapi.middleware.cors import CORSMiddleware
import uvicorn
import asyncio
from typing import Dict, Any, List
import json
from datetime import datetime

class WebInterfaceServer:
    """HTTP API сервер для веб-интерфейса"""
    
    def __init__(self, agent):
        self.agent = agent
        self.app = FastAPI(title="SLC Agent Web API")
        self.websocket_clients = set()
        self.setup_middleware()
        self.setup_routes()
        
    def setup_middleware(self):
        """Настройка CORS и других middleware"""
        self.app.add_middleware(
            CORSMiddleware,
            allow_origins=["http://localhost:3000", "http://localhost:5173"],
            allow_credentials=True,
            allow_methods=["*"],
            allow_headers=["*"],
        )
    
    def setup_routes(self):
        """Настройка API эндпоинтов"""
        
        # Agent Status
        @self.app.get("/api/status")
        async def get_agent_status():
            return {
                "status": "online" if self.agent.is_running else "offline",
                "uptime": self.agent.get_uptime(),
                "version": self.agent.version,
                "lastActivity": datetime.now().isoformat(),
                "currentTask": self.agent.current_task
            }
        
        # System Metrics  
        @self.app.get("/api/metrics")
        async def get_system_metrics():
            return await self.agent.get_system_metrics()
            
        # API Stats
        @self.app.get("/api/api-stats")
        async def get_api_stats():
            if hasattr(self.agent, 'ai_manager'):
                return self.agent.ai_manager.get_api_statistics()
            return []
            
        # Minds Management
        @self.app.get("/api/minds")
        async def get_minds():
            if hasattr(self.agent, 'mind_manager'):
                return self.agent.mind_manager.get_all_minds_status()
            return []
            
        @self.app.post("/api/minds/{mind_id}/activate")
        async def activate_mind(mind_id: str):
            if hasattr(self.agent, 'mind_manager'):
                success = await self.agent.mind_manager.activate_mind(mind_id)
                return {"success": success}
            return {"success": False, "error": "Mind manager not available"}
        
        # Chat
        @self.app.post("/api/chat")
        async def send_message(request: dict):
            message = request.get("message")
            mind_id = request.get("mindId")
            
            # Отправляем сообщение агенту
            response = await self.agent.process_message(message, mind_id)
            
            # Уведомляем WebSocket клиентов
            await self.broadcast_to_websockets("new_message", {
                "id": response.get("id"),
                "type": "assistant", 
                "content": response.get("content"),
                "timestamp": datetime.now().isoformat(),
                "mindId": mind_id
            })
            
            return response
            
        # Logs
        @self.app.get("/api/logs")
        async def get_logs(level: str = None, source: str = None, 
                          search: str = None, limit: int = 100):
            # Получаем логи из системы логирования агента
            return await self.agent.get_logs(level, source, search, limit)
            
        # Context Search
        @self.app.get("/api/context/search")
        async def search_context(query: str):
            if hasattr(self.agent, 'context_manager'):
                return await self.agent.context_manager.search(query)
            return []
            
        # SSH Connections (если есть SSH manager)
        @self.app.get("/api/ssh/connections")
        async def get_ssh_connections():
            if hasattr(self.agent, 'ssh_manager'):
                return self.agent.ssh_manager.get_connections()
            return []
            
        @self.app.post("/api/ssh/connections/{conn_id}/execute")
        async def execute_ssh_command(conn_id: str, request: dict):
            command = request.get("command")
            if hasattr(self.agent, 'ssh_manager'):
                result = await self.agent.ssh_manager.execute_command(conn_id, command)
                
                # Уведомляем WebSocket о выводе
                await self.broadcast_to_websockets("ssh_output", {
                    "connectionId": conn_id,
                    "output": result.get("output"),
                    "timestamp": datetime.now().isoformat()
                })
                
                return result
            return {"success": False, "error": "SSH manager not available"}
        
        # System Operations
        @self.app.post("/api/system/restart")
        async def restart_agent():
            try:
                await self.agent.restart()
                return {"success": True, "message": "Agent restart initiated"}
            except Exception as e:
                raise HTTPException(status_code=500, detail=str(e))
                
        @self.app.post("/api/system/shutdown") 
        async def shutdown_agent():
            try:
                await self.agent.shutdown()
                return {"success": True, "message": "Agent shutdown initiated"}
            except Exception as e:
                raise HTTPException(status_code=500, detail=str(e))
        
        # WebSocket endpoint
        @self.app.websocket("/ws")
        async def websocket_endpoint(websocket: WebSocket):
            await websocket.accept()
            self.websocket_clients.add(websocket)
            
            try:
                while True:
                    data = await websocket.receive_text()
                    message = json.loads(data)
                    await self.handle_websocket_message(websocket, message)
            except Exception as e:
                print(f"WebSocket error: {e}")
            finally:
                self.websocket_clients.discard(websocket)
    
    async def handle_websocket_message(self, websocket: WebSocket, message: dict):
        """Обработка WebSocket сообщений"""
        event_type = message.get("type")
        data = message.get("data", {})
        
        if event_type == "send_message":
            # Обработка сообщения чата
            response = await self.agent.process_message(data.get("message"))
            await websocket.send_text(json.dumps({
                "type": "new_message",
                "data": response
            }))
            
        elif event_type == "request_status":
            # Запрос статуса
            status = await self.get_current_status()
            await websocket.send_text(json.dumps({
                "type": "agent_status", 
                "data": status
            }))
    
    async def broadcast_to_websockets(self, event_type: str, data: dict):
        """Рассылка события всем WebSocket клиентам"""
        if not self.websocket_clients:
            return
            
        message = json.dumps({"type": event_type, "data": data})
        disconnected = set()
        
        for client in self.websocket_clients:
            try:
                await client.send_text(message)
            except:
                disconnected.add(client)
        
        # Удаляем отключенных клиентов
        self.websocket_clients -= disconnected
    
    async def start_server(self, host: str = "0.0.0.0", port: int = 8080):
        """Запуск HTTP сервера"""
        config = uvicorn.Config(
            app=self.app,
            host=host, 
            port=port,
            log_level="info"
        )
        server = uvicorn.Server(config)
        await server.serve()
```

### 2. Интеграция в основной Agent класс

```python
# В src/core/agent.py

class Agent:
    def __init__(self, config):
        # ... существующий код ...
        
        # Добавляем веб-сервер
        if config.get("web_interface", {}).get("enabled", False):
            from ..api.web_interface_server import WebInterfaceServer
            self.web_server = WebInterfaceServer(self)
        else:
            self.web_server = None
    
    async def start(self):
        """Запуск агента"""
        # ... существующий код запуска ...
        
        # Запускаем веб-сервер
        if self.web_server:
            web_config = self.config.get("web_interface", {})
            host = web_config.get("host", "0.0.0.0") 
            port = web_config.get("port", 8080)
            
            # Запускаем в отдельной задаче
            asyncio.create_task(self.web_server.start_server(host, port))
            logger.info(f"🌐 Web Interface Server started on {host}:{port}")
    
    async def process_message(self, message: str, mind_id: str = None):
        """Обработка сообщения из веб-интерфейса"""
        # Используем существующую логику обработки сообщений
        # (аналогично Telegram боту)
        
        if hasattr(self, 'ai_manager'):
            response = await self.ai_manager.process_request(message, mind_id)
            return {
                "id": str(uuid.uuid4()),
                "content": response,
                "timestamp": datetime.now().isoformat(),
                "mindId": mind_id
            }
        
        return {
            "id": str(uuid.uuid4()), 
            "content": "Agent not ready",
            "timestamp": datetime.now().isoformat()
        }
    
    def get_uptime(self) -> int:
        """Получение времени работы агента"""
        if hasattr(self, 'start_time'):
            return int((datetime.now() - self.start_time).total_seconds())
        return 0
    
    async def get_system_metrics(self) -> List[dict]:
        """Получение системных метрик"""
        import psutil
        
        return [{
            "cpu": psutil.cpu_percent(),
            "memory": psutil.virtual_memory().percent,
            "disk": psutil.disk_usage('/').percent,
            "network": {
                "bytesIn": psutil.net_io_counters().bytes_recv,
                "bytesOut": psutil.net_io_counters().bytes_sent
            },
            "timestamp": datetime.now().isoformat()
        }]
    
    async def get_logs(self, level=None, source=None, search=None, limit=100):
        """Получение логов системы"""
        # Интеграция с системой логирования
        # Можно читать из файлов логов или использовать loguru handler
        logs = []
        
        # Пример чтения из файла логов
        try:
            import re
            log_file = "logs/agent.log"
            
            with open(log_file, 'r') as f:
                lines = f.readlines()[-limit:]
                
            for i, line in enumerate(lines):
                # Парсинг строки лога
                match = re.match(r'(\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2}) \| (\w+)\s+\| (.+)', line)
                if match:
                    timestamp, log_level, message = match.groups()
                    
                    if level and log_level not in level:
                        continue
                    if search and search.lower() not in message.lower():
                        continue
                        
                    logs.append({
                        "id": str(i),
                        "level": log_level,
                        "message": message.strip(),
                        "timestamp": timestamp,
                        "source": "agent"
                    })
        except Exception as e:
            logger.error(f"Error reading logs: {e}")
            
        return logs
```

### 3. Конфигурация в agent_config.yaml

```yaml
# Добавить в agent_config.yaml

web_interface:
  enabled: true
  host: "0.0.0.0"
  port: 8080
  cors_origins:
    - "http://localhost:3000"
    - "http://localhost:5173"
  authentication:
    enabled: false  # Пока отключено
    jwt_secret: "your-secret-key"
```

### 4. Обновление Node.js backend

Нужно изменить `web-interface/server/routes/api.js`, чтобы он проксировал запросы к реальному агенту:

```javascript
// В web-interface/server/routes/api.js

const AGENT_API_URL = process.env.AGENT_API_URL || 'http://localhost:8080';

// Прокси к реальному агенту
router.get('/status', async (req, res) => {
  try {
    const response = await fetch(`${AGENT_API_URL}/api/status`);
    const data = await response.json();
    res.json(data);
  } catch (error) {
    console.error('Error fetching agent status:', error);
    // Fallback к mock данным
    res.json(mockAgentStatus);
  }
});

// Аналогично для других эндпоинтов...
```

## 🔄 Процесс интеграции

### Этап 1: Создание HTTP сервера в агенте
1. Создать `src/api/web_interface_server.py`
2. Добавить зависимости: `fastapi`, `uvicorn`, `websockets`
3. Интегрировать в основной Agent класс

### Этап 2: Обновление веб-интерфейса
1. Настроить проксирование в Node.js backend
2. Обновить WebSocket соединение
3. Добавить обработку ошибок и fallback

### Этап 3: Тестирование
1. Запустить SLC Agent с веб-сервером
2. Запустить веб-интерфейс
3. Проверить все функции

## 🚀 Запуск интегрированной системы

```bash
# 1. Запуск SLC Agent (с веб-сервером)
cd slc-agent
python main.py

# 2. Запуск веб-интерфейса backend
cd web-interface/server  
npm start

# 3. Запуск веб-интерфейса frontend
cd web-interface
npm run dev
```

## 🔗 Результат интеграции

После интеграции веб-интерфейс будет:
- ✅ Получать **реальные данные** от агента
- ✅ Отправлять **реальные команды** агенту  
- ✅ Показывать **актуальные логи** и метрики
- ✅ Управлять **реальным мультисознанием**
- ✅ Выполнять **реальные SSH команды**
- ✅ Получать **real-time обновления** через WebSocket

Веб-интерфейс станет полноценной альтернативой Telegram боту с богатым UI/UX! 