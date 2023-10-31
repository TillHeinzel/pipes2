#pragma once

#include "detail/ForEach.hpp"

namespace pipes::detail::api
{
  auto for_each(std::ranges::range auto const& r) { return source(ForEach{r}); }

  auto zip(std::ranges::range auto const&... vs)
  {
    return source(MultiForEach{std::tie(vs...)});
  }

  auto generic_source(auto f) { return source(GenericSource{f}); }
} // namespace pipes::detail::api

namespace pipes::detail
{
  auto defaultSource(std::ranges::range auto const& r)
  {
    return api::for_each(r);
  }

} // namespace pipes::detail
