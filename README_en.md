<p align="center">
  <a href="./README.md"><img src="https://img.shields.io/badge/中文-d32f2f?style=for-the-badge" alt="中文"></a>
  <a href="./README_en.md"><img src="https://img.shields.io/badge/English-1565c0?style=for-the-badge" alt="English"></a>
</p>

# AxionRover

Two-wheel differential rover: ESP32, TB6612, wheel encoders. Velocity commands plus encoder `d` (distance) and `r` (yaw). Maps to ROS `/cmd_vel` v–ω. **Not** custom SLAM / Nav2. Phase 2 is serial ROS 2; navigation is phase 3.

## Operations manual

[book/zh/00-目录.md](book/zh/00-目录.md) (**00** = table of contents)

- **10x Phase 1** (current): 101 flash → 106 vehicle test
- **20x Phase 2**: ROS (stub 200)
- **30x Phase 3**: navigation (stub 300)

English map: [book/en/00-contents.md](book/en/00-contents.md)

## Status (Phase 1)

- Straight: `d 0.96` ≈ 1 m (open-loop, negligible lateral error when squared up)
- Turn: `r ±90` / `r ±180` (encoder stop, negligible error on the floor tape)
- Track `L = 0.159 m` (outer 185 mm − wheel width 26 mm)

## Demos

- Motor A open-loop: [YouTube](https://www.youtube.com/shorts/FS3IdpszdbM) · [Bilibili](https://www.bilibili.com/video/BV1PCHp6ZEFa/)
- Motor B open-loop: [YouTube](https://www.youtube.com/shorts/xwJDwbOH4fE) · [Bilibili](https://www.bilibili.com/video/BV1skHp6bE7Z/)
- A+B open-loop: [YouTube](https://www.youtube.com/shorts/fDkDXerzhRc) · [Bilibili](https://www.bilibili.com/video/BV1PkHp6bE5H/)
- ~1 m straight: [YouTube](https://youtu.be/OrOgSyiR2sc) · [Bilibili](https://www.bilibili.com/video/BV1LJHp65Eb5/)
- 90° / 180° in place: [YouTube](https://youtu.be/yye5On57KsE) · [Bilibili](https://www.bilibili.com/video/BV1o7Hp61EPy/)

Xiaohongshu / WeChat carry the same clips. This repo links YouTube and Bilibili only.

## Layout

```text
book/zh/          operations manual (00, 10x / 20x / 30x)
book/en/          English table of contents
sketchbook/       Arduino sketches (folder name = .ino name)
images/           photos named 101-01.jpg, matching chapter ids
```

## License

[MIT](LICENSE) · AxioncoreLabs
