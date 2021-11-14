cmake_minimum_required(VERSION 3.0)
project(chat)

# 配置编译选项,  -o  -g  
# set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -g -O2)
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -g)
add_compile_options(-std=c++14)

# 配置最终的可执行文件的输出路径,
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)


# 配置头文件的搜索路径,
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/include/server)
include_directories(${PROJECT_SOURCE_DIR}/include/server/db)
include_directories(${PROJECT_SOURCE_DIR}/include/server/model)
include_directories(${PROJECT_SOURCE_DIR}/include/server/redis)
# include_directories(${PROJECT_SOURCE_DIR}/include/client)
include_directories(${PROJECT_SOURCE_DIR}/thirdparty)





# 指定搜索的子目录,
# 加载子目录,
add_subdirectory(src) 
# add_subdirectory(./test/test_muduo)

