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

static size_t parse_pbm_space(const char *buff, size_t offset, size_t size)
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

static size_t parse_pbm_header(
	const char *file, size_t size,
	uint32_t *width, uint32_t *height)
{
	if (!file) return 0;

	size_t o = 0;
	if ((size < 2) || (file[0] != 'P') || (file[1] != '4'))
		return 0;
	o += 2;

	size_t s;

	s = parse_pbm_space(file, o, size);
	if (s == 0) return 0;
	o += s;

	uint32_t w;
	s = parse_u32(file, o, size, &w);
	if (s == 0) return 0;
	o += s;

	s = parse_pbm_space(file, o, size);
	if (s == 0) return 0;
	o += s;

	uint32_t h;
	s = parse_u32(file, o, size, &h);
	if (s == 0) return 0;
	o += s;

	if (!isspace(file[o++]))
		return 0;

	if (width ) *width  = w;
	if (height) *height = h;
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


static bool bitmap_read_pixel(
	const uint8_t *bitmap,
	uint32_t w, uint32_t h,
	uint32_t x, uint32_t y)
{
	if (!bitmap) return false;
	if (x >= w ) return false;
	if (y >= h ) return false;

	uint32_t bw = (w / 8);
	uint32_t bx = (x / 8);
	uint32_t sx = (x % 8);

	uint8_t pb = bitmap[(y * bw) + bx];
	return ((pb >> (7 - sx)) & 1);
}

static bool bitmap_read_font_pixel(
	const uint8_t *bitmap, uint32_t bitmap_width, uint32_t bitmap_height,
	uint32_t font_width, uint32_t font_height,
	uint8_t c, uint8_t fx, uint8_t fy)
{
	if (fx > font_width ) return false;
	if (fy > font_height) return false;

	uint32_t font_cols = bitmap_width  / font_width;
	uint32_t font_rows = bitmap_height / font_height;

	uint32_t cx = c % font_cols;
	uint32_t cy = c / font_cols;

	if (cy >= font_rows) return false;

	return bitmap_read_pixel(
		bitmap, bitmap_width, bitmap_height,
		((cx * font_width ) + fx),
		((cy * font_height) + fy));
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
		"USAGE: %s <path> <width> <height> <cols> <rows> <hspace> <vspace> <min> <max> [type_prefix] [name]\n", name);
}

int main(int argc, char *argv[])
{
	uint32_t font_cols;
	uint32_t font_rows;

	uint32_t font_width;
	uint32_t font_height;

	uint32_t space[2];

	uint32_t range_min;
	uint32_t range_max;

	if ((argc < 10) || (argc > 12)
		|| !parse_u32z(argv[2], &font_width)
		|| !parse_u32z(argv[3], &font_height)
		|| !parse_u32z(argv[4], &font_cols)
		|| !parse_u32z(argv[5], &font_rows)
		|| !parse_u32z(argv[6], &space[0])
		|| !parse_u32z(argv[7], &space[1])
		|| !parse_u32z(argv[8], &range_min)
		|| !parse_u32z(argv[9], &range_max))
	{
		fprintf(stderr, "Error: Invalid invocation.\n");
		print_usage(stderr, argv[0]);
		return EXIT_FAILURE;
	}

	if ((range_min >= (font_cols * font_rows))
		|| (range_max >= (font_cols * font_rows)))
	{
		fprintf(stderr, "Error: Range exceeds provided cols and rows.\n");
		return EXIT_FAILURE;
	}

	const char *path = argv[1];

	const char *type_prefix = "gbfw";
	if (argc > 10) type_prefix = argv[10];

	const char *name = "";
	if (argc > 11) name = argv[11];

	if ((name[0] != '\0') && !is_ident(name))
	{
		fprintf(stderr, "Error: Invalid name '%s'.\n", name);
		return EXIT_FAILURE;
	}

	size_t name_len = strlen(name) + 1;
	char name_lower[name_len + 1];
	if (name[0] == '\0')
	{
		name_lower[0] = '\0';
	}
	else
	{
		name_lower[0] = '_';
		for (size_t i = 0; i < (name_len - 1); i++)
			name_lower[i + 1] = tolower(name[i]);
		name_lower[name_len] = '\0';
	}

	uint32_t bitmap_width  = 0;
	uint32_t bitmap_height = 0;

	size_t file_size = 0;
	uint8_t *file = file_read(path, &file_size);
	if (!file)
	{
		fprintf(stderr, "Error: Failed to import '%s'.\n", path);
		return EXIT_FAILURE;
	}

	size_t bitmap_offset = parse_pbm_header(
		(char *)file, file_size, &bitmap_width, &bitmap_height);
	if (bitmap_offset == 0)
	{
		fprintf(stderr, "Error: Invalid binary pbm file '%s'.\n", path);
		free(file);
		return EXIT_FAILURE;
	}
	uint8_t *bitmap = &file[bitmap_offset];

	if ((bitmap_width % font_cols)
		|| (bitmap_height % font_rows))
	{
		fprintf(stderr, "Error: Font bitmap dimensions %ux%u"
			" don't match provided cols and rows %ux%u.\n",
			bitmap_width, bitmap_height, font_cols, font_rows);
		free(file);
		return EXIT_FAILURE;
	}

	const uint8_t font_stride[2] =
	{
		(bitmap_width  / font_cols),
		(bitmap_height / font_rows),
	};

	if ((font_width > font_stride[0])
		|| (font_height > font_stride[1]))
	{
		fprintf(stderr, "Bitmap too small (%ux%u)"
			" to contain glyphs of specified size (%ux%u).\n",
			bitmap_width, bitmap_height,
			font_width, font_height);
		free(file);
		return EXIT_FAILURE;
	}

	printf("static const uint8_t font%s_bitmap[] = \n", name_lower);
	printf("{\n");

	uint32_t byte_offset = 0;
	uint8_t  bit_offset  = 0;
	uint8_t  byte        = 0x00;
	uint8_t  col_width   = 8;

	for (unsigned c = range_min; c <= range_max; c++)
	{
		for (unsigned y = 0; y < font_height; y++)
		{
			for (unsigned x = 0; x < font_width; x++)
			{
				bool pixel = bitmap_read_font_pixel(
					bitmap, bitmap_width, bitmap_height,
					font_stride[0], font_stride[1],
					c, x, y);

				byte |= pixel << bit_offset++;
				if (bit_offset >= 8)
				{
					uint8_t col = (byte_offset % col_width);
					printf(col == 0 ? "\t" : " ");

					printf("0x%02X,", byte);

					if (col == (col_width - 1))
						printf("\n");

					bit_offset = 0;
					byte_offset++;
					byte = 0x00;
				}
			}
		}
	}

	if (bit_offset > 0)
	{
		uint8_t col = (byte_offset % col_width);
		printf(col == 0 ? "\t" : " ");

		printf("0x%02X,", byte);
		printf("\n");
	}

	printf("};\n");

	printf("\n");

	printf("const %s_font_t font%s = \n{\n", type_prefix, name_lower);
	printf("\t.hspace = %u,\n", space[0]);
	printf("\t.vspace = %u,\n", space[1]);
	printf("\t.width  = %u,\n", font_width);
	printf("\t.height = %u,\n", font_height);
	printf("\t.range  = { %" PRIu8 ", %" PRIu8 " },\n", range_min, range_max);
	printf("\t.bitmap = font%s_bitmap,\n", name_lower);
	printf("};\n");

	free(file);
	return EXIT_SUCCESS;
}
