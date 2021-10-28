#pragma once
#include<iostream>
#include<string>
#include<vector>
#include <cassert>
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


struct FirstOrderLogical
{
public:
	bool isNull() const {
		return preLable.empty() && postLable.empty();
	}

	static string findAssignVariable(const string& statement) {
		std::regex reg("(\\w+)\\s*=\\s*");
		std::smatch m;
		if (std::regex_search(statement, m, reg)) {
			string s = m[1];
			return s;
		}
		else {
			return string();
		}
	}

	string toString() const {
		if (!condition.empty()) {
			return formatString("pc=%s ∧ pc'=%s ∧ (%s) ∧ SAME(V)", preLable.c_str(), postLable.c_str(), condition.c_str());
		}
		else {
			string tmp = formatString("pc=%s ∧ pc'=%s ∧ (%s)", preLable.c_str(), postLable.c_str(), opr.c_str());
			string var = findAssignVariable(opr);
			if (var.empty()) {
				tmp += " ∧ SAME(V)";
			}
			else {
				tmp += formatString(" ∧ SAME(V\\{%s})", var.c_str());
			}
			return tmp;
		}
	}

	string valueToString() const {
		string tmp;
		for (const auto &v : vars) {
			if (!tmp.empty()) {
				tmp += ",";
			}
			tmp += formatString("%c=%d", v.name, v.value);
		}
		return tmp;
	}

	pair<char, int> assign()  const {
		if (opr.empty() || !(opr.find('=') != -1)) {
			return pair<char, int>();
		}

		std::regex re("(\\w)\\s*=\\s*(\\w)\\s*([\\*+-])\\s*(\\w)");
		std::smatch m;
		if (std::regex_search(opr, m, re)) {

			char ret = string(m[1]).at(0);
			char left = string(m[2]).at(0);
			char midOpr = string(m[3]).at(0);
			char right = string(m[4]).at(0);

			int nLeft = getVarValue(left);
			int nRight = getVarValue(right);
			int nRet = 0;
			if (midOpr == '*') {
				nRet = nLeft * nRight % ModValue;
			}
			else if (midOpr == '+') {
				nRet = (nLeft + nRight) % ModValue;
			}
			else if (midOpr == '-') {
				nRet = (nLeft - nRight + 3) % ModValue;
			}

			return pair<char, int>(ret, nRet);
		}

		std::regex re2("(\\w+)\\s*=\\s*(\\d+)");
		std::smatch m2;
		if (std::regex_search(opr, m2, re2)) {
			return pair<char, int>{ string(m2[1]).at(0), std::stoi(string(m2[2]))};
		}
		return pair<char, int>();
	}

	bool hasAssign() const {
		if (!condition.empty() || opr.empty()) {
			return false;
		}
		string var = findAssignVariable(opr);
		if (var.empty())
			return false;

		return true;
	}

	bool isConditionOk() const {
		if (condition.empty())
			return true;
		//if (0 == condition.compare("true", Qt::CaseInsensitive))
		if (condition == "true")
			return true;
		//if (0 == condition.compare("false", Qt::CaseInsensitive))
		if (condition == "false")
			return false;

		bool hasNot = false;
		string conditionNew = condition;
		if (conditionNew.find("not") != -1) {
			hasNot = true;
			//conditionNew.remove("not");
			conditionNew = remove(conditionNew, "not");
			conditionNew = trimmed(conditionNew);
		}

		if (conditionNew == "true")
			return hasNot?!true:true;
		if (conditionNew == "false")
			return hasNot ? !false : false;

		std::regex re("(\\w)\\s*([><=andotr]+)\\s*(\\w)");
		std::smatch m;
		assert(std::regex_search(conditionNew, m, re));

		char varLeft = string(m[1]).at(0);
		string midOpr = string(m[2]);
		char varRight = string(m[3]).at(0);

		int left = getVarValue(varLeft);
		int right = getVarValue(varRight);

		bool res = true;
		if (midOpr == ">=")
			res = left >= right;
		else if (midOpr == "==")
			res = left == right;
		else if (midOpr == "<=")
			res = left <= right;
		else if (midOpr == ">")
			res = left > right;
		else if (midOpr == "<")
			res = left < right;
		else if (midOpr == "and")
			res = left && right;
		else if (midOpr == "not")
			res = !right;
		else
			assert(false);

		return hasNot ? !res : res;
	}

	int getVarValue(const char var) const {
		//if (var.isNumber()) 
		if (var >= '0'&&var <= '9')
		{
			//return formatString("%c", var).toInt();
			return var - '0';
		}

		for (const auto& v : vars) {
			if (v.name == var)
				return v.value;
		}
		return 0;
	}

	string preLable;
	string postLable;
	string condition;
	string opr;
	vector<Variable> vars;
};
struct KsR
{
	string preLabel;
	string postLabel;
	Variables preVars;
	Variables postVars;
	string opr;

	string toString() const {
		if (opr.empty() && preLabel.empty() && postLabel.empty())
			return string();

		vector<string> unknowndVars;
		for (const auto &v : postVars) {
			bool conatins = false;
			for (const auto& vPre : preVars) {
				if (vPre.name == v.name) {
					conatins = true;
					break;
				}
			}
			if (!conatins)
			{
				//unknowndVars.push_back(v.name);
				string temp(1, v.name);
				unknowndVars.push_back(temp);//?不知道行吗
			}
		}

		string tmp;
		tmp += "(pc=";
		tmp += preLabel.empty() ? "U" : preLabel;

		for (const auto& v : preVars) {
			tmp += ",";
			tmp += v.toString();
		}

		//加进入未定义的变量
		for (const auto& v : unknowndVars) {
			tmp += ",";
			tmp += formatString("%s=u", v.c_str());
		}

		tmp += ") -> (pc'=";
		tmp += postLabel.empty() ? "U" : postLabel;

		int n = 0;
		for (const auto& v : postVars) {
			tmp += ",";
			tmp += v.toString(true);
		}

		tmp += ")";

		return tmp;
	}
};