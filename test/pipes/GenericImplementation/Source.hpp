#pragma once

#include <tuple>

#include "../Utility/FWD.hpp"
#include "../Utility/RETURN.hpp"

#include "Node.hpp"
#include "Sink.hpp"

namespace pipes::detail
{
  template<typename S>
  concept Source = requires(S) { typename S::OutputType; };

  struct DummySink
  {
    void push(auto const&...) {}
  };

  template<typename Chain, typename T>
  concept ValidChainFor =
    SinkFor<decltype(connect_to_sink(std::declval<Chain>(), DummySink{})), T>;

  template<typename S, typename Ops>
  concept ValidSource = Source<S> && ValidChainFor<Ops, typename S::OutputType>;

  template<Source Root, class... Ops>
  struct SourceSection
  {
    SourceSection(Root source, Section<Ops...> ops)
      requires(ValidSource<Root, Section<Ops...>>)
      : source(source), ops(ops)
    {
    }

    SourceSection(Root source) : root(source), ops() {}

    Root source;
    Section<Ops...> ops;
  };

} // namespace pipes::detail
