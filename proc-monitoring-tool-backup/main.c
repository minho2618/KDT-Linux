#include <dirent.h> // 디렉토리 처리를 위한 헤더 파일
#include <stdio.h>  // 표준 입출력 처리를 위한 헤더 파일
#include <unistd.h>
#include <ncurses.h>
#include <fcntl.h>
#include <string.h>

#define BUF_SIZE 99999

int check_file(char *process)
{
    if (process == NULL)
        return 1;

    if (strcmp(process, ".") == 0 || strcmp(process, "..") == 0)
        return 1;

    if (strspn(process, "0123456789") != strlen(process))
        return 1;

    return 0;
}

int main(void)
{
    struct dirent *de; // dirent 구조체의 포인터. 이 구조체는 디렉토리의 항목을 나타냄.
    int fd;            // File Descritptor
    unsigned int counter = 0;

    char buf[BUF_SIZE] = {
        0,
    };

    initscr();

    DIR *dr = opendir("/proc"); // '/proc' 디렉토리를 열고, 디렉토리 스트림의 포인터를 반환.

    // 디렉토리 열기가 실패하면 오류 메시지를 출력하고 프로그램 종료
    if (dr == NULL)
    {
        perror("Could not open /proc directory");
        return 0;
    }

    

    while (1)
    {
        refresh();
        printw("PID TTY CMD\n");
        // readdir() 함수를 사용해 디렉토리 항목을 순차적으로 읽음.
        // 더 이상 읽을 항목이 없을 때까지 반복.
        while ((de = readdir(dr)) != NULL)
        {
            char pathname[1024] = "/proc/";
            char pid_stat[] = "/stat";

            char process[256];
            if (strlen(de->d_name) < sizeof(process))
            {
                strcpy(process, de->d_name);
            }
            else
            {
                perror("Too Long\n");
                continue;
            }

            // 해당 항목이 pid 인지 미리 걸러내기
            if (check_file(process) == 1)
                continue;

            // printf("%s : ", process); // 각 항목의 이름을 출력.

            // pathname에 process를 추가하기 전에 크기 검사
            size_t available_size = BUF_SIZE - strlen(pathname) - 1;
            if (strlen(process) + strlen(pid_stat) + 1 < available_size)
            {
                strcat(pathname, process);
                strcat(pathname, pid_stat);
            }
            else
            {
                // 경로가 너무 길 경우 처리
                continue; // 또는 오류 처리
            }

            // printf("%s\n", pathname); // 항목의 경로를 출력

            if ((fd = open(pathname, O_RDONLY)) < 0)
            {
                perror("Fail to read process!\n");
                continue;
            }

            char *tokens[64];

            ssize_t bytes_read = read(fd, buf, BUF_SIZE - 1);
            if (bytes_read > 0)
            {
                buf[bytes_read] = '\0'; // 널 종료 추가
                // printf("%s\n", buf);

                int i = 0;
                char *token = strtok(buf, " ");
                while (token != NULL)
                {
                    tokens[i++] = token;
                    token = strtok(NULL, " ");
                }

                // 나누어진 토큰들을 출력합니다.
                printw("%s\t%s\t%s\n", tokens[0], tokens[6], tokens[1]);
            }
            // memset 함수를 사용하여 전체 배열을 널 문자로 초기화합니다.
            // memset(buf, '\0', sizeof(buf));

            close(fd);
        }
        sleep(1);
        clear;
    }

    closedir(dr); // 디렉토리 스트림을 닫음

    endwin();

    return 0;
}