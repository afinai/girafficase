
if (Flatbuffers_FOUND)
    find_program(CMAKE_FLATBUFFERS_COMPILER NAMES flatc)
else()
    message(FATAL_ERROR "FLATBUFFERSR not foud!")
endif()

message(STATUS ${CMAKE_FLATBUFFERS_COMPILER})

file(GLOB schema_files "${CMAKE_CURRENT_SOURCE_DIR}/schema/*.fbs")

set(schema_header "schema.hpp")
set(messages_header "messages.hpp")

set(input_paths)
foreach(schema_filename ${schema_files})
    list(APPEND input_paths ${schema_filename})
endforeach()

set(output_paths)
foreach(schema_filename ${schema_files})
    get_filename_component(filename "${schema_files}" NAME_WE)
    set(output_file "${CMAKE_CURRENT_BINARY_DIR}/generated/${filename}.hpp")
    list(APPEND output_paths ${output_file})
endforeach()

# Copy main header
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/${schema_header}"
    "${CMAKE_CURRENT_BINARY_DIR}/${messages_header}"
    COPYONLY)
# Append to list of outputs
list(APPEND ${output_paths} "${CMAKE_CURRENT_BINARY_DIR}/${messages_header}")

# Generate C++ headers
add_custom_command(
    OUTPUT ${output_paths}
    COMMAND ${CMAKE_FLATBUFFERS_COMPILER} -o "${CMAKE_CURRENT_BINARY_DIR}/generated" --cpp
    ARGS ${input_paths}
    COMMENT "Generating flattbuffers message C++ headers")

add_custom_target(messages DEPENDS ${output_paths})
