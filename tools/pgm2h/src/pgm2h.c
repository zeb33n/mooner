#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <inttypes.h>



static size_t parse_u32(const char *buff, size_t offset, size_t size, uint32_t *value)
{
	uint32_t v = 0;

	size_t o;
	for (o = 0; ((offset + o) < size) && isdigit(buff[offset + o]); o++)
	{
		if ((v * 10) < v) return 0;

		v *= 10;
		v += (buff[offset + o] - '0');
	}

	if ((o != 0) && value) *value = v;
	return o;
}

static bool parse_u32z(const char *str, uint32_t *value)
{
	if (!str) return false;
	size_t size = strlen(str);

	uint32_t v = 0;
	size_t s = parse_u32(str, 0, size, &v);
	if ((s == 0) || (str[s] != '\0'))
		return false;

	if (value) *value = v;
	return true;
}

static size_t parse_pgm_space(const char *buff, size_t offset, size_t size)
{
	size_t o = 0;

	while (true)
	{
		while (((offset + o) < size) && isspace(buff[offset + o]))
			o++;

		if (buff[offset + o] != '#')
			break;
		o++;

		while (((offset + o) < size) && (buff[offset + o] != '\n'))
			o++;
	}

	return o;
}

static size_t parse_pgm_header(
	const char *file, size_t size,
	uint32_t *width, uint32_t *height, uint8_t *bits)
{
	if (!file) return 0;

	size_t o = 0;
	if ((size < 2) || (file[0] != 'P') || (file[1] != '5'))
		return 0;
	o += 2;

	size_t s;

	s = parse_pgm_space(file, o, size);
	if (s == 0) return 0;
	o += s;

	uint32_t w;
	s = parse_u32(file, o, size, &w);
	if (s == 0) return 0;
	o += s;

	s = parse_pgm_space(file, o, size);
	if (s == 0) return 0;
	o += s;

	uint32_t h;
	s = parse_u32(file, o, size, &h);
	if (s == 0) return 0;
	o += s;

	s = parse_pgm_space(file, o, size);
	if (s == 0) return 0;
	o += s;

	uint32_t m;
	s = parse_u32(file, o, size, &m);
	if (s == 0) return 0;
	o += s;

	// Max value must indicate an integer number of bits.
	if (m & (m + 1)) return 0;

	uint8_t b;
	for (b = 0; m > 0; b++, m >>= 1);

	if (!isspace(file[o++]))
		return 0;

	if (width ) *width  = w;
	if (height) *height = h;
	if (bits  ) *bits   = b;
	return o;
}

static uint8_t *file_read(const char *path, size_t *size)
{
	FILE *stream = fopen(path, "rb");
	if (!stream) return NULL;

	if (fseek(stream, 0, SEEK_END) != 0)
		return NULL;

	long lsize = ftell(stream);
	if (lsize < 0)
	{
		fclose(stream);
		return NULL;
	}

	if (fseek(stream, 0, SEEK_SET) != 0)
		return NULL;

	uint8_t *buff = malloc(lsize + 1);
	if (!buff)
	{
		fclose(stream);
		return NULL;
	}

	bool success = (fread(buff, lsize, 1, stream) == 1);
	fclose(stream);

	if (!success)
	{
		free(buff);
		return NULL;
	}

	buff[lsize] = '\0';

	if (size) *size = (size_t)lsize;
	return buff;
}


static uint32_t pixmap_read_pixel(
	const uint8_t *pixmap, uint8_t bits,
	uint32_t w, uint32_t h,
	uint32_t x, uint32_t y)
{
	if (!pixmap) return 0;
	if (x >= w ) return 0;
	if (y >= h ) return 0;

	uint8_t  bytes  = ((bits + 7) / 8);
	uint32_t stride = w * bytes;

	uint32_t pixel = 0;
	for (uint8_t i = 0; i < bytes; i++)
	{
		pixel <<= 8;
		pixel  |= pixmap[(y * stride) + x + i];
	}

	return pixel;
}

static bool is_ident(const char *name)
{
	// Disallow underscore at start or end as it looks messy.
	if (!isalnum(name[0]))
		return false;

	size_t o;
	for (o = 1; isalnum(name[o]) || (name[o] == '_'); o++);
	return ((name[o] == '\0') && (name[o - 1] != '_'));
}


