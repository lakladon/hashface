#include "avatar_generator.hpp"
#include "md5.hpp"
#include <fstream>
#include <stdexcept>
#include <cmath>
#include <algorithm>
#include <zlib.h>

namespace hashface {

// CRC32 table for PNG
static uint32_t crc_table[256];
static bool crc_table_computed = false;

static void make_crc_table() {
    if (crc_table_computed) return;
    
    for (uint32_t n = 0; n < 256; n++) {
        uint32_t c = n;
        for (int k = 0; k < 8; k++) {
            if (c & 1)
                c = 0xedb88320L ^ (c >> 1);
            else
                c = c >> 1;
        }
        crc_table[n] = c;
    }
    crc_table_computed = true;
}

static uint32_t update_crc(uint32_t crc, const uint8_t* buf, size_t len) {
    make_crc_table();
    uint32_t c = crc;
    for (size_t n = 0; n < len; n++) {
        c = crc_table[(c ^ buf[n]) & 0xff] ^ (c >> 8);
    }
    return c;
}

static uint32_t crc(const uint8_t* buf, size_t len) {
    return update_crc(0xffffffffL, buf, len) ^ 0xffffffffL;
}

static void write_be32(std::vector<uint8_t>& out, uint32_t val) {
    out.push_back((val >> 24) & 0xff);
    out.push_back((val >> 16) & 0xff);
    out.push_back((val >> 8) & 0xff);
    out.push_back(val & 0xff);
}

static std::vector<uint8_t> create_png_chunk(const std::string& type, const std::vector<uint8_t>& data) {
    std::vector<uint8_t> chunk;
    write_be32(chunk, static_cast<uint32_t>(data.size()));
    
    // Type
    for (char c : type) {
        chunk.push_back(static_cast<uint8_t>(c));
    }
    
    // Data
    chunk.insert(chunk.end(), data.begin(), data.end());
    
    // CRC (over type + data)
    uint32_t crc_val = crc(chunk.data() + 4, 4 + data.size());
    write_be32(chunk, crc_val);
    
    return chunk;
}

AvatarGenerator::AvatarGenerator(int size, int grid_size)
    : size_(size), grid_size_(grid_size), bg_r_(255), bg_g_(255), bg_b_(255) {
    if (size <= 0 || grid_size <= 0) {
        throw std::invalid_argument("Size and grid_size must be positive");
    }
}

void AvatarGenerator::set_background_color(uint8_t r, uint8_t g, uint8_t b) {
    bg_r_ = r;
    bg_g_ = g;
    bg_b_ = b;
}

uint32_t AvatarGenerator::get_color(const std::vector<uint8_t>& hash) {
    // GitHub uses the last 3 bytes of the hash for color
    // But we'll use first 3 for more variation
    uint8_t r = hash[0];
    uint8_t g = hash[1];
    uint8_t b = hash[2];
    
    // Ensure the color is not too light (would blend with background)
    // and not too dark
    double luminance = 0.299 * r + 0.587 * g + 0.114 * b;
    
    if (luminance > 200) {
        // Darken
        r = static_cast<uint8_t>(r * 0.6);
        g = static_cast<uint8_t>(g * 0.6);
        b = static_cast<uint8_t>(b * 0.6);
    } else if (luminance < 55) {
        // Lighten
        r = static_cast<uint8_t>(std::min(255.0, r * 1.5 + 50));
        g = static_cast<uint8_t>(std::min(255.0, g * 1.5 + 50));
        b = static_cast<uint8_t>(std::min(255.0, b * 1.5 + 50));
    }
    
    return (r << 16) | (g << 8) | b;
}

std::vector<std::vector<bool>> AvatarGenerator::generate_grid(const std::vector<uint8_t>& hash) {
    std::vector<std::vector<bool>> grid(grid_size_, std::vector<bool>(grid_size_, false));
    
    // GitHub-style: symmetric pattern
    // Use hash bits to determine which cells are filled
    int half = (grid_size_ + 1) / 2;
    
    for (int y = 0; y < grid_size_; y++) {
        for (int x = 0; x < half; x++) {
            // Use different bytes from hash for each cell
            int byte_index = (y * half + x) % hash.size();
            // Use lower bit to determine if cell is filled
            bool filled = (hash[byte_index] & 0x01) == 0;
            
            grid[y][x] = filled;
            // Mirror to right side (except middle column for odd sizes)
            grid[y][grid_size_ - 1 - x] = filled;
        }
    }
    
    return grid;
}

bool AvatarGenerator::write_png(const std::string& filename,
                                 const std::vector<uint8_t>& pixels,
                                 int width, int height) {
    std::ofstream file(filename, std::ios::binary);
    if (!file) return false;
    
    std::vector<uint8_t> png_data;
    
    // PNG signature
    const uint8_t signature[] = {0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A};
    png_data.insert(png_data.end(), signature, signature + 8);
    
    // IHDR chunk
    std::vector<uint8_t> ihdr_data;
    write_be32(ihdr_data, width);
    write_be32(ihdr_data, height);
    ihdr_data.push_back(8);  // bit depth
    ihdr_data.push_back(2);  // color type (RGB)
    ihdr_data.push_back(0);  // compression
    ihdr_data.push_back(0);  // filter
    ihdr_data.push_back(0);  // interlace
    
    auto ihdr_chunk = create_png_chunk("IHDR", ihdr_data);
    png_data.insert(png_data.end(), ihdr_chunk.begin(), ihdr_chunk.end());
    
    // Prepare raw image data with filter bytes
    std::vector<uint8_t> raw_data;
    for (int y = 0; y < height; y++) {
        raw_data.push_back(0);  // No filter for this row
        for (int x = 0; x < width * 3; x++) {
            raw_data.push_back(pixels[y * width * 3 + x]);
        }
    }
    
    // Compress with zlib
    uLongf compressed_size = compressBound(raw_data.size());
    std::vector<uint8_t> compressed(compressed_size);
    
    if (compress2(compressed.data(), &compressed_size,
                  raw_data.data(), raw_data.size(), 9) != Z_OK) {
        return false;
    }
    compressed.resize(compressed_size);
    
    // IDAT chunk
    auto idat_chunk = create_png_chunk("IDAT", compressed);
    png_data.insert(png_data.end(), idat_chunk.begin(), idat_chunk.end());
    
    // IEND chunk
    auto iend_chunk = create_png_chunk("IEND", {});
    png_data.insert(png_data.end(), iend_chunk.begin(), iend_chunk.end());
    
    file.write(reinterpret_cast<const char*>(png_data.data()), png_data.size());
    return file.good();
}

std::vector<uint8_t> AvatarGenerator::generate(const std::string& input) {
    // Compute MD5 hash
    auto hash = MD5::hash(input);
    
    // Get color from hash
    uint32_t color = get_color(hash);
    uint8_t cr = (color >> 16) & 0xff;
    uint8_t cg = (color >> 8) & 0xff;
    uint8_t cb = color & 0xff;
    
    // Generate pattern grid
    auto grid = generate_grid(hash);
    
    // Calculate cell size
    int cell_size = size_ / grid_size_;
    int actual_size = cell_size * grid_size_;
    
    // Create pixel buffer (RGB)
    std::vector<uint8_t> pixels(actual_size * actual_size * 3);
    
    // Fill pixels
    for (int gy = 0; gy < grid_size_; gy++) {
        for (int gx = 0; gx < grid_size_; gx++) {
            bool filled = grid[gy][gx];
            
            for (int cy = 0; cy < cell_size; cy++) {
                for (int cx = 0; cx < cell_size; cx++) {
                    int px = gx * cell_size + cx;
                    int py = gy * cell_size + cy;
                    int idx = (py * actual_size + px) * 3;
                    
                    if (filled) {
                        pixels[idx] = cr;
                        pixels[idx + 1] = cg;
                        pixels[idx + 2] = cb;
                    } else {
                        pixels[idx] = bg_r_;
                        pixels[idx + 1] = bg_g_;
                        pixels[idx + 2] = bg_b_;
                    }
                }
            }
        }
    }
    
    return pixels;
}

bool AvatarGenerator::generate_to_file(const std::string& input, const std::string& filename) {
    auto pixels = generate(input);
    int cell_size = size_ / grid_size_;
    int actual_size = cell_size * grid_size_;
    return write_png(filename, pixels, actual_size, actual_size);
}

} // namespace hashface