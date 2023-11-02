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

  auto discard() { return sink(Discard{}); }

  template<class... T>
  auto discard()
  {
    return sink(TypedDiscard<T...>{});
  }

  auto generic_sink(auto f) { return sink(GenericSink{f}); }

  template<class K, class V>
  auto map_aggregator(std::map<K, V>& m, auto f)
  {
    auto ff = [f](auto& m, auto&& t)
    {
      auto [it, didInsert] = m.insert(PIPES_FWD(t));

      if(!didInsert)
      {
        it->second = f(it->second, PIPES_FWD(t).second);
      }
    };

    return sink(ValueSink{PIPES_FWD(m), ff});
  }

  auto set_aggregator(std::set<int>& s, auto f)
  {
    auto ff = [f](auto& r, auto&& t)
    {
      auto recur = [f, &r](auto&& t, auto&& recur) -> void
      {
        auto [it, didInsert] = r.insert(PIPES_FWD(t));
        if(!didInsert)
        {
          recur(f(r.extract(it).value(), PIPES_FWD(t)), recur);
        }
      };

      recur(PIPES_FWD(t), recur);
    };

    return sink(ValueSink{PIPES_FWD(s), ff});
  }
} // namespace pipes::detail::api

namespace pipes::detail
{
  template<class T>
  auto defaultSink(std::vector<T>& r)
  {
    return api::push_back(r);
  }

  template<class K, class V>
  auto defaultSink(std::map<K, V>& m)
  {
    return api::insert_or_assign(m);
  }
} // namespace pipes::detail
