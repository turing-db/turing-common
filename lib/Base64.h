#pragma once


#include <cstdint>
#include <string>
#include <vector>
class Base64 {
private:
    // Encoding table - aligned for cache efficiency
    static constexpr char encode_table[64] = {
        'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
        'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
        'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
        'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
        'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
        'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
        'w', 'x', 'y', 'z', '0', '1', '2', '3',
        '4', '5', '6', '7', '8', '9', '+', '/'};

    // Decode table - 256 entries for direct lookup, invalid chars = 255
    static constexpr uint8_t decode_table[256] = {
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 62, 255, 255, 255, 63,
        52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 255, 255, 255, 254, 255, 255,
        255, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
        15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 255, 255, 255, 255, 255,
        255, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
        41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255};

public:
    // Calculate exact output size for encoding
    static constexpr std::size_t encode_size(const std::size_t input_len) noexcept {
        return ((input_len + 2) / 3) * 4;
    }

    // Calculate maximum possible output size for decoding (actual may be less due to padding)
    static constexpr std::size_t decode_max_size(const std::size_t input_len) noexcept {
        return (input_len / 4) * 3;
    }

    // High-performance encoder - processes 3 bytes at a time with 64-bit operations
    static std::string encode(const uint8_t* data, const std::size_t len) {
        if (len == 0) {
            return "";
        }

        std::string result;
        result.reserve(encode_size(len));

        const uint8_t* end = data + len;
        const uint8_t* chunk_end = data + (len / 3) * 3; // Process full 3-byte chunks

        // Process 3-byte chunks using 64-bit operations
        while (data < chunk_end) {
            // Load 3 bytes into a 32-bit value (big-endian style)
            uint32_t chunk = (static_cast<uint32_t>(data[0]) << 16) |
                           (static_cast<uint32_t>(data[1]) << 8) |
                           static_cast<uint32_t>(data[2]);

            // Extract 4 x 6-bit values and encode
            result += encode_table[(chunk >> 18) & 0x3F];
            result += encode_table[(chunk >> 12) & 0x3F];
            result += encode_table[(chunk >> 6) & 0x3F];
            result += encode_table[chunk & 0x3F];

            data += 3;
        }

        // Handle remaining 1-2 bytes
        if (data < end) {
            uint32_t chunk = static_cast<uint32_t>(data[0]) << 16;
            if (data + 1 < end) {
                chunk |= static_cast<uint32_t>(data[1]) << 8;
            }

            result += encode_table[(chunk >> 18) & 0x3F];
            result += encode_table[(chunk >> 12) & 0x3F];
            result += (data + 1 < end) ? encode_table[(chunk >> 6) & 0x3F] : '=';
            result += '=';
        }

        return result;
    }

    // Convenience overload for std::vector
    static std::string encode(const std::vector<uint8_t>& data) {
        return encode(data.data(), data.size());
    }

    // Convenience overload for string
    static std::string encode(const std::string& data) {
        return encode(reinterpret_cast<const uint8_t*>(data.data()), data.size());
    }

    // High-performance decoder with validation
    static std::vector<uint8_t> decode(const char* data, const std::size_t len) {
        if (len == 0) {
            return {};
        }
        if (len % 4 != 0) {
            return {}; // Invalid base64 length
        }

        std::vector<uint8_t> result;
        result.reserve(decode_max_size(len));

        const char* end = data + len;

        // Process 4-character chunks
        while (data < end) {
            // Load and validate 4 characters
            uint8_t a = decode_table[static_cast<uint8_t>(data[0])];
            uint8_t b = decode_table[static_cast<uint8_t>(data[1])];
            uint8_t c = decode_table[static_cast<uint8_t>(data[2])];
            uint8_t d = decode_table[static_cast<uint8_t>(data[3])];

            // Check for invalid characters (255 = invalid, 254 = padding)
            if (a == 255 || b == 255) {
                return {}; // Invalid chars in required positions
            }
            if (a == 254 || b == 254) {
                return {}; // Padding not allowed in first 2 positions
            }
            if (c == 255 || d == 255) {
                return {}; // Invalid chars anywhere
            }
            if (c == 254 && d != 254) {
                return {}; // If 3rd is padding, 4th must be too
            }
            if (d == 254 && c != 254 && data < end - 4) {
                return {}; // Padding only allowed at very end
            }


            // Combine 4 x 6-bit values into 24-bit chunk
            uint32_t chunk = (static_cast<uint32_t>(a) << 18) |
                           (static_cast<uint32_t>(b) << 12) |
                           (static_cast<uint32_t>(c & 0x3F) << 6) |
                           static_cast<uint32_t>(d & 0x3F);

            // Extract bytes based on padding
            result.push_back((chunk >> 16) & 0xFF);

            if (c != 254) { // Not padding
                result.push_back((chunk >> 8) & 0xFF);
                if (d != 254) { // Not padding
                    result.push_back(chunk & 0xFF);
                }
            }

            data += 4;
        }

        return result;
    }

    // Convenience overload for std::string
    static std::vector<uint8_t> decode(const std::string& data) {
        return decode(data.data(), data.size());
    }

    // Decode to string (useful for text data)
    static std::string decode_to_string(const std::string& data) {
        auto bytes = decode(data);
        return std::string(bytes.begin(), bytes.end());
    }

    // Validate base64 string without decoding
    static bool is_valid(const char* data, std::size_t len) noexcept {
        if (len % 4 != 0) {
            return false;
        }

        int padding = 0;
        for (std::size_t i = 0; i < len; ++i) {
            uint8_t c = static_cast<uint8_t>(data[i]);
            if (decode_table[c] == 255) {
                return false;
            }
            if (decode_table[c] == 254) { // Padding character
                padding++;
                if (padding > 2 || i < len - 2) {
                    return false; // Padding only at end, max 2
                }
            } else if (padding > 0) {
                return false; // Non-padding after padding
            }
        }
        return true;
    }
};
