#pragma once

#include <ranges>
#include <tuple>

#include "Utility/FWD.hpp"
#include "Utility/ViewWrapper.hpp"
#include "Utility/functional.hpp"
#include "Utility/tuples.hpp"

#include "GenericImplementation/impl.hpp"

namespace pipes::detail
{
  template<IsViewWrapper R>
  struct ForEach
  {
    R r;

    void push(SinkFor<value_t<R>> auto& sink)
    {
      for(auto const& t : r)
      {
        sink.push(t);
      }
    }
  };
} // namespace pipes::detail

namespace pipes::detail
{
  template<class F>
  struct GenericSource
  {
    F f;

    auto push(auto& sink) PIPES_RETURN(f(sink));
  };
} // namespace pipes::detail

namespace pipes::detail
{
  template<IsViewWrapper R>
  struct AdjacentSource
  {
    R r;

    auto push(SinkFor<value_t<R>, value_t<R>> auto& sink)
    {
      if(r.begin() == r.end()) return;

      auto it1 = r.begin();
      auto it2 = std::next(it1);

      while(it2 != r.end())
      {
        sink.push(*it1++, *it2++);
      }
    };
  };
} // namespace pipes::detail

namespace pipes::detail
{
  template<IsViewWrapper R>
  struct AddAll
  {
    R r;

    template<class... Ts>
    auto push(SinkFor<Ts..., value_t<R>> auto& sink, Ts&&... ts)
    {
      for(const auto& x : r)
      {
        sink.push(PIPES_FWD(ts)..., x);
      }
    }
  };
} // namespace pipes::detail

namespace pipes::detail
{
  template<IsViewWrapper R>
  struct AddEach
  {
    R r;

    AddEach(R r) : r(r), it(r.begin()) {}

    AddEach(AddEach&& other) : r(std::move(other.r)), it(r.begin()) {}

    AddEach(AddEach const& other) : r(other.r), it(r.begin()) {}

    decltype(r.begin()) it = r.begin();

    template<class... Ts>
    auto push(SinkFor<Ts..., value_t<R>> auto& sink, Ts&&... ts)
    {
      if(it == r.end()) return;
      sink.push(PIPES_FWD(ts)..., *it++);
    }
  };
} // namespace pipes::detail

namespace pipes::detail
{
  template<IsViewWrapper R>
  struct Combinations
  {
    R r;

    void push(SinkFor<value_t<R>, value_t<R>> auto& sink)
    {
      if(r.begin() == r.end()) return;

      for(auto it1 = r.begin(); std::next(it1) != r.end(); ++it1)
      {
        for(auto it2 = std::next(it1); it2 != r.end(); ++it2)
        {
          sink.push(*it1, *it2);
        }
      }
    }
  };
} // namespace pipes::detail
