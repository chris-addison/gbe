set(SDL2_INCLUDE_DIRS "${CMAKE_CURRENT_LIST_DIR}/include/SDl2")

set(SDL2_LIBRARIES "${CMAKE_CURRENT_LIST_DIR}/lib/libSDL2.a;${CMAKE_CURRENT_LIST_DIR}/lib/libSDL2main.a")

string(STRIP "${SDL2_LIBRARIES}" SDL2_LIBRARIES)

set(SDL2_LIBRARIES -lmingw32 ${SDL2_LIBRARIES} -lm -ldinput8 -ldxguid -ldxerr8 -luser32 -lgdi32 -lwinmm -limm32 -lole32 -loleaut32 -lshell32 -lversion -luuid)