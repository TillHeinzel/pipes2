#pragma once


#define PIPES_FWD(...)                                                         \
  ->decltype(__VA_ARGS__) { return __VA_ARGS__; }