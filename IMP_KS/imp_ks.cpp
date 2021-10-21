#define _CRT_SECURE_NO_WARNINGS
#include "imp_ks.h"
#include"P1.h"
#include "basic_method.h"
#include "basicStruct.h"
#include <iostream>
#include <cassert>
#include <string>
#include <map>
#include <list>
#include <vector>
#include <memory>
#include <cstdio>
// ������ʽ��
#include <regex> 

using namespace std;

const string g_input[] = {
R"(a=0;
b=2;
while a<b do
    a=a+1;
endwhile;
a=2;
b=0;)",

R"(a=0;
b=1;
if a < b then
    a=a+1;
else
    b=b+1;
endif;
a=0;
b=0;)",

R"(x=0;
y=0;
z=0;
x=y+1; 
z=z+2;
while y<2 do
    if x<y then 
        x=x+1; 
    else 
        y=y+1;
    endif;
endwhile;)",

R"(x=0;
y=2;
if x<y then
    while x<2 do
        x=x+1;
    endwhile;
else 
    y=y+1;
endif;)",

R"(cobegin P0||P1 coend;
P0::
t=0;
while true do
    wait(t==0);
    t=1; 
endwhile;

P1::
while true do
    wait(t==1);
    t=0; 
endwhile;)",
};


void output_node(Statement node)
{
	cout << "{" << endl;
	if (StatementType::While == node.type)
	{
		cout << "StatementType:" << "While" << endl;
		output_node(node.whileBody[0]);
	}
	else
	{
		cout << "StatementType:"<<"else" << endl;
	}
	cout << "label:" + node.label << endl;
	cout << "condition:" + node.condition << endl;
	cout << "seqBody:" + node.seqBody << endl;
	cout << "}" << endl;

}

