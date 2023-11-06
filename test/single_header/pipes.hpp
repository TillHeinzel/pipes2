#pragma once 

//////////////////////////////////////////
//detail/Utility/FWD.hpp
//////////////////////////////////////////

#define PIPES_FWD(a) std::forward<decltype(a)>(a)
#define PIPES_LAMBDA(f) [](auto&&... ts) PIPES_RETURN(f(PIPES_FWD(ts)...))

//////////////////////////////////////////
//detail/Utility/RETURN.hpp
//////////////////////////////////////////

#define PIPES_RETURN(...)                                                         ;  ->decltype(__VA_ARGS__) { return __VA_ARGS__; }
//
//#define PIPES_REQ_RETURN(...)                                                      ;//  ->decltype(__VA_ARGS__) requires requires(__VA_ARGS__) { return __VA_ARGS__; }

//////////////////////////////////////////
//detail/Utility/functional.hpp
//////////////////////////////////////////

namespace pipes::detail
{
  auto negate(auto f)
  {
    return [f](auto&&... ts) mutable PIPES_RETURN(!f(PIPES_FWD(ts)...));
  }
} // namespace pipes::detail
namespace pipes::detail
{
  auto curry(auto f, auto val)
  {
    return
      [val, f](auto&&... ts) mutable PIPES_RETURN(f(val, PIPES_FWD(ts)...));
  }
  template<class T>
  auto curry(auto f, std::reference_wrapper<T> val_ref)
  {
    auto& val = val_ref.get();
    return
      [&val, f](auto&&... ts) mutable PIPES_RETURN(f(val, PIPES_FWD(ts)...));
  }
  template<class T>
  auto curry(auto f, std::reference_wrapper<const T> val_ref)
  {
    auto const& val = val_ref.get();
    return
      [&val, f](auto&&... ts) mutable PIPES_RETURN(f(val, PIPES_FWD(ts)...));
  }
  auto curry(auto f, auto x, auto... xs) { return curry(curry(f, x), xs...); }
} // namespace pipes::detail
namespace pipes::detail
{
  auto chain(auto&& x) { return PIPES_FWD(x); }
  auto chain(auto&& x, auto&& f)
  {
    return std::invoke(PIPES_FWD(f), PIPES_FWD(x));
  }
  auto chain(auto&& x, auto&& f1, auto&&... fs)
  {
    return chain(chain(PIPES_FWD(x), PIPES_FWD(f1)), PIPES_FWD(fs)...);
  }
} // namespace pipes::detail

//////////////////////////////////////////
//detail/Utility/tuples.hpp
//////////////////////////////////////////

