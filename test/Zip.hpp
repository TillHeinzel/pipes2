#pragma once

#include <vector>

#include "impl.hpp"

namespace pipes::detail
{
  template<class T>
  struct Zip
  {
    using OutputType = std::tuple<T>;

    std::vector<T> const& v;

    void push(SinkFor<std::tuple<T>> auto sink)
    {
      for(auto const& t : v) { sink.push(std::tuple{t}); }
    }
  };

  template<class T1, class T2>
  auto begins(std::vector<T1> const& v1, std::vector<T2> const& v2)
  {
    return std::tuple{v1.begin(), v2.begin()};
  }

  template<class T1, class T2>
  struct Zip2
  {
    using OutputType = std::tuple<T1, T2>;

    std::vector<T1> const& v1;
    std::vector<T2> const& v2;

    void push(SinkFor<std::tuple<T1, T2>> auto sink)
    {
      auto [it1, it2] = begins(v1, v2);

      while(it1 != v1.end() && it2 != v2.end())
      {
        sink.push(OutputType{*it1, *it2});
        ++it1;
        ++it2;
      }
    }
  };

  namespace api
  {
    template<class T>
    auto zip(const std::vector<T>& v) PIPES_FWD(Source{Zip{v}});

    template<class T1, class T2>
    auto zip(const std::vector<T1>& v1, const std::vector<T2>& v2)
      PIPES_FWD(Source{
        Zip2{v1, v2}
    });

  } // namespace api
} // namespace pipes::detail