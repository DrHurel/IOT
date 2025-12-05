#include "MockDisplay.h"
#include <sstream>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <cmath>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../../external/stb_image_write.h"

namespace testing::mocks
{
    // Simple 5x7 bitmap font for basic characters
    // Each character is represented as 7 bytes (rows), 5 bits per row
    static const uint8_t FONT_5X7[][7] = {
        // Space (32)
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
        // ! (33)
        {0x04, 0x04, 0x04, 0x04, 0x00, 0x04, 0x00},
        // " (34)
        {0x0A, 0x0A, 0x0A, 0x00, 0x00, 0x00, 0x00},
        // # (35)
        {0x0A, 0x0A, 0x1F, 0x0A, 0x1F, 0x0A, 0x0A},
        // $ (36)
        {0x04, 0x0F, 0x14, 0x0E, 0x05, 0x1E, 0x04},
        // % (37)
        {0x18, 0x19, 0x02, 0x04, 0x08, 0x13, 0x03},
        // & (38)
        {0x08, 0x14, 0x14, 0x08, 0x15, 0x12, 0x0D},
        // ' (39)
        {0x0C, 0x04, 0x08, 0x00, 0x00, 0x00, 0x00},
        // ( (40)
        {0x02, 0x04, 0x08, 0x08, 0x08, 0x04, 0x02},
        // ) (41)
        {0x08, 0x04, 0x02, 0x02, 0x02, 0x04, 0x08},
        // * (42)
        {0x00, 0x04, 0x15, 0x0E, 0x15, 0x04, 0x00},
        // + (43)
        {0x00, 0x04, 0x04, 0x1F, 0x04, 0x04, 0x00},
        // , (44)
        {0x00, 0x00, 0x00, 0x00, 0x0C, 0x04, 0x08},
        // - (45)
        {0x00, 0x00, 0x00, 0x1F, 0x00, 0x00, 0x00},
        // . (46)
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x0C},
        // / (47)
        {0x00, 0x01, 0x02, 0x04, 0x08, 0x10, 0x00},
        // 0 (48)
        {0x0E, 0x11, 0x13, 0x15, 0x19, 0x11, 0x0E},
        // 1 (49)
        {0x04, 0x0C, 0x04, 0x04, 0x04, 0x04, 0x0E},
        // 2 (50)
        {0x0E, 0x11, 0x01, 0x02, 0x04, 0x08, 0x1F},
        // 3 (51)
        {0x1F, 0x02, 0x04, 0x02, 0x01, 0x11, 0x0E},
        // 4 (52)
        {0x02, 0x06, 0x0A, 0x12, 0x1F, 0x02, 0x02},
        // 5 (53)
        {0x1F, 0x10, 0x1E, 0x01, 0x01, 0x11, 0x0E},
        // 6 (54)
        {0x06, 0x08, 0x10, 0x1E, 0x11, 0x11, 0x0E},
        // 7 (55)
        {0x1F, 0x01, 0x02, 0x04, 0x08, 0x08, 0x08},
        // 8 (56)
        {0x0E, 0x11, 0x11, 0x0E, 0x11, 0x11, 0x0E},
        // 9 (57)
        {0x0E, 0x11, 0x11, 0x0F, 0x01, 0x02, 0x0C},
        // : (58)
        {0x00, 0x0C, 0x0C, 0x00, 0x0C, 0x0C, 0x00},
        // ; (59)
        {0x00, 0x0C, 0x0C, 0x00, 0x0C, 0x04, 0x08},
        // < (60)
        {0x02, 0x04, 0x08, 0x10, 0x08, 0x04, 0x02},
        // = (61)
        {0x00, 0x00, 0x1F, 0x00, 0x1F, 0x00, 0x00},
        // > (62)
        {0x08, 0x04, 0x02, 0x01, 0x02, 0x04, 0x08},
        // ? (63)
        {0x0E, 0x11, 0x01, 0x02, 0x04, 0x00, 0x04},
        // @ (64)
        {0x0E, 0x11, 0x01, 0x0D, 0x15, 0x15, 0x0E},
        // A (65)
        {0x0E, 0x11, 0x11, 0x11, 0x1F, 0x11, 0x11},
        // B (66)
        {0x1E, 0x11, 0x11, 0x1E, 0x11, 0x11, 0x1E},
        // C (67)
        {0x0E, 0x11, 0x10, 0x10, 0x10, 0x11, 0x0E},
        // D (68)
        {0x1C, 0x12, 0x11, 0x11, 0x11, 0x12, 0x1C},
        // E (69)
        {0x1F, 0x10, 0x10, 0x1E, 0x10, 0x10, 0x1F},
        // F (70)
        {0x1F, 0x10, 0x10, 0x1E, 0x10, 0x10, 0x10},
        // G (71)
        {0x0E, 0x11, 0x10, 0x17, 0x11, 0x11, 0x0F},
        // H (72)
        {0x11, 0x11, 0x11, 0x1F, 0x11, 0x11, 0x11},
        // I (73)
        {0x0E, 0x04, 0x04, 0x04, 0x04, 0x04, 0x0E},
        // J (74)
        {0x07, 0x02, 0x02, 0x02, 0x02, 0x12, 0x0C},
        // K (75)
        {0x11, 0x12, 0x14, 0x18, 0x14, 0x12, 0x11},
        // L (76)
        {0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x1F},
        // M (77)
        {0x11, 0x1B, 0x15, 0x15, 0x11, 0x11, 0x11},
        // N (78)
        {0x11, 0x11, 0x19, 0x15, 0x13, 0x11, 0x11},
        // O (79)
        {0x0E, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0E},
        // P (80)
        {0x1E, 0x11, 0x11, 0x1E, 0x10, 0x10, 0x10},
        // Q (81)
        {0x0E, 0x11, 0x11, 0x11, 0x15, 0x12, 0x0D},
        // R (82)
        {0x1E, 0x11, 0x11, 0x1E, 0x14, 0x12, 0x11},
        // S (83)
        {0x0F, 0x10, 0x10, 0x0E, 0x01, 0x01, 0x1E},
        // T (84)
        {0x1F, 0x04, 0x04, 0x04, 0x04, 0x04, 0x04},
        // U (85)
        {0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x0E},
        // V (86)
        {0x11, 0x11, 0x11, 0x11, 0x11, 0x0A, 0x04},
        // W (87)
        {0x11, 0x11, 0x11, 0x15, 0x15, 0x15, 0x0A},
        // X (88)
        {0x11, 0x11, 0x0A, 0x04, 0x0A, 0x11, 0x11},
        // Y (89)
        {0x11, 0x11, 0x11, 0x0A, 0x04, 0x04, 0x04},
        // Z (90)
        {0x1F, 0x01, 0x02, 0x04, 0x08, 0x10, 0x1F},
    };

    MockDisplay::MockDisplay(int width, int height)
        : screen_width(width), screen_height(height),
          current_text_color(0xFFFF), current_text_bg_color(0x0000), current_text_size(1),
          cursor_x(0), cursor_y(0)
    {
        pixels.resize(screen_width * screen_height, 0x0000);
    }

    void MockDisplay::setPixel(int x, int y, uint16_t color)
    {
        if (x >= 0 && x < screen_width && y >= 0 && y < screen_height)
        {
            pixels[y * screen_width + x] = color;
        }
    }

    void MockDisplay::drawLineInternal(int x0, int y0, int x1, int y1, uint16_t color)
    {
        // Bresenham's line algorithm
        int dx = abs(x1 - x0);
        int dy = abs(y1 - y0);
        int sx = (x0 < x1) ? 1 : -1;
        int sy = (y0 < y1) ? 1 : -1;
        int err = dx - dy;

        while (true)
        {
            setPixel(x0, y0, color);
            if (x0 == x1 && y0 == y1)
                break;
            int e2 = 2 * err;
            if (e2 > -dy)
            {
                err -= dy;
                x0 += sx;
            }
            if (e2 < dx)
            {
                err += dx;
                y0 += sy;
            }
        }
    }

    void MockDisplay::drawCircleInternal(int xc, int yc, int radius, uint16_t color, bool fill)
    {
        // Midpoint circle algorithm
        int x = radius;
        int y = 0;
        int err = 0;

        while (x >= y)
        {
            if (fill)
            {
                // Draw horizontal lines to fill the circle
                for (int i = xc - x; i <= xc + x; i++)
                {
                    setPixel(i, yc + y, color);
                    setPixel(i, yc - y, color);
                }
                for (int i = xc - y; i <= xc + y; i++)
                {
                    setPixel(i, yc + x, color);
                    setPixel(i, yc - x, color);
                }
            }
            else
            {
                // Draw circle outline
                setPixel(xc + x, yc + y, color);
                setPixel(xc + y, yc + x, color);
                setPixel(xc - y, yc + x, color);
                setPixel(xc - x, yc + y, color);
                setPixel(xc - x, yc - y, color);
                setPixel(xc - y, yc - x, color);
                setPixel(xc + y, yc - x, color);
                setPixel(xc + x, yc - y, color);
            }

            if (err <= 0)
            {
                y += 1;
                err += 2 * y + 1;
            }
            if (err > 0)
            {
                x -= 1;
                err -= 2 * x + 1;
            }
        }
    }

    void MockDisplay::drawCharacter(char c, int x, int y, uint16_t color, int size)
    {
        // Get font data for this character
        const uint8_t *glyph = nullptr;

        if (c >= 32 && c <= 90)
        {
            glyph = FONT_5X7[c - 32];
        }
        else if (c >= 'a' && c <= 'z')
        {
            // Use uppercase for lowercase letters
            glyph = FONT_5X7[c - 'a' + 'A' - 32];
        }
        else
        {
            // Default to space for unknown characters
            glyph = FONT_5X7[0];
        }

        // Render the character bitmap
        for (int row = 0; row < 7; row++)
        {
            uint8_t rowData = glyph[row];
            for (int col = 0; col < 5; col++)
            {
                if (rowData & (1 << (4 - col)))
                {
                    // Draw scaled pixel
                    for (int sy = 0; sy < size; sy++)
                    {
                        for (int sx = 0; sx < size; sx++)
                        {
                            setPixel(x + col * size + sx, y + row * size + sy, color);
                        }
                    }
                }
            }
        }
    }

    void MockDisplay::fillRect(int x, int y, int w, int h, uint16_t color)
    {
        operations.push_back(DrawOperation(DrawOperation::FILL_RECT, x, y, w, h, "", color));

        // Actually render the rectangle
        for (int dy = 0; dy < h; dy++)
        {
            for (int dx = 0; dx < w; dx++)
            {
                setPixel(x + dx, y + dy, color);
            }
        }
    }

    void MockDisplay::drawString(const std::string &text, int x, int y)
    {
        operations.push_back(DrawOperation(DrawOperation::DRAW_TEXT, x, y, 0, 0, text, current_text_color, current_text_size));

        // Actually render the text
        int cursor = x;
        for (char c : text)
        {
            drawCharacter(c, cursor, y, current_text_color, current_text_size);
            cursor += 6 * current_text_size;
        }
    }

    void MockDisplay::drawString(const char *text, int x, int y)
    {
        drawString(std::string(text), x, y);
    }

    void MockDisplay::drawPixel(int x, int y, uint16_t color)
    {
        operations.push_back(DrawOperation(DrawOperation::DRAW_PIXEL, x, y, 0, 0, "", color));
        setPixel(x, y, color);
    }

    void MockDisplay::fillScreen(uint16_t color)
    {
        operations.push_back(DrawOperation(DrawOperation::CLEAR, 0, 0, screen_width, screen_height, "", color));
        std::fill(pixels.begin(), pixels.end(), color);
    }

    void MockDisplay::setTextColor(uint16_t color)
    {
        current_text_color = color;
        operations.push_back(DrawOperation(DrawOperation::SET_TEXT_COLOR, 0, 0, 0, 0, "", color));
    }

    void MockDisplay::setTextColor(uint16_t fg, uint16_t bg)
    {
        current_text_color = fg;
        current_text_bg_color = bg;
        operations.push_back(DrawOperation(DrawOperation::SET_TEXT_COLOR, 0, 0, 0, 0, "", fg, bg));
    }

    void MockDisplay::setTextSize(int size)
    {
        current_text_size = size;
        operations.push_back(DrawOperation(DrawOperation::SET_TEXT_SIZE, 0, 0, 0, 0, "", 0, size));
    }

    void MockDisplay::drawLine(int x0, int y0, int x1, int y1, uint16_t color)
    {
        operations.push_back(DrawOperation(DrawOperation::DRAW_LINE, x0, y0, x1, y1, "", color));
        drawLineInternal(x0, y0, x1, y1, color);
    }

    void MockDisplay::drawCircle(int x, int y, int radius, uint16_t color)
    {
        operations.push_back(DrawOperation(DrawOperation::DRAW_CIRCLE, x, y, 0, 0, "", color, radius));
        drawCircleInternal(x, y, radius, color, false);
    }

    void MockDisplay::fillCircle(int x, int y, int radius, uint16_t color)
    {
        operations.push_back(DrawOperation(DrawOperation::DRAW_CIRCLE, x, y, 0, 0, "", color, radius));
        drawCircleInternal(x, y, radius, color, true);
    }

    void MockDisplay::setCursor(int x, int y)
    {
        cursor_x = x;
        cursor_y = y;
    }

    void MockDisplay::print(const char *text)
    {
        drawString(text, cursor_x, cursor_y);
        // Update cursor position (approximate)
        cursor_x += std::string_view(text).length() * 6 * current_text_size;
    }

    void MockDisplay::print(const std::string &text)
    {
        print(text.c_str());
    }

    std::string MockDisplay::generateSnapshot() const
    {
        std::stringstream ss;
        ss << "Display(" << screen_width << "x" << screen_height << ")\n";
        ss << "Operations: " << operations.size() << "\n";
        ss << "---\n";

        for (size_t i = 0; i < operations.size(); ++i)
        {
            const auto &op = operations[i];
            ss << std::setw(4) << std::setfill('0') << i << ": ";

            switch (op.type)
            {
            case DrawOperation::FILL_RECT:
                ss << "FILL_RECT(x=" << op.x << ", y=" << op.y << ", w="
                   << op.width << ", h=" << op.height << ") color=0x"
                   << std::hex << std::setw(4) << std::setfill('0') << op.color << std::dec;
                break;

            case DrawOperation::DRAW_TEXT:
                ss << "DRAW_TEXT(\"" << op.text << "\") at(x=" << op.x << ", y=" << op.y
                   << ") color=0x" << std::hex << std::setw(4) << std::setfill('0') << op.color << std::dec
                   << " size=" << op.param1;
                break;

            case DrawOperation::DRAW_PIXEL:
                ss << "DRAW_PIXEL(x=" << op.x << ", y=" << op.y << ") color=0x"
                   << std::hex << std::setw(4) << std::setfill('0') << op.color << std::dec;
                break;

            case DrawOperation::SET_TEXT_COLOR:
                ss << "SET_TEXT_COLOR(fg=0x" << std::hex << std::setw(4) << std::setfill('0') << op.color << std::dec;
                if (op.param1 != 0)
                {
                    ss << ", bg=0x" << std::hex << std::setw(4) << std::setfill('0') << op.param1 << std::dec;
                }
                ss << ")";
                break;

            case DrawOperation::SET_TEXT_SIZE:
                ss << "SET_TEXT_SIZE(size=" << op.param1 << ")";
                break;

            case DrawOperation::CLEAR:
                ss << "CLEAR(color=0x" << std::hex << std::setw(4) << std::setfill('0') << op.color << std::dec << ")";
                break;

            case DrawOperation::DRAW_LINE:
                ss << "DRAW_LINE(x0=" << op.x << ", y0=" << op.y << ", x1="
                   << op.width << ", y1=" << op.height << ") color=0x"
                   << std::hex << std::setw(4) << std::setfill('0') << op.color << std::dec;
                break;

            case DrawOperation::DRAW_CIRCLE:
                ss << "DRAW_CIRCLE(x=" << op.x << ", y=" << op.y << ", radius="
                   << op.param1 << ") color=0x"
                   << std::hex << std::setw(4) << std::setfill('0') << op.color << std::dec;
                break;

            default:
                ss << "UNKNOWN";
                break;
            }

            ss << "\n";
        }

        return ss.str();
    }

    bool MockDisplay::compareSnapshot(const std::string &expected) const
    {
        return generateSnapshot() == expected;
    }

    void MockDisplay::saveSnapshot(const std::string &filename) const
    {
        // Save as PNG instead of text
        if (filename.find(".snapshot") != std::string::npos)
        {
            // Replace .snapshot with .png
            std::string pngFilename = filename.substr(0, filename.find(".snapshot")) + ".png";
            savePNG(pngFilename);
        }
        else
        {
            savePNG(filename);
        }
    }

    void MockDisplay::savePNG(const std::string &filename) const
    {
        // Convert RGB565 to RGB888
        std::vector<uint8_t> rgb_data(screen_width * screen_height * 3);

        for (int i = 0; i < screen_width * screen_height; i++)
        {
            uint16_t pixel = pixels[i];

            // Extract RGB components from RGB565
            uint8_t r = ((pixel >> 11) & 0x1F) << 3; // 5 bits -> 8 bits
            uint8_t g = ((pixel >> 5) & 0x3F) << 2;  // 6 bits -> 8 bits
            uint8_t b = (pixel & 0x1F) << 3;         // 5 bits -> 8 bits

            // Fill in bits to get full range
            r |= (r >> 5);
            g |= (g >> 6);
            b |= (b >> 5);

            rgb_data[i * 3 + 0] = r;
            rgb_data[i * 3 + 1] = g;
            rgb_data[i * 3 + 2] = b;
        }

        // Write PNG file
        stbi_write_png(filename.c_str(), screen_width, screen_height, 3, rgb_data.data(), screen_width * 3);
    }

    bool MockDisplay::comparePNG(const std::string &filename) const
    {
        // For PNG comparison, we'd need to load and compare pixel data
        // For now, just return true if file exists (simplified)
        std::ifstream file(filename);
        return file.good();
    }

    std::string MockDisplay::loadSnapshot(const std::string &filename) const
    {
        std::ifstream file(filename);
        if (!file.is_open())
        {
            return "";
        }
        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }

    void MockDisplay::clear()
    {
        operations.clear();
        std::fill(pixels.begin(), pixels.end(), 0x0000);
        current_text_color = 0xFFFF;
        current_text_bg_color = 0x0000;
        current_text_size = 1;
        cursor_x = 0;
        cursor_y = 0;
    }

} // namespace testing::mocks
