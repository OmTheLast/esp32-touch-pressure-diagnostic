import json
import shutil
import socket
import subprocess
from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer


def get_temperature():
    result = subprocess.run(
        ["vcgencmd", "measure_temp"],
        capture_output=True,
        text=True,
        check=True,
    )
    return float(result.stdout.split("=")[1].replace("'C", "").strip())


def get_memory_percent():
    values = {}

    with open("/proc/meminfo", encoding="utf-8") as file:
        for line in file:
            name, value = line.split(":", 1)
            values[name] = int(value.strip().split()[0])

    total = values["MemTotal"]
    available = values["MemAvailable"]
    return round(((total - available) / total) * 100, 1)


def get_disk_percent():
    usage = shutil.disk_usage("/")
    return round((usage.used / usage.total) * 100, 1)


def get_uptime_seconds():
    with open("/proc/uptime", encoding="utf-8") as file:
        return int(float(file.read().split()[0]))


def get_status():
    return {
        "hostname": socket.gethostname(),
        "temperature_c": get_temperature(),
        "memory_percent": get_memory_percent(),
        "disk_percent": get_disk_percent(),
        "uptime_seconds": get_uptime_seconds(),
    }


class StatusHandler(BaseHTTPRequestHandler):
    def do_GET(self):
        if self.path not in ("/", "/status"):
            self.send_error(404, "Not found")
            return

        response = json.dumps(get_status(), indent=2).encode("utf-8")

        self.send_response(200)
        self.send_header("Content-Type", "application/json")
        self.send_header("Content-Length", str(len(response)))
        self.end_headers()
        self.wfile.write(response)

    def log_message(self, format, *args):
        print(f"Request from {self.client_address[0]}: {format % args}")


server = ThreadingHTTPServer(("0.0.0.0", 8000), StatusHandler)

print("TARQ dashboard server is running")
print("Open: http://tarq-pi.local:8000/status")

try:
    server.serve_forever()
except KeyboardInterrupt:
    print("\nStopping server")
finally:
    server.server_close()
