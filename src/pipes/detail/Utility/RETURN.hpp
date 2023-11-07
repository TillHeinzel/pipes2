#pragma once


#define PIPES_RETURN(...)                                                         \
  ->decltype(__VA_ARGS__) { return __VA_ARGS__; }

//
//#define PIPES_REQ_RETURN(...)                                                      \
//  ->decltype(__VA_ARGS__) requires requires(__VA_ARGS__) { return __VA_ARGS__; }
