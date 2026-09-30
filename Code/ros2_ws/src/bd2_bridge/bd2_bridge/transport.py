import json
import socket
import time
import uuid


class RobotLink:
    """Request/reply UDP. A lost reply is reported, never assumed successful."""

    def __init__(self, host, port=4242, timeout=0.15):
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.socket.settimeout(timeout)
        self.timeout = timeout
        self.socket.connect((host, port))

    def request(self, command):
        # Drain delayed replies before sending a fresh request.
        self.socket.setblocking(False)
        try:
            while True:
                self.socket.recv(4096)
        except BlockingIOError:
            pass
        finally:
            self.socket.settimeout(self.timeout)
        request_id = uuid.uuid4().hex
        command = dict(command, request_id=request_id)
        self.socket.send(json.dumps(command, allow_nan=False).encode())
        deadline = time.monotonic() + self.timeout
        while True:
            remaining = deadline - time.monotonic()
            if remaining <= 0:
                raise TimeoutError("Robot reply timed out")
            self.socket.settimeout(remaining)
            reply = json.loads(self.socket.recv(4096))
            if not isinstance(reply, dict) or not isinstance(reply.get("ok"), bool):
                raise ValueError("Invalid robot response")
            if reply.get("request_id") == request_id:
                return reply

    def close(self):
        self.socket.close()
