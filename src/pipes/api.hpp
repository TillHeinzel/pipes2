#pragma once

#include "detail/AddAll.hpp"
#include "detail/AddEach.hpp"
#include "detail/Adjacent.hpp"
#include "detail/Combinations.hpp"
#include "detail/Discard.hpp"
#include "detail/DropTake.hpp"
#include "detail/Filter.hpp"
#include "detail/Flatten.hpp"
#include "detail/ForEach.hpp"
#include "detail/Fork.hpp"
#include "detail/FromStream.hpp"
#include "detail/GenericSink.hpp"
#include "detail/GenericSource.hpp"
#include "detail/IteratorSink.hpp"
#include "detail/MixIn.hpp"
#include "detail/OutputIterator.hpp"
#include "detail/Partition.hpp"
#include "detail/Reduce_each.hpp"
#include "detail/RemoveElement.hpp"
#include "detail/Stride.hpp"
#include "detail/Switch.hpp"
#include "detail/Tee.hpp"
#include "detail/ToStream.hpp"
#include "detail/Transform.hpp"
#include "detail/rangeSinks.hpp"

namespace pipes::detail
{
  auto negate(auto f)
  {
    return [f](auto&&... ts) mutable PIPES_RETURN(!f(PIPES_FWD(ts)...));
  }
} // namespace pipes::detail

namespace pipes::detail::api
{
  auto for_each(std::ranges::range auto&& r)
  {
    return source(ForEach{ViewWrapper{PIPES_FWD(r)}});
  }

  auto zip(std::ranges::range auto&& r, std::ranges::range auto&&... rs)
  {
    return (source(ForEach{ViewWrapper{PIPES_FWD(r)}}) + ...
            + pipe(AddEach{ViewWrapper{PIPES_FWD(rs)}}));
  }

  auto adjacent(std::ranges::range auto&& r)
  {
    return source(AdjacentSource{ViewWrapper{PIPES_FWD(r)}});
  }

  auto cartesian_product(std::ranges::range auto&& r,
                         std::ranges::range auto&&... rs)
  {
    return (source(ForEach{ViewWrapper{PIPES_FWD(r)}}) + ...
            + pipe(AddAll{ViewWrapper{PIPES_FWD(rs)}}));
  }

  auto combinations(std::ranges::range auto&& r)
  {
    return source(Combinations{ViewWrapper{PIPES_FWD(r)}});
  }

  auto interleave(std::ranges::range auto&& r, std::ranges::range auto&&... rs)
  {
    constexpr auto impl =
      []<std::size_t... Is>(std::index_sequence<Is...>, auto&& r, auto&&... rs)
    {
      return (source(ForEach{ViewWrapper{PIPES_FWD(r)}}) + ...
              + pipe(MixIn{ViewWrapper{PIPES_FWD(rs)}, Is + 1}));
    };

    return impl(std::index_sequence_for<decltype(rs)...>{},
                PIPES_FWD(r),
                PIPES_FWD(rs)...);
  }

  inline auto output_iterator() { return OutputIteratorSection{}; }

  auto generic_source(auto f) { return source(GenericSource{f}); }

  template<class T = char>
  auto from_stream(std::istream& stream)
  {
    return source(FromStream<T>{stream});
  }
} // namespace pipes::detail::api

namespace pipes::detail::api
{
  auto transform(auto f) { return pipe(Transform{f}); }

  auto filter(auto f) { return pipe(Filter{f}); }

  auto reduce_each(auto f, auto initial)
  {
    return transform(reduce_f(f, initial));
  };

  auto reduce_each(auto f) { return transform(reduce_f(f)); };

  auto drop_until(auto f) { return filter(fulfilledOnce(f)); }

  auto drop_while(auto f) { return drop_until(negate(f)); }

  auto drop(std::size_t count) { return drop_until(invokedTimes(count)); }

  auto take_until(auto f) { return filter(negate(fulfilledOnce(f))); }

  auto take_while(auto f) { return take_until(negate(f)); }

  auto take(std::size_t count) { return take_until(invokedTimes(count)); }

  auto stride(std::size_t step, std::size_t offset = 0)
  {
    return pipe(Filter{stride_f(step, offset)});
  }

  inline auto flatten() { return pipe(Flatten{}); }

  auto tee(UsableAsSink auto&& v) { return pipe(Tee{useAsSink(PIPES_FWD(v))}); }

