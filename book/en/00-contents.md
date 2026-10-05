<p align="center">
  <a href="../../README.md"><img src="https://img.shields.io/badge/中文-d32f2f?style=for-the-badge" alt="中文"></a>
  <a href="../../README_en.md"><img src="https://img.shields.io/badge/English-1565c0?style=for-the-badge" alt="English"></a>
</p>

# AxionRover operations manual · 00 contents

IDs: **101 flash → 102 open-loop → 103 encoder → 104 power → 105 PID → 106 vehicle test (1.6)**.

Chinese: [book/zh/00-目录.md](../zh/00-目录.md)

## Phase 1 · Chassis (10x) · current

| ID | Topic | Sketch |
|----|--------|--------|
| [101](../zh/101-ESP32串口与烧录.md) | ESP32 UART + Arduino flash | `Blink`, `SerialTest` |
| [102](../zh/102-TB6612开环.md) | TB6612 open-loop A then A+B | `MotorOpenLoopTest`, `_AB` |
| [103](../zh/103-编码器读脉冲.md) | JGB37-520 encoders | `EncoderTest_A`, `EncoderTest_B` |
| [104](../zh/104-电源定型.md) | VIN + LM2596 + 4 USB states | — |
| [105](../zh/105-速度PI与差速.md) | CPR, speed PI, DiffDrive | `EncoderCal`, `SpeedPI_*`, `DiffDrive` |
| [106](../zh/106-样车验证.md) | 1.6 chassis, photos, 1 m, yaw | `DiffDrive` `d` / `r` |

## Phase 2 · ROS (20x) · not started

| ID | Topic |
|----|--------|
| [200](../zh/200-ROS接入.md) | ROS 2 + `cmd_vel` (stub). Later: 201, … |

## Phase 3 · Navigation (30x) · not started

| ID | Topic |
|----|--------|
| [300](../zh/300-导航.md) | Application-layer nav (stub). Later: 301, … |

Photos: [`images/`](../../images/README.md). Arduino: folder name = `.ino` name.
