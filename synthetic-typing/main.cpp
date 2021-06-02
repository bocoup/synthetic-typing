#define WINVER 0x0500
#include <windows.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <map>
using namespace std;

std::map<std::string, WORD> keys = {
	{"Backspace", VK_BACK},
	{"Tab", VK_TAB},
	{"Enter", VK_RETURN},
	{"Shift", VK_SHIFT},
	{"Control", VK_CONTROL},
	{"ArrowUp", VK_UP},
	{"ArrowRight", VK_RIGHT},
	{"ArrowDown", VK_DOWN},
	{"ArrowLeft", VK_LEFT},
	{"Home", VK_HOME},
	{"End", VK_END},
	{"PageUp", VK_PRIOR},
	{"PageDown", VK_NEXT}
};

std::vector<vector<WORD>> parse_keys(char* input) {
	vector<vector<WORD>> result;
	string str = std::string(input);
	stringstream str_stream1(str);

	while (str_stream1.good()) {
		std::string subsequence;
		std::getline(str_stream1, subsequence, ',');

		vector<WORD> r;
		stringstream str_stream2(subsequence);
		while (str_stream2.good()) {
			std::string sub;
			std::getline(str_stream2, sub, '+');

			if (sub.length() == 1) {
				r.push_back(sub[0]);
			}
			else {
				auto found = keys.find(sub);
				if (found == keys.end()) {
					throw std::runtime_error("Unrecognized key.");
				}
				r.push_back(found->second);
			}
		}

		result.push_back(r);
	}
	return result;
}

enum KEYSTROKE_DIRECTION {up, down};

void press(WORD key_code, KEYSTROKE_DIRECTION direction) {
	INPUT ip;
	ip.type = INPUT_KEYBOARD;
	ip.ki.wScan = 0;
	ip.ki.time = 0;
	ip.ki.dwExtraInfo = 0;

	ip.ki.wVk = key_code;
	ip.ki.dwFlags = direction == down ? 0 : KEYEVENTF_KEYUP;
	SendInput(1, &ip, sizeof(INPUT));
}

int main(int argc, char* argv[]) {
	if (argc != 2) {
		fprintf(stderr, "Program requires exactly one command-line argument.");
		return 1;
	}
	vector<vector<WORD>> parsed;
	try {
		parsed = parse_keys(argv[1]);
	}
	catch (const std::runtime_error& error) {
		fprintf(stderr, "Error parsing.");
		return 1;
	}

	for (int index = 0; index < parsed.size(); index += 1) {
		for (int jindex = 0; jindex < parsed.at(index).size(); jindex += 1) {
			printf("%c (0x%x)\n", parsed.at(index).at(jindex), parsed.at(index).at(jindex));
			press(parsed.at(index).at(jindex), down);
		}
		Sleep(1000);
		for (int jindex = parsed.at(index).size() - 1; jindex >= 0; jindex -= 1) {
			press(parsed.at(index).at(jindex), up);
		}
	}

	return 0;
}