#pragma once

#include <concepts>
#include <tuple>
#include <vector>

#include "Utility/RETURN.hpp"
#include "Utility/FWD.hpp"

#include "forUI.hpp"

namespace pipes::detail
{
  template<class SourceSection, class SinkSection>
  auto operator>>=(SourceSection&& source, SinkSection&& sink)
  {
    return link(PIPES_FWD(source), PIPES_FWD(sink));
  }
}
