#pragma once

#include "BuildingBlocks.hpp"
#include "ConnectedSinkSection.hpp"
#include "defaulting.hpp"

namespace pipes::detail
{
  auto source(auto&& sourcePiece) //
  {
    return SourceSection{PIPES_FWD(sourcePiece)};
  }

  auto sink(auto&& sinkPiece) //
  {
    return SinkSection{PIPES_FWD(sinkPiece)};
  }

  auto pipe(auto&& op) //
  {
    return Section{std::tuple{PIPES_FWD(op)}};
  }
} // namespace pipes::detail
