#pragma once

#include <ranges>

#include "detail/ForEach.hpp"
#include "detail/FromStream.hpp"

namespace pipes::detail::api
{
  auto for_each(std::ranges::range auto&& r)
  {
    return source(ForEach{ViewWrapper{PIPES_FWD(r)}});
  }

  auto zip(std::ranges::range auto&& r, std::ranges::range auto&&... rs)
  {
    return (source(ForEach{ViewWrapper{PIPES_FWD(r)}}) + ...
            + pipe(AddEach{ViewWrapper{PIPES_FWD(rs)}}));
  }

  auto adjacent(std::ranges::range auto&& r)
  {
    return source(AdjacentSource{ViewWrapper{PIPES_FWD(r)}});
  }

  auto cartesian_product(std::ranges::range auto&& r,
                         std::ranges::range auto&&... rs)
  {
    return (source(ForEach{ViewWrapper{PIPES_FWD(r)}}) + ...
            + pipe(AddAll{ViewWrapper{PIPES_FWD(rs)}}));
  }

  auto combinations(std::ranges::range auto&& r)
  {
    return source(Combinations{ViewWrapper{PIPES_FWD(r)}});
  }
} // namespace pipes::detail::api

namespace pipes::detail::api
{
  auto generic_source(auto f) { return source(GenericSource{f}); }
} // namespace pipes::detail::api

namespace pipes::detail::api
{
  template<class T = char>
  auto from_stream(std::istream& stream)
  {
    return source(FromStream<T>{stream});
  }
} // namespace pipes::detail::api

namespace pipes::detail
{
  auto defaultSource(std::ranges::range auto&& r)
  {
    return api::for_each(PIPES_FWD(r));
  }

} // namespace pipes::detail
