find_package(BISON REQUIRED)
find_package(Readline)

include(ConcatFiles)
include(CCompilerOptions)
include(MRubyCompiler)

# Reset CFLAGS and our include directories
#set(SAVED_C_FLAGS ${CMAKE_C_FLAGS})
#get_property(CMAKE_C_FLAGS GLOBAL PROPERTY ORIG_CMAKE_C_FLAGS)
set_property(DIRECTORY PROPERTY INCLUDE_DIRECTORIES)

set(MRUBY_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(MRUBY_VM_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set_property(GLOBAL PROPERTY MRUBY_INCLUDE_DIR ${MRUBY_INCLUDE_DIR})
set_property(GLOBAL PROPERTY MRUBY_VM_INCLUDE_DIR ${MRUBY_VM_INCLUDE_DIR})

include_directories(
  ${MRUBY_INCLUDE_DIR}
  ${MRUBY_VM_INCLUDE_DIR}
  )

#CCompilerOptions(
#  LANG C99
#  DEBUG ${DEBUG_LEVEL}
#  WARNINGS ALL)

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
set_target_properties(mruby PROPERTIES COMPILE_FLAGS "-Wno-pedantic")

add_executable(mrbc_exe tools/mrbc/mrbc.c)
set_target_properties(mrbc_exe PROPERTIES OUTPUT_NAME mrbc)
target_link_libraries(mrbc_exe mruby)

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

add_library(mrblib STATIC mrblib_irep.c mrblib/init_mrblib.c)

#set(CMAKE_C_FLAGS ${SAVED_C_FLAGS})

