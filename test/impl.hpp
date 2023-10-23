#pragma once

#define PIPES_FWD(...)                                                         \
  ->decltype(__VA_ARGS__) { return __VA_ARGS__; }

namespace pipes
{
  template<typename O, class Next, class T>
  concept Operation = requires(O op, Next& next, T t) { op.push(next, t); };

  template<class Op, class Next>
  struct Node
  {
    static constexpr bool isOpenSink = true;

    Op op;
    Next next;

    template<class T>
      requires(Operation<Op, Next, T>)
    void push(T t)
    {
      op.push(next, t);
    }
  };

  template<class... Ops>
  struct RawNodes
  {
    RawNodes(Ops... ops) : ops(ops...) {}
    RawNodes(std::tuple<Ops...> ops) : ops(ops) {}

    std::tuple<Ops...> ops;
  };
} // namespace pipes

namespace pipes
{
  template<typename T, class I>
  concept Sink = requires(T t, I i) { t.push(i); };

  template<typename T>
  concept OpenSink = requires(T) { T::isOpenSink; };

} // namespace pipes

namespace pipes
{
  template<typename S>
  concept RootSource = requires(S) { typename S::OutputType; };

  struct DummySink
  {
    static constexpr bool isOpenSink = true;

    void push(auto i) {}
  };

  template<typename Chain, typename T>
  concept ValidChainFor =
    Sink<decltype(connect_links(std::declval<Chain>(), DummySink{})), T>;

  template<typename S, typename Ops>
  concept ValidSource =
    RootSource<S> && ValidChainFor<Ops, typename S::OutputType>;

  template<RootSource Root, class... Ops>
  struct Source
  {
    Source(Root root, RawNodes<Ops...> ops)
      requires(ValidSource<Root, RawNodes<Ops...>>)
      : root(root), ops(ops)
    {
    }

    Root root;
    RawNodes<Ops...> ops;
  };

  template<class FinalSink, class... Ops>
  struct PSink
  {
    FinalSink finalSink;
    RawNodes<Ops...> ops;

    auto collapse() const { return connect_links(ops, finalSink); }
  };

  template<class S, class T>
  concept ValidPSink = Sink<decltype(std::declval<S>().collapse()), T>;

  template<typename X, typename T>
  concept ValidReceiverFor =
    Sink<X, T> || ValidChainFor<X, T> || ValidPSink<X, T>;
} // namespace pipes

namespace pipes
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
} // namespace pipes

namespace pipes
{
  auto connect_links_impl(OpenSink auto s) -> decltype(s) { return s; }

  template<class Op, class... Ops>
  auto connect_links_impl(OpenSink auto s, Op op, Ops... ops)
    PIPES_FWD(connect_links_impl(Node{op, s}, ops...));

  auto connect_links_f(OpenSink auto s)
  {
    return [s](auto... n) { return connect_links_impl(s, n...); };
  }

  template<class... Ops>
  auto connect_links(RawNodes<Ops...> ops, OpenSink auto s)
    PIPES_FWD(std::apply(connect_links_f(s), reverse(ops.ops)));
} // namespace pipes

namespace pipes
{
  template<RootSource S>
  auto finish(S source, Sink<typename S::OutputType> auto sink)
  {
    source.push(sink);
  }

  template<class... Ops>
  auto finish(RootSource auto source, RawNodes<Ops...> ops, auto finalSink)
  {
    finish(source, connect_links(ops, finalSink));
  }
} // namespace pipes

namespace pipes
{
  template<class... Ops>
  auto append(RawNodes<Ops...> ops, OpenSink auto sink)
    PIPES_FWD(connect_links(ops, sink));
} // namespace pipes

namespace pipes
{
  template<class... Ops1, class... Ops2>
  auto append(RawNodes<Ops1...> ops1, RawNodes<Ops2...> ops2)
    PIPES_FWD(RawNodes{std::tuple_cat(ops1.ops, ops2.ops)});

  template<class... LaterOps, class... EarlierOps>
  auto append(Source<EarlierOps...> source, RawNodes<LaterOps...> laterOps)
    PIPES_FWD(Source{source.root, append(source.ops, laterOps)});

  template<class... Ops, class... Ts>
  auto append(RawNodes<Ops...> ops, PSink<Ts...> sink)
    PIPES_FWD(PSink{sink.finalSink, append(ops, sink.ops)});

  template<class... T1s, class... T2s>
  auto append(Source<T1s...> source, PSink<T2s...> sink) PIPES_FWD(
    finish(source.root, append(source.ops, sink.ops), sink.finalSink));
} // namespace pipes