#include <ranges>
#include <tuple>
namespace pipes::detail
{
  template<typename T, size_t... I>
  auto reverse_impl(T&& t, std::index_sequence<I...>)
  {
    return std::make_tuple(
      std::get<sizeof...(I) - 1 - I>(std::forward<T>(t))...);
  }
  template<typename T>
  auto reverse(T&& t)
  {
    return reverse_impl(
      std::forward<T>(t),
      std::make_index_sequence<
        std::tuple_size<std::remove_reference_t<T>>::value>());
  }
} // namespace pipes::detail
namespace pipes::detail
{
  template<class... Ts, class F>
  auto transform(std::tuple<Ts...>& ts, F const& f)
  {
    return std::apply([&f](auto&&... ts) { return std::tuple{f(ts)...}; },
                      PIPES_FWD(ts));
  }
} // namespace pipes::detail
namespace pipes::detail
{
  template<class Tuple_>
  auto for_each(auto&& f, Tuple_&& tup)
    PIPES_RETURN(std::apply([&f](auto&&... ts) { (f(ts), ...); },
                            PIPES_FWD(tup)))
} // namespace pipes::detail
namespace pipes::detail
{
  template<class It, class End>
  struct IteratorPair
  {
    It it;
    End end;
  };
  template<std::ranges::range... Rs>
  auto getIteratorPairs(std::tuple<Rs...>& rs)
  {
    constexpr auto getIteratorPair = [](auto const& v) {
      return IteratorPair{v.begin(), v.end()};
    };
    return transform(rs, getIteratorPair);
  }
  template<class... Its, class... Ends>
  void parallelIterate_impl(auto f, IteratorPair<Its, Ends>... itss)
  {
    while(!((itss.it == itss.end) || ...))
    {
      f(*itss.it...);
      (++itss.it, ...);
    }
  }
  template<class... Its, class... Ends>
  void parallelIterate(std::tuple<IteratorPair<Its, Ends>...> itss, auto f)
  {
    auto ff = [f](auto&&... itss)
    { parallelIterate_impl(f, PIPES_FWD(itss)...); };
    std::apply(ff, itss);
  }
  template<std::ranges::range... Rs>
  void parallelIterate(std::tuple<Rs...>& ranges, auto f)
  {
    parallelIterate(getIteratorPairs(ranges), f);
  }
} // namespace pipes::detail
namespace pipes::detail
{
  template<class F, class... Ts>
  void callFirstSuccess(std::tuple<Ts...> cases, F f)
  {
    auto ff = [f](auto... ts)
    {
      // short-circuits. after success, no more calls to f
      return (f(ts) || ...);
    };
    std::apply(ff, cases);
  }
} // namespace pipes::detail
namespace pipes::detail
{
  template<class Tup1, class Tup2, std::size_t... Is>
  auto tuple_zip_impl(Tup1 tup1, Tup2 tup2, std::index_sequence<Is...>)
  {
    return std::make_tuple(
      std::tuple{std::get<Is>(tup1), std::get<Is>(tup2)}...);
  }
  template<class Tup1, class Tup2>
  auto tuple_zip(Tup1 tup1, Tup2 tup2)
  {
    static_assert(std::tuple_size_v<Tup1> == std::tuple_size_v<Tup2>);
    return tuple_zip_impl(tup1,
                          tup2,
                          std::make_index_sequence<std::tuple_size_v<Tup1>>());
  }
} // namespace pipes::detail
namespace pipes::detail
{
  template<class X>
  struct is_tuple : std::false_type
  {
  };
  template<class... Ts>
  struct is_tuple<std::tuple<Ts...>> : std::true_type
  {
  };
  template<class Tup>
  constexpr static bool is_tuple_v = is_tuple<Tup>::value;
  template<class X>
  struct is_pair : std::false_type
  {
  };
  template<class T1, class T2>
  struct is_pair<std::pair<T1, T2>> : std::true_type
  {
  };
  template<class Tup>
  constexpr static bool is_pair_v = is_pair<Tup>::value;
  template<class Tup>
  concept Tuple_like = is_tuple_v<Tup> || is_pair_v<Tup>;
  template<class Tup>
  concept Pair_like =
    is_pair_v<Tup> || (is_tuple_v<Tup> && std::tuple_size_v<Tup> == 2);
} // namespace pipes::detail
namespace pipes::detail
{
  template<class... Ts>
  bool any_of(std::tuple<Ts...> ts)
  {
    return std::apply([](auto... ts) { return (ts || ...); }, ts);
  }
  template<class F, class... Ts>
  bool any_of(std::tuple<Ts...> ts, F f)
  {
    return std::apply([](auto... ts) { return (ts || ...); }, transform(ts, f));
  }
} // namespace pipes::detail

//////////////////////////////////////////
//detail/GenericImplementation/BuildingBlocks.hpp
//////////////////////////////////////////

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
    return Section{std::tuple_cat(lhs.pieces, rhs.pieces)};
  }
  template<class... T1s, class... T2s>
  auto operator+(Section<T1s...> pipe, SinkSection<T2s...> sink)
  {
    return SinkSection{sink.finalSink, pipe + sink.pipe};
  }
  template<class... T1s, class... T2s>
  auto operator+(SourceSection<T1s...> source, Section<T2s...> pipe)
  {
    return SourceSection{std::move(source.source),
                         std::move(source.pipe) + std::move(pipe)};
  }
  template<class... T1s, class... T2s>
  auto operator+(SourceSection<T1s...> source, SinkSection<T2s...> sink)
  {
    return Pipeline{source.source, sink.finalSink, source.pipe + sink.pipe};
  }
} // namespace pipes::detail

//////////////////////////////////////////
//detail/GenericImplementation/ConnectedSinkSection.hpp
//////////////////////////////////////////

