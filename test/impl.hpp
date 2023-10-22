#pragma once

namespace pipes
{
  template<class Op, class Next>
  struct Node
  {
    static constexpr bool isOpenSink = true;

    Op op;
    Next next;

    void push(int i) { op.push(next, i); }
  };

  template<class... Ops>
  struct RawNodes : public std::tuple<Ops...>
  {
  };
} // namespace pipes

namespace pipes
{
  template<typename T, class I>
  concept Sink = requires(T t, I i) { t.push(i); };

  template<typename T>
  concept OpenSink = requires(T) { T::isOpenSink; };
} // namespace pipes
