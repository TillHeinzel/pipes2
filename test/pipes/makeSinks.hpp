#pragma once

#include "detail/Discard.hpp"
#include "detail/PushBack.hpp"

namespace pipes::detail::api
{
  auto push_back(RangeAbleTo<decltype(push_back_f)> auto& r)
  {
    return sink(ReferenceSink{r, push_back_f});
  }

  auto push_front(RangeAbleTo<decltype(push_front_f)> auto& r)
  {
    return sink(ReferenceSink{r, push_front_f});
  }

  auto insert(RangeAbleTo<decltype(insert_f)> auto& r)
  {
    return sink(ReferenceSink{r, insert_f});
  }

  auto insert_or_assign(RangeAbleTo<decltype(insert_or_assign_f)> auto& r)
  {
    return sink(ReferenceSink{r, insert_or_assign_f});
  }

  auto to_iterator(std::input_or_output_iterator auto it)
  {
    return sink(IteratorSink{it});
  }

  auto discard() { return sink(Discard{}); }

  template<class... T>
  auto discard()
  {
    return sink(TypedDiscard<T...>{});
  }

  auto generic_sink(auto f) { return sink(GenericSink{f}); }
} // namespace pipes::detail::api

namespace pipes::detail
{
  template<class T>
  auto defaultSink(std::vector<T>& r)
  {
    return api::push_back(r);
  }
} // namespace pipes::detail