namespace pipes::detail
{
  template<class X>
  concept hasValue = requires(X x) { x.value(); };
  template<class F>
  struct FlowNode
  {
    F f;
    template<class... Ts>
    auto push(Ts&&... ts) PIPES_RETURN(f(PIPES_FWD(ts)...));
    // no recursive reference here, to avoid issues with tuples of tuples being
    // unpacked
    template<class... Ts>
    auto push(std::tuple<Ts...> ts) PIPES_RETURN(std::apply(f, ts));
    template<class... Ts>
      requires(!std::regular_invocable<F, Ts...>)
    auto push(Ts&&... ts) PIPES_RETURN(f(std::tuple{PIPES_FWD(ts)...}));
    decltype(auto) value()
      requires(hasValue<F>)
    {
      return f.value();
    }
  };
  template<class S>
  struct EndNode
  {
    S s;
    auto operator()(auto&&... ts) PIPES_RETURN(s.push(PIPES_FWD(ts)...));
    decltype(auto) value()
      requires(hasValue<S>)
    {
      return s.value();
    }
  };
  template<class Next, class Op>
  struct PieceNode
  {
    Next next;
    Op op;
    auto operator()(auto&&... ts) PIPES_RETURN(op.push(next, PIPES_FWD(ts)...));
    decltype(auto) value()
      requires(hasValue<Next>)
    {
      return next.value();
    }
  };
  auto endNode(auto s) { return FlowNode{EndNode{s}}; }
  auto pieceNode(auto op, auto next)
  {
    return FlowNode{
      PieceNode{next, op}
    };
  }
} // namespace pipes::detail
namespace pipes::detail
{
  auto connect_to_sink_impl(auto s) -> decltype(s) { return s; }
  template<class Piece, class... Pieces>
  auto connect_to_sink_impl(auto sink, Piece piece, Pieces... pieces)
    PIPES_RETURN(connect_to_sink_impl(pieceNode(piece, sink), pieces...));
  auto connect_to_sink_f(auto s)
  {
    return [s](auto... n) { return connect_to_sink_impl(s, n...); };
  }
  template<class... Pieces>
  auto connect_to_sink(Section<Pieces...> pipe, auto sink)
    PIPES_RETURN(std::apply(connect_to_sink_f(endNode(sink)),
                            reverse(pipe.pieces)));
} // namespace pipes::detail
namespace pipes::detail
{
  template<typename S, class... T>
  concept SinkFor = requires(S s, T... t) { s.push(t...); };
  struct DummySink
  {
    void push(auto&&...) {}
  };
} // namespace pipes::detail
namespace pipes::detail
{
  template<typename Source, typename Sink>
  concept CanPushRef =
    requires(Source source, Sink sink) { source.push(sink); };
  template<typename Source, typename Sink>
  concept CanPushMove =
    requires(Source source, Sink sink) { source.push(std::move(sink)); };
  template<typename Source, typename Sink>
  concept ValidConnectedPipeline =
    CanPushRef<Source, Sink> && !CanPushMove<Source, Sink>;
} // namespace pipes::detail
namespace pipes::detail
{
  template<class Source, class Sink>
    requires(ValidConnectedPipeline<Source, Sink>)
  struct ConnectedPipeline
  {
    Source source;
    Sink sink;
    decltype(auto) run() &&
    {
      source.push(sink);
      if constexpr(hasValue<Sink>)
      {
        return sink.value();
      }
    }
  };
  auto connectPipeline(auto p)
    PIPES_RETURN(ConnectedPipeline{p.source, connect_to_sink(p.pipe, p.sink)});
} // namespace pipes::detail

//////////////////////////////////////////
//detail/GenericImplementation/impl.hpp
//////////////////////////////////////////

namespace pipes::detail
{
  auto source(auto&& sourcePiece) //
    PIPES_RETURN(SourceSection{PIPES_FWD(sourcePiece)});
  auto sink(auto&& sinkPiece) //
    PIPES_RETURN(SinkSection{PIPES_FWD(sinkPiece)});
  auto pipe(auto&& op) //
    PIPES_RETURN(Section{std::tuple{PIPES_FWD(op)}});
} // namespace pipes::detail

