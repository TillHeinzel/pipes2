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

    template<class T>
    void push(T&& t)
    {
      f(r, PIPES_FWD(t));
    }

    R value() const& { return r; }

    R value() && { return std::move(r); }
  };

  template<class R, class F>
  struct ValueSink<std::reference_wrapper<R>, F>
  {
    std::reference_wrapper<R> r;
    F f;

    template<class T>
    void push(T&& t)
    {
      f(r.get(), PIPES_FWD(t));
    }

    R& value() { return r.get(); }
  };

  template<class T, class F>
  ValueSink(T&, F) -> ValueSink<std::reference_wrapper<T>, F>;

  template<class T, class F>
  ValueSink(T&&, F) -> ValueSink<T, F>;
} // namespace pipes::detail

namespace pipes::detail
{
  constexpr auto push_back_f = [](auto& r, auto t)
  { return r.push_back(std::move(t)); };

  constexpr auto push_front_f = [](auto& r, auto t)
  { return r.push_front(std::move(t)); };

  constexpr auto insert_f = [](auto& r, auto t)
  { return r.insert(std::move(t)); };

  constexpr auto insert_or_assign_f = [](auto& r, auto t)
  { return r.insert_or_assign(std::get<0>(std::move(t)), std::get<1>(std::move(t))); };

} // namespace pipes::detail

namespace pipes::detail
{
  auto set_aggregate_f(auto f)
  {
    return [f](auto& r, auto t)
    {
      auto recur = [f, &r](auto t, auto const& recur) -> void
      {
        auto [it, didInsert] = r.insert(std::move(t));
        if(!didInsert)
        {
          recur(f(r.extract(it).value(), std::move(t)), recur);
        }
      };

      recur(std::move(t), recur);
    };
  }

  auto map_aggregate_f(auto f)
  {
    return [f](auto& m, auto t)
    {
      auto [it, didInsert] = m.insert(std::move(t));

      if(!didInsert)
      {
        it->second = f(it->second, std::move(t).second);
      }
    };
  }
} // namespace pipes::detail
