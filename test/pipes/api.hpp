#pragma once

#include "detail/Discard.hpp"
#include "detail/Drop.hpp"
#include "detail/Filter.hpp"
#include "detail/ForEach.hpp"
#include "detail/Fork.hpp"
#include "detail/PushBack.hpp"
#include "detail/Transform.hpp"

#include "apihelpers.hpp"

namespace pipes::detail::api
{
  auto for_each(std::ranges::range auto const& r) { return source(ForEach{r}); }

  auto zip(std::ranges::range auto const&... vs)
  {
    return source(MultiForEach{std::tie(vs...)});
  }
} // namespace pipes::detail::api

namespace pipes::detail::api
{
  auto fork(UsableAsSink auto&&... s)
  {
    return sink(Fork{std::tuple{useAsSink(PIPES_FWD(s))...}});
  }

  auto transform(auto f) { return pipe(Transform{f}); }

  auto filter(auto f) { return pipe(Filter{f}); }

  auto drop_until(auto f) { return filter(fulfilledOnce(f)); }

  auto drop_while(auto f) { return drop_until(negate(f)); }

  auto drop(std::size_t count) { return drop_until(invokedTimes(count)); }

} // namespace pipes::detail::api

namespace pipes::detail::api
{
  auto push_back(PushBackAble auto& v) { return make_push_back_sink(v); }

  auto discard() { return sink(Discard{}); }

  template<class... T>
  auto discard()
  {
    return sink(TypedDiscard<T...>{});
  }
} // namespace pipes::detail::api
