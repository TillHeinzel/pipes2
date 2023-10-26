#pragma once

#include <vector>

#include "../GenericImplementation/impl.hpp"

#include "PushBack.hpp"

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
    return api::push_back(v);
  }

  template<class S>
  concept SinkSectionable = requires(S&& s) { asSinkSection(PIPES_FWD(s)); };

} // namespace pipes::detail