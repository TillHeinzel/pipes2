#pragma once

namespace pipes
{
  template<class Op, class Next>
  struct Node
  {
    static constexpr bool isOpenSink = true;

    Op op;
    Next next;

    void push(int i) { op.push(next, i); }
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
  auto addBefore(OpenSink auto s) { return s; }

  template<class Op, class... Ops>
  auto addBefore(OpenSink auto s, Op op, Ops... ops)
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
} // namespace pipes

namespace pipes
{

  struct ForEachSource
  {
    static constexpr bool isRootSource = true;

    std::vector<int> const& v;

    void push(Sink<int> auto sink)
    {
      for(const int i : v) { sink.push(i); }
    }
  };

  template<typename T>
  concept RootSource = requires(T) { T::isRootSource; };

  template<class... Ops>
  struct Source
  {
    ForEachSource root;
    RawNodes<Ops...> ops;
  };

  template<class... LaterOps, class... EarlierOps>
  auto addBefore(RawNodes<LaterOps...> laterOps, Source<EarlierOps...> source)
  {
    return Source<LaterOps..., EarlierOps...>{source.root,
                                              addBefore(laterOps, source.ops)};
  }

  // todo: generalize Sources to include others than just vectors
  //  todo: put in checks, so that ops are required to be able to be called with
  //  output of root. Can use a dummy-Sink to see if it works out somehow?
  // todo: take in temporaries as valid sources, not just const&

} // namespace pipes

namespace pipes
{
  template<class... Ops>
  void finish(Source<Ops...> source, Sink<int> auto sink)
  {
    source.root.push(addBefore(sink, source.ops));
  }
} // namespace pipes

namespace pipes
{
  Source<> forEach(const std::vector<int>& v)
  {
    return Source<>{ForEachSource{v}};
  }
} // namespace pipes