//////////////////////////////////////////
//detail/Discard.hpp
//////////////////////////////////////////

namespace pipes::detail
{
  struct Discard
  {
    void push(auto const&) {}
  };
  template<class... T>
  struct TypedDiscard
  {
    void push(T const&...) {}
  };
} // namespace pipes::detail

//////////////////////////////////////////
//detail/DropTake.hpp
//////////////////////////////////////////

#include <concepts>
namespace pipes::detail
{
  auto fulfilledOnce(auto f)
  {
    return [f, fulfilled = false](auto&&... ts) mutable requires(
      std::invocable<decltype(f), decltype(ts)...>) {
      fulfilled = fulfilled || f(ts...);
      return fulfilled;
    };
  }
  auto invokedTimes(std::size_t count)
  {
    return [count, count_so_far = std::size_t(0)](auto const&...) mutable {
      ++count_so_far;
      return count_so_far > count;
    };
  }
} // namespace pipes::detail

//////////////////////////////////////////
//detail/Filter.hpp
//////////////////////////////////////////

namespace pipes::detail
{
  template<class F>
  struct Filter
  {
    F f;
    template<class... Ts>
      requires(std::invocable<F, Ts...>)
    auto push(SinkFor<Ts...> auto& next, Ts&&... ts)
    {
      if(f(ts...)) next.push(PIPES_FWD(ts)...);
    }
  };
} // namespace pipes::detail

//////////////////////////////////////////
//detail/Flatten.hpp
//////////////////////////////////////////

#include <ranges>
#include <vector>
namespace pipes::detail
{
  struct Flatten
  {
    template<std::ranges::range R>
    void push(SinkFor<std::ranges::range_value_t<R>> auto& next, R const& v)
    {
      for(auto const& t : v) next.push(t);
    }
  };
} // namespace pipes::detail

//////////////////////////////////////////
//detail/ForEach.hpp
//////////////////////////////////////////

#include <ranges>
#include <tuple>
namespace pipes::detail
{
  template<std::ranges::range R>
  struct ViewWrapper
  {
    R r;
    auto begin() { return r.begin(); }
    auto end() { return r.end(); }
    using value_type = std::ranges::range_value_t<R>;
  };
  template<class T>
  ViewWrapper(T const&) -> ViewWrapper<T const&>;
  template<class T>
  ViewWrapper(T&) -> ViewWrapper<T const&>;
  template<class T>
  ViewWrapper(T&&) -> ViewWrapper<T>;
  template<class R>
  using value_t = typename R::value_type;
  template<class>
  struct is_viewWrapper : std::false_type
  {
  };
  template<class R>
  struct is_viewWrapper<ViewWrapper<R>> : std::true_type
  {
  };
  template<class X>
  concept IsViewWrapper = is_viewWrapper<X>::value;
} // namespace pipes::detail
namespace pipes::detail
{
  template<IsViewWrapper R>
  struct ForEach
  {
    R r;
    void push(SinkFor<value_t<R>> auto& sink)
    {
      for(auto const& t : r)
      {
        sink.push(t);
      }
    }
  };
} // namespace pipes::detail
namespace pipes::detail
{
  template<class F>
  struct GenericSource
  {
    F f;
    auto push(auto& sink) PIPES_RETURN(f(sink));
  };
} // namespace pipes::detail
namespace pipes::detail
{
  template<IsViewWrapper R>
  struct AdjacentSource
  {
    R r;
    auto push(SinkFor<value_t<R>, value_t<R>> auto& sink)
    {
      if(r.begin() == r.end()) return;
      auto it1 = r.begin();
      auto it2 = std::next(it1);
      while(it2 != r.end())
      {
        sink.push(*it1++, *it2++);
      }
    };
  };
} // namespace pipes::detail
namespace pipes::detail
{
  template<IsViewWrapper R>
  struct AddAll
  {
    R r;
    template<class... Ts>
    auto push(SinkFor<Ts..., value_t<R>> auto& sink, Ts&&... ts)
    {
      for(const auto& x : r)
      {
        sink.push(PIPES_FWD(ts)..., x);
      }
    }
  };
} // namespace pipes::detail
namespace pipes::detail
{
  template<IsViewWrapper R>
  struct AddEach
  {
    R r;
    AddEach(R r) : r(r), it(r.begin()) {}
    AddEach(AddEach&& other) : r(std::move(other.r)), it(r.begin()) {}
    AddEach(AddEach const& other) : r(other.r), it(r.begin()) {}
    decltype(r.begin()) it = r.begin();
    template<class... Ts>
    auto push(SinkFor<Ts..., value_t<R>> auto& sink, Ts&&... ts)
    {
      if(it == r.end()) return;
      sink.push(PIPES_FWD(ts)..., *it++);
    }
  };
} // namespace pipes::detail
namespace pipes::detail
{
  struct Combinations
  {
    std::vector<int> r;
    void push(SinkFor<int, int> auto& sink) {}
  };
} // namespace pipes::detail

