add_library(tests OBJECT  
  "reduce_each.cpp" 
  "for_each.cpp" 
  "push_back.cpp" 
  "filter.cpp" 
  "add_all.cpp"
  "add_each.cpp"
  "drop.cpp"
 "transform.cpp" 
 "chained_operations.cpp" 
 "take.cpp" 
 "fork.cpp" 
 "flatten.cpp"
 "partition.cpp"
 "switch.cpp"
 "stride.cpp"
 "tee.cpp"
 "unzip.cpp"
 "chunk.cpp" 
 "push_front.cpp" 
 "discard.cpp" 
 "insert_or_assign.cpp" 
 "insert.cpp" 
 "to_iterator.cpp" 
 "set_aggregator.cpp" 
 "map_aggregator.cpp" 
 "to_ostream.cpp" 
 "generic_sink.cpp" 
 "reduce.cpp" 
 "default_sinks.cpp"
 "adjacent.cpp" 
 "cartesian_product.cpp" 
 "combinations.cpp" 
 "interleave.cpp" 
 "intersperse.cpp" 
 "zip.cpp" 
 "generic_source.cpp" 
 "from_stream.cpp"
 "remove_element.cpp")

target_link_libraries(tests PRIVATE pch_lib)
target_precompile_headers(tests REUSE_FROM pch_lib)

set_target_properties(tests PROPERTIES UNITY_BUILD ON) 
set_target_properties(tests PROPERTIES UNITY_BUILD_BATCH_SIZE 6) 

target_include_directories(tests PRIVATE "${CMAKE_CURRENT_LIST_DIR}/..")

  # todo: make switch cases possible to chain with >>
  #
  # todo: use as output iterator for std::algorithms
  # 
  # todo: different operators, including <<= >>= << >> |
  #
  # todo: move-only types
  #
  # todo: big types with lots of data (do not have to copy everything all the
  # time)
  #
  # todo: streams
  #
  # todo: generators and take-from for generator with effectively infinite
  # sources
  #
  # todo: make work with tuples! pipelines for tuples! that would be cool and
  # useful
  #
  # todo: make examples that use the single-header generated from the headers
  # and links with cmake