void print_usage(FILE *fp, const char *name)
{
	fprintf(fp,
		"USAGE: %s <path> <bits> [name]\n", name);
}

int main(int argc, char *argv[])
{
	uint32_t target_bits;

	if ((argc < 2) || (argc > 4)
		|| !parse_u32z(argv[2], &target_bits))
	{
		fprintf(stderr, "Error: Invalid invocation.\n");
		print_usage(stderr, argv[0]);
		return EXIT_FAILURE;
	}

	if ((target_bits != 2) && (target_bits != 4) && (target_bits != 8))
	{
		fprintf(stderr, "Error: Invalid number of bits, expect 2, 4 or 8.\n");
		print_usage(stderr, argv[0]);
		return EXIT_FAILURE;
	}

	const char *path = argv[1];

	bool is_raw = (argc <= 3);

	const char *name = "";
	if (!is_raw) name = argv[3];

	if ((name[0] != '\0') && !is_ident(name))
	{
		fprintf(stderr, "Error: Invalid name '%s'.\n", name);
		return EXIT_FAILURE;
	}

	size_t name_len = strlen(name) + 1;
	char name_lower[name_len + 1];
	char name_upper[name_len + 1];
	if (name[0] == '\0')
	{
		name_lower[0] = '\0';
		name_upper[0] = '\0';
	}
	else
	{
		name_lower[0] = '_';
		name_upper[0] = '_';
		for (size_t i = 0; i < (name_len - 1); i++)
		{
			name_lower[i + 1] = tolower(name[i]);
			name_upper[i + 1] = toupper(name[i]);
		}
		name_lower[name_len] = '\0';
		name_upper[name_len] = '\0';
	}

	uint32_t pixmap_width  = 0;
	uint32_t pixmap_height = 0;
	uint8_t  pixmap_bits   = 0;

	size_t file_size = 0;
	uint8_t *file = file_read(path, &file_size);
	if (!file)
	{
		fprintf(stderr, "Error: Failed to import '%s'.\n", path);
		return EXIT_FAILURE;
	}

	size_t pixmap_offset = parse_pgm_header(
		(char *)file, file_size, &pixmap_width, &pixmap_height, &pixmap_bits);
	if (pixmap_offset == 0)
	{
		fprintf(stderr, "Error: Invalid binary pgm file '%s'.\n", path);
		free(file);
		return EXIT_FAILURE;
	}
	uint8_t *pixmap = &file[pixmap_offset];

	if ((pixmap_width == 0) || (pixmap_height == 0))
	{
		fprintf(stderr,
			"Error: Invalid pixmap dimensions %ux%u in pgm file '%s'.\n",
			pixmap_width, pixmap_height, path);
		free(file);
		return EXIT_FAILURE;
	}
	uint32_t pixmap_stride = ((pixmap_width * target_bits) + 7) / 8;

	if (pixmap_bits < target_bits)
	{
		fprintf(stderr,
			"Error: Grayscale image has %u bits which is less than specified target of %u bits in '%s'.\n",
			pixmap_bits, target_bits, path);
		free(file);
		return EXIT_FAILURE;
	}

	if (!is_raw)
	{
		printf("#define PIXMAP%s_WIDTH  %u\n", name_upper, pixmap_width );
		printf("#define PIXMAP%s_HEIGHT %u\n", name_upper, pixmap_height);
		printf("#define PIXMAP%s_STRIDE %u\n", name_upper, pixmap_stride);
		printf("#define PIXMAP%s_BPP    %u\n", name_upper, target_bits);
		printf("\n");
		printf("static const uint8_t pixmap%s[] = \n", name_lower);
		printf("{\n");
	}

	for (unsigned y = 0; y < pixmap_height; y++)
	{
		if (!is_raw) printf("\t");

		for (unsigned b = 0, x = 0; b < pixmap_stride; b++)
		{
			uint8_t byte = 0x00;
			for (unsigned i = 0; (i < 8) && (x < pixmap_width); i += target_bits, x++)
			{
				uint32_t pixel = pixmap_read_pixel(
					pixmap, pixmap_bits, pixmap_width, pixmap_height, x, y);
				pixel >>= (pixmap_bits - target_bits);
				byte |= pixel << i;
			}

			if (b > 0) printf(" ");
			printf("0x%02X,", byte);
		}

		printf("\n");
	}

	if (!is_raw) printf("};\n");

	free(file);
	return EXIT_SUCCESS;
}
