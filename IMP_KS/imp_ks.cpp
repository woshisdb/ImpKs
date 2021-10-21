#define _CRT_SECURE_NO_WARNINGS
#include "imp_ks.h"
#include"P1.h"
#include"P2.h"
#include"P3.h"
#include"P4.h"
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
//KS �е�R�ṹ
//����,��ʼ��ҵ����
ImpKs::ImpKs()
{

}

//������������Ϊ�﷨��



bool checkInputOk(const string& input) {
	return true;
}

//��ʼִ�г���
void ImpKs::onStart()
{
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
		/*
		cout << "���statement" << endl;
		//
		for (const auto node : tmp)
		{
			output_node(node);
		}
		*/
		statements.push_back(tmp);
	}

	//�����������ϱ�ǩ
	P2 program_1;
	program_1.labledStatements(statements);
	string label_code=program_1.out_result(statements);

	//����߼���ʽ
	P3 program_2;
	vector<vector<FirstOrderLogical>> lgss;
	string logic_code;//-------------------------------����߼���ʽ-----------------
	lgss = program_2.to_logic(statements,logic_code);
	cout << "\n������������߼���ʽ" << endl;
	cout << logic_code << endl;
	cout << "************************" << endl;

	P4 program3;

	vector<string> pcs;
	vector<pair<string, string>> relations;
	vector<string> lables;
	vector<FirstOrderLogical> lastLgs;
	vector<string> states;
	vector<KsR> Rs;
	Variables vars;
	program3.to_label(lgss,pcs,relations, lables,lastLgs,states, Rs,vars);
	program3.createKsLables(lgss, pcs, relations, lables, lastLgs, vars, states, Rs);

	string result = program3.out_result(lables,Rs);

	cout << "\n���Ĳ����ݣ�KS״̬������" << endl;
	cout << result<< endl;

	
}