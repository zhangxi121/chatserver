
# 设置需要编译的源文件列表
# 定义了一个 SRC_LIST 变量, 包含了该目录下所有的源文件,
aux_source_directory(. SRC_LIST)  
aux_source_directory(./db DB_LIST)  
aux_source_directory(./model MODEL_LIST)  
aux_source_directory(./redis REDIS_LIST)  

# 生成可执行文件 server, 由  ${SRC_LIST} 所有定义的源文件编译而来,
add_executable(ChatServer ${SRC_LIST} ${DB_LIST} ${MODEL_LIST} ${REDIS_LIST})


# 指定可执行文件链接时, 需要依赖的库文件,
target_link_libraries(ChatServer  muduo_net muduo_base mysqlclient hiredis pthread)

# 
# 


