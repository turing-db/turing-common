#include <cstddef>
#include <utility>

#include <range/v3/view/enumerate.hpp>
#include <range/v3/view/iota.hpp>

namespace db {
    namespace rg = ranges;
    namespace rv = rg::views;

    // Enumerate from starting value @ref start (inclusive)
    template <typename Rng>
    auto EnumerateFrom(std::ptrdiff_t start, Rng&& rng) {
        return rv::zip(rv::iota(start), std::forward<Rng>(rng));
    } 
}
