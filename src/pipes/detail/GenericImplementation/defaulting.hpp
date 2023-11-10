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
  template<class T>
  concept DefaultSinkAble = requires(T t) {
    DefaultSinkFor<std::remove_reference_t<T>>::make(PIPES_FWD(t));
  };

  auto defaultSink(DefaultSinkAble auto&& s)
  {
    return DefaultSinkFor<std::remove_reference_t<decltype(s)>>::make(
      PIPES_FWD(s));
  }
} // namespace pipes::detail

namespace pipes::detail
{
  template<class... Ts>
  auto asSinkSection(SinkSection<Ts...> s)
  {
    return s;
  }

  template<DefaultSinkAble T>
  auto asSinkSection(T&& t)
  {
    return defaultSink(PIPES_FWD(t));
  }
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
  template<class T>
  concept DefaultSourceAble = requires(T t) {
    DefaultSourceFor<std::remove_reference_t<T>>::make(PIPES_FWD(t));
  };

  auto defaultSource(DefaultSourceAble auto&& s)
  {
    return DefaultSourceFor<std::remove_reference_t<decltype(s)>>::make(
      PIPES_FWD(s));
  }
} // namespace pipes::detail

namespace pipes::detail
{
  template<class... Ts>
  auto asSourceSection(SourceSection<Ts...> s)
  {
    return s;
  }

  template<DefaultSourceAble T>
  auto asSourceSection(T&& t)
  {
    return defaultSource(PIPES_FWD(t));
  }
} // namespace pipes::detail

namespace pipes::detail
{
  template<class S>
  concept UsableAsSource = requires(S&& s) { asSourceSection(PIPES_FWD(s)); };
} // namespace pipes::detail
