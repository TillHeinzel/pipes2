#pragma once

#include "BuildingBlocks.hpp"

#include "ConnectedSinkSection.hpp"

namespace pipes::detail
{
  template<class X>
  struct DefaultSinkFor;

} // namespace pipes::detail

namespace pipes::detail
{
  template<class... Ts>
  auto asSinkSection(SinkSection<Ts...> s)
  {
    return s;
  }

  template<class T>
  auto asSinkSection(T&& t) PIPES_RETURN(
    DefaultSinkFor<std::remove_reference_t<T>>::make(PIPES_FWD(t)));
} // namespace pipes::detail

namespace pipes::detail
{
  template<class S>
  concept UsableAsSink = requires(S&& s) { asSinkSection(PIPES_FWD(s)); };

  auto useAsSink(UsableAsSink auto&& s)
  {
    return [](auto s) {
      return connect_to_sink(s.pipe, s.finalSink);
    }(asSinkSection(PIPES_FWD(s)));
  }
} // namespace pipes::detail

namespace pipes::detail
{
  template<class X>
  struct DefaultSourceFor;
} // namespace pipes::detail

namespace pipes::detail
{
  template<class... Ts>
  auto asSourceSection(SourceSection<Ts...> s)
  {
    return s;
  }

  template<class T>
  auto asSourceSection(T&& t) PIPES_RETURN(
    DefaultSourceFor<std::remove_reference_t<T>>::make(PIPES_FWD(t)));
} // namespace pipes::detail

namespace pipes::detail
{
  template<class S>
  concept UsableAsSource = requires(S&& s) { asSourceSection(PIPES_FWD(s)); };
} // namespace pipes::detail
