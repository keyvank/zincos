#include "api.h"

#define MAX_LENGTH (256)

void function();
void thread1(){
    print("This is from first thread!\n");
    exit(223);
}
void thread2(){
    print("This is from second thread!\n");
    exit(438);
}
extern "C" int main() {
  print("Hello World!\n");
  print("This is Userland!!!\n");
  function();
  print("This is from main thread!\n");
  thread(thread1);
  thread(thread2);
  char *alloc = reinterpret_cast<char *>(alloc_pages(2));
  *(alloc + 8191) = 'a';

  char cmd[MAX_LENGTH];

  while(true){
    read_line(cmd);
    print("You said: ");
    print(cmd);
    print("!\n");
  }
  exit(123);
  return 0;
}
void function(){
  print("And I am a function!\n");
}
