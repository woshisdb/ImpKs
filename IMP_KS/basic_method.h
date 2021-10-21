#pragma once
#include <iostream>
#include <string>
#include <map>
#include <list>
#include<cstring>
#include <vector>
using namespace std;
string jointList(vector<string> l, string aim);
string remove(string u, string v);
template<class container>//Ò»¸ö
typename container::iterator at(container &u, int no);
int lastIndexOf(string u, char v);
string trimmed(string origin);
vector<string> split(string str, string delim);
template<typename ... Args>
static std::string formatString(const std::string &format, Args ... args)
{
	auto size = std::snprintf(nullptr, 0, format.c_str(), args ...) + 1; // Extra space for '\0'
	std::unique_ptr<char[]> buf(new char[size]);
	std::snprintf(buf.get(), size, format.c_str(), args ...);
	string ret = std::string(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside
	return ret;
}
