#pragma once

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
  template<typename T>
  concept RootSource = requires(T) { T::isRootSource; };

  template<typename T, typename Ops>
  concept ValidSource =
    Sink<decltype(addBefore(DiscardSink{}, std::declval<Ops>())), int>;
  ;

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
  auto addBefore(OpenSink auto s) -> decltype(s) { return s; }

  template<class Op, class... Ops>
  auto addBefore(OpenSink auto s, Op op, Ops... ops)
    -> decltype(addBefore(Node{op, s}, ops...))
  {
    return addBefore(Node{op, s}, ops...);
  }

  template<class... Ops>
  auto addBefore(OpenSink auto s, RawNodes<Ops...> ops)
  {
    auto f = [&](auto... n) { return addBefore(s, n...); };
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
} // namespace pipes

namespace pipes
{
  template<class... Ops>
  void finish(Source<Ops...> source, Sink<int> auto sink)
  {
    source.root.push(addBefore(sink, source.ops));
  }  
  
  template<class... Ops>
  void finish(Source<Ops...> source, Sink<std::string> auto sink)
  {
    source.root.push(addBefore(sink, source.ops));
  }
} // namespace pipes
