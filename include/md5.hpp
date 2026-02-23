#ifndef MD5_HPP
#define MD5_HPP

#include <string>
#include <vector>
#include <cstdint>

namespace hashface {

/**
 * @brief MD5 hash implementation (no external dependencies)
 */
class MD5 {
public:
    MD5();
    
    /**
     * @brief Update hash with data
     */
    void update(const uint8_t* data, size_t len);
    void update(const std::string& str);
    
    /**
     * @brief Finalize and get hash
     * @return 16-byte MD5 hash
     */
    std::vector<uint8_t> finalize();
    
    /**
     * @brief Compute MD5 hash of string in one call
     */
    static std::vector<uint8_t> hash(const std::string& input);
    
    /**
     * @brief Convert hash bytes to hexadecimal string
     */
    static std::string to_hex(const std::vector<uint8_t>& hash);

private:
    uint32_t state_[4];
    uint32_t count_[2];
    uint8_t buffer_[64];
    bool finalized_;
    
    void transform(const uint8_t block[64]);
    void encode(uint8_t* output, const uint32_t* input, size_t len);
    void decode(uint32_t* output, const uint8_t* input, size_t len);
};

} // namespace hashface

#endif // MD5_HPP