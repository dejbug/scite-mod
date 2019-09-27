# Optimizing `ParseKeyCode` (Windows edition)

I've been nervous lately, looking for a job and expecting having to move to Cologne or Berlin. So I was looking for something straightforward to focus on.

I have one of those MMORPG mouses, with the little keypad by the thumb with 12 programmable keys. I've set two of those to VK_BROWSER_BACK and VK_BROWSER_FORWARD. SciTE doesn't handle these but they would be perfect for switching buffers, so that was a feature I was really missing but could quickly add.

# What was optimized ?

Skimming the source got me to the `ParseKeyCode` function which looked perfectly readable but therefore really slow. For example:

```c++
	if ((sKey.at(0) == 'F') && (isdigit(sKey.at(1)))) {
		sKey.erase(0, 1);
		const int fkeyNum = atoi(sKey.c_str());
```

instead of

```c++
	if ((sKey.at(0) == 'F') && (isdigit(sKey.at(1)))) {
		const int fkeyNum = atoi(sKey.c_str() + 1);
```

or

```c++
} else if (StartsWith(sKey, "Keypad")) {
	sKey.erase(0, strlen("Keypad"));
```

instead of

```c++
} else if (StartsWith(sKey, "Keypad")) {
	sKey.erase(0, 6);
```

Given that the possible input was a finite list, it was easy to drastically minimize the number of comparisons.

# A caveat

The only **downside** (curiously also a feature) is that the sped-up version doesn't strictly fail on invalid input. This is because it doesn't do an exact match but checks only the unique prefixes. Concretely, this means that their behaviors differ __given erroneous input__ (only). I guess another way of putting it, if I were a salesperson, is that the sped-up version will "just work" whereas the original would "degrade" into a no-op?

# Demo

## Correctness test

Just `make run`.

## Speed test

Do `make run SPEEDTEST=3`.

Possible output:
```
g++ -o main.o -c main.cpp -std=c++17 -Os -DSPEEDTEST
g++ -o ParseKeyCodeEmpty.o -c ParseKeyCodeEmpty.cpp -std=c++17 -Os -DSPEEDTEST
g++ -o ParseKeyCode.o -c ParseKeyCode.cpp -std=c++17 -Os -DSPEEDTEST
g++ -o ParseKeyCodeFast.o -c ParseKeyCodeFast.cpp -std=c++17 -Os -DSPEEDTEST
g++ -o main.exe main.o ParseKeyCodeEmpty.o ParseKeyCode.o ParseKeyCodeFast.o -std=c++17 -Os -DSPEEDTEST
ParseKeyCodeEmpty 0
ParseKeyCodeFast  16
ParseKeyCode      260
ParseKeyCodeFast is 16.25 times faster.
```

### Note

The speedup factor may reach up to 30 (or an insane 50 once or twice) if `-gp` is added (for use in `gprof`), which will inject profiling logic into the code. One would have thought that all functions would be encumbered equally, so the factor would stay valid, but either that is not so or something really interesting is going on with caching. Try `make profile SPEEDTEST=1` (a `run` is implicit in `profile`).

## Usage

There are three speed tests. `SPEEDTEST=1` doesn't optimize, `SPEEDTEST=2` does max optimization, and `SPEEDTEST=3` does the (size-) optimization used in the default SciTE build.

`make coverage` (implies `profile`) and `make profile` (implies `run`) are possible to trigger `gcov/gprof` and `gprof` respectively.

# TODO

The parser is hard-coded. A parser-generator would be nice.

# Why bother ?

Look at where and how `ParseKeyCode` is used, it's in `SciTEWin::KeyDown` (via `KeyMatch`). So it runs at each and every key press. And SciTE is a text editor! And then you see it called inside loops.

```
	for (unsigned int j = 0; j < languageMenu.size(); j++) {
		if (KeyMatch(languageMenu[j].menuKey, keyVal, modifierAsInt)) {
```

and later

```
	for (const ShortcutItem &scut : shortCutItemList) {
		if (KeyMatch(scut.menuKey, static_cast<int>(wParam), modifiers)) {
```

and also (via `MatchKeyCode`)

```
	for (int tool = 0; tool < toolMax; ++tool) {
		//...
			if (SciTEKeys::MatchKeyCode(static_cast<long>(mii.dwItemData), keyVal, modifierAsInt)) {
```

The best way would be to translate the string labels into integer identifiers once at the start of the app. I guess the reasoning is that it wouldn't work if the user somehow (for some reason) change those strings. But even then, you would simply update the lookup table at each such change. I need to loock into this. It may turn out that my optimization here will have been unnecessary, as they often are ;). 

Anyway, all is good. I am no longer anxious. Mission accomplished.
