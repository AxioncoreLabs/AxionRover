# tools/

Windows 上还不装 ROS 2 时，用串口遥控 **已烧录的 `DiffDrive.ino`**。不要另写一份电调固件。

```powershell
pip install pyserial
python tools/serial_teleop.py --list
python tools/serial_teleop.py --port COM3
```

设备管理器里看 CH340 的 COMx。先关掉 Arduino 串口监视器（端口会被占）。地面：主开关 ON、VIN OFF、USB（手册 104）。

W/S 前进后退，A/D 原地转，空格停，Q 退出。周期约 10 Hz，发 `c <v> <w>`，停时发 `0`。这不是 `/cmd_vel`；派上 Jazzy 再把同一行协议接到 ROS 2。
