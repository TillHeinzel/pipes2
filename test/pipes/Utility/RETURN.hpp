#pragma once


#define PIPES_RETURN(...)                                                         \
  ->decltype(__VA_ARGS__) { return __VA_ARGS__; }