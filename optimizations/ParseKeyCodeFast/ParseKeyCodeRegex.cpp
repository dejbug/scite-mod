#include <cstdlib>
#include <regex>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

static std::regex const rex{R"((Ctrl\+)?(Shift\+)?(Alt\+)?(Left|Right|Up|Down|Insert|End|Home|Enter|Space|Tab|KeypadPlus|KeypadMinus|KeypadMultiply|KeypadDivide|Escape|Delete|PageUp|PageDown|Win))"};

long ParseKeyCodeRegex(const char *s) {
	std::cmatch mm;
	if (std::regex_search(s, mm, rex)) {
		return 1;
	}
	return 0;
}
