#pragma once

#include <variant>

#include "PackTypeIndex.h"

template <typename Type, typename... ArgsT>
inline constexpr auto& FastGet(std::variant<ArgsT...>& variant) {
#if defined(__clang__)
    return std::get<Type>(variant);
#else
    constexpr size_t index = PackIndexOfType<Type, ArgsT...>::get();
    return std::__detail::__variant::__get<index>(variant);
#endif
}
