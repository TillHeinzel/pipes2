#pragma once

#include <ranges>

#include "detail/ForEach.hpp"

namespace pipes::detail::api
{
  auto for_each(std::ranges::range auto const& r) { return source(ForEach{r}); }

  auto zip(std::ranges::range auto const& r,
           std::ranges::range auto const&... rs)
  {
    return (source(ForEach{r}) + ... + pipe(AddEach{rs}));
  }

  auto generic_source(auto f) { return source(GenericSource{f}); }

  auto adjacent(std::ranges::range auto const& r)
  {
    return source(AdjacentSource{r});
  }

  auto cartesian_product(std::ranges::range auto const& r,
                         std::ranges::range auto const&... rs)
  {
    return (source(ForEach{r}) + ... + pipe(AddAll{rs}));
  }
} // namespace pipes::detail::api

namespace pipes::detail
{
  auto defaultSource(std::ranges::range auto const& r)
  {
    return api::for_each(r);
  }

} // namespace pipes::detail
