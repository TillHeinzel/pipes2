#pragma once

#include "SinkPiece.hpp"

namespace pipes::detail
{
  template<class Sink_, class... Ops>
  struct SinkSection
  {
    SinkSection(Sink_ finalSink, Section<Ops...> ops) :
      finalSink(PIPES_FWD(finalSink)), ops(PIPES_FWD(ops))
    {
    }

    SinkSection(SinkPiece<Sink_> finalSink, Section<Ops...> ops) :
      finalSink(PIPES_FWD(finalSink)), ops(PIPES_FWD(ops))
    {
    }

    SinkSection(Sink_ finalSink) : finalSink(PIPES_FWD(finalSink)), ops() {}

    SinkSection(SinkPiece<Sink_> finalSink) :
      finalSink(PIPES_FWD(finalSink)), ops()
    {
    }

    SinkPiece<Sink_> finalSink;
    Section<Ops...> ops;
  };
} // namespace pipes::detail
