#pragma once

#include "detail/Utility/functional.hpp"

#include "detail/ForEach.hpp"
#include "detail/PushBack.hpp"

namespace pipes::detail
{
  auto defaultSource(std::ranges::range auto const& r)
  {
    return source(ForEach{r});
  }

  template<class S>
  concept HasDefaultSource = requires(S const& s) { defaultSource(s); };
} // namespace pipes::detail

namespace pipes::detail
{

  template<class T>
  auto defaultSink(std::vector<T>& v)
  {
    return make_push_back_sink(v);
  }

  template<class S>
  concept HasDefaultSink = requires(S& s) { defaultSink(s); };

} // namespace pipes::detail

namespace pipes::detail
{
  template<class... Ts>
  auto asSinkSection(SinkSection<Ts...> s)
  {
    return s;
  }

  auto asSinkSection(auto&& t) PIPES_RETURN(defaultSink(PIPES_FWD(t)));

  template<class S>
  concept UsableAsSink = requires(S&& s) { asSinkSection(PIPES_FWD(s)); };

  auto useAsSink(UsableAsSink auto&& s)
  {
    constexpr auto connect = [](auto s)
    { return connect_to_sink(s.pipe, s.finalSink); };
    return connect(asSinkSection(PIPES_FWD(s)));
  }

} // namespace pipes::detail