//////////////////////////////////////////
//detail/Fork.hpp
//////////////////////////////////////////

#include <tuple>
namespace pipes::detail
{
  template<class... Nexts>
  struct Fork
  {
    std::tuple<Nexts...> nexts;
    template<class T>
      requires(SinkFor<Nexts, T> && ...)
    void push(T const& t)
    {
      for_each([&t](auto& n) { n.push(t); }, nexts);
    }
  };
} // namespace pipes::detail

//////////////////////////////////////////
//detail/FromStream.hpp
//////////////////////////////////////////

#include <iostream>
namespace pipes::detail
{
  template<class T>
  struct FromStream
  {
    std::istream& stream;
    void push(auto& sink)
    {
      T x;
      while(stream >> x)
      {
        sink.push(x);
      }
    }
  };
} // namespace pipes::detail
namespace pipes::detail
{
  struct ToStream
  {
    std::ostream& stream;
    auto push(auto&& x) PIPES_RETURN(stream << x);
    auto& value() { return stream; }
  };
} // namespace pipes::detail

//////////////////////////////////////////
//detail/Partition.hpp
//////////////////////////////////////////

#include <concepts>
namespace pipes::detail
{
  template<class F, class IfTrue, class IfFalse>
  struct Partition
  {
    F f;
    IfTrue ifTrue;
    IfFalse ifFalse;
    template<class... Ts>
      requires(std::invocable<F, Ts...> && SinkFor<IfTrue, Ts...> &&
               SinkFor<IfFalse, Ts...>)
    void push(Ts&&... ts)
    {
      if(f(ts...)) { ifTrue.push(PIPES_FWD(ts)...); }
      else { ifFalse.push(PIPES_FWD(ts)...); }
    }
  };
} // namespace pipes::detail

//////////////////////////////////////////
//detail/PushBack.hpp
//////////////////////////////////////////

#include <vector>
#include <iostream>
namespace pipes::detail
{
  template<class R, class F>
  struct ValueSink
  {
    R r;
    F f;
    auto push(auto&& t) PIPES_RETURN(f(r, PIPES_FWD(t)));
    R value() { return r; }
  };
  template<class T, class F>
  ValueSink(T&, F) -> ValueSink<T&, F>;
  template<class T, class F>
  ValueSink(T&&, F) -> ValueSink<T, F>;
  constexpr auto push_back_f =
    [](auto& r, auto&& t) PIPES_RETURN(r.push_back(PIPES_FWD(t)));
  constexpr auto push_front_f =
    [](auto& r, auto&& t) PIPES_RETURN(r.push_front(PIPES_FWD(t)));
  constexpr auto insert_f =
    [](auto& r, auto&& t) PIPES_RETURN(r.insert(PIPES_FWD(t)));
  constexpr auto insert_or_assign_f = [](auto& r, auto&& t)
    PIPES_RETURN(r.insert_or_assign(std::get<0>(t), PIPES_FWD(std::get<1>(t))));
  template<class R, class F>
  concept RangeAbleTo =
    std::invocable<F,
                   std::remove_reference_t<R>&,
                   typename std::remove_reference_t<R>::value_type>;
} // namespace pipes::detail
namespace pipes::detail
{
  auto set_aggregate_f(auto f)
  {
    return [f](auto& r, auto&& t)
    {
      auto recur = [f, &r](auto&& t, auto&& recur) -> void
      {
        auto [it, didInsert] = r.insert(PIPES_FWD(t));
        if(!didInsert)
        {
          recur(f(r.extract(it).value(), PIPES_FWD(t)), recur);
        }
      };
      recur(PIPES_FWD(t), recur);
    };
  }
  auto map_aggregate_f(auto f)
  {
    return [f](auto& m, auto&& t)
    {
      auto [it, didInsert] = m.insert(PIPES_FWD(t));
      if(!didInsert)
      {
        it->second = f(it->second, PIPES_FWD(t).second);
      }
    };
  }
} // namespace pipes::detail
namespace pipes::detail
{
  template<class It>
  struct IteratorSink
  {
    It it;
    template<class T>
      requires(std::output_iterator<It, T>)
    auto push(T&& t) PIPES_RETURN(*(it++) = PIPES_FWD(t));
    It value() { return it; }
  };
} // namespace pipes::detail
namespace pipes::detail
{
  template<class F>
  struct GenericSink
  {
    F f;
    auto push(auto&& t) PIPES_RETURN(f(PIPES_FWD(t)));
  };
} // namespace pipes::detail

