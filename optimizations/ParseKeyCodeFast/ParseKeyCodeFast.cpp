#include <cstdlib>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

/*
A -> Alt+
B -> Back
C -> Ctrl+
De -> Delete
Do -> Down
Es -> Escape
End -> End
Ent -> Enter
F ->
	o -> Forward
	\# -> F1..F24
H -> Home
I -> Insert
K -> Keypad
	P -> Plus
	Mi -> Minus
	Mu -> Multiply
	Di -> Divide
	De -> Decimal
L -> Left
M -> Menu
P -> Page
	D -> Down
	U -> Up
R -> Right
Sh -> Shift+
Sp -> Space
T -> Tab
U -> Up
V -> \# -> V1..V32767
W -> Win
*/

template<size_t N>
bool skip(char const *& s) {
	char const * const end = s + N;
	while (*s && s<end) ++s;
	return !!*s;
};

template<>
bool skip<3u>(char const *& s) {
	return *s && *(++s) && *(++s) && *(++s);
};

template<>
bool skip<4u>(char const *& s) {
	return *s && *(++s) && *(++s) && *(++s) && *(++s);
};

template<>
bool skip<5u>(char const *& s) {
	return *s && *(++s) && *(++s) && *(++s) && *(++s) && *(++s);
};

template<>
bool skip<6u>(char const *& s) {
	return *s && *(++s) && *(++s) && *(++s) && *(++s) && *(++s) && *(++s);
};

long ParseKeyCodeFast(const char *s) {
	if (!s || !*s) return 0;
	if (!s[1]) return VkKeyScan(*s) & 0xff;

	static constexpr const size_t Shift = 0x10000;
	static constexpr const size_t Ctrl = 0x20000;
	static constexpr const size_t Alt = 0x40000;
	// static constexpr const size_t Super = 0x80000;
	// static constexpr const size_t Meta = 0x160000;

	int mods = 0;

	// auto skip = [&s](size_t n) -> bool {
	// 	for (; *s && n;  ++s, --n);
	// 	return !!*s;
	// };

	if ('C'==*s || 'c'==*s) { // Can only be 'Ctrl+'.
		if (skip<4>(s) && '+'==*s) mods |= Ctrl, ++s;
		else return 0;
	}

	if ('S'==*s || 's'==*s) { // 'Shift' or 'Space' ?
		if ('h'==s[1]) {
			if (skip<5>(s) && '+'==*s) mods |= Shift, ++s;
			else return 0;
		}
		else if ('p'==s[1])
			return mods | VK_SPACE;
	}

	if ('A'==*s || 'a'==*s) { // Only 'Alt+'
		if (skip<3>(s) && '+'==*s) mods |= Alt, ++s;
		else return 0;
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
		if (!skip<4>(s)) return 0;
		if ('U'==*s || 'u'==*s) return mods | VK_PRIOR;
		if ('D'==*s || 'd'==*s) return mods | VK_NEXT;
		return 0;
	}

	if ('D'==*s || 'd'==*s) { // 'Delete' or 'Down' ?
		if ('e'==s[1]) return mods | VK_DELETE;
		if ('o'==s[1]) return mods | VK_DOWN;
		return 0;
	}

	if ('F'==*s || 'f'==*s) { // 'Forward' or 'F1'-'F24' ?
		if ('o'==s[1]) return mods | VK_BROWSER_FORWARD;
		const int i = std::atoi(&s[1]);
		if (i < 1) return 0;
		return mods | VK_F1 + ((i-1) % 24);
	}

	if ('E'==*s || 'e'==*s) { // 'Escape', 'Enter', or 'End' ?
		if ('s'==s[1]) return mods | VK_ESCAPE;
		if ('n'==s[1]) {
			if ('t'==s[2]) return mods | VK_RETURN;
			if ('d'==s[2]) return mods | VK_END;
		}
		return 0;
	}

	// 'Keypad...' is the only option left.
	if (!skip<6>(s)) return 0;
	if ('P'==*s || 'p'==*s) return mods | VK_ADD;
	if ('D'==*s || 'd'==*s) {
		if ('e'==s[1]) return mods | VK_DECIMAL;
		if ('i'==s[1]) return mods | VK_DIVIDE;
		return 0;
	}
	if ('M'==*s || 'm'==*s) {
		if ('i'==s[1]) return mods | VK_SUBTRACT;
		if ('u'==s[1]) return mods | VK_MULTIPLY;
		// return 0;
	}
	return 0;
}

