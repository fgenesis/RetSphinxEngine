set(game_src
GameEngine.cpp
GameEngine.h
GameEngine_color.cpp
GameEngine_events.cpp
GameEngine_xmlparse.cpp
DebugUI.cpp
DebugUI.h
luafuncs.cpp
main.cpp
)

add_executable(game ${game_src})
target_link_libraries(game engine)
