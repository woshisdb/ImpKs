#pragma once
#include <iostream>
#include <string>
#include <map>
#include <list>
#include <vector>
using namespace std;
string jointList(vector<string> l, string aim);
string remove(string u, string v);
template<class container>//Ò»¸ö
typename container::iterator at(container &u, int no);
int lastIndexOf(string u, char v);
string trimmed(string origin);
vector<string> split(string str, string delim);
