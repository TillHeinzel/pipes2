#pragma once

namespace pipes::detail
{
  template<class... Pieces>
  struct Section
  {
    Section(Pieces... pieces) : pieces(pieces...) {}
    Section(std::tuple<Pieces...> pieces) : pieces(pieces) {}

    std::tuple<Pieces...> pieces;
  };

  template<class... T1s, class... T2s>
  Section<T1s..., T2s...> operator+(Section<T1s...> lhs, Section<T2s...> rhs)
  {
    return Section{std::tuple_cat(lhs.pieces, rhs.pieces)};
  }
} // namespace pipes::detail
