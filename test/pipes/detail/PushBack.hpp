#pragma once
#include <vector>

#include <iostream>

#include "GenericImplementation/impl.hpp"

namespace pipes::detail
{
  template<class R, class F>
  struct ReferenceSink
  {
    R& r;
    F f;

    auto push(auto&& t) PIPES_RETURN(f(r, PIPES_FWD(t)));

    R& value() { return r; }
  };

  constexpr auto push_back_f =
    [](auto& r, auto&& t) PIPES_RETURN(r.push_back(PIPES_FWD(t)));

  constexpr auto push_front_f =
    [](auto& r, auto&& t) PIPES_RETURN(r.push_front(PIPES_FWD(t)));

  constexpr auto insert_f =
    [](auto& r, auto&& t) PIPES_RETURN(r.insert(PIPES_FWD(t)));

  constexpr auto insert_or_assign_f = [](auto& r, auto&& t)
    PIPES_RETURN(r.insert_or_assign(std::get<0>(t), PIPES_FWD(std::get<1>(t))));

  template<class R, class F>
  concept RangeAbleTo = std::invocable<F, R&, typename R::value_type>;
} // namespace pipes::detail

namespace pipes::detail
{
  template<class It>
  struct IteratorSink
  {
    It it;

    template<class T>
      requires(std::output_iterator<It, T>)
    auto push(T&& t) PIPES_RETURN(*(it++) = PIPES_FWD(t));

    It value() { return it; }
  };
} // namespace pipes::detail

namespace pipes::detail
{
  template<class F>
  struct GenericSink
  {
    F f;

    auto push(auto&& t) PIPES_RETURN(f(PIPES_FWD(t)));
  };
} // namespace pipes::detail
