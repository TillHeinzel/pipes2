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
    Sink<decltype(append(std::declval<Chain>(), DiscardSink{})), T>;

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
  auto prepend(OpenSink auto s) -> decltype(s) { return s; }

  template<class Op, class... Ops>
  auto prepend(OpenSink auto s, Op op, Ops... ops)
    PIPES_FWD(prepend(Node{op, s}, ops...));

  auto prepend_f(OpenSink auto s)
  {
    return [s](auto... n) { return prepend(s, n...); };
  }

  template<class... Ops>
  auto append(RawNodes<Ops...> ops, OpenSink auto s)
    PIPES_FWD(std::apply(prepend_f(s), reverse(ops.ops)));

  template<class... Ops1, class... Ops2>
  auto append(RawNodes<Ops1...> ops1, RawNodes<Ops2...> ops2)
    PIPES_FWD(RawNodes{std::tuple_cat(ops1.ops, ops2.ops)});

  template<class... LaterOps, class... EarlierOps>
  auto append(Source<EarlierOps...> source, RawNodes<LaterOps...> laterOps)
    PIPES_FWD(Source{source.root, append(source.ops, laterOps)});

  template<class... Ops>
  auto append(Source<Ops...> source, auto sink)
    PIPES_FWD(source.root.push(append(source.ops, sink)));
} // namespace pipes
