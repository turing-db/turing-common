#include "MermaidLink.h"

#include <vector>
#include <stdint.h>
#include <string.h>

#include "Base64.h"

#include "FatalException.h"

using Bytes = std::vector<uint8_t>;

namespace {

constexpr const char* BASE_MERMAID_PAKO_URL = "http://mermaid.live/view#pako:";

void pakoDeflate(const std::string& input, Bytes& bytes) {
    const uint8_t* inputData = (const uint8_t*)input.data();
    const size_t inputSize = input.size();

    bytes.clear();

    z_stream stream;
    stream.zalloc = Z_NULL;
    stream.zfree = Z_NULL;
    stream.opaque = Z_NULL;

    // level=9, method=DEFLATED, windowBits=15, memLevel=8, strategy=Z_DEFAULT_STRATEGY
    const int initRes = deflateInit2(&stream,
                                     9,                    // compression level
                                     Z_DEFLATED,           // method
                                     15,                   // windowBits
                                     8,                    // memLevel
                                     Z_DEFAULT_STRATEGY);  // strategy
    if (initRes != Z_OK) {
        throw FatalException("deflateInit2 failed");
    }

    stream.avail_in = inputSize;
    stream.next_in = const_cast<Bytef*>(inputData);

    constexpr size_t CHUNK_SIZE = 16384;
    uint8_t out[CHUNK_SIZE];
    memset(out, 0, CHUNK_SIZE);

    do {
        stream.avail_out = CHUNK_SIZE;
        stream.next_out = out;

        const int deflateRes = deflate(&stream, Z_FINISH);
        if (deflateRes == Z_STREAM_ERROR) {
            deflateEnd(&stream);
            throw FatalException("deflate failed");
        }

        const size_t have = CHUNK_SIZE - stream.avail_out;
        bytes.insert(bytes.end(), out, out + have);

    } while (stream.avail_out == 0);

    const int deflateEndRes = deflateEnd(&stream);
    if (deflateEndRes == Z_STREAM_ERROR) {
        throw FatalException("deflate did not complete");
    }
}

void appendEscapedString(const std::string& input, std::string& output) {
    char buf[7];
    for (char c : input) {
        switch (c) {
            case '"':  output += "\\\""; break;
            case '\\': output += "\\\\"; break;
            case '\b': output += "\\b"; break;
            case '\f': output += "\\f"; break;
            case '\n': output += "\\n"; break;
            case '\r': output += "\\r"; break;
            case '\t': output += "\\t"; break;
            default:
                if (c < 32) {
                    buf[0] = '\0';
                    snprintf(buf, sizeof(buf), "\\u%04x", c);
                    output += buf;
                } else {
                    output += c;
                }
        }
    }
}

void createJsonObject(const std::string& input, std::string& json) {
    json = "{\"code\": \"";
    appendEscapedString(input, json);
    json.append("\", \"mermaid\": {\"theme\": \"default\"}}");
}

}

// Translated into C++ from python based on https://github.com/mermaid-js/mermaid-live-editor/discussions/1291
void MermaidLink::makeLink(const std::string& input, std::string& link) {
    std::string json;
    createJsonObject(input, json);

    Bytes compressed;
    pakoDeflate(json, compressed);

    link = BASE_MERMAID_PAKO_URL;
    Base64::encode(compressed, link);
}
