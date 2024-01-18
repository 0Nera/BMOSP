#include <system.h>

#define TGA_ERR( ) fb_printf("Ошибка декодирования TGA на строчке: %u\n", __LINE__);

typedef struct {
	unsigned char magic1;             // должно быть нулевым
	unsigned char colormap;           // должно быть нулевым
	unsigned char encoding;           // должно быть 2
	unsigned short cmaporig, cmaplen; // должны быть нулевыми
	unsigned char cmapent;            // должно быть нулевым
	unsigned short x;                 // должно быть нулевым
	unsigned short y;                 // высота изображения
	unsigned short h;                 // высота изображения
	unsigned short w;                 // ширина изображения
	unsigned char bpp;                // должно быть 32
	unsigned char pixeltype;          // должно быть 40
} __attribute__((packed)) tga_header_t;

static unsigned int *tga_parse(unsigned char *ptr, int size) {
	unsigned int *data;
	int i, j, k, x, y, w = (ptr[13] << 8) + ptr[12], h = (ptr[15] << 8) + ptr[14], o = (ptr[11] << 8) + ptr[10];
	int m = ((ptr[1] ? (ptr[7] >> 3) * ptr[5] : 0) + 18);

	if (w < 1 || h < 1) return NULL;

	data = (unsigned int *)alloc((w * h + 2) * sizeof(unsigned int));
	if (!data) { return NULL; }
	memset(data, 0, (w * h + 2) * sizeof(unsigned int));

	switch (ptr[2]) {
		case 1:
			if (ptr[6] != 0 || ptr[4] != 0 || ptr[3] != 0 || (ptr[7] != 24 && ptr[7] != 32)) {
				TGA_ERR( );
				free(data);
				return NULL;
			}
			for (y = i = 0; y < h; y++) {
				k = ((!o ? h - y - 1 : y) * w);
				for (x = 0; x < w; x++) {
					j = ptr[m + k++] * (ptr[7] >> 3) + 18;
					data[2 + i++] =
					    ((ptr[7] == 32 ? ptr[j + 3] : 0xFF) << 24) | (ptr[j + 2] << 16) | (ptr[j + 1] << 8) | ptr[j];
				}
			}
			break;
		case 2:
			if (ptr[5] != 0 || ptr[6] != 0 || ptr[1] != 0 || (ptr[16] != 24 && ptr[16] != 32)) {
				TGA_ERR( );
				free(data);
				return NULL;
			}
			for (y = i = 0; y < h; y++) {
				j = ((!o ? h - y - 1 : y) * w * (ptr[16] >> 3));
				for (x = 0; x < w; x++) {
					data[2 + i++] =
					    ((ptr[16] == 32 ? ptr[j + 3] : 0xFF) << 24) | (ptr[j + 2] << 16) | (ptr[j + 1] << 8) | ptr[j];
					j += ptr[16] >> 3;
				}
			}
			break;
		case 9:
			if (ptr[6] != 0 || ptr[4] != 0 || ptr[3] != 0 || (ptr[7] != 24 && ptr[7] != 32)) {
				TGA_ERR( );
				free(data);
				return NULL;
			}
			y = i = 0;
			for (x = 0; x < w * h && m < size;) {
				k = ptr[m++];
				if (k > 127) {
					k -= 127;
					x += k;
					j = ptr[m++] * (ptr[7] >> 3) + 18;
					while (k--) {
						if (!(i % w)) {
							i = ((!o ? h - y - 1 : y) * w);
							y++;
						}
						data[2 + i++] = ((ptr[7] == 32 ? ptr[j + 3] : 0xFF) << 24) | (ptr[j + 2] << 16) |
						                (ptr[j + 1] << 8) | ptr[j];
					}
				} else {
					k++;
					x += k;
					while (k--) {
						j = ptr[m++] * (ptr[7] >> 3) + 18;
						if (!(i % w)) {
							i = ((!o ? h - y - 1 : y) * w);
							y++;
						}
						data[2 + i++] = ((ptr[7] == 32 ? ptr[j + 3] : 0xFF) << 24) | (ptr[j + 2] << 16) |
						                (ptr[j + 1] << 8) | ptr[j];
					}
				}
			}
			break;
		case 10:
			if (ptr[5] != 0 || ptr[6] != 0 || ptr[1] != 0 || (ptr[16] != 24 && ptr[16] != 32)) {
				TGA_ERR( );
				free(data);
				return NULL;
			}
			y = i = 0;
			for (x = 0; x < w * h && m < size;) {
				k = ptr[m++];
				if (k > 127) {
					k -= 127;
					x += k;
					while (k--) {
						if (!(i % w)) {
							i = ((!o ? h - y - 1 : y) * w);
							y++;
						}
						data[2 + i++] = ((ptr[16] == 32 ? ptr[m + 3] : 0xFF) << 24) | (ptr[m + 2] << 16) |
						                (ptr[m + 1] << 8) | ptr[m];
					}
					m += ptr[16] >> 3;
				} else {
					k++;
					x += k;
					while (k--) {
						if (!(i % w)) {
							i = ((!o ? h - y - 1 : y) * w);
							y++;
						}
						data[2 + i++] = ((ptr[16] == 32 ? ptr[m + 3] : 0xFF) << 24) | (ptr[m + 2] << 16) |
						                (ptr[m + 1] << 8) | ptr[m];
						m += ptr[16] >> 3;
					}
				}
			}
			break;
		default: {
			TGA_ERR( );
			free(data);
			return NULL;
		}
	}
	data[0] = w;
	data[1] = h;
	return data;
}

static void *handler(uint64_t func) {
	switch (func) {
		case 0: return tga_parse;

		default: return NULL;
	}
}

module_info_t __attribute__((section(".minit"))) init(env_t *env) {
	init_env(env);

	module_info_t *boot_tga = get_module("[BOOTIMG]");

	if (boot_tga != NULL) {
#if 0
		framebuffer_t screen = alloc_framebuffer( );
		uint32_t *screen_buf = screen.address;

		uint32_t *img = tga_parse(boot_tga->data, boot_tga->data_size);
		uint32_t width = img[0];
		uint32_t height = img[1];
		uint32_t *img_data = (uint32_t *)img + 2;

		for (uint32_t w = 0; w < width; w++) {
			for (uint32_t h = 0; h < height; h++) {
				if (*img_data == 0x013220) {
					*img_data++;
					continue;
				}
				screen_buf[w * height + h] = *img_data++;
			}
		}
#endif
	}

	return (module_info_t){ .name = (char *)"[MEDIA][TGA]",
		                    .message = (char *)"Отрисовка TGA изображений",
		                    .type = 0,
		                    .data_size = 0,
		                    .data = (void *)0,
		                    .err_code = 0,
		                    .module_id = 0,
		                    .irq = 0,
		                    .irq_handler = 0,
		                    .get_func = handler };
}
