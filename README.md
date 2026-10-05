<p align="center">
  <a href="./README.md"><img src="https://img.shields.io/badge/中文-d32f2f?style=for-the-badge" alt="中文"></a>
  <a href="./README_en.md"><img src="https://img.shields.io/badge/English-1565c0?style=for-the-badge" alt="English"></a>
</p>

# AxionRover

个人两轮差速小车：ESP32 + TB6612 + 编码器。开环速度，编码器直行 `d`、转角 `r`。对应 ROS `/cmd_vel` 的 v–ω。**不是** SLAM / Nav2 自研；阶段二才对接 ROS 2 串口，阶段三才谈导航。

## 操作手册

[book/zh/00-目录.md](book/zh/00-目录.md)（**00** = 总目录）

- **10x 阶段一**：101 烧录 → 106 样车验证  
- **20x 阶段二**：ROS 接入（未开始）
- **30x 阶段三**：导航（未开始）

英文目录：[book/en/00-contents.md](book/en/00-contents.md)

## 阶段一

- 直行：`d 0.96` ≈ 1 m（开环, 横向几乎无偏移）
- 转角：`r ±90` / `r ±180`（编码器停角, 实测几乎无偏差）
- 轨距 `L = 0.159 m`（外边距 185 mm − 轮宽 26 mm）

## 演示

- A 路开环：[YouTube](https://www.youtube.com/shorts/FS3IdpszdbM) · [Bilibili](https://www.bilibili.com/video/BV1PCHp6ZEFa/)
- B 路开环：[YouTube](https://www.youtube.com/shorts/xwJDwbOH4fE) · [Bilibili](https://www.bilibili.com/video/BV1skHp6bE7Z/)
- AB 双路：[YouTube](https://www.youtube.com/shorts/fDkDXerzhRc) · [Bilibili](https://www.bilibili.com/video/BV1PkHp6bE5H/)
- 直行约 1 m：[YouTube](https://youtu.be/OrOgSyiR2sc) · [Bilibili](https://www.bilibili.com/video/BV1LJHp65Eb5/)
- 原地 90° / 180°：[YouTube](https://youtu.be/yye5On57KsE) · [Bilibili](https://www.bilibili.com/video/BV1o7Hp61EPy/)

## 仓库结构

```text
book/zh/          操作手册（00 目录，10x / 20x / 30x）
book/en/          英文目录
sketchbook/       Arduino 工程（文件夹名 = .ino 名）
images/           配图(101-01.png与章节编号一致)
```

## 许可

[MIT](LICENSE) · AxioncoreLabs
