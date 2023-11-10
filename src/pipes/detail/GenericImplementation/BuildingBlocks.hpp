#pragma once

#include <tuple>

namespace pipes::detail
{
  template<class... Pieces>
  struct Section
  {
    std::tuple<Pieces...> pieces;
  };

  template<class Sink_, class... Ops>
  struct SinkSection
  {
    Sink_ finalSink;
    Section<Ops...> pipe;
  };

  template<class Source_, class... Ops>
  struct SourceSection
  {
    Source_ source;
    Section<Ops...> pipe;
  };

  template<class Source_, class Sink_, class... Ops>
  struct Pipeline
  {
    Source_ source;
    Sink_ sink;
    Section<Ops...> pipe;
  };
} // namespace pipes::detail

namespace pipes::detail
{
  template<class... T1s, class... T2s>
  auto operator+(Section<T1s...> lhs, Section<T2s...> rhs)
  {
    return Section{
      std::tuple_cat(std::move(lhs).pieces, std::move(rhs).pieces)};
  }

  template<class... T1s, class... T2s>
  auto operator+(Section<T1s...> pipe, SinkSection<T2s...> sink)
  {
    return SinkSection{std::move(sink).finalSink,
                       std::move(pipe) + std::move(sink).pipe};
  }

  template<class... T1s, class... T2s>
  auto operator+(SourceSection<T1s...> source, Section<T2s...> pipe)
  {
    return SourceSection{std::move(source).source,
                         std::move(source).pipe + std::move(pipe)};
  }

  template<class... T1s, class... T2s>
  auto operator+(SourceSection<T1s...> source, SinkSection<T2s...> sink)
  {
    return Pipeline{std::move(source).source,
                    std::move(sink).finalSink,
                    std::move(source).pipe + std::move(sink).pipe};
  }
} // namespace pipes::detail
