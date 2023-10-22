
#include <concepts>
#include <vector>

namespace pipes
{
  template<class F>
  struct TransformNode
  {
    F f;
  };

  template<class F>
  struct Sink
  {
    TransformNode<F> node;
    std::vector<int>& v;
  };

  template<class F>
  Sink<F> operator>>=(TransformNode<F> node, std::vector<int>& v)
  {
    return {node, v};
  }

  template<class F>
  void operator>>=(std::vector<int> const& source, Sink<F> sink)
  {
    for(const int i : source) { sink.v.push_back(sink.node.f(i)); }
  }

  template<typename F>
  TransformNode<F> transform(F f)
    requires std::invocable<F, int> &&
             std::same_as<std::invoke_result_t<F, int>, int>
  {
    return {f};
  };
} // namespace pipes
