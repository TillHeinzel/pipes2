#pragma once

#include "detail/Utility/functional.hpp"

#include "detail/PushBack.hpp"

namespace pipes::detail
{
  template<class T>
  auto make_push_back_sink(std::vector<T>& v)
  {
    return sink(PushBackSink{v});
  }

  template<class S>
  concept PushBackAble = requires(S& s) { make_push_back_sink(s); };
} // namespace pipes::detail

namespace pipes::detail
{
  template<class... Ts>
  auto connect(SinkSection<Ts...> s)
  {
    return connect_to_sink(s.pipe, s.finalSink);
  }

  template<class... Ts>
  auto asSinkSection(SinkSection<Ts...> s)
  {
    return s;
  }

  template<class T>
  auto asSinkSection(std::vector<T>& v)
  {
    return make_push_back_sink(v);
  }

  template<class S>
  concept UsableAsSink = requires(S&& s) { asSinkSection(PIPES_FWD(s)); };

  auto useAsSink(UsableAsSink auto&& s)
  {
    return connect(asSinkSection(PIPES_FWD(s)));
  }

} // namespace pipes::detail
