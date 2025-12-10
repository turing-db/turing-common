#include "BioAssert.h"

#include "FatalException.h"

void __bioAssertWithLocation(const char* file,
                             unsigned line,
                             const char* expr,
                             std::string&& msg) {
    throw FatalException(fmt::format("Internal Error: The assertion '{}' failed at {}:{}\n{}\n", expr, file, line, msg));
}

