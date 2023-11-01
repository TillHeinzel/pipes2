#pragma once

#include <ranges>
#include <tuple>

#include "Utility/FWD.hpp"
#include "Utility/functional.hpp"
#include "Utility/tuples.hpp"

#include "GenericImplementation/impl.hpp"

namespace pipes::detail
{
  template<std::ranges::range R>
  struct ForEach
  {
    R const& v;

    void push(SinkFor<std::ranges::range_value_t<R>> auto& sink)
    {
      for(auto const& t : v)
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
  template<std::ranges::range R>
  struct AdjacentSource
  {
    R const& r;

    auto push(SinkFor<std::ranges::range_value_t<R>,
                      std::ranges::range_value_t<R>> auto& sink)
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
  template<std::ranges::range R>
  struct AddAll
  {
    R const& r;

    template<class... Ts>
    auto push(SinkFor<Ts..., std::ranges::range_value_t<R>> auto& sink,
              Ts&&... ts)
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
  template<std::ranges::range R>
  struct AddEach
  {
    R const& r;

    decltype(r.begin()) it = r.begin();

    template<class... Ts>
    auto push(SinkFor<Ts..., std::ranges::range_value_t<R>> auto& sink,
              Ts&&... ts)
    {
      if(it == r.end()) return;
      sink.push(PIPES_FWD(ts)..., *it++);
    }
  };
} // namespace pipes::detail
