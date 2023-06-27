#include <stdio.h>
#include <wchar.h>
#include <locale.h>

int main(void) {
    setlocale(LC_ALL, "en_US.UTF-8");
    wchar_t box = 0x2592;
    wprintf(L"%lc\n", box);  // or simply printf("%lc\n", box);
    return 0;
}
