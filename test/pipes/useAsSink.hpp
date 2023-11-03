#pragma once

#include "detail/Utility/functional.hpp"

#include "detail/GenericImplementation/impl.hpp"

#include "makeSinks.hpp"
#include "makeSources.hpp"

namespace pipes::detail
{

  template<class S>
  concept HasDefaultSource = requires(S const& s) { defaultSource(s); };

  template<class S>
  concept HasDefaultSink = requires(S& s) { defaultSink(s); };

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