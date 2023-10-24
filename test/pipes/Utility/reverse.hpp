#pragma once

#include <tuple>

namespace pipes::detail
{
  template<typename T, size_t... I>
  auto reverse_impl(T&& t, std::index_sequence<I...>)
  {
    return std::make_tuple(
      std::get<sizeof...(I) - 1 - I>(std::forward<T>(t))...);
  }

  template<typename T>
  auto reverse(T&& t)
  {
    return reverse_impl(
      std::forward<T>(t),
      std::make_index_sequence<
        std::tuple_size<std::remove_reference_t<T>>::value>());
  }
} // namespace pipes
