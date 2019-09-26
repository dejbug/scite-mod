#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <map>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "Clocker.h"

extern long ParseKeyCodeEmpty(const char *);
extern long ParseKeyCode(const char *);
extern long ParseKeyCodeFast(const char *);
// extern long ParseKeyCodeRegex(const char *);

using Map = std::map<std::string, UINT>;
using Item = Map::value_type;
using TestFunction = long (*)(char const *);

static constexpr const size_t Ctrl = 0x20000;
static constexpr const size_t Shift = 0x10000;
static constexpr const size_t Alt = 0x40000;

static Map const mods{
	// No "Super" nor "Meta" for Windows.

	{"", 0}, {"Ctrl", Ctrl}, {"Shift", Shift}, {"Alt", Alt},
	{"Ctrl+Shift", Ctrl|Shift}, {"Ctrl+Shift+Alt", Ctrl|Shift|Alt},
	{"Shift+Alt", Shift|Alt}, {"Ctrl+Alt", Ctrl|Alt}
};

static Map const keys{
	// No "Slash", "Question", nor "Equal" ? They weren't in
	// KeyvalFromMnemonic(), so we too best leave them out.

	{"Left", VK_LEFT}, {"Right", VK_RIGHT}, {"Up", VK_UP}, {"Down", VK_DOWN},
	{"Insert", VK_INSERT}, {"End", VK_END}, {"Home", VK_HOME},
	{"Enter", VK_RETURN}, {"Space", VK_SPACE}, {"Tab", VK_TAB},
	{"KeypadPlus", VK_ADD}, {"KeypadMinus", VK_SUBTRACT},
	{"KeypadMultiply", VK_MULTIPLY}, {"KeypadDivide", VK_DIVIDE}, {"KeypadDecimal", VK_DECIMAL}, {"Escape", VK_ESCAPE},
	{"Delete", VK_DELETE}, {"PageUp", VK_PRIOR}, {"PageDown", VK_NEXT},
	{"Win", VK_LWIN}, {"#", VkKeyScan('#')}, {"F24", VK_F24}, {"V4711", 4711}
};

Item prependModifierToKey(Item const & k, Item const & m) {
	if (!m.second) return k;
	std::string mk = m.first;
	mk += "+";
	mk += k.first;
	return Item(mk, k.second | m.second);
}

void print(Item const & k, Item const & m, TestFunction f) {
	Item const & x = prependModifierToKey(k, m);
	long const val = f(x.first.c_str());
	char const tick = x.second == val ? 'x' : ' ';
	std::cout.fill('0');
	std::cout << std::hex << std::setw(8) << val
		<< " == " << std::setw(8) << x.second
		<< " [" << tick << ']'
		<< " | " << x.first
		<< std::endl;
}

void print(TestFunction f) {
	for (auto const & k : keys)
		for (auto const & m : mods)
			print(k, m, f);
}

bool check(Item const & k, Item const & m, TestFunction f) {
	Item const & x = prependModifierToKey(k, m);
	long const val = f(x.first.c_str());
	return x.second == val ? 'x' : ' ';
}

void test(TestFunction f, size_t runs) {
	for (int i=0; i<runs; ++i)
		for (auto const & k : keys)
			for (auto const & m : mods)
				check(k, m, f);
}

void testSpeed() {
	long const runs = 10000;
	Clocker clocker;

	clocker.reset();
	test(ParseKeyCodeEmpty, runs);
	double const minimal = clocker.get() / runs;
	std::cout << "ParseKeyCodeEmpty " << 0 << std::endl;

	clocker.reset();
	test(ParseKeyCodeFast, runs);
	double const fast = clocker.get() / runs;
	std::cout << "ParseKeyCodeFast  " << fast - minimal << std::endl;

	clocker.reset();
	test(ParseKeyCode, runs);
	double const normal = clocker.get() / runs;
	std::cout << "ParseKeyCode      " << normal - minimal << std::endl;

	std::cout << "ParseKeyCodeFast is ";
	if (fast) std::cout << (normal - minimal) / (fast - minimal);
	else std::cout << "+inf";
	std::cout << " times faster.\n";

	// clocker.reset();
	// test(ParseKeyCodeRegex, runs);
	// std::cout << "ParseKeyCodeRegex " << clocker.get() / runs << std::endl;
}

void testCorrectness() {
	// std::cout << "\nParseKeyCode\n\n";
	// print(ParseKeyCode);

	std::cout << "\nParseKeyCodeFast\n\n";
	print(ParseKeyCodeFast);
}

int main() {
#ifdef TEST_SPEED
	testSpeed();
#else
	testCorrectness();
#endif
	return 0;
}
