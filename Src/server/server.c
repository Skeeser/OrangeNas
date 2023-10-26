#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/wait.h>

// 可以修改以下的值，连接的端口，最大客户端数量，文件缓冲区大小
#define PORT 8888
#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

#define PRINTF_LOGO printf( \
" $$$$$$\\  $$$$$$$$\\ $$$$$$$\\  $$\\    $$\\ $$$$$$$$\\ $$$$$$$\\\n\
$$  __$$\\ $$  _____|$$  __$$\\ $$ |   $$ |$$  _____|$$  __$$\\\n\
$$ /  \\__|$$ |      $$ |  $$ |$$ |   $$ |$$ |      $$ |  $$ |\\\n\
\\$$$$$$\\  $$$$$\\    $$$$$$$  |\\$$\\  $$  |$$$$$\\    $$$$$$$  |\n\
 \\____$$\\ $$  __|   $$  __$$<  \\$$\\$$  / $$  __|   $$  __$$<\n\
$$\\   $$ |$$ |      $$ |  $$ |  \\$$$  /  $$ |      $$ |  $$ |\n\
\\$$$$$$  |$$$$$$$$\\ $$ |  $$ |   \\$  /   $$$$$$$$\\ $$ |  $$ |\n\
 \\______/ \\________|\\__|  \\__|    \\_/    \\________|\\__|  \\__|\n\
 ")                                                      
                                                             
                                                             
void clearBuff(char * buffer){
    int i;
     // 清空数组
    for(i = 0; i < BUFFER_SIZE; i++){
        buffer[i] = '\0';
    }
}


void getFileDir(char * buffer){
    
    int file_count = 0;
    struct dirent *de;
    int buffer_ptr = 0;
    // 先尝试打开当前目录 
    DIR *dir = opendir(".");

   clearBuff(buffer);

    if(dir == NULL){
        printf("无法打开当前目录\n");
        return;
    }

    // 遍历文件
    while ((de = readdir(dir)) != NULL) {
        if (de->d_type == DT_REG) {
            int len = strlen(de->d_name);

            if(buffer_ptr < BUFFER_SIZE){
                strncpy(buffer, de->d_name, len);
                buffer += len;
                *buffer = '\n';
                buffer++;
                buffer_ptr += len + 1;
                file_count++;
            }            
        }
    }

    // 关闭目录
    closedir(dir);
    printf("总共找到 %d 个文件\n", file_count);

}

void run() {
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    
    int opt = 1;
    
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};
    char file_names[BUFFER_SIZE] = {0};
    
   

    // 创建服务器套接字
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket创建错误");
        return;
    }

    // 设置套接字选项，允许多个客户端连接
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("设置套接字选项错误");
        return;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // 绑定服务器地址和端口
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror(" 绑定服务器地址和端口错误");
        return;
    }

    // 监听传入连接
    if (listen(server_fd, MAX_CLIENTS) < 0) {
        perror("监听错误");
        return;
    }

    // 等待客户端连接
    while (1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            printf("接收失败");
            return;
        }
        clearBuff(buffer);
        // 处理客户端请求
        while(valread = read(new_socket, buffer, BUFFER_SIZE) > 0){
            printf("从客户端获取的信息为: %s\n", buffer);
            
            // 处理各种操作
            if(strncmp(buffer, "get_file_list", 13) == 0){
                // 发送文件列表
                getFileDir(file_names);
                // printf("%s",file_names);
                send(new_socket, file_names, strlen(file_names), 0);
                
            }else if(strncmp(buffer, "to_get_file", 11) == 0){   // 获取文件
                clearBuff(file_names);
                valread = read(new_socket, file_names, BUFFER_SIZE);
                if(valread > 0){
                    FILE *file = fopen(file_names, "rb");
                    if (file == NULL) {
                    printf("文件打开失败");
                    char no_file[] = "have_no_file";
                    send(new_socket, no_file, strlen(no_file), 0);
                    continue;
                    }
                    char have_file[] = "have_file";
                    send(new_socket, have_file, strlen(have_file), 0);
                    // 读取文件并通过套接字发送
                    int bytesRead;
                    clearBuff(buffer);
                    while ((bytesRead = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
                        printf("发送的文件内容为：%s\n\n", buffer);
                        if (send(new_socket, buffer, bytesRead, 0) < 0) {
                            printf("发送文件失败");
                            return;
                        }
                    }
                    
                    if (bytesRead < 0) {
                        printf("读取失败");
                        return;
                    }
                    fclose(file);
                    break;
                }else{
                    printf("收到的文件名错误");
                    char no_file[] = "have_no_file";
                    send(new_socket, no_file, strlen(no_file), 0);
                }
            }else if(strncmp(buffer, "send_file", 9) == 0){   // 接受文件
            
                clearBuff(file_names);
                printf("客户端想要发送的文件名：");
                clearBuff(file_names);
                valread = read(new_socket, file_names, BUFFER_SIZE);
                printf("%s", file_names);
                printf("\n");
  
                // 获取文件
                clearBuff(buffer);

                // 读取套接字数据并写入文件
                FILE *file = fopen(file_names, "wb");
                if (file == NULL) {
                    printf("文件打开失败\n");
                    close(new_socket);
                    continue;
                }

                while ((valread = read(new_socket, buffer, BUFFER_SIZE)) > 0) {
                    // printf("接收到的文件内容为：%s\n\n", buffer);
                    fwrite(buffer, 1, valread, file);
                    clearBuff(buffer);
                    fclose(file);
                }            
                printf("接收文件完毕");
               
            }else if(strncmp(buffer, "end_connect", 11) == 0){ 
                break;
            }
            clearBuff(buffer);
            
        }
       
        close(new_socket);
    }

    return;
}


int main() {
    PRINTF_LOGO;

    while (1) {
        pid_t pid = fork();

        if (pid < 0) {
            // fork 失败，打印错误信息并退出程序
            perror("Fork failed");
            exit(1);
        } else if (pid == 0) {
            // 子进程执行服务端代码
            // 替换以下命令为你的服务端程序命令
            run();
            exit(1);
        } else {
            // 父进程等待子进程结束
            int status;
            waitpid(pid, &status, 0);

            // 检查子进程的退出状态
            if (WIFEXITED(status)) {
                // 子进程正常退出
                if (WEXITSTATUS(status) == 0) {
                    // 子进程返回值为0，表示正常结束
                    printf("子进程正常结束\n");
                    break;  // 跳出循环，程序结束
                } else {
                    // 子进程返回值非0，表示发生错误，重新启动程序
                    printf("子进程发生错误，重新启动程序\n");
                    continue;  // 继续下一次循环，重新启动程序
                }
            } else {
                // 子进程异常退出，重新启动程序
                printf("子进程异常退出，重新启动程序\n");
                continue;  // 继续下一次循环，重新启动程序
            }
        }
    }

    return 0;
}