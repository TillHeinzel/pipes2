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
  struct ViewWrapper
  {
    R r;

    auto begin() { return r.begin(); }

    auto end() { return r.end(); }

    using value_type = std::ranges::range_value_t<R>;
  };

  template<class T>
  ViewWrapper(T const&) -> ViewWrapper<T const&>;

  template<class T>
  ViewWrapper(T&) -> ViewWrapper<T const&>;

  template<class T>
  ViewWrapper(T&&) -> ViewWrapper<T>;

  template<class R>
  using value_t = typename R::value_type;

  template<class>
  struct is_viewWrapper : std::false_type
  {
  };

  template<class R>
  struct is_viewWrapper<ViewWrapper<R>> : std::true_type
  {
  };

  template<class X>
  concept IsViewWrapper = is_viewWrapper<X>::value;
} // namespace pipes::detail

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
  struct Combinations
  {
    std::vector<int> r;

    void push(SinkFor<int, int> auto& sink) {}
  };
} // namespace pipes::detail
