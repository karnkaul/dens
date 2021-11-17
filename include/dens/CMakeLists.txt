set(sources
  detail/archetype.hpp
  detail/sign.hpp
  detail/tarray.hpp
  entity.hpp
  registry.hpp
  system_group.hpp
  system.hpp
)

target_sources(${PROJECT_NAME} INTERFACE ${sources})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${sources})
