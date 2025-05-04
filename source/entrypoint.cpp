#include <stdio.h>

#include <Windows.h>

int main() {
	char const* text = "The gay foxes are coming for youuu :3";
	printf("%s\n", text);

	MessageBoxA(NULL, "Gay", "Foxes", MB_RIGHT | MB_ICONWARNING | MB_HELP);
}