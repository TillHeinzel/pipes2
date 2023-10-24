#pragma once


namespace pipes::detail
{
  template<class... Ops>
  struct RawNodes
  {
    RawNodes(Ops... ops) : ops(ops...) {}
    RawNodes(std::tuple<Ops...> ops) : ops(ops) {}

    std::tuple<Ops...> ops;
  };

  template<class... Ops1, class... Ops2>
  RawNodes<Ops1..., Ops2...> operator+(RawNodes<Ops1...> lhs,
                                       RawNodes<Ops2...> rhs)
  {
    return RawNodes{std::tuple_cat(lhs.ops, rhs.ops)};
  }
} // namespace pipes
