add_library(pch_lib "dummy.cpp")
target_precompile_headers(pch_lib PUBLIC pch.hpp)