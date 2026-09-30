import json
import math

import rclpy
from rclpy.node import Node
from std_msgs.msg import Bool, Float32, String

from .transport import RobotLink


class Bridge(Node):
    def __init__(self):
        super().__init__("bd2_bridge")
        self.declare_parameter("robot_host", "127.0.0.1")
        self.declare_parameter("robot_port", 4242)
        self.link = RobotLink(self.get_parameter("robot_host").value,
                              self.get_parameter("robot_port").value)
        self.telemetry = self.create_publisher(String, "/bd2/telemetry", 10)
        self.reply = self.create_publisher(String, "/bd2/command_reply", 10)
        self.connected = self.create_publisher(Bool, "/bd2/link_connected", 10)
        self.values = {key: self.create_publisher(Float32, f"/bd2/{key}", 10)
                       for key in ("uptime_s", "stick_x", "stick_y", "gain_kp",
                                   "stick_deadzone", "stick_scale")}
        self.create_subscription(String, "/bd2/command", self.command, 10)
        self.create_timer(0.1, self.poll)
        self.online = None

    def status(self, online):
        self.connected.publish(Bool(data=online))
        if self.online != online:
            self.get_logger().info("Robot link online" if online else "Robot link offline")
        self.online = online

    def poll(self):
        try:
            reply = self.link.request({"op": "get"})
            self.status(True)
            self.telemetry.publish(String(data=json.dumps(reply)))
            fields = {**reply, **reply.get("settings", {})}
            for key, pub in self.values.items():
                value = fields.get(key)
                if isinstance(value, (int, float)) and math.isfinite(value):
                    pub.publish(Float32(data=float(value)))
        except (OSError, ValueError, TypeError):
            self.status(False)

    def command(self, message):
        try:
            command = json.loads(message.data)
            if not isinstance(command, dict) or command.get("op") not in ("set", "save"):
                raise ValueError("Use a set or save command")
            reply = self.link.request(command)
        except (OSError, ValueError, TypeError) as error:
            reply = {"ok": False, "message": str(error),
                     "note": "A timed-out command may have reached the robot; read back settings."}
        self.reply.publish(String(data=json.dumps(reply)))


def main(args=None):
    rclpy.init(args=args)
    node = Bridge()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        node.link.close()
        node.destroy_node()
        if rclpy.ok():
            rclpy.shutdown()
