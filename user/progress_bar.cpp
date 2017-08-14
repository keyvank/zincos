#include "api.h"

void function();
extern "C" int main() {
  print("Press enter to start!\n");
  getch();
  print("Loading...\n");
  for(unsigned int i = 0; i < 80; i++) {
    for(unsigned int j = 0; j < 10000000; j++) { }
    print("#");
  }
  return 0;
}
