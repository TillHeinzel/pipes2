#pragma once

#include "BuildingBlocks.hpp"
#include "ConnectedSinkSection.hpp"
#include "defaulting.hpp"

namespace pipes::detail
{
  auto source(auto&& sourcePiece) //
    PIPES_RETURN(SourceSection{PIPES_FWD(sourcePiece)});

  auto sink(auto&& sinkPiece) //
    PIPES_RETURN(SinkSection{PIPES_FWD(sinkPiece)});

  auto pipe(auto&& op) //
    PIPES_RETURN(Section{std::tuple{PIPES_FWD(op)}});
} // namespace pipes::detail
