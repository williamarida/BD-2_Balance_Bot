"""Read/set bench firmware values without ROS. Uses standard Python only."""
import argparse
import json
from pathlib import Path
import sys

sys.path.insert(0, str(Path(__file__).resolve().parents[1] / "Code/ros2_ws/src/bd2_bridge"))
from bd2_bridge.transport import RobotLink

parser = argparse.ArgumentParser()
parser.add_argument("--host", default="127.0.0.1")
parser.add_argument("--port", type=int, default=4242)
parser.add_argument("op", choices=["get", "set", "save"])
parser.add_argument("key", nargs="?")
parser.add_argument("value", nargs="?", type=float)
args = parser.parse_args()
command = {"op": args.op}
if args.op == "set":
    if args.key is None or args.value is None:
        parser.error("set requires a key and numeric value")
    command.update(key=args.key, value=args.value)
link = RobotLink(args.host, args.port)
try:
    reply = link.request(command)
    print(json.dumps(reply, indent=2))
    sys.exit(0 if reply["ok"] else 1)
except (OSError, ValueError) as error:
    print(f"Robot request failed: {error}. Read back settings before retrying.", file=sys.stderr)
    sys.exit(2)
finally:
    link.close()
