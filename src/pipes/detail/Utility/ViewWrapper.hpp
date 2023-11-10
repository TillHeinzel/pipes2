#pragma once

#include <ranges>

namespace pipes::detail
{
  template<std::ranges::range R>
  struct ViewWrapper
  {
    R r;

    auto begin() { return r.begin(); }

    auto end() { return r.end(); }

    using value_type = std::ranges::range_value_t<R>;

    template<class T>
      requires(std::is_lvalue_reference_v<R>)
    static auto const& fwd(T& x) { return x; }

    template<class T>
      requires(!std::is_lvalue_reference_v<R>)
    static auto fwd(T& x) { return std::move(x); }
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
