set(CMAKE_CXX_STANDARD 17)

file(GLOB LIBMUX_BASE_SRC libmux.cpp)

file(GLOB NETMUX_BASE_SRC  _build.cpp alarm.cpp alloc.cpp attrcache.cpp boolexp.cpp
                      bsd.cpp command.cpp comsys.cpp conf.cpp cque.cpp create.cpp db.cpp
                      db_rw.cpp eval.cpp file_c.cpp flags.cpp funceval.cpp funceval2.cpp
                      functions.cpp funmath.cpp game.cpp help.cpp htab.cpp local.cpp log.cpp
                      look.cpp mail.cpp match.cpp mathutil.cpp mguests.cpp modules.cpp move.cpp
                      muxcli.cpp netcommon.cpp object.cpp predicates.cpp player.cpp player_c.cpp
                      plusemail.cpp powers.cpp quota.cpp rob.cpp pcre.cpp set.cpp sha1.cpp
                      speech.cpp stringutil.cpp strtod.cpp svdrand.cpp svdhash.cpp timer.cpp
                      timeabsolute.cpp timedelta.cpp timeparser.cpp timeutil.cpp timezone.cpp
                      unparse.cpp utf8tables.cpp vattr.cpp walkdb.cpp wild.cpp wiz.cpp)

file (GLOB VER_SRC version.cpp)

file (GLOB SLAVE_SRC slave.cpp slave.h)

add_library(libmux SHARED ${LIBMUX_BASE_SRC})
target_include_directories(libmux PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
set_target_properties(libmux PROPERTIES OUTPUT_NAME libmux)

add_executable(netmux ${NETMUX_BASE_SRC} ${NETMUX_BASE_HDRS} ${VER_SRC})
target_link_libraries(netmux WS2_32 libmux)
target_include_directories(netmux PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

if (!WIN32)
add_executable(slave ${SLAVE_SRC} ${SLAVE_HDRS})
target_include_directories(slave PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
endif()

add_subdirectory(modules)