/**
* \b һ���߼����ݽṹ�����Դ�һ���߼�����KS�ṹ
  \a preLable ǰ�ñ�ǩ
  \a postLable ���ñ�ǩ
  \a condition ����
  \a string opr ����
  \a vector<Variable> vars;
*/

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
			return formatString("pc=%s and pc'=%s and (%s) and SAME(V)", preLable.c_str(), postLable.c_str(), condition.c_str());
		}
		else {
			string tmp = formatString("pc=%s and pc'=%s and (%s)", preLable.c_str(), postLable.c_str(), opr.c_str());
			string var = findAssignVariable(opr);
			if (var.empty()) {
				tmp += " and SAME(V)";
			}
			else {
				tmp += formatString(" and SAME(V\\{%s})", var.c_str());
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
		if (condition== "true")
			return true;
		//if (0 == condition.compare("false", Qt::CaseInsensitive))
		if(condition== "false")
			return false;

		bool hasNot = false;
		string conditionNew = condition;
		if (conditionNew.find("not") != -1) {
			hasNot = true;
			//conditionNew.remove("not");
			conditionNew = remove(conditionNew, "not");
			conditionNew = trimmed(conditionNew);
		}

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
		if (var>='0'&&var<='9')
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


//KS �е�R�ṹ
struct KsR
{
	string preLabel;
	string postLabel;
	Variables preVars;
	Variables postVars;
	string opr;

	string toString() const {
		if (opr.empty() && preLabel.empty() && postLabel.empty() )
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
				//unknowndVars.push_back(v.name);
				unknowndVars.push_back(v.name+"");//?��֪������
		}

		string tmp;
		tmp += "(pc=";
		tmp += preLabel.empty() ? "U" : preLabel ;

		for (const auto& v : preVars) {
			tmp += ",";
			tmp += v.toString();
		}

		//�ӽ���δ����ı���
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

		//ע�͵��任����
		//tmp += "and (";
		//tmp += opr;
		//tmp += ")";
		return tmp;
	}
};




//���������ת��Ϊ�߼���ʽ
static FirstOrderLogical toFormula(const Statement& pre, const Statement& post) {
	FirstOrderLogical lg;
	lg.preLable = pre.label;
	lg.postLable = post.label;
	lg.condition = pre.condition;
	lg.opr = pre.seqBody;
	lg.vars = pre.vars;
	return lg;
}

//���������ת��Ϊ�߼���ʽ
vector<FirstOrderLogical> toFormula(const Statements& statements, Statement &out) {
	if (statements.empty())
		return vector<FirstOrderLogical>();
	if (out.label.empty()) {
		//string prefix = statements.at(0).label.left(1);
		string prefix = (statements[0]).label.substr(0, 1);
		//split = split.substr(0, lastIndexOf(split, ';') + 1);//
		out.label = prefix + "E";
	}

	vector<FirstOrderLogical> list;
	for (int i = 0; i < statements.size(); ++i) {
		Statement postSm = out;
		//Statement sm=statements[i];
		Statement sm = statements[i];
		if (i + 1 < statements.size()) {
			//postSm = statements.at(i + 1);
			postSm = statements[i + 1];
		}

		if (sm.type == StatementType::If) {
			if (!sm.ifBody.empty()) {
				list.push_back( toFormula(sm, *(sm.ifBody.begin()) ) ); //?
				auto list_tmp = toFormula(sm.ifBody, postSm);
				list.insert(list.end(), list_tmp.begin(), list_tmp.end());
			}
			if (!sm.elseBody.empty()) {
				sm.reversedCondition();
				list.push_back( toFormula(sm, *(sm.elseBody.begin() ) ));
				auto list_tmp = toFormula(sm.elseBody, postSm);
				list.insert(list.end(), list_tmp.begin(), list_tmp.end());
			}
		}
		else if (sm.type == StatementType::While) {
			if (!sm.whileBody.empty()) {
				list.push_back( toFormula(sm, *(sm.whileBody.begin() ) ));
				auto list_tmp = toFormula(sm.whileBody, sm);
				list.insert(list.end(), list_tmp.begin(), list_tmp.end());
			}
			sm.reversedCondition();
			list.push_back( toFormula(sm, postSm));
		}
		else if (sm.type == StatementType::Wait) {
			sm.reversedCondition();
			list .push_back( toFormula(sm, sm) );
			sm.reversedCondition();
			list .push_back( toFormula(sm, postSm));
		}
		else {
			list.push_back( toFormula(sm, postSm));
		}
	}
	return list;
}



//����,��ʼ��ҵ����
ImpKs::ImpKs()
{

}


//������������Ϊ�﷨��
/*
bool parseStatements(const string& input, Statements& statements) {
	int s = 0;
	int e = 0;
	while (s < input.length())
	{
		//int pos = input.indexOf(QRegularExpression("(if|while)"), s);
		//int pos = input.find(QRegularExpression("(if|while)"), s);
		int pos = input.find("if", s);
		if (pos == -1) {
			pos = input.find("while", s);
		}
		if (-1 == pos) {
			e = input.length();
			string inputSplit = input.substr(s, e - s);
			auto states_tmp = parseSequence(inputSplit);
			statements.insert(statements.end(), states_tmp.begin(), states_tmp.end());
			s = e;
		}
		else {
			if (pos > s) {
				//statements. merge( parseSequence(input.mid(s, pos - s)) );
				auto states_tmp = parseSequence(input.substr(s, pos - s));
				statements.insert(statements.end(), states_tmp.begin(), states_tmp.end());
			}
			s = pos;
			if (input.at(pos) == 'i') {
				//e = input.indexOf("endif");
				e = input.find("endif");
				e += 6;
				statements .push_back( parseIf(input.substr(s, e - s)) );
			}
			else {
				//e = input.indexOf("endwhile");
				e = input.find("endwhile");
				e += 9;
				statements .push_back( parseWhile(input.substr(s, e - s)) );
			}
			s = e;
		}
	}
	return true;
}
*/

void changeValue(Variables &vars, char var, int value) {
	bool find = false;
	for (auto& v : vars) {
		if (v.name == var) {
			v.value = value;
			find = true;
			break;
		}
	}
	if (!find)
		//vars << Variable{ Variable::Int, var, value };
		vars .push_back( Variable{ Variable::Int, var, value } );
}


// �����������ϱ�ǩ
// һ���߼���ʽ��Ҫʹ������ǩ
// ͬʱҲ��������չʾ����ǩ�Ĵ����
void labledStatements(const char& prefix, int& index, Statements& sms) {
	if (sms.empty()) {
		return;
	}
	for (auto& v : sms) {
		v.label = formatString("%c%d", prefix, index++);
		if (v.type == StatementType::If) {
			labledStatements(prefix, index, v.ifBody);
			labledStatements(prefix, index, v.elseBody);
		}
		else if (v.type == StatementType::While) {
			labledStatements(prefix, index, v.whileBody);
		}
	}
}

//�����������ϱ�ǩ
void labledStatements(vector<Statements> &smss) {
	char prefix = 'A';
	for (auto& sms : smss) {
		int index = 0;
		labledStatements(prefix, index, sms);
		//prefix = char(prefix.unicode() + 1);
		prefix = char(prefix + 1);
	}
}
//void statementToList(const Statements &sms, QStringList &list, QString &space=QString())
void statementToList(const Statements &sms, vector<string> &lis, string &space) {//?
	if (sms.empty())
		return;

	string SpaceNew = space + "    ";
	vector<string> ls;
	for (const auto& v : sms) {
		if (v.type == StatementType::Squence) {
			lis.push_back(v.label + ": " + space + v.seqBody + ';');
			//lis << v.label + ": " + space + v.seqBody + ';';
		}
		else if (v.type == StatementType::Wait) {
			lis.push_back(v.label + ": " + space + "wait (" + v.condition + ");");
			//lis << v.label + ": " + space + "wait (" + v.condition + ");";
		}
		else if (v.type == StatementType::If) {
			lis.push_back(v.label + ": " + space + "if " + v.condition + " then");
			//lis << v.label + ": " + space  + "if " + v.condition + " then";
			statementToList(v.ifBody, lis, SpaceNew);
			lis.push_back(space + "   else");
			//lis <<  space + "   else";
			statementToList(v.elseBody, lis, SpaceNew);
			lis.push_back(space + "   endif");
			//lis << space + "   endif";
		}
		else if (v.type == StatementType::While) {
			lis.push_back(v.label + ": " + space + "while " + v.condition + " do");
			//lis << v.label +": " + space + "while " + v.condition + " do";
			statementToList(v.whileBody, lis, SpaceNew);
			lis.push_back(space + "   endwhile;");
			//lis << space + "   endwhile;";
		}
	}
}

void ImpKs::showTip(const string& tip)
{
	//QMessageBox::warning(Q_NULLPTR, "", tip);
	cout << "showTip";
}


void nextVars(const Variables& src, FirstOrderLogical &dst) {
	dst.vars.clear();
	dst.vars = src;
	pair<char,int> pir = dst.assign();

	if (   !(   pir.first==NULL  )   )
		changeValue(dst.vars, pir.first, pir.second);					
}

//������һ����λ�ã�������Ҫ��������
FirstOrderLogical nextStep(vector<FirstOrderLogical> &lgs, FirstOrderLogical &cur) {

	FirstOrderLogical lg;
	if (cur.isNull()) {
		lg.postLable = (*(lgs.begin())).preLable;
		nextVars(cur.vars, lg);
		return lg;
	}

	for (auto& v : lgs) {
		if (v.preLable == cur.postLable) {
			nextVars(cur.vars, v);
			if ( v.isConditionOk() )
				return v;
		}
	}
	return lg;
}



void createKsLables(vector<vector<FirstOrderLogical>>& lgss,
	//����PC��ָ��ǰִ�еĳ����
	const vector<string>& pcs,
	vector<pair<string, string>>& relations,
	vector<string>& labels,
	vector<FirstOrderLogical>& lastLgs,
	//��ǰ����ֵ
	const Variables& vars,
	vector<string> &states,
	vector<KsR> &Rs, int deep = 0) {

	++deep;
	vector<string> tmp = pcs;
	vector<FirstOrderLogical> lastLgsTmp = lastLgs;
	for (int i = 0; i < pcs.size(); ++i) {
		//�û�ִ����֮��ָ�
		KsR oneRs;
		//string oldLabel = tmp.join(' ');
		string oldLabel = jointList(tmp," ");
		oneRs.preLabel = oldLabel;

		//ֻ�����ո�����Ϊ�ǿ�
		string oldTmp = oldLabel;
		oldTmp=remove(oldTmp," ");
		//oldTmp.remove(' ');
		if (oldTmp.empty())
			oldLabel.clear();

		//ִ����Ҫ�ָ�����һ����״̬
		lastLgsTmp[i].vars = vars;
		FirstOrderLogical lastLg = lastLgsTmp[i];
		string lastArgsStr = lastLgsTmp[i].valueToString();
		oneRs.preVars = lastLgsTmp[i].vars;
		if (!oldLabel.empty() && !lastArgsStr.empty() )
			oldLabel += ',' + lastArgsStr;

		if (oldLabel.find("A1") != -1 && !(oldLabel.find("B")!=-1)) {
			int a = 10;
		}

		lastLgsTmp[i] = nextStep(lgss[i], lastLgsTmp[i]);
		
		tmp[i]= lastLgsTmp[i].postLable;
		Variables newVars = lastLgsTmp[i].vars;
		//string newLabel = tmp.join(' ');
		string newLabel = jointList(tmp, " ");

		//�ռ�R�任
		oneRs.postLabel = newLabel;
		oneRs.postVars = lastLgsTmp[i].vars;
		oneRs.opr = lastLgsTmp[i].opr;
		Rs.push_back( oneRs);

		//�ռ�״̬S
		string oneState = lastLgsTmp[i].valueToString();
		
		if (!oneState.empty() && !is_contains_list(states,oneState)) {
			states.push_back(oneState);
		}

		if (!newLabel.empty() && !lastLgsTmp[i].valueToString().empty())
			newLabel += ',' + lastLgsTmp[i].valueToString();

		pair<string, string> r{ oldLabel, newLabel };
		if (is_contains_list(relations,r)) {
			tmp[i] = pcs[i];//����
			lastLgsTmp[i] = lastLg;
			continue;
		}

		if (!oldLabel.empty() && !newLabel.empty()) {
			//relations << pair<string, string>{ oldLabel, newLabel };
			relations.push_back( pair<string, string>{ oldLabel, newLabel } );
		}	
		if (! is_contains_list(labels,oldLabel) && !oldLabel.empty()) {
			labels.push_back(oldLabel);
		}

		if (!is_contains_list(labels, newLabel)&& !newLabel.empty()) {
			labels.push_back(newLabel);
		}
		createKsLables(lgss, tmp, relations, labels, lastLgsTmp, newVars, states, Rs, deep);
		tmp[i] = pcs[i];
		lastLgsTmp[i] = lastLg;
	}
}


bool checkInputOk(const string& input) {
	return true;
}


//��ʼִ�г���
void ImpKs::onStart()
{
	/*
	ui.outputEdit->clear();
	ui.scrollArea->setWidget(Q_NULLPTR);
	*/


	string input = g_input[0];//��������
	cout << "\n���㲽�����ԭʼIMP����" << endl;
	cout << input << endl;


	//���������г����
	P1 program_0;
	vector<string> processes =program_0.parseCoProcesses(input);
	cout << "\n���㲽�����������������" << endl;
	for (int i = 0; i < processes.size(); i++)
	{
		cout << processes[i] << endl;
		cout << "-------------"<<endl;
	}

	
	vector<Statements> statements;
	for (const auto& v : processes) {
		Statements tmp;//��ÿ������p1,p2�ֱ���
		program_0.parseStatements(v, tmp);
		cout << "���statement" << endl;
		//
		for (const auto node : tmp)
		{
			output_node(node);
		}
		//
		statements.push_back(tmp);
	}


	//�����������ϱ�ǩ
	labledStatements(statements);

	//�������ǩ�Ĵ���
	string label_code;//-------------------------------�����ǩ-----------------
	label_code.append("Labeled function:\n");
	//ui.outputEdit->append("Labeled function:\n");
	for (const auto& v : statements) {
		vector<string> list;
		string space;
		statementToList(v, list, space);
		//���һ��������ǩ
		string prefix = list[0].substr(0, 1);
		list.push_back(prefix + "E:");
		//ui.outputEdit->append(list.join('\n'));
		label_code.append(jointList(list, "\n"));

		label_code.append("\n\n");
	}
	cout << "\n�ڶ�����������ǩ�����ĳ���" << endl;
	cout << label_code << endl;

	//����߼���ʽ
	vector<vector<FirstOrderLogical>> lgss;
	string logic_code;//-------------------------------����߼���ʽ-----------------
	logic_code.append("First order logical formula:");
	bool hasPc = statements.size() > 1;
	for (int i = 0; i < statements.size(); ++i) {
		Statement out;
		vector<FirstOrderLogical> formulas = toFormula(statements[i], out);
		lgss.push_back(formulas);
		for (const auto& v : formulas) {
			if (hasPc) {
				string pc = formatString("pc%d", i);
				string formulaNew = v.toString();
				formulaNew.replace(formulaNew.find("pc"), 2, pc);
				logic_code.append(formatString("pc=%s and %s", pc.c_str(), formulaNew.c_str()));
			}
			else
				logic_code.append(v.toString());
		}
	}
	cout << "\n������������߼���ʽ" << endl;
	cout << logic_code << endl;

	vector<string> pcs;
	vector<pair<string, string>> relations;
	vector<string> lables;
	vector<FirstOrderLogical> lastLgs;
	vector<string> states;
	vector<KsR> Rs;
	Variables vars;
	for (const auto& v : lgss) {
		if (lgss.size() > 1) {
			pcs.push_back("U");
		}
		else {
			pcs.push_back("");
		}
		lastLgs.push_back(FirstOrderLogical());
	}
	createKsLables(lgss, pcs, relations, lables, lastLgs, vars, states, Rs);

	//�������S״̬
	string state_code;//------------------------���״̬s
	state_code.append("\n\nAll States:\n");
	int index = 0;
	for (const auto& v : lables) {
		state_code.append(formatString("S%d:(%s)", index++, v.c_str()));
	}

	state_code.append("\n");
	index = 0;
	for (const auto& v : Rs) {
		if (!v.toString().empty())
			state_code.append(formatString("R%d:= %s", index++, v.toString().c_str()));
	}
	cout << "\n���Ĳ����ݣ�KS״̬������" << endl;
	cout << state_code << endl;
	/*
	//����KSͼ
	QWidget* widget = new KsGraphicDrawer(lables, relations);
	ui.scrollArea->setWidget(widget);
	widget->setGeometry(0, 0, ui.scrollArea->width(), ui.scrollArea->height());
	*/
}