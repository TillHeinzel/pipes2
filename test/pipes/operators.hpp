#pragma once

#include <concepts>
#include <tuple>
#include <vector>

#include "Utility/RETURN.hpp"
#include "Utility/FWD.hpp"

#include "forUI.hpp"

namespace pipes::detail
{
  template<class Source, class Sink>
  auto operator>>=(Source&& source, Sink&& sink)
  {
    return link(PIPES_FWD(source), PIPES_FWD(sink));
  }
}
