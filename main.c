#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_DEV 16
#define MAX_SER 128

int get_serials(char serials[][MAX_SER]) {
  FILE *fp = _popen("adb devices", "r");
  assert(fp);

  char line[256];
  int n = 0;

  fgets(line, sizeof(line), fp);

  for (; fgets(line, sizeof(line), fp) && n < MAX_DEV;) {
    char *tab = strstr(line, "\tdevice");
    if (tab)
      *tab = '\0', strncpy(serials[n++], line, MAX_SER);
  }
  _pclose(fp);
  return n;
}

int main() {
  char serials[MAX_DEV][MAX_SER];
  int n, i;
  assert(system("adb --version >nul 2>&1") == 0 && "adb not found");
  assert(system("scrcpy --version >nul 2>&1") == 0 && "scrcpy not found");

  n = get_serials(serials);
  for (i = 0; i < n; i++)
    printf("%s\n", serials[i]);
  return 0;
}
