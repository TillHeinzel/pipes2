#pragma once

#define PIPES_FWD(...)                                                           \
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

  struct DiscardSink
  {
    static constexpr bool isOpenSink = true;

    void push(auto i) {}
  };
} // namespace pipes

namespace pipes
{
  template<typename S>
  concept RootSource = requires(S) { typename S::OutputType; };

  template<typename Chain, typename T>
  concept ValidChainFor =
    Sink<decltype(addBefore(DiscardSink{}, std::declval<Chain>())), T>;

  template<typename S, typename Ops>
  concept ValidSource =
    RootSource<S> && ValidChainFor<Ops, typename S::OutputType>;

  template<typename X, typename T>
  concept ValidReceiverFor = Sink<X, T> || ValidChainFor<X, T>;

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

  // todo: generalize Sources to include others than just vectors
  //  todo: put in checks, so that ops are required to be able to be called with
  //  output of root. Can use a dummy-Sink to see if it works out somehow?
  // todo: take in temporaries as valid sources, not just const&

} // namespace pipes

namespace pipes
{
  auto prepend(OpenSink auto s) -> decltype(s) { return s; }

  template<class Op, class... Ops>
  auto prepend(OpenSink auto s, Op op, Ops... ops)
    PIPES_FWD(prepend(Node{op, s}, ops...));

  template<class... Ops>
  auto addBefore(OpenSink auto s, RawNodes<Ops...> ops)
  {
    auto f = [&](auto... n) { return prepend(s, n...); };
    return std::apply(f, ops.ops);
  }

  template<class... Ops1, class... Ops2>
  auto addBefore(RawNodes<Ops2...> ops2, RawNodes<Ops1...> ops1)
  {
    return RawNodes{std::tuple_cat(ops2.ops, ops1.ops)};
  }

  template<class... LaterOps, class... EarlierOps>
  auto addBefore(RawNodes<LaterOps...> laterOps, Source<EarlierOps...> source)
  {
    return Source{source.root, addBefore(laterOps, source.ops)};
  }

  template<class... Ops>
  auto addBefore(auto sink, Source<Ops...> source)
    PIPES_FWD(source.root.push(addBefore(sink, source.ops)));

  template<class T1, class T2>
  auto append(T1 t1, T2 t2) PIPES_FWD(addBefore(t2, t1));
} // namespace pipes

namespace pipes
{
  template<class... Ops>
  auto finish(Source<Ops...> source, auto sink)
    PIPES_FWD(addBefore(sink, source));
} // namespace pipes
