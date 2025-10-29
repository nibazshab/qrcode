# qrcode

二维码生成器，将文本内容转换为 bmp 格式的二维码图像文件

## 说明

仅使用 C 标准库，C99 标准语法，支持跨平台，甚至支持在 Windows XP 上运行

### 如何使用

- 参数 1：被转换的文本
- 参数 2：保存的图片路径

```sh
qrcode "2a9b3f692b1715a6" "D:\qr.bmp"
```

### 如何编译

一般情况，直接编译即可

```sh
gcc -std=c99 -O2 main.c lib/qrcodegen.c -o qrcode
```

兼容 Windows XP 的情况，需要使用 winlibs-i686-posix-dwarf-gcc-10.5.0-mingw-w64msvcrt-11.0.1-r2 编译

```sh
gcc -std=c99 -O2 -static -m32 "-Wl,--subsystem,console:5.01" main.c lib/qrcodegen.c -o qrcode.exe
```

## 致谢

- nayuki/QR-Code-generator
