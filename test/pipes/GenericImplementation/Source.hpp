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

  template<class Source_, class... Ops>
    requires(ValidSource<Source_, Section<Ops...>>)
  struct SourceSection
  {
    Source_ source;
    Section<Ops...> ops;
  };

} // namespace pipes::detail
