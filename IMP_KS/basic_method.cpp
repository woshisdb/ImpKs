#include "basic_method.h"
string jointList(vector<string> l, string aim)
{
	string ret = *(l.begin());
	vector<string>::iterator it = l.begin();
	for (it++; it != l.end(); it++)
	{
		ret += aim;
		ret += *(it);
	}
	return ret;
}
string remove(string u, string v)
{
	int pos = u.find(v);
	while (pos != -1)
	{
		u.erase(pos, v.length());
		pos = u.find(v);
	}
	return u;
}
template<class container>//一个
typename container::iterator at(container &u, int no)
{
	typename container::iterator it = u.begin();
	for (int i = 0; i < no; i++, it++)
	{

	}
	return it;
}
int lastIndexOf(string u, char v)
{
	for (int i = u.length() - 1; i >= 0; i--)
	{
		if (u[i] == v)
		{
			return i;
		}
	}
	return -1;//不能找到
}
string trimmed(string origin)
{
	//printf("%d",origin.length());
	int len = 0;
	for (int i = 0; i < origin.length(); i++)
	{
		//cout<<origin[i]<<endl;
		if (origin[i] == '\t' || origin[i] == '\n' || origin[i] == '\v' || origin[i] == '\f' || origin[i] == '\r' || origin[i] == ' ')
		{
			len++;
		}
		else
		{
			break;
		}
	}
	origin.erase(0, len);
	len = 0;
	for (int i = origin.length() - 1; i >= 0; i--)
	{
		//cout<<origin[i]<<endl;
		if (origin[i] == '\t' || origin[i] == '\n' || origin[i] == '\v' || origin[i] == '\f' || origin[i] == '\r' || origin[i] == ' ')
		{
			len++;
		}
		else
		{
			break;
		}
	}
	origin.erase(origin.length() - len, len);
	return origin;
}


vector<string> split(string str, string delim)
{
	vector<string> res;
	if ("" == str) return res;
	//先将要切割的字符串从string类型转换为char*类型  
	char * strs = new char[str.length() + 1]; //不要忘了  
	strcpy(strs, str.c_str());

	char * d = new char[delim.length() + 1];
	strcpy(d, delim.c_str());

	char *p = strtok(strs, d);
	while (p) {
		string s = p; //分割得到的字符串转换为string类型  
		res.push_back(s); //存入结果数组  
		p = strtok(NULL, d);
	}
	return res;
}
