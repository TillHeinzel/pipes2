#pragma once

#include <iostream>

#include "pipes/pipes.hpp"

template<typename T>
concept Streamable = requires(std::ostream& os, T value) {
  {
    os << value
  } -> std::convertible_to<std::ostream&>;
};

template<class Tuple, size_t... Is>
std::ostream& println_tuple_impl(std::ostream& os,
                                 Tuple tuple,
                                 std::index_sequence<Is...>)
{
  return ((os << std::get<Is>(tuple)
              << (Is != std::tuple_size_v<Tuple> - 1 ? "," : "")),
          ...);
}

namespace std
{
  template<pipes::detail::Tuple_like Tup>
  std::ostream& operator<<(std::ostream& stream, Tup const& t)
  {
    stream << "<";
    println_tuple_impl(stream,
                       t,
                       std::make_index_sequence<std::tuple_size_v<Tup>>{});
    stream << ">";
    return stream;
  }

  template<std::ranges::range R>
  std::ostream& operator<<(std::ostream& stream, R const& v)
    requires(Streamable<std::ranges::range_value_t<R>>
             && !std::same_as<R, std::string>)

  {
    using namespace std::literals::string_literals;

    stream << "{"s;
    if(!v.empty())
    {
      stream << *v.begin();

      for(auto it = std::next(v.begin()); it != v.end(); ++it)
      {
        stream << ", "s << *it;
      }
    }
    stream << "}"s;
    return stream;
  }
} // namespace std
