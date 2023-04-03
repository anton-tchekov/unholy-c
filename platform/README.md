# PLATFORM

- `platform_init`

## MEMORY

### Read

- `memory_rs8(addr)`
- `memory_rs16(addr)`
- `memory_ru8(addr)`
- `memory_ru16(addr)`
- `memory_r32(addr)`

### Write

- `memory_w8(addr, value)`
- `memory_w16(addr, value)`
- `memory_w32(addr, value)`

### Bulk

- `memory_cpy`
- `memory_cmp`
- `memory_chr`

### Dynamic

- `memory_alloc`
- `memory_free`

## STREAM

### Char

- `stream_fputc`
- `stream_fputs`

### String

- `stream_fputs_P`
- `stream_fputs_X`
- `stream_fputse_X`

### Decimal

- `stream_fputd`
- `stream_fputde`

### Hex

- `stream_fputx`
- `stream_fputxe`

### Float

- `stream_fputfe`
- `stream_fputf`

## GRAPHICS

- `color = graphics_color(r, g, b)`
- `graphics_rect(x, y, w, h, color)`
- `graphics_image_1bit(x, y, w, h, image, fg, bg)`
- `graphics_image_grayscale(x, y, w, h, image)`
- `graphics_image_rgb565(x, y, w, h, image)`
- `graphics_image_rgb888(x, y, w, h, image)`
- `graphics_image_rgba(x, y, w, h, image)`

## TIMER

- `millis = timer_millis()`

## KEYBOARD

- `keyboard_get`

