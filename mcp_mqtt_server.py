import os, sys, json, time, threading, logging
from collections import deque
import paho.mqtt.client as mqtt
from mcp.server.fastmcp import FastMCP

# ------------ Logging a STDERR (MCP no usa STDOUT) ------------
logging.basicConfig(stream=sys.stderr, level=logging.INFO, format="%(levelname)s: %(message)s")

# ------------ Variables de entorno ------------
MQTT_HOST = os.getenv("MQTT_HOST", "broker.hivemq.com")
MQTT_PORT = int(os.getenv("MQTT_PORT", "1883"))
MQTT_USER = os.getenv("MQTT_USER", "")
MQTT_PASS = os.getenv("MQTT_PASS", "")
SENSOR_TOPIC = os.getenv("SENSOR_TOPIC", "ucb/iot/sensor/distance")
CMD_TOPIC    = os.getenv("CMD_TOPIC",    "ucb/lock/request")

# ------------ Puente MQTT ------------
class MqttBridge:
    def __init__(self, host, port, user, pw, sensor_topic, cmd_topic):
        self.host = host
        self.port = port
        self.user = user
        self.pw = pw
        self.sensor_topic = sensor_topic
        self.cmd_topic = cmd_topic

        # API v2 para evitar deprecations
        self.client = mqtt.Client(mqtt.CallbackAPIVersion.VERSION2)
        if self.user and self.pw:
            self.client.username_pw_set(self.user, self.pw)

        self.client.on_connect = self._on_connect
        self.client.on_message = self._on_message
        self.client.on_disconnect = self._on_disconnect

        self.last = None
        self.series = deque(maxlen=1000)
        self.lock = threading.Lock()

        # Hilo de conexión/reintento
        self._thread = threading.Thread(target=self._loop, daemon=True)
        self._thread.start()

    def _loop(self):
        while True:
            try:
                self.client.connect(self.host, self.port, keepalive=30)
                # Reintentos automáticos si se cae
                self.client.loop_forever(retry_first_connection=True)
            except Exception as e:
                logging.error(f"MQTT reconnect in 3s: {e}")
                time.sleep(3)

    # Callbacks
    def _on_connect(self, client, userdata, flags, rc, properties=None):
        logging.info(f"✅ MQTT connected host={self.host} port={self.port} rc={rc}")
        if self.sensor_topic:
            client.subscribe(self.sensor_topic)

    def _on_disconnect(self, client, userdata, rc, properties=None):
        logging.warning(f"⚠️ MQTT disconnected rc={rc}")

    def _on_message(self, client, userdata, msg):
        payload = msg.payload.decode("utf-8", errors="ignore").strip()
        ts = int(time.time() * 1000)
        try:
            data = json.loads(payload)
            value = data.get("distance", data.get("value", None))
        except Exception:
            try:
                value = float(payload)
            except Exception:
                value = payload

        with self.lock:
            self.last = {"topic": msg.topic, "value": value, "ts": ts}
            self.series.append(self.last)

    # Publicaciones
    def publish_cmd(self, payload):
        if isinstance(payload, (dict, list)):
            payload = json.dumps(payload)
        logging.info(f"→ publish CMD topic={self.cmd_topic} payload={payload}")
        self.client.publish(self.cmd_topic, payload, qos=0, retain=False)

    def publish_raw(self, topic, payload):
        if isinstance(payload, (dict, list)):
            payload = json.dumps(payload)
        logging.info(f"→ publish RAW topic={topic} payload={payload}")
        self.client.publish(topic, payload, qos=0, retain=False)

    # Lecturas
    def get_last(self):
        with self.lock:
            return self.last

    def get_series(self, n):
        with self.lock:
            return list(self.series)[-n:]

# Instancia global
bridge = MqttBridge(MQTT_HOST, MQTT_PORT, MQTT_USER, MQTT_PASS, SENSOR_TOPIC, CMD_TOPIC)

# ------------ Servidor MCP (vía STDIO) ------------
server = FastMCP("mqtt-servo-controller")

@server.tool()
def open_lock() -> str:
    """Abre la puerta (payload que tu ESP32 ya entiende)."""
    bridge.publish_cmd("INTENTO_DE_APERTURA")
    return "INTENTO_DE_APERTURA sent"

@server.tool()
def close_lock() -> str:
    """Cierra la puerta (payload que tu ESP32 ya entiende)."""
    bridge.publish_cmd("OFF")
    return "OFF sent"

@server.tool()
def set_servo(angle: int) -> str:
    """Mueve el servo a un ángulo (0-180)."""
    angle = max(0, min(180, int(angle)))
    bridge.publish_cmd({"angle": angle})
    return f"Servo angle set to {angle}°"

@server.tool()
def get_distance() -> dict:
    """Devuelve la última lectura del sensor (si hay)."""
    last = bridge.get_last()
    return last if last is not None else {"status": "no-data"}

@server.tool()
def get_series(n: int = 30) -> list:
    """Devuelve las últimas N lecturas del sensor."""
    n = max(1, min(500, int(n)))
    return bridge.get_series(n)

@server.tool()
def publish_raw(topic: str, payload: str) -> str:
    """Publica un mensaje arbitrario en un tópico."""
    bridge.publish_raw(topic, payload)
    return f"published to {topic}"

if __name__ == "__main__":
    # Importante: no imprimir por stdout. FastMCP usa stdio internamente.
    server.run()