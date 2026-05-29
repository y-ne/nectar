#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
  assert(system("adb --version >nul 2>&1") == 0 && "adb not found");
  assert(system("scrcpy --version >nul 2>&1") == 0 && "scrcpy not found");

  return 0;
}
