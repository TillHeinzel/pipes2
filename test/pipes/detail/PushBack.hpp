#pragma once
#include <vector>

#include "GenericImplementation/impl.hpp"

namespace pipes::detail
{
  template<class R, class F>
  struct ReferenceSink
  {
    R& r;
    F f;

    auto push(auto&& t) PIPES_RETURN(f(r, PIPES_FWD(t)));

    R& value() { return r; }
  };

  template<class R, class T>
  concept PushBackAbleFor = requires(R r, T t) { r.push_back(t); };

  template<class R>
    requires(PushBackAbleFor<R, typename R::value_type>)
  auto make_push_back_sink(R& r)
  {
    auto f = [](auto& r, auto&& t) PIPES_RETURN(r.push_back(PIPES_FWD(t)));
    return sink(ReferenceSink{r, f});
  }

  template<class R, class T>
  concept PushFrontAbleFor = requires(R r, T t) { r.push_front(t); };

  template<class R>
    requires(PushFrontAbleFor<R, typename R::value_type>)
  auto make_push_front_sink(R& r)
  {
    auto f = [](auto& r, auto&& t) PIPES_RETURN(r.push_front(PIPES_FWD(t)));
    return sink(ReferenceSink{r, f});
  }

  template<class R, class T>
  concept InsertableFor = requires(R r, T t) { r.insert(t); };

  template<class R>
    requires(InsertableFor<R, typename R::value_type>)
  auto make_insert_sink(R& r)
  {
    auto f = [](auto& r, auto&& t) PIPES_RETURN(r.insert(PIPES_FWD(t)));
    return sink(ReferenceSink{r, f});
  }

  template<class R, class T>
  concept InsertOrAssignAbleFor = Pair_like<T> && requires(R r, T t) {
    r.insert_or_assign(std::get<0>(t), std::get<1>(t));
  };

  template<class R>
    requires(InsertOrAssignAbleFor<R, typename R::value_type>)
  auto make_insert_or_assign_sink(R& r)
  {
    auto f = [](auto& r, Pair_like auto&& t) PIPES_RETURN(
      r.insert_or_assign(std::get<0>(t), PIPES_FWD(std::get<1>(t))));
    return sink(ReferenceSink{r, f});
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
