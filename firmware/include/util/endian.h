#ifndef ENDIAN_H
#define ENDIAN_H

#define ENDIAN_SWAP16(x) (((x) << 8) | ((x) >> 8))

#define ENDIAN_SWAP32(x) (((x) >> 24) | ((x) << 24) \
	| (((x) & 0x00FF0000) >> 8) | (((x) & 0x0000FF00) << 8))

#define ENDIAN_SWAP64(x) (((x) >> 56) | ((x) << 56) \
	| (((x) & 0x00FF000000000000ULL) >> 40) \
	| (((x) & 0x000000000000FF00ULL) << 40) \
	| (((x) & 0x0000FF0000000000ULL) >> 24) \
	| (((x) & 0x0000000000FF0000ULL) << 24) \
	| (((x) & 0x000000FF00000000ULL) >>  8) \
	| (((x) & 0x00000000FF000000ULL) <<  8))

#endif
