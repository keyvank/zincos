#include "api.h"

#define MAX_LENGTH (256)

void function();
extern "C" int main() {
  print("Hello World!\n");
  print("This is Userland!!!\n");
  char cmd[MAX_LENGTH];
  function();
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
