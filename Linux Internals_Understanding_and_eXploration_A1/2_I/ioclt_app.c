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

#include "ioctl.h"


// used when ioctl is used so this structure is passed as a argument to send to kernal 
struct write_data {
    unsigned long phys_addr;
    uint8_t byte_value;
};

#define IOCTL_DRIVER_NAME "/dev/ioctl"

//used when ioctl is used so this structure is passed as a argument to send to kernal. This i have used to get the physical address
struct ioctl_data {
    unsigned long virt_addr;
    pid_t process_id;
	  unsigned long phys_addr;
};


int open_driver(const char* driver_name);
void close_driver(const char* driver_name, int fd_driver);

int open_driver(const char* driver_name) {

    printf("* Open Driver\n");

    int fd_driver = open(driver_name, O_RDWR);
    if (fd_driver == -1) {
        printf("ERROR: could not open \"%s\".\n", driver_name);
        printf("    errno = %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }

	return fd_driver;
}

void close_driver(const char* driver_name, int fd_driver) {

    printf("* Close Driver\n");

    int result = close(fd_driver);
    if (result == -1) {
        printf("ERROR: could not close \"%s\".\n", driver_name);
        printf("    errno = %s\n", strerror(errno));
        exit(EXIT_FAILURE);
    }
}


int main(void) {

  int fd_ioctl = open_driver(IOCTL_DRIVER_NAME);

  if (fd_ioctl == -1) {
        perror("open");
        return 1;
    }
  int a = 5;
  struct ioctl_data data;
  data.virt_addr = &a;  //  virtual address
  data.process_id = getpid();
   if (ioctl(fd_ioctl, IOCTL_VIR_TO_PHY, &data) == -1) {
        perror("ioctl");
        close(fd_ioctl);
        exit(EXIT_FAILURE);
    }

    // Now data.virt_addr, data.phys_addr, and data.process_id are valid
  //  printf("Virtual Address: 0x%lx\n", data.virt_addr);
   // printf("Physical Address: 0x%lx\n", data.phys_addr);

  
    printf("The Physical address of the corresponding virtal address %lx of currently running process is %lx\n" , data.virt_addr , data.phys_addr );
	
     
    struct write_data write_data;
    write_data.phys_addr = data.phys_addr;
    write_data.byte_value = 10;
    printf("The initial value of a is %d\n" , a);
    // Call the IOCTL_WRITE_TO_PHY ioctl
    if (ioctl(fd_ioctl, IOCTL_WRITE_TO_PHY, &write_data) == -1) {
        perror("ioctl");
        close(fd_ioctl);
        exit(EXIT_FAILURE);
    }
    
    printf("The new value of a is %d \n" , a);
    close_driver(IOCTL_DRIVER_NAME, fd_ioctl);
	return EXIT_SUCCESS;
}