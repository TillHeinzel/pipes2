#pragma once

#include <ranges>

#include "detail/ForEach.hpp"
#include "detail/FromStream.hpp"
#include "detail/MixIn.hpp"

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

  auto interleave(std::ranges::range auto&& r, std::ranges::range auto&&... rs)
  {
    constexpr auto impl =
      []<std::size_t... Is>(std::index_sequence<Is...>, auto&& r, auto&&... rs)
    {
      return (source(ForEach{ViewWrapper{PIPES_FWD(r)}}) + ...
              + pipe(MixIn{ViewWrapper{PIPES_FWD(rs)}, Is + 1}));
    };

    return impl(std::index_sequence_for<decltype(rs)...>{},
                PIPES_FWD(r),
                PIPES_FWD(rs)...);
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
