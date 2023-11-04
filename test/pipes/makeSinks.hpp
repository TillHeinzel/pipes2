#pragma once

#include "detail/Discard.hpp"
#include "detail/PushBack.hpp"

namespace pipes::detail::api
{
  auto push_back(RangeAbleTo<decltype(push_back_f)> auto&& r)
  {
    return sink(ValueSink{PIPES_FWD(r), push_back_f});
  }

  auto push_front(RangeAbleTo<decltype(push_front_f)> auto&& r)
  {
    return sink(ValueSink{PIPES_FWD(r), push_front_f});
  }

  auto insert(RangeAbleTo<decltype(insert_f)> auto&& r)
  {
    return sink(ValueSink{PIPES_FWD(r), insert_f});
  }

  auto insert_or_assign(RangeAbleTo<decltype(insert_or_assign_f)> auto&& r)
  {
    return sink(ValueSink{PIPES_FWD(r), insert_or_assign_f});
  }

  auto to_iterator(std::input_or_output_iterator auto it)
  {
    return sink(IteratorSink{it});
  }

  inline auto discard() { return sink(Discard{}); }

  template<class... T>
  auto discard()
  {
    return sink(TypedDiscard<T...>{});
  }

  auto generic_sink(auto f) { return sink(GenericSink{f}); }

  template<class K, class V>
  auto map_aggregator(std::map<K, V>& m, auto f)
  {
    return sink(ValueSink{PIPES_FWD(m), map_aggregate_f(f)});
  }

  template<class T>
  auto set_aggregator(std::set<T>& s, auto f)
  {
    return sink(ValueSink{PIPES_FWD(s), set_aggregate_f(f)});
  }

  template<class T>
  auto set_aggregator(std::set<T>&& s, auto f)
  {
    return sink(ValueSink{PIPES_FWD(s), set_aggregate_f(f)});
  }
} // namespace pipes::detail::api

namespace pipes::detail
{
  template<class T>
  auto defaultSink(std::vector<T>& r)
  {
    return api::push_back(r);
  }

  template<class T>
  auto defaultSink(std::vector<T>&& r)
  {
    return api::push_back(PIPES_FWD(r));
  }

  template<class K, class V>
  auto defaultSink(std::map<K, V>& m)
  {
    return api::insert_or_assign(m);
  }

  template<class K, class V>
  auto defaultSink(std::map<K, V>&& m)
  {
    return api::insert_or_assign(PIPES_FWD(m));
  }
} // namespace pipes::detail