//////////////////////////////////////////
//detail/Reduce_each.hpp
//////////////////////////////////////////

#include <numeric>
#include <ranges>
namespace pipes::detail
{
  auto reduce_f(auto f)
  {
    return [f](auto&& r)
      PIPES_RETURN(std::reduce(r.begin(),
                               r.end(),
                               std::ranges::range_value_t<decltype(r)>{},
                               f));
  }
  auto reduce_f(auto f, auto init)
  {
    return [f, init](auto&& r)
      PIPES_RETURN(std::reduce(r.begin(), r.end(), init, f));
  }
} // namespace pipes::detail

//////////////////////////////////////////
//detail/RemoveElement.hpp
//////////////////////////////////////////

namespace pipes::detail
{
  template<class Side>
  struct RemoveElement
  {
    Side side;
    template<class T, class... Ts>
      requires(SinkFor<Side, T>)
    void push(SinkFor<Ts...> auto& next, T&& t, Ts&&... ts)
    {
      side.push(PIPES_FWD(t));
      next.push(PIPES_FWD(ts)...);
    }
  };
} // namespace pipes::detail

//////////////////////////////////////////
//detail/Stride.hpp
//////////////////////////////////////////

namespace pipes::detail
{
  auto stride_f(std::size_t step, std::size_t offset)
  {
    return [step, offset, count = 0](auto&&...) mutable {
      const auto effectiveCount = count >= offset ? (count - offset) : 1;
      ++count;
      return effectiveCount % step == 0;
    };
  }
} // namespace pipes::detail

//////////////////////////////////////////
//detail/Switch.hpp
//////////////////////////////////////////

#include <concepts>
namespace pipes::detail
{
  template<class F>
  struct Case
  {
    F f;
  };
  template<class F, class S>
  struct CaseSink
  {
    F f;
    S sink;
    template<class... T>
    auto check(T const&... t) PIPES_RETURN(f(t...));
    template<class... T>
    auto push(T&&... t) PIPES_RETURN(sink.push(PIPES_FWD(t)...));
  };
  template<class... Cases>
  struct Switch
  {
    std::tuple<Cases...> cases;
    template<class... T>
      requires(SinkFor<Cases, T...> && ...)
    void push(T&&... t)
    {
      auto checkPush = [&t...](auto&& c) {
        if(c.check(t...))
        {
          c.push(PIPES_FWD(t)...);
          return true;
        }
        return false;
      };
      callFirstSuccess(cases, checkPush);
    }
  };
} // namespace pipes::detail

//////////////////////////////////////////
//detail/Tee.hpp
//////////////////////////////////////////

namespace pipes::detail
{
  template<class Side>
  struct Tee
  {
    Side v;
    template<class T>
      requires(SinkFor<Side, T>)
    auto push(SinkFor<T> auto& next, T&& t)
    {
      v.push(t);
      next.push(PIPES_FWD(t));
    }
  };
} // namespace pipes::detail

//////////////////////////////////////////
//detail/Transform.hpp
//////////////////////////////////////////

