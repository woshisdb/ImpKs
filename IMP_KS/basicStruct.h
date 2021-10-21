#pragma once
#include<iostream>
#include<string>
#include<vector>
#include"basic_method.h"
using namespace std;
static const int ModValue = 3;


/**
	\a StatementType ������ͣ�Ŀǰ֧��4�ַ���������ͣ�˳����䣬
	������䣬ѭ����䣬�ȴ���䡣
**/
enum class StatementType {
	Squence = 0,  //˳�����
	If,       //If ���
	While,    //While ���
	Wait      //wait ���
};


/**
	IMP��������
	֧��boolean �� int ����
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
		// �ַ�������
		if (condition.find("not") != -1) {
			//condition.remove("not");
			condition = remove(condition, "not");
			//condition = condition.trimmed();
		}
		else {
			condition = formatString("not %s", condition.c_str());
		}
	}
	StatementType type;  //�������
	string label;       //����ǩ
	string condition;
	string seqBody;
	Statements ifBody;
	Statements elseBody;
	Statements whileBody;
	vector<Variable>  vars;
};
template<class nump, class sea>
bool is_contains_list(nump l, sea aim)//����������Ƿ���aim
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