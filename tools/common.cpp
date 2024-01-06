#include "common.h"


void info(std::ostream& out) {
	std::stringstream ss;
	ss << out.rdbuf();
	info(ss.str());
}

void info(std::string s) {
	auto t = std::time(nullptr);
	struct tm tm;
	localtime_s(&tm, &t);
	std::cout << (std::put_time(&tm, "\033[1;36m[%d-%m-%Y %H:%M:%S] \033[0m- ")) << s << std::endl;
}

void info_i(std::string s) {
	info("\033[0m[\033[1;32m*\033[0m] " + s);
}

void info_w(std::string s) {
	info("\033[0m[\033[1;33m!\033[0m] " + s);
}

void info_e(std::string s) {
	info("\033[0m[\033[1;91m!\033[0m] " + s);
}

void info(int i) {
	info(std::to_string(i));
}

void info(double d) {
	info(std::to_string(d));
}

short* rgb(std::string hex) {
	unsigned int r, g, b;
	sscanf_s(hex.c_str(), "%02x%02x%02x", &r, &g, &b);
	short nc[3] = { (short)r, (short)g, (short)b };
	return new short[3]{ (short)r, (short)g, (short)b };
}
