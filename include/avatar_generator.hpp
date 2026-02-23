#ifndef AVATAR_GENERATOR_HPP
#define AVATAR_GENERATOR_HPP

#include <string>
#include <vector>
#include <cstdint>

namespace hashface {

/**
 * @brief GitHub-style avatar generator
 * 
 * Generates identicon-style avatars similar to GitHub's default avatars.
 * Uses MD5 hash of input string to create deterministic patterns.
 */
class AvatarGenerator {
public:
    /**
     * @brief Construct a new Avatar Generator
     * @param size Output image size in pixels (default 420)
     * @param grid_size Grid size for pattern (default 5x5)
     */
    explicit AvatarGenerator(int size = 420, int grid_size = 5);
    
    /**
     * @brief Generate avatar from input string
     * @param input String to hash (e.g., username, email)
     * @return Vector of bytes representing PNG image data
     */
    std::vector<uint8_t> generate(const std::string& input);
    
    /**
     * @brief Generate avatar and save to file
     * @param input String to hash
     * @param filename Output filename (should end with .png)
     * @return true on success, false on failure
     */
    bool generate_to_file(const std::string& input, const std::string& filename);
    
    /**
     * @brief Set custom background color
     * @param r Red component (0-255)
     * @param g Green component (0-255)
     * @param b Blue component (0-255)
     */
    void set_background_color(uint8_t r, uint8_t g, uint8_t b);
    
private:
    int size_;
    int grid_size_;
    uint8_t bg_r_, bg_g_, bg_b_;
    
    /**
     * @brief Get color from hash bytes
     * @param hash MD5 hash bytes
     * @return RGB color packed into uint32_t
     */
    uint32_t get_color(const std::vector<uint8_t>& hash);
    
    /**
     * @brief Generate pixel grid from hash
     * @param hash MD5 hash bytes
     * @return 2D grid of boolean values (true = colored, false = background)
     */
    std::vector<std::vector<bool>> generate_grid(const std::vector<uint8_t>& hash);
    
    /**
     * @brief Write PNG file
     * @param filename Output filename
     * @param pixels Pixel data (RGB)
     * @param width Image width
     * @param height Image height
     * @return true on success
     */
    bool write_png(const std::string& filename, 
                   const std::vector<uint8_t>& pixels,
                   int width, int height);
};

} // namespace hashface

#endif // AVATAR_GENERATOR_HPP