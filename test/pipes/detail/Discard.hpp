#pragma once

namespace pipes::detail
{
  struct Discard
  {
    void push(auto const&) {}
  };

  template<class... T>
  struct TypedDiscard
  {
    void push(T const&...) {}
  };
} // namespace pipes::detail
