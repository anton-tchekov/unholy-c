#ifndef __PLATFORM_H__
#define __PLATFORM_H__

/* Memory banks */
#define BANK_INPUT             0
#define BANK_PARSER            0
#define BANK_INTERPRETER       1

/* Input */
#define OFFSET_INPUT           0x0000

/* Parser */
#define OFFSET_BREAK_STACK     0x8000
#define OFFSET_CONTINUE_STACK  0x8400
#define OFFSET_VARIABLES       0x8800
#define OFFSET_FUNCTIONS       0x8C00
#define OFFSET_CONSTANTS       0x9000
#define OFFSET_FUNCTION_PARAMS 0x9400
#define OFFSET_FUNCTION_ADDRS  0x9800
#define OFFSET_FUNCTION_USAGES 0x9C00

/* Interpreter */
#define OFFSET_CODE            0x0000
#define OFFSET_RODATA          0x4000
#define OFFSET_DATA            0x8000

#define PROGMEM

#define pgm_read_word(address) *(address)
#define pgm_read_byte(address) *(address)
#define pgm_read_ptr(address) *(address)

/* --- PLATFORM --- */
void platform_init(void);

/* --- MEMORY --- */
u8 memory_r8(u8 bank, u16 addr);
u16 memory_r16(u8 bank, u16 addr);
u32 memory_r32(u8 bank, u16 addr);
void memory_w8(u8 bank, u16 addr, u8 val);
void memory_w16(u8 bank, u16 addr, u16 val);
void memory_w32(u8 bank, u16 addr, u32 val);
void memory_cpy(u8 bank, u16 dest, u16 src, u16 count);
i8 memory_cmp(u8 bank, u16 a, u16 b, u16 count);
u16 memory_chr(u8 bank, u16 addr, u8 value, u16 count);
void memory_set(u8 bank, u16 addr, u8 value, u16 size);
void xmem_read(u8 bank, u16 addr, void *data, u16 size);
void xmem_write(u8 bank, u16 addr, void *data, u16 size);

/* --- FILE --- */
u8 fs_fopen(const char *filename, const char *mode);
void fs_fclose(u8 file);
u16 fs_fread(u8 file, u8 bank, u16 addr, u16 size);
u16 fs_fwrite(u8 file, u8 bank, u16 addr, u16 size);
void fs_fseek(u8 file, u32 pos);
u32 fs_ftell(u8 file);
void fs_fputc(u8 file, char c);

/* --- STREAM --- */
void stream_fputc(u8 stream, char value);
void stream_fputs(u8 stream, const char *s);
void stream_fputs_P(u8 stream, const char *s);
void stream_fputs_X(u8 stream, u8 bank, u16 addr);
void stream_fputse_X(u8 stream, u8 bank, u16 addr, u16 len);
void stream_fputd(u8 stream, i32 value);
void stream_fputde(u8 stream, i32 value, u8 width);
void stream_fputx(u8 stream, u32 value);
void stream_fputxe(u8 stream, u32 value, u8 width);
void stream_fputfe(u8 stream, f32 value, u8 width, u8 decimal);
void stream_fputf(u8 stream, f32 value);

/* --- TIMER --- */
u32 timer_millis(void);

/* --- GRAPHICS --- */
void graphics_init(void);
void graphics_destroy(void);
u32 graphics_color(u8 r, u8 g, u8 b);
void graphics_rect(i32 x, i32 y, i32 w, i32 h, u32 color);
void graphics_update(void);
void os_events(void);

#endif /* __PLATFORM_H__ */