namespace pipes::detail
{
  template<class F>
  struct Transform
  {
    F f;
    template<class... Ts>
    void push(SinkFor<std::invoke_result_t<F, Ts...>> auto& next, Ts&&... ts)
    {
      return next.push(f(PIPES_FWD(ts)...));
    }
  };
} // namespace pipes::detail

//////////////////////////////////////////
//makeSinks.hpp
//////////////////////////////////////////

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
namespace pipes::detail
{
  template<class T>
  auto defaultSink(std::vector<T>& r)
  {
    return api::push_back(r);
  }
  template<class T>
  auto defaultSink(std::vector<T>&& r)
  {
    return api::push_back(PIPES_FWD(r));
  }
  template<class K, class V>
  auto defaultSink(std::map<K, V>& m)
  {
    return api::insert_or_assign(m);
  }
  template<class K, class V>
  auto defaultSink(std::map<K, V>&& m)
  {
    return api::insert_or_assign(PIPES_FWD(m));
  }
} // namespace pipes::detail

//////////////////////////////////////////
//makeSources.hpp
//////////////////////////////////////////

#include <ranges>
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
  inline auto combinations(std::vector<int>&& r)
  {
    return source(Combinations(PIPES_FWD(r)));
  }
} // namespace pipes::detail::api
namespace pipes::detail::api
{
  auto generic_source(auto f) { return source(GenericSource{f}); }
} // namespace pipes::detail::api
namespace pipes::detail::api
{
  template<class T = char>
  auto from_stream(std::istream& stream)
  {
    return source(FromStream<T>{stream});
  }
} // namespace pipes::detail::api
namespace pipes::detail
{
  auto defaultSource(std::ranges::range auto&& r)
  {
    return api::for_each(PIPES_FWD(r));
  }
} // namespace pipes::detail

//////////////////////////////////////////
//useAsSink.hpp
//////////////////////////////////////////

namespace pipes::detail
{
  template<class S>
  concept HasDefaultSource = requires(S const& s) { defaultSource(s); };
  template<class S>
  concept HasDefaultSink = requires(S& s) { defaultSink(s); };
  template<class... Ts>
  auto asSinkSection(SinkSection<Ts...> s)
  {
    return s;
  }
  auto asSinkSection(auto&& t) PIPES_RETURN(defaultSink(PIPES_FWD(t)));
  template<class S>
  concept UsableAsSink = requires(S&& s) { asSinkSection(PIPES_FWD(s)); };
  auto useAsSink(UsableAsSink auto&& s)
  {
    constexpr auto connect = [](auto s)
    { return connect_to_sink(s.pipe, s.finalSink); };
    return connect(asSinkSection(PIPES_FWD(s)));
  }
} // namespace pipes::detail

//////////////////////////////////////////
//makePipes.hpp
//////////////////////////////////////////

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
  constexpr auto default_ = Case{[](auto&&...) { return true; }};
  auto case_(auto f) { return Case{f}; }
  template<class... Fs, class... Ss>
  auto switch_(CaseSink<Fs, Ss>... cs)
  {
    return sink(Switch{std::tuple{cs...}});
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
} // namespace pipes::detail::api

//////////////////////////////////////////
//operators.hpp
//////////////////////////////////////////

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
namespace pipes::detail
{
  auto link(auto&& r, auto n)
    PIPES_RETURN(link(defaultSource(PIPES_FWD(r)), n));
  template<class... Ops>
  auto link(SourceSection<Ops...> source, auto&& p)
    PIPES_RETURN(link(source, defaultSink(PIPES_FWD(p))));
  template<class... Ops>
  auto link(Section<Ops...> n, auto&& p)
    PIPES_RETURN(link(n, defaultSink(PIPES_FWD(p))));
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
namespace pipes::detail
{
  template<class Source, class Sink>
  decltype(auto) operator>>=(Source&& source, Sink&& sink)
  {
    return link(PIPES_FWD(source), PIPES_FWD(sink));
  }  
  template<class Source, class Sink>
  decltype(auto) operator>>(Source&& source, Sink&& sink)
  {
    return link(PIPES_FWD(source), PIPES_FWD(sink));
  }
} // namespace pipes::detail

//////////////////////////////////////////
//pipes.hpp
//////////////////////////////////////////

namespace pipes
{
  using namespace detail::api;
}
