#pragma once

#define PIPES_FWD(a) std::forward<decltype(a)>(a)

#define PIPES_LAMBDA(f) [](auto&&... ts) PIPES_RETURN(f(PIPES_FWD(ts)...))