import json
from pathlib import Path
import socket
import sys
import threading
import time
import unittest

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "Code/ros2_ws/src/bd2_bridge"))
from bd2_bridge.transport import RobotLink
from mock_robot import handle


class ProtocolTests(unittest.TestCase):
    def test_invalid_commands_preserve_settings(self):
        settings = dict(gain_kp=0, stick_deadzone=0.08, stick_scale=0.25)
        original = dict(settings)
        for command in (None, [], {"op": "set", "key": "stick_scale", "value": 2},
                        {"op": "set", "key": "gain_kp", "value": float("nan")},
                        {"op": "set", "key": [], "value": 1},
                        {"op": "set", "key": "motor_power", "value": 1}):
            self.assertFalse(handle(command, settings, time.monotonic())["ok"])
            self.assertEqual(settings, original)

    def test_udp_set_then_readback(self):
        server = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        server.bind(("127.0.0.1", 0))
        settings = dict(gain_kp=0, stick_deadzone=0.08, stick_scale=0.25)
        def respond():
            for _ in range(2):
                data, peer = server.recvfrom(4096)
                server.sendto(json.dumps(handle(json.loads(data), settings, time.monotonic())).encode(), peer)
        worker = threading.Thread(target=respond, daemon=True)
        worker.start()
        link = RobotLink("127.0.0.1", server.getsockname()[1])
        try:
            self.assertTrue(link.request(dict(op="set", key="gain_kp", value=25))["ok"])
            self.assertEqual(link.request(dict(op="get"))["settings"]["gain_kp"], 25)
        finally:
            link.close()
            worker.join(1)
            server.close()

    def test_offline_does_not_report_success(self):
        server = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        server.bind(("127.0.0.1", 0))
        link = RobotLink("127.0.0.1", server.getsockname()[1])
        try:
            with self.assertRaises(OSError):
                link.request(dict(op="get"))
        finally:
            link.close()
            server.close()

    def test_delayed_reply_is_not_accepted_for_new_command(self):
        server = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        server.bind(("127.0.0.1", 0))
        def respond():
            data, peer = server.recvfrom(4096)
            command = json.loads(data)
            server.sendto(json.dumps(dict(ok=True, request_id="old_request")).encode(), peer)
            server.sendto(json.dumps(dict(ok=False, request_id=command["request_id"])).encode(), peer)
        worker = threading.Thread(target=respond, daemon=True)
        worker.start()
        link = RobotLink("127.0.0.1", server.getsockname()[1])
        try:
            self.assertFalse(link.request(dict(op="set", key="stick_scale", value=2))["ok"])
        finally:
            link.close()
            worker.join(1)
            server.close()


if __name__ == "__main__":
    unittest.main()
