#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include "imp_ks.h"
using namespace std;

string readIMPcode(string filepath)
{
	ifstream ifs;
	ifs.open(filepath, ios::in);

	if (!ifs.is_open())
	{
		cout << "read fail: " << filepath << endl;
		return "";
	}

	string buf, result = "";
	while (getline(ifs, buf))
	{
		result += buf;
	}
	return result;
}

int main(int argc, char *argv[]) {
	cout << "Start IMPtoKS" << endl;
	string code = "";
	if (argc == 2) {
		string filepath = argv[1];
		code = readIMPcode(filepath);
	} else {
		cout << "Parameter error, I need an imp file path." << endl;
		return 1;
	}
	ImpKs impKs;
	impKs.run(code);
	return 0;
}