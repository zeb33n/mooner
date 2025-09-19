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

	uint32_t bs = ((w + 7) / 8);
	uint32_t bx = (x / 8);
	uint32_t sx = (x % 8);

	uint8_t pb = bitmap[(y * bs) + bx];
	return ((pb >> (7 - sx)) & 1);
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
		"USAGE: %s <path> [name]\n", name);
}

int main(int argc, char *argv[])
{
	if ((argc < 2) || (argc > 3))
	{
		fprintf(stderr, "Error: Invalid invocation.\n");
		print_usage(stderr, argv[0]);
		return EXIT_FAILURE;
	}

	const char *path = argv[1];

	const char *name = "";
	if (argc > 2) name = argv[2];

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
		(const char *)file, file_size, &bitmap_width, &bitmap_height);
	if (bitmap_offset == 0)
	{
		fprintf(stderr, "Error: Invalid binary pbm file '%s'.\n", path);
		free(file);
		return EXIT_FAILURE;
	}
	uint8_t *bitmap = &file[bitmap_offset];

	if ((bitmap_width == 0) || (bitmap_height == 0))
	{
		fprintf(stderr,
			"Error: Invalid bitmap dimensions %ux%u in pbm file '%s'.\n",
			bitmap_width, bitmap_height, path);
		free(file);
		return EXIT_FAILURE;
	}
	uint32_t bitmap_stride = (bitmap_width + 7) / 8;

	printf("#define BITMAP%s_WIDTH  %u\n", name_upper, bitmap_width );
	printf("#define BITMAP%s_HEIGHT %u\n", name_upper, bitmap_height);
	printf("#define BITMAP%s_STRIDE %u\n", name_upper, bitmap_stride);
	printf("\n");
	printf("static const uint8_t bitmap%s[] = \n", name_lower);
	printf("{\n");

	for (unsigned y = 0; y < bitmap_height; y++)
	{
		printf("\t");

		for (unsigned b = 0, x = 0; b < bitmap_stride; b++)
		{
			uint8_t byte = 0x00;
			for (unsigned i = 0; (i < 8) && (x < bitmap_width); i++, x++)
			{
				bool pixel = bitmap_read_pixel(
					bitmap, bitmap_width, bitmap_height, x, y);
				byte |= pixel << i;
			}

			if (b > 0) printf(" ");
			printf("0x%02X,", byte);
		}

		printf("\n");
	}

	printf("};\n");

	free(file);
	return EXIT_SUCCESS;
}
