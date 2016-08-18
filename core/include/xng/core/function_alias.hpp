#pragma once

#include <utility>

#define XNG_CORE_FUNCTION_ALIAS(Function, Alias) template <typename ... Args> auto Alias(Args && ... args) -> decltype(Function(std::forward<Args>(args) ...)) { return Function(std::forward<Args>(args) ...); }