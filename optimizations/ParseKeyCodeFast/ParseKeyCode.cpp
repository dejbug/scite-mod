#include <cstdlib>
#include <string>
#include <string_view>

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

constexpr bool IsADigit(int ch) noexcept {
	return (ch >= '0') && (ch <= '9');
}

bool StartsWith(std::string_view s, std::string_view start) {
	return (s.size() >= start.size()) &&
	       (std::equal(s.begin(), s.begin() + start.size(), start.begin()));
}

bool RemoveStringOnce(std::string &s, const char *marker) {
	const size_t modText = s.find(marker);
	if (modText != std::string::npos) {
		s.erase(modText, strlen(marker));
		return true;
	}
	return false;
}

long ParseKeyCode(const char *mnemonic) {
	long modsInKey = static_cast<long>(SA::KeyMod::Norm);
	int keyval = -1;

	if (mnemonic && *mnemonic) {
		std::string sKey = mnemonic;

		if (RemoveStringOnce(sKey, "Ctrl+"))
			modsInKey = modsInKey | static_cast<long>(SA::KeyMod::Ctrl);
		if (RemoveStringOnce(sKey, "Shift+"))
			modsInKey = modsInKey | static_cast<long>(SA::KeyMod::Shift);
		if (RemoveStringOnce(sKey, "Alt+"))
			modsInKey = modsInKey | static_cast<long>(SA::KeyMod::Alt);

		if (sKey.length() == 1) {
			keyval = VkKeyScan(sKey.at(0)) & 0xFF;
		} else if (sKey.length() > 1) {
			if ((sKey.at(0) == 'F') && (IsADigit(sKey.at(1)))) {
				const int fkeyNum = atoi(sKey.c_str()+1);
				if (fkeyNum >= 1 && fkeyNum <= 24)
					keyval = fkeyNum - 1 + VK_F1;
			} else if ((sKey.at(0) == 'V') && (IsADigit(sKey.at(1)))) {
				const int vkey = atoi(sKey.c_str()+1);
				if (vkey > 0 && vkey <= 0x7FFF)
					keyval = vkey;
			} else if (StartsWith(sKey, "Keypad")) {
				sKey.erase(0, 6); // 6 == strlen("Keypad");
				if ((sKey.length() > 0) && IsADigit(sKey.at(0))) {
					const int keyNum = atoi(sKey.c_str());
					if (keyNum >= 0 && keyNum <= 9)
						keyval = keyNum + VK_NUMPAD0;
				} else if (sKey == "Plus") {
					keyval = VK_ADD;
				} else if (sKey == "Minus") {
					keyval = VK_SUBTRACT;
				} else if (sKey == "Decimal") {
					keyval = VK_DECIMAL;
				} else if (sKey == "Divide") {
					keyval = VK_DIVIDE;
				} else if (sKey == "Multiply") {
					keyval = VK_MULTIPLY;
				}
			} else if (sKey == "Left") {
				keyval = VK_LEFT;
			} else if (sKey == "Right") {
				keyval = VK_RIGHT;
			} else if (sKey == "Up") {
				keyval = VK_UP;
			} else if (sKey == "Down") {
				keyval = VK_DOWN;
			} else if (sKey == "Insert") {
				keyval = VK_INSERT;
			} else if (sKey == "End") {
				keyval = VK_END;
			} else if (sKey == "Home") {
				keyval = VK_HOME;
			} else if (sKey == "Enter") {
				keyval = VK_RETURN;
			} else if (sKey == "Space") {
				keyval = VK_SPACE;
			} else if (sKey == "Tab") {
				keyval = VK_TAB;
			} else if (sKey == "Escape") {
				keyval = VK_ESCAPE;
			} else if (sKey == "Delete") {
				keyval = VK_DELETE;
			} else if (sKey == "PageUp") {
				keyval = VK_PRIOR;
			} else if (sKey == "PageDown") {
				keyval = VK_NEXT;
			} else if (sKey == "Win") {
				keyval = VK_LWIN;
			} else if (sKey == "Menu") {
				keyval = VK_APPS;
			} else if (sKey == "Back") {
				keyval = VK_BROWSER_BACK;
			} else if (sKey == "Forward") {
				keyval = VK_BROWSER_FORWARD;
			}
		}
	}

	return (keyval > 0) ? (keyval | (static_cast<int>(modsInKey)<<16)) : 0;
}
