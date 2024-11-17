target_include_directories(app PRIVATE .)
target_sources(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/messages.cpp)
target_sources_ifdef(CONFIG_MODULE1 app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/entry.cpp)