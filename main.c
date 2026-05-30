#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

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

int diff_devices(char serials[][MAX_SER], int n, char unmirrored[][MAX_SER]) {
	int count = 0;
	char cmd[512];

	for (int i = 0; i < n; i++) {
		snprintf(
			cmd, sizeof(cmd),
			"wmic process where \"name='scrcpy.exe'\" get commandline 2>%s | "
			"findstr /c:\"%s\" >%s",
			NUL, serials[i], NUL);
		if (system(cmd) != 0)
			strcpy(unmirrored[count++], serials[i]);
	}
	return count;
}

void launch_scrcpy(const char *serial) {
	char cmd[256];
	snprintf(cmd, sizeof(cmd), "scrcpy -s %s -b 2M -m 1024", serial);
	STARTUPINFO si = {0};
	PROCESS_INFORMATION pi = {0};
	si.cb = sizeof(si);
	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_SHOW;
	CreateProcess(NULL, cmd, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL,
				  &si, &pi);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
}

int main() {
	assert(system("adb --version >nul 2>&1") == 0 && "adb not found");
	assert(system("scrcpy --version >nul 2>&1") == 0 && "scrcpy not found");

	char serials[MAX_DEV][MAX_SER];
	char unmirrored[MAX_DEV][MAX_SER];

	for (;;) {
		int n = get_serials(serials);
		int m = diff_devices(serials, n, unmirrored);
		for (int i = 0; i < m; i++)
			launch_scrcpy(unmirrored[i]);
		Sleep(5000);
	}
}
