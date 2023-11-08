#pragma once
#include <vector>

#include <iostream>

#include "GenericImplementation/impl.hpp"

namespace pipes::detail
{
  template<class R, class F>
  concept RangeAbleTo =
    std::invocable<F,
                   std::remove_reference_t<R>&,
                   typename std::remove_reference_t<R>::value_type>;
}

namespace pipes::detail
{
  template<class R, class F>
  struct ValueSink
  {
    R r;
    F f;

    auto push(auto&& t) PIPES_RETURN(f(r, PIPES_FWD(t)));

    R value() { return r; }
  };

  template<class R, class F>
  struct ValueSink<std::reference_wrapper<R>, F>
  {
    std::reference_wrapper<R> r;
    F f;

    auto push(auto&& t) PIPES_RETURN(f(r.get(), PIPES_FWD(t)));

    R& value() { return r.get(); }
  };

  template<class T, class F>
  ValueSink(T&, F) -> ValueSink<std::reference_wrapper<T>, F>;

  template<class T, class F>
  ValueSink(T&&, F) -> ValueSink<T, F>;
} // namespace pipes::detail

namespace pipes::detail
{
  constexpr auto push_back_f =
    [](auto& r, auto&& t) PIPES_RETURN(r.push_back(PIPES_FWD(t)));

  constexpr auto push_front_f =
    [](auto& r, auto&& t) PIPES_RETURN(r.push_front(PIPES_FWD(t)));

  constexpr auto insert_f =
    [](auto& r, auto&& t) PIPES_RETURN(r.insert(PIPES_FWD(t)));

  constexpr auto insert_or_assign_f = [](auto& r, auto&& t)
    PIPES_RETURN(r.insert_or_assign(std::get<0>(t), PIPES_FWD(std::get<1>(t))));

} // namespace pipes::detail

namespace pipes::detail
{
  auto set_aggregate_f(auto f)
  {
    return [f](auto& r, auto&& t)
    {
      auto recur = [f, &r](auto&& t, auto&& recur) -> void
      {
        auto [it, didInsert] = r.insert(PIPES_FWD(t));
        if(!didInsert)
        {
          recur(f(r.extract(it).value(), PIPES_FWD(t)), recur);
        }
      };

      recur(PIPES_FWD(t), recur);
    };
  }

  auto map_aggregate_f(auto f)
  {
    return [f](auto& m, auto&& t)
    {
      auto [it, didInsert] = m.insert(PIPES_FWD(t));

      if(!didInsert)
      {
        it->second = f(it->second, PIPES_FWD(t).second);
      }
    };
  }
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
