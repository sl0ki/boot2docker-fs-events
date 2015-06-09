#include <stdio.h>
#include <string.h>

#include "fsevent.h"
#include "network.h"


// struct finfo {
//   char *filename;
//   int flag;
//   int mode;
// };

char *host_path, *virt_path, *virt_ip;


void handler(char *file) {

  char virt_file[512];

  strcat(strcpy(virt_file, virt_path), &file[strlen(host_path)]);

  printf("chnaged: %s\n", virt_file);
  udp_send(virt_file, strlen(virt_file));
}

int main(int argc, char **argv) {

    if (argc < 3) {
        printf("No paths specified.\n");
        printf("Usage: %s ip  /host/shared/path  /boot2dokcer/virtual/path\n", argv[0]);
        exit(1);
    }


    virt_ip = argv[1];
    host_path = argv[2];
    virt_path = argv[3];

    // Open UDP socket
    udp_init(virt_ip, 49717);

    // Init FsEvent watcher loop
    fsevent_init(host_path, &handler);

    return (0);
}
