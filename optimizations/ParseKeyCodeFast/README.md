# Optimizing `ParseKeyCode` (Windows edition)

I've been nervous lately, looking for a job and expecting having to move to Cologne or Berlin. So I was looking for something straightforward to focus on.

I have one of those MMORPG mouses, with the little keypad by the thumb with 12 programmable keys. I've set two of those to VK_BROWSER_BACK and VK_BROWSER_FORWARD. SciTE doesn't handle these but they would be perfect for switching buffers, so that was a feature I was really missing but could quickly add.

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

The only **downside** (curiously also a feature) is that the sped-up version doesn't strictly fail on invalid input. Thi is because it doesn't do an exact match but checks only the unique prefixes. Concretely, this means that their behaviors differ __given erroneous input__ (only). I guess another way of putting it, if I were a salesperson, is that the sped-up version will "just work" whereas the original would "degrade" into a no-op?

# Correctness test

Just `make run`.

# Speed test

Do `make PROFILE=1`.

Possible output:
```
g++ -o main.o -c main.cpp -std=c++17 -pg -DTEST_SPEED
g++ -o main.exe main.o ParseKeyCodeEmpty.o ParseKeyCode.o ParseKeyCodeFast.o ParseKeyCodeRegex.o -std=c++17 -pg -DTEST_SPEED
ParseKeyCodeEmpty 0
ParseKeyCodeFast  20
ParseKeyCode      1040
ParseKeyCodeFast is 36.1379 times faster.
gprof main.exe >main.stats
BFD: Dwarf Error: Could not find abbrev number 108.
```

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
