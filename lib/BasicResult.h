#pragma once

#include <expected>

template <class TValue, class TError>
using BasicResult = std::expected<TValue, TError>;

template <class TError>
using BadResult = std::unexpected_type<TError>;
