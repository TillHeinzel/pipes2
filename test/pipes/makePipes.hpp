#pragma once

#include "detail/DropTake.hpp"
#include "detail/Filter.hpp"
#include "detail/Flatten.hpp"
#include "detail/Fork.hpp"
#include "detail/Partition.hpp"
#include "detail/Reduce_each.hpp"
#include "detail/RemoveElement.hpp"
#include "detail/Stride.hpp"
#include "detail/Switch.hpp"
#include "detail/Tee.hpp"
#include "detail/Transform.hpp"

#include "useAsSink.hpp"

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
