#pragma once

#include "detail/GenericImplementation/impl.hpp"

namespace pipes::detail
{
  template<typename Source, typename Sink, typename Pipe>
  concept ValidPipeline = requires(Source source, Sink sink, Pipe pipe) {
    connectPipeline(Pipeline{source, sink, pipe}).run();
  };

  template<typename Source, typename Pipe>
  concept ValidSource = ValidPipeline<Source, DummySink, Pipe>;
} // namespace pipes::detail

namespace pipes::detail
{
  template<class... Ops1, class... Ops2>
  auto link(Section<Ops1...> ops1, Section<Ops2...> ops2)
    PIPES_RETURN(ops1 + ops2);

  template<class Source_, class... Ts, class... Ops>
    requires(ValidSource<Source_, Section<Ts..., Ops...>>)
  auto link(SourceSection<Source_, Ts...> source, Section<Ops...> laterOps)
    PIPES_RETURN(source + laterOps);

  template<class... Ops, class... Ts>
  auto link(Section<Ops...> earlierOps, SinkSection<Ts...> sink)
    PIPES_RETURN(earlierOps + sink);

  template<class Source, class... T1s, class Sink, class... T2s>
    requires(ValidPipeline<Source, Sink, Section<T1s..., T2s...>>)
  auto link(SourceSection<Source, T1s...> source,
            SinkSection<Sink, T2s...> sink)
    PIPES_RETURN(connectPipeline(source + sink).run());
} // namespace pipes::detail

#include "api.hpp"

namespace pipes::detail
{
  auto link(std::ranges::range auto const& r, auto n)
    PIPES_RETURN(link(api::for_each(r), n));

  template<class... Ops, class T>
  auto link(SourceSection<Ops...> source, std::vector<T>& p)
    PIPES_RETURN(link(source, make_push_back_sink(p)));

  template<class... Ops, class T>
  auto link(Section<Ops...> n, std::vector<T>& p)
    PIPES_RETURN(link(n, make_push_back_sink(p)));
} // namespace pipes::detail

namespace pipes::detail
{
  template<class F>
  auto link(Case<F> c, UsableAsSink auto&& s)
  {
    return CaseSink{c.f, useAsSink(PIPES_FWD(s))};
  }
} // namespace pipes::detail

namespace pipes::detail::api
{
  template<class SourceSection, class SinkSection>
  concept CanLink =
    requires(SourceSection source, SinkSection sink) { link(source, sink); };
}