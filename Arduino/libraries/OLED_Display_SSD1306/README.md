# OLED_Display_SSD1306 Library

This library is a modified version of Adafruit's SSD1306 library, optimized for OLED displays with reduced PROGMEM usage and essential functions for drawing pixels and bitmaps.

## Purpose

The primary objectives of this library are:
- Minimize PROGMEM usage for storing bitmaps.
- Focus on essential functions: `OLED_Display_SSD1306_drawPixel`, `OLED_Display_SSD1306_drawBitmap`, `OLED_Display_SSD1306_display`, and `OLED_Display_SSD1306_init`.

## Features

- **OLED_Display_SSD1306_drawPixel**: Draw a single pixel on the OLED display.
- **OLED_Display_SSD1306_drawBitmap**: Draw a bitmap image on the OLED display.
- **OLED_Display_SSD1306_display**: Update the display with buffered data.
- **OLED_Display_SSD1306_init**: Initialize the OLED display with necessary settings.

## License

This library is licensed under the BSD License. See the [LICENSE](LICENSE) file for details.

### Original License (Adafruit SSD1306 Library)

The original Adafruit SSD1306 library is also licensed under the BSD License. For more details, refer to Adafruit's repository.

## Installation

### Arduino Library Manager

1. Open Arduino IDE.
2. Go to `Sketch` -> `Include Library` -> `Manage Libraries...`.
3. Search for `OLED_Display_SSD1306`.
4. Click `Install`.

### Manual Installation

1. Download the latest release from [GitHub](https://github.com/AbdulBasitKhatri/OLED_Display_SSD1306/releases).
2. Extract the ZIP file.
3. Rename the extracted folder to `OLED_Display_SSD1306`.
4. Move the `OLED_Display_SSD1306` folder to your Arduino libraries directory:
   - **Windows**: `My Documents\Arduino\libraries`
   - **Mac**: `Documents/Arduino/libraries`
   - **Linux**: `~/Arduino/libraries`

## Usage

### Example Sketch

```cpp
#include <OLED_Display_SSD1306.h>

// Define your bitmap here
const unsigned char bitmap[] = {
  // Bitmap data
  // Use image2cpp https://javl.github.io/image2cpp/ an amazing online tool to convert bitmaps to bytes
};

void setup() {
  OLED_Display_SSD1306_init();  // Initialize the OLED display
}

void loop() {
  OLED_Display_SSD1306_drawBitmap(0, 0, bitmap, 32, 32, 1);  // Draw bitmap at position (0, 0) and size of 32x32 pixels more than it will overload the RAM 
  // Use chunking to display full screen bitmaps
  OLED_Display_SSD1306_display();  // Update the display
}
```

## Contributing

Contributions are welcome! Please fork the repository and submit a pull request.

## Support

For issues, bugs, or feature requests, please open an issue.

## Acknowledgments

This library is based on the Adafruit SSD1306 library. I acknowledge Adafruit Industries for their original work.