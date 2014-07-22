include_directories(${PROJECT_BINARY_DIR}/simple)

add_executable(Client Client.cc)
target_link_libraries(Client zqrpc)

add_executable(Server Server.cc)
target_link_libraries(Server zqrpc)
