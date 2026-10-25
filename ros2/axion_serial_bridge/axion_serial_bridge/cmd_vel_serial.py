# Pi 5 / Jazzy: Twist -> DiffDrive "c <vx> <wz>\\n"
# Firmware stays DiffDrive.ino. Not Nav2.

import time

import rclpy
from geometry_msgs.msg import Twist
from rclpy.node import Node

try:
    import serial
except ImportError:
    serial = None


class CmdVelSerial(Node):
    def __init__(self):
        super().__init__("cmd_vel_serial")
        self.declare_parameter("port", "/dev/ttyUSB0")
        self.declare_parameter("baud", 115200)
        self.declare_parameter("hz", 10.0)
        port = self.get_parameter("port").get_parameter_value().string_value
        baud = self.get_parameter("baud").get_parameter_value().integer_value
        hz = self.get_parameter("hz").get_parameter_value().double_value
        if serial is None:
            raise RuntimeError("pip/apt: python3-serial")
        self._ser = serial.Serial(port, baud, timeout=0.05)
        time.sleep(0.2)
        self._vx = 0.0
        self._wz = 0.0
        self.create_subscription(Twist, "cmd_vel", self._on_twist, 10)
        self.create_timer(1.0 / hz, self._tick)
        self.get_logger().info(f"serial {port} {baud} -> c vx wz")

    def _on_twist(self, msg: Twist):
        self._vx = float(msg.linear.x)
        self._wz = float(msg.angular.z)

    def _tick(self):
        if abs(self._vx) < 1e-4 and abs(self._wz) < 1e-4:
            line = "0\n"
        else:
            line = f"c {self._vx:.3f} {self._wz:.3f}\n"
        self._ser.write(line.encode("ascii"))


def main():
    rclpy.init()
    node = CmdVelSerial()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    node._ser.write(b"0\n")
    node._ser.close()
    node.destroy_node()
    rclpy.shutdown()
