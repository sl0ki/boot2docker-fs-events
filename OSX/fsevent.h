#include <libgen.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>

#include <stdio.h>

#include <CoreFoundation/CoreFoundation.h>
#include <CoreServices/CoreServices.h>

#ifndef NOTIFYWAIT_H
#define NOTIFYWAIT_H

#include <CoreFoundation/CoreFoundation.h>
#include <CoreServices/CoreServices.h>

typedef struct {
    size_t len;
    size_t size;
    char **paths;
} file_paths_t;


void add_file(file_paths_t *file_paths, char *path);

void event_cb(ConstFSEventStreamRef streamRef,
              void *ctx,
              size_t count,
              void *paths,
              const FSEventStreamEventFlags flags[],
              const FSEventStreamEventId ids[]);

#endif


void (*callback)(char *path);

void event_cb(ConstFSEventStreamRef streamRef,
              void *ctx,
              size_t count,
              void *paths,
              const FSEventStreamEventFlags flags[],
              const FSEventStreamEventId ids[]) {

    file_paths_t *file_paths = (file_paths_t *)ctx;
    size_t i;
    size_t ignored_paths = 0;

    for (i = 0; i < count; i++) {
        char *path = ((char **)paths)[i];
        /* flags are unsigned long, IDs are uint64_t */
        // printf("Change %llu in %s, flags %lu\n", ids[i], path, (long)flags[i]);
        (*callback)(path);
        // size_t j;
        // for (j = 0; j < file_paths->len; j++) {
        //     char *file_path = file_paths->paths[j];
        //     // printf("%s %s\n", file_path, path);
        //     // if (strcmp(file_path, path) == 0) {
        //     //     printf("File %s changed.\n", file_path);
        //     //     FSEventStreamStop((FSEventStreamRef)streamRef);
        //     //     FSEventStreamScheduleWithRunLoop(streamRef, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);
        //     //     exit(0);
        //     // }
        // }
        /* TODO: this logic is wrong */
        ignored_paths++;
    }
    // if (count > ignored_paths) {
    //     /* OS X occasionally leaks event streams. Manually stop the stream just to make sure. */
    //     FSEventStreamStop((FSEventStreamRef)streamRef);
    //     FSEventStreamScheduleWithRunLoop(streamRef, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);
    //     exit(0);
    // }
}


void fsevent_init(char *path, char *cb) {

  callback = cb;

  CFMutableArrayRef paths = CFArrayCreateMutable(NULL, 1, NULL);
  CFStringRef cfs_path;
  char *dir_path;
  char *file_name;
  int i;
  int rv;
  struct stat s;

  file_paths_t *file_paths = malloc(sizeof(file_paths_t));
  file_paths->len = 0;
  file_paths->size = 2;
  file_paths->paths = malloc(file_paths->size * sizeof(char *));

  dir_path = NULL;
  path = realpath(path, NULL);

  /* At this point, path should be an absolute path. */
  printf("Path is %s\n", path);
  rv = stat(path, &s);
  if (rv < 0) {
      if (errno != 2) {
          fprintf(stderr, "Error %i stat()ing %s: %s\n", errno, path, strerror(errno));
          goto cleanup;
      }
      /* File doesn't exist yet. Watch parent dir instead. */
      s.st_mode = S_IFREG;
  }

  if (s.st_mode & S_IFDIR) {
      /* Yay a directory! */
      dir_path = path;
  } else {
      fprintf(stderr, "I don't know what to do with %u\n", s.st_mode);
      goto cleanup;
  }

  cfs_path = CFStringCreateWithCString(NULL, dir_path, kCFStringEncodingUTF8);
  printf("Watching %s\n", path);
  CFArrayAppendValue(paths, cfs_path); /* pretty sure I'm leaking this */

  // todo: replace err returned
  cleanup:
      ;
      if (dir_path != path) {
          free(dir_path);
      }
      free(path);


  FSEventStreamContext ctx = {
      0,
      file_paths,
      NULL,
      NULL,
      NULL
  };
  FSEventStreamRef stream;
  FSEventStreamCreateFlags flags = kFSEventStreamCreateFlagFileEvents;

  stream = FSEventStreamCreate(NULL, &event_cb, &ctx, paths, kFSEventStreamEventIdSinceNow, 0, flags);
  FSEventStreamScheduleWithRunLoop(stream, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);
  FSEventStreamStart(stream);

  CFRunLoopRun();
  /* We never get here */
}
