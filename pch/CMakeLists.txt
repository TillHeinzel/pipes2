add_library(pch_lib "dummy.cpp")
target_link_libraries(pch_lib doctest)
target_precompile_headers(pch_lib PUBLIC pch.hpp)