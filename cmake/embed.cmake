# modified https://github.com/magcks/embed
# original license at https://github.com/magcks/embed/blob/master/LICENSE.md

set(EMBED_RES_ID_DEFAULT 16384)
set(EMBED_RES_ID ${EMBED_RES_ID_DEFAULT})
set(EMBED_STRUCT
"#include \"stddef.h\"
struct Res {
  const char *data;
  const size_t size;
};"
)
set(EMBED_CREATED_OUTPUTS "")
if(MSVC)
  set(EMBED_COMMON_RC "${CMAKE_CURRENT_BINARY_DIR}/embed/embed.rc")
  set(EMBED_COMMON_C "${CMAKE_CURRENT_BINARY_DIR}/embed/embed.c")
endif()

function(EMBED_TARGET target input) # optional argument: name
  get_filename_component(absolute_path ${input} REALPATH)

  if(MSVC)
    # Use RC file
    if(DEFINED ARGV2)
      set(output_rc "${CMAKE_CURRENT_BINARY_DIR}/embed/${ARGV2}.rc")
      set(output_c "${CMAKE_CURRENT_BINARY_DIR}/embed/${ARGV2}.c")
    else()
      set(output_rc ${EMBED_COMMON_RC})
      set(output_c ${EMBED_COMMON_C})
    endif()

    # message(STATUS "Embedding ${absolute_path} to ${output_rc}")

    if(NOT output_c IN_LIST EMBED_CREATED_OUTPUTS)
      list(APPEND EMBED_CREATED_OUTPUTS ${output_c})
      set(EMBED_CREATED_OUTPUTS ${EMBED_CREATED_OUTPUTS} PARENT_SCOPE)
      set(${target}_OUTPUTS "${output_c};${output_rc}" PARENT_SCOPE)

      file(WRITE ${output_rc} "${EMBED_RES_ID} RCDATA \"${absolute_path}\"\n")
      file(WRITE ${output_c}
"#include \"windows.h\"

${EMBED_STRUCT}

#define EMBED_RES(name, res_id)\\
  struct Res name(void) {\\
    HMODULE handle = GetModuleHandle(NULL);\\
    HRSRC res = FindResource(handle, MAKEINTRESOURCE(res_id), RT_RCDATA);\\
    struct Res r = {\\
      (const char*) LockResource(LoadResource(handle, res)),\\
      SizeofResource(handle, res)\\
    };\\
    return r;\\
  }

EMBED_RES(${target}, ${EMBED_RES_ID});\n"
      )
    else()
      file(APPEND ${output_rc} "${EMBED_RES_ID} RCDATA \"${absolute_path}\"\n")
      file(APPEND ${output_c} "EMBED_RES(${target}, ${EMBED_RES_ID});\n")
    endif()
    
    math(EXPR EMBED_RES_ID "${EMBED_RES_ID} + 1")
    set(EMBED_RES_ID ${EMBED_RES_ID} PARENT_SCOPE)
  else()
    # Use incbin directive
    # TODO use a common C
    set(output "${CMAKE_CURRENT_BINARY_DIR}/embed/${target}.c")

    if(APPLE)
      set(section ".const_data")
    else()
      set(section ".section \\\".rodata\\\", \\\"a\\\", @progbits")
    endif()
# \".align ${CMAKE_SIZEOF_VOID_P}\\n\"
    file(WRITE ${output}
"${EMBED_STRUCT}

extern char data_start[];
extern char data_end[];
asm(
  \"${section}\\n\"
  \"data_start:\\n\"
    \".incbin \\\"${absolute_path}\\\"\\n\"
  \"data_end:\\n\"
    \".previous\\n\"
);

struct Res ${target}(void) {
  struct Res r = { data_start, data_end - data_start };
  return r;
}"
    )
    
    add_custom_command(
      OUTPUT ${output}
      COMMAND ${CMAKE_COMMAND} -E touch ${output}
      DEPENDS ${input}
    )

    set(${target}_OUTPUTS ${output} PARENT_SCOPE)
  endif()

  set(${target}_DEFINED TRUE PARENT_SCOPE)
endfunction()

function(EMBED_INTO target embeddings)
  file(GLOB_RECURSE embeddings_glob ${embeddings})
  set(embed_outputs "")

  # message(STATUS "${target} => ${embeddings_glob}")

  foreach(embedding ${embeddings_glob}) # optional parameters: prefix postfix
    get_filename_component(filename ${embedding} NAME)
    string(REPLACE "." "_" filename ${filename})
    string(TOUPPER ${filename} filename)

    if(DEFINED ARGV2)
      set(filename "${ARGV2}_${filename}")
    endif()
    if(DEFINED ARGV3)
      set(filename "${filename}_${ARGV3}")
    endif()
    
    EMBED_TARGET(${filename} ${embedding} ${target})

    foreach(output ${${filename}_OUTPUTS})
      list(APPEND embed_outputs ${output})
    endforeach()

    if(MSVC)
      set(EMBED_RES_ID ${EMBED_RES_ID} PARENT_SCOPE)
      set(EMBED_CREATED_OUTPUTS ${EMBED_CREATED_OUTPUTS} PARENT_SCOPE)
    endif()
  endforeach()
  
  target_sources(${target} PUBLIC ${embed_outputs})
endfunction()
