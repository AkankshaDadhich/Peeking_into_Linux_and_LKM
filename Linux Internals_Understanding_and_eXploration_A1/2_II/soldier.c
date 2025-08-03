#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/ioctl.h>



#define IOCTL_DRIVER_NAME "/dev/ioctlChgParent"


#define IOCTL_CHG_PARENT _IOW('q', 5, struct ioctl_data)


// Include header or define the IOCTL call interface and devide name




struct ioctl_data {
    pid_t parent_pid;
    pid_t process_id;
	 
};

//**************************************************

int open_driver(const char* driver_name) {

    int fd_driver = open(driver_name, O_RDWR);
    if (fd_driver == -1) {
        perror("ERROR: could not open driver");
    }

	return fd_driver;
}

void close_driver(const char* driver_name, int fd_driver) {

    int result = close(fd_driver);
    if (result == -1) {
        perror("ERROR: could not close driver");
    }
}


int main(int argc, char** argv) {

    if (argc != 2) {
        printf("Usage: %s <parent_pid>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    pid_t parent_pid = atoi(argv[1]);
    struct ioctl_data data;
    data.process_id = getpid();
    data.parent_pid =  parent_pid;

    // open ioctl driver
    int fd_ioctl = open_driver(IOCTL_DRIVER_NAME);

    
    // call ioctl with parent pid as argument to change the parent
    // I am sending a structure contatining the current pid of the running process and the parent_pid taken in the argument
     if (ioctl(fd_ioctl, IOCTL_CHG_PARENT, &data) == -1) {
        perror("ioctl");
        close(fd_ioctl);
        exit(EXIT_FAILURE);
    }
    
	//printf("Parent changed with the given parent_id");
    // close ioctl driver
    close_driver(IOCTL_DRIVER_NAME, fd_ioctl);

	return EXIT_SUCCESS;
}


