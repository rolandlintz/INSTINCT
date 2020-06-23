option(ENABLE_CPPCHECK "Enable static analysis with cppcheck" OFF)
option(ENABLE_CLANG_TIDY "Enable static analysis with clang-tidy" ON)
option(ENABLE_INCLUDE_WHAT_YOU_USE "Enable static analysis with include-what-you-use" OFF)

if(ENABLE_CPPCHECK)
  find_program(CPPCHECK cppcheck)
  if(CPPCHECK)
    set(CMAKE_CXX_CPPCHECK
        ${CPPCHECK}
        --error-exitcode=1
        --suppress=missingInclude
        --suppress=unmatchedSuppression
        --enable=warning
        --enable=style
        --enable=performance
        --enable=portability
        --enable=information
        --enable=missingInclude
        --inline-suppr
        --inconclusive
        # --project=${CMAKE_CURRENT_SOURCE_DIR}/compile_commands.json
        -i${CMAKE_SOURCE_DIR}/lib
        -j4)
  else()
    message(SEND_ERROR "cppcheck requested but executable not found")
  endif()
endif()

if(ENABLE_CLANG_TIDY)
  find_program(CLANGTIDY clang-tidy-10)
  if(CLANGTIDY)
    set(CMAKE_CXX_CLANG_TIDY ${CLANGTIDY} -extra-arg=-Wno-unknown-warning-option)
  else()
    message(SEND_ERROR "clang-tidy requested but executable not found")
  endif()
endif()

if(ENABLE_INCLUDE_WHAT_YOU_USE)
  find_program(INCLUDE_WHAT_YOU_USE include-what-you-use)
  if(INCLUDE_WHAT_YOU_USE)
    set(CMAKE_CXX_INCLUDE_WHAT_YOU_USE ${INCLUDE_WHAT_YOU_USE})
  else()
    message(SEND_ERROR "include-what-you-use requested but executable not found")
  endif()
endif()
