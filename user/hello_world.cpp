#include "api.h"

void function();
extern "C" int main() {
  print("Hello World!\n");
  print("This is Userland!!!\n");
  function();
  while(true);
  return 0;
}
void function(){
  print("And I am a function!\n");
}
