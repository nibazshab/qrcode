#include "lib/qrcodegen.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#pragma pack(push, 1)
typedef struct {
	uint16_t bfType;
	uint32_t bfSize;
	uint16_t bfReserved1;
	uint16_t bfReserved2;
	uint32_t bfOffBits;
} BITMAPFILEHEADER;

typedef struct {
	uint32_t biSize;
	int32_t biWidth;
	int32_t biHeight;
	uint16_t biPlanes;
	uint16_t biBitCount;
	uint32_t biCompression;
	uint32_t biSizeImage;
	int32_t biXPelsPerMeter;
	int32_t biYPelsPerMeter;
	uint32_t biClrUsed;
	uint32_t biClrImportant;
} BITMAPINFOHEADER;

typedef struct {
	uint8_t rgbBlue;
	uint8_t rgbGreen;
	uint8_t rgbRed;
	uint8_t rgbReserved;
} RGBQUAD;
#pragma pack(pop)

static bool write_to_bmp(const char *fname, const uint8_t data[])
{
	const int scale = 10;
	int size = qrcodegen_getSize(data) * scale;

	int block = ((size + 31) / 32) * 4;
	int bytes = block * size;

	RGBQUAD pal[2] = {{0xFF, 0xFF, 0xFF, 0x00}, {0x00, 0x00, 0x00, 0x00}};

	const uint32_t head = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + sizeof(pal);

	BITMAPFILEHEADER fh = {0x4D42, head + (uint32_t)bytes, 0, 0, head};
	BITMAPINFOHEADER ih = {sizeof(BITMAPINFOHEADER), size, size, 1, 1, 0, (uint32_t)bytes, 2835, 2835, 2, 2};

	FILE *fp = fopen(fname, "wb");
	if (!fp)
		return false;

	fwrite(&fh, sizeof(fh), 1, fp);
	fwrite(&ih, sizeof(ih), 1, fp);
	fwrite(pal, sizeof(pal), 1, fp);

	uint8_t *ln = calloc(block, 1);
	if (!ln) {
		fclose(fp);
		return false;
	}

	for (int y = size - 1; y >= 0; y--) {
		memset(ln, 0x00, (size_t)block);
		int _y = y / scale;

		for (int x = 0; x < size; x++) {
			int _x = x / scale;

			if (qrcodegen_getModule(data, _x, _y)) {
				int a = x / 8;
				int b = 7 - (x % 8);

				ln[a] |= (1 << b);
			}
		}

		fwrite(ln, (size_t)block, 1, fp);
	}

	free(ln);
	fclose(fp);
	return true;
}

int main(int argc, char *argv[])
{
	if (argc < 2)
		return 1;

	const char *txt = argv[1];
	const char *out = (argc > 2) ? argv[2] : "qr.bmp";

	uint8_t qr[qrcodegen_BUFFER_LEN_MAX];
	uint8_t buf[qrcodegen_BUFFER_LEN_MAX];

	bool ok = qrcodegen_encodeText(txt, buf, qr, qrcodegen_Ecc_MEDIUM, qrcodegen_VERSION_MIN, qrcodegen_VERSION_MAX,
				       qrcodegen_Mask_AUTO, true);

	if (ok && write_to_bmp(out, qr)) {
		return 0;
	} else {
		return 1;
	}
}
