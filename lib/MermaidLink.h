#pragma once

#include <string>

#include <zlib.h>

class MermaidLink {
public:
    static void makeLink(const std::string& input, std::string& link);
};
