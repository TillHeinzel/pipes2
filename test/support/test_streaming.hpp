#pragma once

#include <iostream>
#include <variant>

#include "pipes/pipes.hpp"

namespace pipes::detail
{
  template<class X>
  struct is_tuple : std::false_type
  {
  };

  template<class... Ts>
  struct is_tuple<std::tuple<Ts...>> : std::true_type
  {
  };

  template<class Tup>
  constexpr static bool is_tuple_v = is_tuple<Tup>::value;

  template<class X>
  struct is_pair : std::false_type
  {
  };

  template<class T1, class T2>
  struct is_pair<std::pair<T1, T2>> : std::true_type
  {
  };

  template<class Tup>
  constexpr static bool is_pair_v = is_pair<Tup>::value;

  template<class Tup>
  concept Tuple_like = is_tuple_v<Tup> || is_pair_v<Tup>;

  template<class Tup>
  concept Pair_like =
    is_pair_v<Tup> || (is_tuple_v<Tup> && std::tuple_size_v<Tup> == 2);
} // namespace pipes::detail


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
  template<class T>
  std::ostream& operator<<(std::ostream& stream, std::unique_ptr<T> const& t)
  {
    stream << "*" << *t;
    return stream;
  }

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

template<class... Ts>
  std::ostream& operator<<(std::ostream& stream, std::variant<Ts...> const& var)
  {
    std::visit([&stream](auto const& x) { stream << x; }, var);
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
