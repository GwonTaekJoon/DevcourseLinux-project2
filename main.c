#include <dirent.h>  // 디렉토리 처리를 위한 헤더 파일 
#include <stdio.h>  // 표준 입출력 처리를 위한 헤더 파일
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

void print_frame(){
    printf("%6s","PID");
    printf("%15s","TTY_NUMBER");
    printf("%13s","TTY_NAME");
    printf("%6s","TIME");
    printf("%6s","CMD");

    printf("\n");

}
char* get_tty_number(int pid){
    char process_dir[100];
    snprintf(process_dir, sizeof(process_dir), "/proc/%d/stat", pid);
    FILE *fd = fopen(process_dir, "r");

    if(fd == NULL) {
        fprintf(stderr, "error while opening process dir");
        return NULL;
    }

    char buffer[1024];
    if(fgets(buffer, sizeof(buffer), fd) != NULL) {
        char *token = strtok(buffer, " "); // process의 stat정보 whitespace로 토큰화
        //tty데이터는 stat의 여러 필드 중 7번째 필드 값
        for(size_t i = 0 ; i < 6; ++i) {
            token = strtok(NULL, " ");
        }
        fclose(fd);
        
        if(token != NULL) {
            return strdup(token); //동적할당하고 반환
        }
    }



    return NULL;
}


char *tty_number_to_name(char *tty_number){
    char tty_name[100];
    char *tty_name_ptr = NULL;
    DIR *pts_dir = opendir("/dev/pts");

    if(pts_dir != NULL) {
        struct dirent *entry;
        while((entry = readdir(pts_dir)) != NULL) {
            if(entry->d_type == DT_CHR) {
              if(strcmp(entry->d_name, tty_number) == 0) {
                tty_name_ptr = strdup(entry->d_name);
                break;
              }
            }

        }
        closedir(pts_dir);
    }
    
    return tty_name_ptr;
}

int main(void) {
    struct dirent *de;  // dirent 구조체의 포인터. 이 구조체는 디렉토리의 항목을 나타냄.
    
    int input_pid;
    DIR *dr = opendir("/proc");  // '/proc' 디렉토리를 열고, 디렉토리 스트림의 포인터를 반환.

    print_frame();

    // 디렉토리 열기가 실패하면 오류 메시지를 출력하고 프로그램 종료
    if (dr == NULL) {
        printf("Could not open /proc directory" );
        return 0;
    }
    
    
    // readdir() 함수를 사용해 디렉토리 항목을 순차적으로 읽음. 
    // 더 이상 읽을 항목이 없을 때까지 반복.
    while ((de = readdir(dr)) != NULL){
        
        if(isdigit(de->d_name[0])) {
            input_pid = atoi(de->d_name);
            printf("%6d", input_pid);  // 각 항목의 이름을 출력.
            char *tty_number = get_tty_number(input_pid); // tty 정보 숫자로 가져오기
            printf("%15s", tty_number);


            if(tty_number != NULL) {
                char *tty_name = tty_number_to_name(tty_number);
                if(tty_name != NULL) {
                    printf("%13s", tty_name);
                    free(tty_name);
                } else {
                    printf("%13s", "N/A");
                }
                free(tty_number);
            } else{
                printf("%13s", "N/A");
            }


            
           printf("\n");
        }
            
    }

    closedir(dr);  // 디렉토리 스트림을 닫음.
    return 0;
}