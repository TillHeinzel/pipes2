#pragma once

#include "Node.hpp"
#include "RawNodes.hpp"
#include "Sink.hpp"
#include "Source.hpp"
#include "SinkSection.hpp"
#include "SinkPiece.hpp"

namespace pipes::detail
{
  auto source(auto&& sourcePiece)
    PIPES_RETURN(SourceSection{PIPES_FWD(sourcePiece)});

  auto sink(auto&& sinkPiece) PIPES_RETURN(SinkSection{PIPES_FWD(sinkPiece)});

  auto pipe(auto&& op) PIPES_RETURN(Section{PIPES_FWD(op)});
} // namespace pipes::detail