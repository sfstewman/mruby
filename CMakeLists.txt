find_package(BISON REQUIRED)
find_package(Readline)

include(ConcatFiles)
include(CCompilerOptions)

# Reset CFLAGS and our include directories
get_property(CMAKE_C_FLAGS GLOBAL PROPERTY ORIG_CMAKE_C_FLAGS)
set_property(DIRECTORY PROPERTY INCLUDE_DIRECTORIES)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  )

CCompilerOptions(
  LANG C99
  DEBUG ${DEBUG_LEVEL}
  WARNINGS ALL)

set(PARSER_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}")
file(MAKE_DIRECTORY ${PARSER_OUTPUT_DIR})
BISON_TARGET(MRubyParser src/parse.y ${CMAKE_CURRENT_BINARY_DIR}/y.tab.c)

set(MRUBY_SRCS
  src/array.c
  src/class.c
  src/codegen.c
  src/compar.c
  src/crc.c
  src/dump.c
  src/enum.c
  src/error.c
  src/error.h
  src/etc.c
  src/gc.c
  src/hash.c
  src/init.c
  src/kernel.c
  src/keywords
  src/lex.def
  src/load.c
  src/node.h
  src/numeric.c
  src/object.c
  src/opcode.h
  src/pool.c
  src/print.c
  src/proc.c
  src/range.c
  src/re.h
  src/state.c
  src/string.c
  src/symbol.c
  src/variable.c
  src/vm.c
  ${BISON_MRubyParser_OUTPUTS}
  )

add_library( mruby STATIC ${MRUBY_SRCS} )

add_executable(mrbc_exe tools/mrbc/mrbc.c)
set_target_properties(mrbc_exe PROPERTIES OUTPUT_NAME mrbc)
target_link_libraries(mrbc_exe mruby)

macro(mrbc _vname _rbfile _cfile)
  add_custom_command(OUTPUT ${_cfile}
    COMMAND mrbc_exe -B${_vname} -o${_cfile} ${_rbfile}
    DEPENDS mrbc_exe ${_rbfile}
    VERBATIM)
endmacro()

CONCAT_FILES(${CMAKE_CURRENT_BINARY_DIR}/mrblib.rb
  mrblib/array.rb
  mrblib/class.rb
  mrblib/compar.rb
  mrblib/enum.rb
  mrblib/error.rb
  mrblib/hash.rb
  mrblib/kernel.rb
  mrblib/numeric.rb
  mrblib/print.rb
  mrblib/range.rb
  mrblib/string.rb
  )
MRBC(mrblib_irep
  ${CMAKE_CURRENT_BINARY_DIR}/mrblib.rb
  ${CMAKE_CURRENT_BINARY_DIR}/mrblib_irep.c)

#mrbgems/mruby-minitest/autorun.rb
#mrbgems/mruby-minitest/benchmark.rb
#mrbgems/mruby-minitest/mock.rb
#mrbgems/mruby-minitest/pride.rb
#mrbgems/mruby-minitest/spec.rb

#CONCAT_FILES(${CMAKE_CURRENT_BINARY_DIR}/minitest_all.rb
#  mrbgems/mruby-minitest/unit.rb
#  )
#MRBC(mrb_mruby_minitest_irep
#  ${CMAKE_CURRENT_BINARY_DIR}/minitest_all.rb
#  ${CMAKE_CURRENT_BINARY_DIR}/minitest_irep.c)

set(MRUBY_GEM_SRCS
  mrbgems/mrbgems_all.c
  mrbgems/mrbgems_registration_list.h

  mrbgems/mruby-math/src/math.c
  mrbgems/mruby-numeric-ext/src/numeric_ext.c
  # mrbgems/mruby-require/src/mrb_require.c
  mrbgems/mruby-sprintf/src/kernel.c
  mrbgems/mruby-sprintf/src/sprintf.c
  mrbgems/mruby-struct/src/struct.c
  mrbgems/mruby-time/src/time.c

  # ${CMAKE_CURRENT_BINARY_DIR}/minitest_irep.c
  # ${CMAKE_CURRENT_BINARY_DIR}/mrbgem_struct_irep.c
  )

unset(MIRB_EXE_LIBS)
add_executable(mirb_exe
  tools/mirb/mirb.c
  mrblib_irep.c
  mrblib/init_mrblib.c
  ${MRUBY_GEM_SRCS})

if (READLINE_FOUND)
  include_directories(${READLINE_INCLUDE_DIR})
  set_property(TARGET mirb_exe
    APPEND PROPERTY COMPILE_DEFINITIONS "ENABLE_READLINE")
  list(APPEND MIRB_EXE_LIBS ${READLINE_LIBRARY})
endif (READLINE_FOUND)

set_target_properties(mirb_exe PROPERTIES OUTPUT_NAME mirb)
target_link_libraries(mirb_exe mruby ${MIRB_EXE_LIBS})

add_executable(mruby_exe
  tools/mruby/mruby.c
  mrblib_irep.c
  mrblib/init_mrblib.c
  ${MRUBY_GEM_SRCS})
set_target_properties(mruby_exe PROPERTIES OUTPUT_NAME mruby)
target_link_libraries(mruby_exe mruby ${MIRB_EXE_LIBS})


