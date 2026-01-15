#include <stdlib.h>
#include <string>
#include <iostream>

#include <spdlog/spdlog.h>

#include "MermaidLink.h"

int main(int argc, char** argv) {
    bool useStdInput = false;
    if (argc == 1) {
        useStdInput = true;
    }

    std::string content;

    if (useStdInput) {
        std::string line;
        while (std::cin >> line) {
            content.append(line);
        }
    }

    std::string link;
    MermaidLink::makeLink(content, link);

    std::cout << link << '\n';

    return EXIT_SUCCESS;
}
