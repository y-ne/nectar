#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_DEV 16
#define MAX_SER 128

#ifdef _WIN32
#define popen _popen
#define pclose _pclose
#define NUL "nul"
#else
#define NUL "/dev/null"
#endif

int get_serials(char serials[][MAX_SER]) {
  FILE *fp = popen("adb devices", "r");
  assert(fp);

  char line[256];
  int n = 0;

  fgets(line, sizeof(line), fp);

  for (; fgets(line, sizeof(line), fp) && n < MAX_DEV;) {
    char *tab = strstr(line, "\tdevice");
    if (tab) {
      *tab = '\0';
      strncpy(serials[n], line, MAX_SER - 1)[MAX_SER - 1] = 0;
      n++;
    }
  }
  pclose(fp);
  return n;
}

int main() {
  char serials[MAX_DEV][MAX_SER];
  char cmd[128];

  snprintf(cmd, sizeof(cmd), "adb --version >%s 2>&1", NUL);
  assert(system(cmd) == 0 && "adb not found");
  snprintf(cmd, sizeof(cmd), "scrcpy --version >%s 2>&1", NUL);
  assert(system(cmd) == 0 && "scrcpy not found");

  int n = get_serials(serials);
  for (int i = 0; i < n; i++)
    printf("%s\n", serials[i]);
  return 0;
}
