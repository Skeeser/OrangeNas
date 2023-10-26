#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <dirent.h>

#define SERVER_IP "127.0.0.1"
#define PORT 8888
#define BUFFER_SIZE 1024

#define PRINTF_LOGO printf( \
"\
 $$$$$$\\  $$\\       $$$$$$\\ $$$$$$$$\\ $$\\   $$\\ $$$$$$$$\\\n\
$$  __$$\\ $$ |      \\_$$  _|$$  _____|$$$\\  $$ |\\__$$  __|\n\
$$ /  \\__|$$ |        $$ |  $$ |      $$$$\\ $$ |   $$ |\n\
$$ |      $$ |        $$ |  $$$$$\\    $$ $$\\$$ |   $$ |\n\
$$ |      $$ |        $$ |  $$  __|   $$ \\$$$$ |   $$ |\n\
$$ |  $$\\ $$ |        $$ |  $$ |      $$ |\\$$$ |   $$ |\n\
\\$$$$$$  |$$$$$$$$\\ $$$$$$\\ $$$$$$$$\\ $$ | \\$$ |   $$ |\n\
 \\______/ \\________|\\______|\\________|\\__|  \\__|   \\__|\n\
")
void cl(void)
{
	char b;
	do
	{
	scanf("%c",&b);
	}
	while(b !='\n');
}

void clearBuff(char * buffer){
    int i;
     // 清空数组
    for(i = 0; i < BUFFER_SIZE; i++){
        buffer[i] = '\0';
    }
}


void getFileDir(char * buffer){
    int i;
    int file_count = 0;
    struct dirent *de;
    int buffer_ptr = 0;
    // 先尝试打开当前目录 
    DIR *dir = opendir(".");

    // 清空数组
    for(i = 0; i < BUFFER_SIZE; i++){
        buffer[i] = '\0';
    }

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

int main() {
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};
    char file_names[BUFFER_SIZE] = {0};

    PRINTF_LOGO;

    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // 将IP地址转换为网络字节序
    if(inet_pton(AF_INET, SERVER_IP, &(serv_addr.sin_addr)) <= 0) {
        printf("\nip地址无效或不支持\n");
        return -1;
    }

   

    while(1){
        // 创建客户端套接字
        if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            printf("\n创建客户端套接字失败\n");
            return -1;
        }



        // 连接到服务器
        if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
            printf("\n连接服务器失败\n");
            return -1;
        }

        printf("\n\n<--------------------------------->\n");
        printf("请输入想要进行的操作(按数字):\n1--从服务器获取文件\n2--发送文件到服务器\n3--退出客户端\n=>");
        int sel_num = 0;
        int status = scanf("%d", &sel_num);
      
        if(!(sel_num == 1 || sel_num == 2 || sel_num == 3)){
            cl();//清除输入缓冲区的错误数据 
            printf("请输入正确的操作\n\n");
            continue;
        }else if(sel_num == 3){
            char end_connect[] = "end_connect";
            send(sock, end_connect, strlen(end_connect), 0);
            close(sock);
            break;
        }

        // 从服务器获取文件
        if(sel_num == 1){
            // 尝试发送
            char file_list[] = "get_file_list";
            send(sock, file_list, strlen(file_list), 0);
            clearBuff(file_names);

            // 从服务器接收文件列表
            valread = read(sock, file_names, BUFFER_SIZE);
            printf("从服务器获取到的文件列表:\n%s\n", file_names);

            clearBuff(file_names);
            printf("请选择想要从服务器获取的文件名：");
            scanf("%s", file_names);
            printf("\n");
            char get_file[] = "to_get_file";
            send(sock, get_file, strlen(get_file), 0);
            send(sock, file_names, strlen(file_names), 0);
     
            // 获取文件
            clearBuff(buffer);
            valread = read(sock, buffer, BUFFER_SIZE);
            if(strncmp(buffer, "have_file", 9) == 0){
                printf("服务器找到了该文件\n");
                 // 读取套接字数据并写入文件
                FILE *file = fopen(file_names, "wb");
                if (file == NULL) {
                    printf("文件打开失败\n");
                    close(sock);
                    continue;
                }
                clearBuff(buffer);
                while ((valread = read(sock, buffer, BUFFER_SIZE)) > 0) {
                   
                    // printf("接收到的文件内容为：%s\n\n", buffer);
                    fwrite(buffer, 1, valread, file);
                    clearBuff(buffer);
                    fclose(file);
                }            
                printf("接收文件完毕");
            }else if(strncmp(file_names, "have_no_file", 12) == 0){
                printf("服务器没有该文件\n");
                // 关闭套接字
                close(sock);
                continue;
            }
        }else if(sel_num == 2){
            char send_file[] = "send_file";
            send(sock, send_file, strlen(send_file), 0);
            // 发送文件列表
            getFileDir(file_names);
            printf("当前目录的文件列表:\n%s\n", file_names);
            clearBuff(file_names);
            printf("请选择想要发送给服务器的文件名：");
            scanf("%s", file_names);
            printf("\n");

            FILE *file = fopen(file_names, "rb");
            if (file == NULL) {
                printf("文件打开失败");
                close(sock);
                continue;
            }
            // 发送文件名
            send(sock, file_names, strlen(file_names), 0);
            // 读取文件并通过套接字发送
            int bytesRead;
            clearBuff(buffer);
            while ((bytesRead = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
                // printf("发送的文件内容为：%s\n\n", buffer);
                if (send(sock, buffer, bytesRead, 0) < 0) {
                    printf("发送文件失败");
                    close(sock);
                    continue;
                }
            }
            
            if (bytesRead < 0) {
                printf("读取失败");
                close(sock);
                continue;
            }
            fclose(file);
           
        }
        // 关闭套接字
        close(sock);

    } 
    
    return 0;
}