  auto fork(UsableAsSink auto&&... s)
  {
    return sink(Fork{std::tuple{useAsSink(PIPES_FWD(s))...}});
  }

  auto partition(auto&& f,
                 UsableAsSink auto&& ifTrue,
                 UsableAsSink auto&& ifFalse)
  {
    return sink(Partition{PIPES_FWD(f),
                          useAsSink(PIPES_FWD(ifTrue)),
                          useAsSink(PIPES_FWD(ifFalse))});
  }

  constexpr auto default_ = CaseSection{[](auto&&...) { return true; }};

  auto case_(auto f) { return CaseSection{f}; }

  auto take_if(auto f, UsableAsSink auto&& r)
  {
    return pipe(TakeIf{f, useAsSink(PIPES_FWD(r))});
  }

  template<class F, class S>
  auto switch_(CaseSink<F, S> c)
  {
    return pipe(TakeIf{c.f, c.sink}) + sink(Discard{});
  }

  template<class F, class S, class... Fs, class... Ss>
  auto switch_(CaseSink<F, S> c, CaseSink<Fs, Ss>... cs)
  {
    return pipe(TakeIf{c.f, c.sink}) + switch_(PIPES_FWD(cs)...);
  }

  auto add_all(std::ranges::range auto&& r)
  {
    return pipe(AddAll{ViewWrapper{PIPES_FWD(r)}});
  }

  auto add_each(std::ranges::range auto&& r)
  {
    return pipe(AddEach{ViewWrapper{PIPES_FWD(r)}});
  }

  auto remove_element(UsableAsSink auto&& sink)
  {
    return pipe(RemoveElement{useAsSink(PIPES_FWD(sink))});
  }

  auto remove_elements(UsableAsSink auto&& sink1, UsableAsSink auto&&... sinks)
  {
    return (pipe(RemoveElement{useAsSink(PIPES_FWD(sink1))}) + ...
            + pipe(RemoveElement{useAsSink(PIPES_FWD(sinks))}));
  }

  auto unzip(UsableAsSink auto&& s) { return asSinkSection(PIPES_FWD(s)); }

  auto unzip(UsableAsSink auto&& s1, UsableAsSink auto&&... ss)
  {
    return remove_element(PIPES_FWD(s1)) + unzip(PIPES_FWD(ss)...);
  }

  auto mix_in(std::ranges::range auto&& r, std::size_t skips = 1)
  {
    return pipe(MixIn{ViewWrapper{PIPES_FWD(r)}, skips});
  }
} // namespace pipes::detail::api

namespace pipes::detail::api
{
  auto push_back(RangeAbleTo<decltype(push_back_f)> auto&& r)
  {
    return sink(ValueSink{PIPES_FWD(r), push_back_f});
  }

  auto push_front(RangeAbleTo<decltype(push_front_f)> auto&& r)
  {
    return sink(ValueSink{PIPES_FWD(r), push_front_f});
  }

  auto insert(RangeAbleTo<decltype(insert_f)> auto&& r)
  {
    return sink(ValueSink{PIPES_FWD(r), insert_f});
  }

  auto insert_or_assign(RangeAbleTo<decltype(insert_or_assign_f)> auto&& r)
  {
    return sink(ValueSink{PIPES_FWD(r), insert_or_assign_f});
  }

  auto to_iterator(std::input_or_output_iterator auto it)
  {
    return sink(IteratorSink{it});
  }

  inline auto discard() { return sink(Discard{}); }

  template<class... T>
  auto discard()
  {
    return sink(TypedDiscard<T...>{});
  }

  auto generic_sink(auto f) { return sink(GenericSink{f}); }

  template<class K, class V>
  auto map_aggregator(std::map<K, V>& m, auto f)
  {
    return sink(ValueSink{PIPES_FWD(m), map_aggregate_f(f)});
  }

  template<class K, class V>
  auto map_aggregator(std::map<K, V>&& m, auto f)
  {
    return sink(ValueSink{PIPES_FWD(m), map_aggregate_f(f)});
  }

  template<class T>
  auto set_aggregator(std::set<T>& s, auto f)
  {
    return sink(ValueSink{PIPES_FWD(s), set_aggregate_f(f)});
  }

  template<class T>
  auto set_aggregator(std::set<T>&& s, auto f)
  {
    return sink(ValueSink{PIPES_FWD(s), set_aggregate_f(f)});
  }

  inline auto to_stream(std::ostream& stream) { return sink(ToStream{stream}); }
} // namespace pipes::detail::api
