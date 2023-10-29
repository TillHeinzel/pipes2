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

} // namespace pipes::detail
