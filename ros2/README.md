# ros2/

Pi 5 + Ubuntu 24.04 + Jazzy 上编译。Windows 电脑不要 `colcon`。操作步骤见 [book/zh/200-ROS2接入.md](../book/zh/200-ROS2接入.md)。

固件仍是 `sketchbook/DiffDrive`。本包只做 `/cmd_vel` → 串口 `c <v> <w>`。

```bash
cd ~/axion_ws/src
# 把本目录 axion_serial_bridge 拷进来
cd ~/axion_ws
colcon build --packages-select axion_serial_bridge
source install/setup.bash
ros2 run axion_serial_bridge cmd_vel_serial --ros-args -p port:=/dev/ttyUSB0
# 另开终端
sudo apt install ros-jazzy-teleop-twist-keyboard
ros2 run teleop_twist_keyboard teleop_twist_keyboard
```
