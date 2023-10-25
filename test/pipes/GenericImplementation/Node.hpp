#pragma once

#include "../Utility/RETURN.hpp"

namespace pipes::detail
{
  template<typename O, class Next, class... T>
  concept Operation = requires(O op, Next& next, T... t) { op.push(next, t...); };

  template<class Op, class Next>
  struct FlowSection
  {
    Op op;
    Next next;

    template<class... Ts>
      //requires(Operation<Op, Next, Ts...>)
    auto operator()(Ts&&... ts) PIPES_RETURN(op.push(next, PIPES_FWD(ts)...));

    //template<class... Ts>
    //  requires(!Operation<Op, Next, Ts...>)
    //auto operator()(Ts&&... ts) PIPES_RETURN(op.push(next, PIPES_FWD(ts)...));

    template<class... Ts>
    auto push(Ts&&... ts) PIPES_RETURN((*this)(PIPES_FWD(ts)...));

    template<class... Ts>
    auto push(std::tuple<Ts...> ts) PIPES_RETURN(std::apply(*this, ts));
  };
} // namespace pipes::detail

#include "../Utility/reverse.hpp"
#include "RawNodes.hpp"

namespace pipes::detail
{
  auto connect_to_sink_impl(auto s) -> decltype(s) { return s; }

  template<class Piece, class... Pieces>
  auto connect_to_sink_impl(auto sink, Piece piece, Pieces... pieces)
    PIPES_RETURN(connect_to_sink_impl(FlowSection{piece, sink}, pieces...));

  auto connect_to_sink_f(auto s)
  {
    return [s](auto... n) { return connect_to_sink_impl(s, n...); };
  }

  template<class... Pieces>
  auto connect_to_sink(Section<Pieces...> pipe, auto sink)
    PIPES_RETURN(std::apply(connect_to_sink_f(sink), reverse(pipe.pieces)));
} // namespace pipes::detail
