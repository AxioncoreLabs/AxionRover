# Windows: keyboard -> DiffDrive serial (not ROS).
# Firmware stays DiffDrive.ino. 10 Hz: "c <v> <w>\n"
# 104: main switch ON, VIN OFF, USB; close Arduino Serial Monitor first.

from __future__ import annotations

import argparse
import sys
import time

try:
    import serial
    import serial.tools.list_ports
except ImportError:
    print("pip install pyserial", file=sys.stderr)
    sys.exit(1)

if sys.platform != "win32":
    print("This teleop uses Windows msvcrt. On the Pi use ROS 2 instead.", file=sys.stderr)
    sys.exit(1)

import msvcrt


def list_ports() -> None:
    rows = list(serial.tools.list_ports.comports())
    if not rows:
        print("No COM ports.")
        return
    for p in rows:
        print(f"  {p.device}\t{p.description}")


def send(ser: serial.Serial, line: str) -> None:
    ser.write((line.strip() + "\n").encode("ascii"))
    ser.flush()


def drain_keys() -> bytes | None:
    last = None
    while msvcrt.kbhit():
        last = msvcrt.getch()
        if last in (b"\x00", b"\xe0") and msvcrt.kbhit():
            msvcrt.getch()
            last = None
    return last


def main() -> int:
    ap = argparse.ArgumentParser(description="WASD teleop for DiffDrive (Windows + pyserial)")
    ap.add_argument("--port", help="COM port, e.g. COM3")
    ap.add_argument("--list", action="store_true", help="list COM ports")
    ap.add_argument("--baud", type=int, default=115200)
    ap.add_argument("--v", type=float, default=0.08, help="linear m/s (W/S)")
    ap.add_argument("--w", type=float, default=0.6, help="yaw rad/s (A/D)")
    ap.add_argument("--hz", type=float, default=10.0)
    args = ap.parse_args()

    if args.list:
        list_ports()
        return 0
    if not args.port:
        print("Need --port COMx  (try --list)", file=sys.stderr)
        return 2

    period = 1.0 / args.hz
    ser = serial.Serial(args.port, args.baud, timeout=0.05)
    time.sleep(0.3)
    send(ser, "0")

    print(f"{args.port} {args.baud}  W/S v=±{args.v}  A/D w=±{args.w}  space=stop  q=quit")
    print("Not ROS. Same protocol the Pi bridge will use: c <vx> <wz>")

    vx = 0.0
    wz = 0.0
    last_cmd = None
    try:
        while True:
            t0 = time.monotonic()
            key = drain_keys()
            if key is not None:
                k = key.lower()
                if k == b"q":
                    break
                if k == b" ":
                    vx, wz = 0.0, 0.0
                elif k == b"w":
                    vx, wz = args.v, 0.0
                elif k == b"s":
                    vx, wz = -args.v, 0.0
                elif k == b"a":
                    vx, wz = 0.0, args.w
                elif k == b"d":
                    vx, wz = 0.0, -args.w

            if vx == 0.0 and wz == 0.0:
                line = "0"
            else:
                line = f"c {vx:.3f} {wz:.3f}"
            if line != last_cmd or (vx != 0.0 or wz != 0.0):
                send(ser, line)
                last_cmd = line

            leftover = period - (time.monotonic() - t0)
            if leftover > 0:
                time.sleep(leftover)
    finally:
        try:
            send(ser, "0")
        except Exception:
            pass
        ser.close()
        print("stopped")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
