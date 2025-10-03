
#include <gbfw/api.h>

static volatile bool exit = false;
static volatile bool home = false;

const gbfw_api_t* gbfw_api = NULL;

typedef struct {
  uint8_t x;
  uint8_t y;
} Coord_uint8;

typedef struct {
  uint32_t x;
  uint32_t y;
} Coord_uint32;

// maybe its bugging because we are using uints and going negative?
Coord_uint32 coord_uint8_2_uint32(Coord_uint8 c) {
  uint32_t x = (uint32_t)(c.x << 24);
  uint32_t y = (uint32_t)(c.y << 24);
  return (Coord_uint32){x, y};
}

uint32_t uint32__min(uint32_t a, uint32_t b) {
  return a < b ? a : b;
}

uint32_t uint32__max(uint32_t a, uint32_t b) {
  return a > b ? a : b;
}

bool point_on_line(Coord_uint8 s, Coord_uint8 e, Coord_uint8 p) {
  // TODO optimise to find equation only once per line
  Coord_uint32 s32 = coord_uint8_2_uint32(s);
  Coord_uint32 e32 = coord_uint8_2_uint32(e);
  Coord_uint32 p32 = coord_uint8_2_uint32(p);

  // need to lshift here before the divide otherwise not at the start of the
  // function?
  uint32_t m = (e32.y - s32.y) / (e32.x - s32.x);
  uint32_t c = e32.y - m * e32.x;
  bool y_in_bounds =
      uint32__min(e32.y, s32.y) <= p32.y && p32.y <= uint32__max(e32.y, s32.y);
  bool x_in_bounds =
      uint32__min(e32.x, s32.x) <= p32.x && p32.x <= uint32__max(e32.x, s32.x);
  bool p_on_line = (p32.y - m * p32.x - c) == 0;
  if (p_on_line && y_in_bounds && x_in_bounds) {
    return true;
  }
  return false;
}

uint8_t rect_w = 0;
uint8_t rect_h = 0;
rgb565_t rect_c = RGB565_BLACK;

static void rect_scan_callback(rgb565_t* color, uint8_t size, uint8_t row) {
  (void)size;
  (void)row;

  rgb565_t colors[] = {RGB565_GREEN, RGB565_RED, RGB565_BLUE};

  for (int i = 0; i < rect_w; i++) {
    color[i] = colors[row];
  }
  // color[1] = RGB565_GREEN;
  // color[2] = RGB565_BLUE;
}

Coord_uint8 coord_e = {50, 50};
Coord_uint8 coord_s = {100, 75};

// static void vector_scan_callback(rgb565_t* color, uint8_t size, uint8_t row)
// {
//   (void)size;
//   (void)row;
//   Coord p;
//   for (int x = 0; x < rect_w; x++) {
//     for (int y = 0; y < rect_h; y++) {
//       p = (Coord){x, y};
//       if (point_on_line(coord_s, coord_e, p)) {
//         color[x] = RGB565_GREEN;
//         continue;
//       }
//       color[x] = RGB565_BLUE;
//     }
//   }
// }

void render_rectangle(uint8_t x,
                      uint8_t y,
                      uint8_t w,
                      uint8_t h,
                      rgb565_t colour) {
  rect_w = w;
  rect_h = h;
  rect_c = colour;

  gbfw_display_render_region(x, y, w, h, rect_scan_callback);
}

bool main(unsigned int signal, const gbfw_api_t* api) {
  switch (signal) {
    case GBFW_SIGNAL_BEGIN:
      break;

    case GBFW_SIGNAL_EXIT:
      exit = true;
      return true;

    case GBFW_SIGNAL_HOME:
      home = true;
      return true;

    default:
      return false;
  }

  gbfw_api_init(api);

  // Coord_uint8 p;

  while (!exit) {
    // for (uint8_t x = 0; x < GBFW_DISPLAY_WIDTH; x++) {
    //   for (uint8_t y = 0; y < GBFW_DISPLAY_HEIGHT; y++) {
    //     p = (Coord_uint8){x, y};
    //     if (point_on_line(coord_s, coord_e, p)) {
    //       render_rectangle(x, y, 1, 1, RGB565_GREEN);
    //       continue;
    //     }
    //     render_rectangle(x, y, 1, 1, RGB565_BLUE);
    //   }
    // }

    render_rectangle(50, 50, 6, 3, RGB565_BLUE);

    // render_rectangle(0, 0, GBFW_DISPLAY_WIDTH, GBFW_DISPLAY_HEIGHT,
    //                  RGB565_BLUE);
    // render_rectangle(50, 50, 3, 10, RGB565_GREEN);
    // gbfw_display_render_region(0, 0, GBFW_DISPLAY_WIDTH, GBFW_DISPLAY_HEIGHT,
    //                            vector_scan_callback);
  }
  return 0;
}
