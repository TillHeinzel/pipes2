#pragma once

#include "api.hpp"

namespace pipes::detail
{
  template<class T>
  struct DefaultSinkFor<std::vector<T>>
  {
    static auto make(std::vector<T>& v) { return api::push_back(v); }

    static auto make(std::vector<T>&& v)
    {
      return api::push_back(PIPES_FWD(v));
    }
  };

  template<class K, class V>
  struct DefaultSinkFor<std::map<K, V>>
  {
    static auto make(std::map<K, V>& v) { return api::insert_or_assign(v); }

    static auto make(std::map<K, V>&& v)
    {
      return api::insert_or_assign(std::move(v));
    }
  };
} // namespace pipes::detail

namespace pipes::detail
{
  template<class X>
  struct DefaultSourceFor
  {
    static auto make(std::ranges::range auto&& r)
    {
      return api::for_each(PIPES_FWD(r));
    }
  };
} // namespace pipes::detail
