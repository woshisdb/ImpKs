#pragma once
#include<iostream>
#include<string>
#include<vector>
#include"basic_method.h"
using namespace std;
static const int ModValue = 3;


/**
	\a StatementType 语句类型，目前支持4种符合语句类型，顺序语句，
	条件语句，循环语句，等待语句。
**/
enum class StatementType {
	Squence = 0,  //顺序语句
	If,       //If 语句
	While,    //While 语句
	Wait      //wait 语句
};


/**
	IMP变量定义
	支持boolean 和 int 类型
*/
struct Variable
{
	bool operator==(const Variable& o) const {
		return name == o.name && value == o.value;
	}
	enum Type
	{
		Int = 0,
		Boolean
	};
	string toString(bool isPost = false) const {
		if (!isPost)
			return formatString("%c=%d", name, value);
		else
			return formatString("%c'=%d", name, value);
	}
	Type  type;
	char name;
	int   value;
};

struct Statement;
using Statements = vector<Statement>;
using Variables = vector<Variable>;

/**

*/
struct Statement
{
	bool isNull() const {
		return condition.empty() && seqBody.empty();
	}
	void reversedCondition() {
		// 字符串处理
		if (condition.find("not") != -1) {
			//condition.remove("not");
			condition = remove(condition, "not");
			//condition = condition.trimmed();
		}
		else {
			condition = formatString("not %s", condition.c_str());
		}
	}
	StatementType type;  //语句类型
	string label;       //语句标签
	string condition;
	string seqBody;
	Statements ifBody;
	Statements elseBody;
	Statements whileBody;
	vector<Variable>  vars;
};
template<class nump, class sea>
bool is_contains_list(nump l, sea aim)//检查链表中是否有aim
{
	bool has = false;
	typename nump::iterator it = l.begin();
	for (; it != l.end(); it++)
	{
		if (aim == *it)
		{
			has = true;
			break;
		}
	}
	return has;
}