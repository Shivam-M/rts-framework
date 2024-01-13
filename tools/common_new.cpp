/*
#include "common_new.h"

#define CON_RED		\033[1;91m
#define CON_YELLOW	\033[1;33m
#define CON_CYAN	\033[1;32m

#define PREFIX_INFO "[*] "

using namespace std;

void info(ostream& out) {
	stringstream ss;
	ss << out.rdbuf();
	info(ss.str());
}

void info(string s) {
	auto t = time(nullptr);
	tm tm;
	localtime_s(&tm, &t);
	cout << (put_time(&tm, "\033[1;36m[%d-%m-%Y %H:%M:%S] \033[0m- ")) << s << endl;
}

void info_i(string s) {
	info("\033[0m[\033[1;32m*\033[0m] " + s);
}

void info_w(string s) {
	info("\033[0m[\033[1;33m!\033[0m] " + s);
}

void info_e(string s) {
	info("\033[0m[\033[1;91m!\033[0m] " + s);
}

template <typename T>
void info(T message) {
	info(PREFIX_INFO, to_string(message));
}

template <typename T>
void info(string prefix, T message) {
	info(prefix + to_string(message));
}
*/