"""PC-only bench simulator. Does not model the robot's physics."""
import argparse
import json
import math
import socket
import time

LIMITS = {"gain_kp": (0, 1000), "stick_deadzone": (0, 0.5), "stick_scale": (0, 1)}


def handle(command, settings, started):
    ok, message = False, "invalid operation"
    if isinstance(command, dict):
        if command.get("op") == "get":
            ok, message = True, "ok"
        elif command.get("op") == "set":
            key, value = command.get("key"), command.get("value")
            if (isinstance(key, str) and key in LIMITS
                    and type(value) in (int, float) and math.isfinite(value)
                    and LIMITS[key][0] <= value <= LIMITS[key][1]):
                settings[key] = value
                ok, message = True, "applied in RAM"
            else:
                message = "unknown key or out-of-range value"
        elif command.get("op") == "save":
            message = "mock does not persist settings"
    return dict(ok=ok, message=message,
                request_id=command.get("request_id") if isinstance(command, dict) else None,
                mode="pc_mock_no_physics",
                uptime_s=time.monotonic() - started, controller_connected=False,
                stick_x=0, stick_y=0, buttons=0, settings=dict(settings))


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--host", default="127.0.0.1")
    parser.add_argument("--port", type=int, default=4242)
    args = parser.parse_args()
    settings = dict(gain_kp=0, stick_deadzone=0.08, stick_scale=0.25)
    started = time.monotonic()
    with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as server:
        server.bind((args.host, args.port))
        print(f"Mock robot listening on {args.host}:{args.port}", flush=True)
        try:
            while True:
                data, peer = server.recvfrom(4096)
                try:
                    command = json.loads(data)
                except (ValueError, UnicodeDecodeError):
                    command = None
                server.sendto(json.dumps(handle(command, settings, started)).encode(), peer)
        except KeyboardInterrupt:
            pass


if __name__ == "__main__":
    main()
