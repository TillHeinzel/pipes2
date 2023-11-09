#pragma once

#define PIPES_FWD(a) std::forward<decltype(a)>(a)

#define PIPES_RETURN(...)                                                      \
  ->decltype(__VA_ARGS__) { return __VA_ARGS__; }
