#ifndef OLED_DISPLAY_SSD1306
#define OLED_DISPLAY_SSD1306

extern uint8_t OLED_Display_SSD1306_i2caddr;
extern uint8_t OLED_Display_SSD1306_Width;
extern uint8_t OLED_Display_SSD1306_Height;
extern int OLED_Display_SSD1306_Rotation;
extern uint8_t *OLED_Display_SSD1306_Buffer;


// Function prototypes
void OLED_Display_SSD1306_drawPixel(int16_t x, int16_t y, uint16_t color);
void OLED_Display_SSD1306_display(void);
void OLED_Display_SSD1306_clearDisplay(void);
void OLED_Display_SSD1306_drawBitmap(int16_t x, int16_t y, uint8_t *bitmap, int16_t w, int16_t h, uint16_t color);
void OLED_Display_SSD1306_init(uint8_t _i2caddr = 0x3C);

#endif // OLED_DISPLAY_SSD1306
