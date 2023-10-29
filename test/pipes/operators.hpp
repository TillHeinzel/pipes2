#pragma once

#include <concepts>
#include <tuple>
#include <vector>

#include "detail/Utility/RETURN.hpp"
#include "detail/Utility/FWD.hpp"

#include "forUI.hpp"

namespace pipes::detail
{
  template<class Source, class Sink>
  decltype(auto) operator>>=(Source&& source, Sink&& sink)
  {
    return link(PIPES_FWD(source), PIPES_FWD(sink));
  }  
}
