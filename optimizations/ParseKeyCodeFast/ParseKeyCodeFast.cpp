#include <cstdlib>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace SA {
	enum class KeyMod {
		Norm = 0,
		Shift = 1,
		Ctrl = 2,
		Alt = 4,
		Super = 8,
		Meta = 16,
	};
}

template<size_t N>
bool skip(char const *& s) {
	char const * const end = s + N;
	while (*s && s<end) ++s;
	return !!*s;
};

template<>
inline bool skip<3u>(char const *& s) {
	return *s && *(++s) && *(++s) && *(++s);
};

template<>
inline bool skip<4u>(char const *& s) {
	return *s && *(++s) && *(++s) && *(++s) && *(++s);
};

template<>
inline bool skip<5u>(char const *& s) {
	return *s && *(++s) && *(++s) && *(++s) && *(++s) && *(++s);
};

template<>
inline bool skip<6u>(char const *& s) {
	return *s && *(++s) && *(++s) && *(++s) && *(++s) && *(++s) && *(++s);
};

long ParseKeyCodeFast(const char *s) {
	if (!s || !*s) return 0;
	if (!s[1]) return VkKeyScan(*s) & 0xff;

	long mods = 0;

	if ('C'==*s || 'c'==*s) { // 'Ctrl+' or 'C'(+garbage).
		if (skip<4>(s) && '+'==*s++)
			mods |= (static_cast<long>(SA::KeyMod::Ctrl) << 16);
		else return VkKeyScan('c');
	}

	if ('S'==*s || 's'==*s) { // 'Shift', 'Space'.
		if (skip<5>(s) && '+'==*s++)
			mods |= (static_cast<long>(SA::KeyMod::Shift) << 16);
		else return mods | VK_SPACE;
	}

	if ('A'==*s || 'a'==*s) { // 'Alt+' or 'A'(+garbage).
		if (skip<3>(s) && '+'==*s++)
			mods |= (static_cast<long>(SA::KeyMod::Alt) << 16);
		else return mods | VkKeyScan('a');
	}

	if (!s[1]) return mods | (VkKeyScan(*s) & 0xff);

	if ('S'==*s || 's'==*s) return mods | VK_SPACE;
	if ('B'==*s || 'b'==*s) return mods | VK_BROWSER_BACK;
	if ('H'==*s || 'h'==*s) return mods | VK_HOME;
	if ('I'==*s || 'i'==*s) return mods | VK_INSERT;
	if ('L'==*s || 'l'==*s) return mods | VK_LEFT;
	if ('M'==*s || 'm'==*s) return mods | VK_APPS;
	if ('R'==*s || 'r'==*s) return mods | VK_RIGHT;
	if ('T'==*s || 't'==*s) return mods | VK_TAB;
	if ('U'==*s || 'u'==*s) return mods | VK_UP;
	if ('W'==*s || 'w'==*s) return mods | VK_LWIN;

	if ('V'==*s || 'v'==*s) // 'V1'-'V32767' .
		return mods | (std::atoi(&s[1]) & 0x7FFF);

	if ('P'==*s || 'p'==*s) { // 'PageUp' or 'PageDown' ?
		if (!skip<4>(s)) return mods | VkKeyScan('p');
		if ('U'==*s || 'u'==*s) return mods | VK_PRIOR;
		if ('D'==*s || 'd'==*s) return mods | VK_NEXT;
		return mods | VkKeyScan('p');
	}

	if ('D'==*s || 'd'==*s) { // 'Delete' or 'Down' ?
		if ('e'==s[1]) return mods | VK_DELETE;
		if ('o'==s[1]) return mods | VK_DOWN;
		return mods | VkKeyScan('d');
	}

	if ('F'==*s || 'f'==*s) { // 'Forward' or 'F1'-'F24' ?
		if ('o'==s[1]) return mods | VK_BROWSER_FORWARD;
		const int i = std::atoi(&s[1]);
		if (i < 1) return mods | VkKeyScan('f');
		return mods | (VK_F1 + ((i-1) % 24));
	}

	if ('E'==*s || 'e'==*s) { // 'Escape', 'Enter', or 'End' ?
		if ('s'==s[1]) return mods | VK_ESCAPE;
		if ('n'==s[1]) {
			if ('t'==s[2]) return mods | VK_RETURN;
			if ('d'==s[2]) return mods | VK_END;
		}
		return mods | VkKeyScan('e');
	}

	// 'Keypad...' is the only option left.
	if (!('K'==*s || 'k'==*s)) return mods | (VkKeyScan(*s) & 0xff);
	if (!skip<6>(s)) return mods | VkKeyScan('k');
	if ('P'==*s || 'p'==*s) return mods | VK_ADD;
	if ('D'==*s || 'd'==*s) {
		if ('e'==s[1]) return mods | VK_DECIMAL;
		if ('i'==s[1]) return mods | VK_DIVIDE;
	}
	else if ('M'==*s || 'm'==*s) {
		if ('i'==s[1]) return mods | VK_SUBTRACT;
		if ('u'==s[1]) return mods | VK_MULTIPLY;
	}
	else {
		const int i = std::atoi(s);
		if (i >= 0) return mods | (VK_NUMPAD0 + (i % 9));
	}
	return mods | VkKeyScan('k');
}
