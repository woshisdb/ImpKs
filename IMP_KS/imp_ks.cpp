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
* \b 一阶逻辑数据结构，可以从一阶逻辑生成KS结构
  \a preLable 前置标签
  \a postLable 后置标签
  \a condition 条件
  \a string opr 操作
  \a vector<Variable> vars;
*/
//KS 中的R结构
//构造,初始化业务类
ImpKs::ImpKs()
{

}

//将输入代码解析为语法树



bool checkInputOk(const string& input) {
	return true;
}

//开始执行程序
void ImpKs::onStart()
{
	string input = g_input[0];//输出结果？
	cout << "\n第零步结果：原始IMP程序" << endl;
	cout << input << endl;

	//解析出所有程序段
	P1 program_0;
	vector<string> processes =program_0.parseCoProcesses(input);
	cout << "\n第零步结果：输出解析程序段" << endl;
	for (int i = 0; i < processes.size(); i++)
	{
		cout << processes[i] << endl;
		cout << "-------------"<<endl;
	}
	vector<Statements> statements;
	for (const auto& v : processes) {
		Statements tmp;//对每个程序p1,p2分别处理
		program_0.parseStatements(v, tmp);
		/*
		cout << "输出statement" << endl;
		//
		for (const auto node : tmp)
		{
			output_node(node);
		}
		*/
		statements.push_back(tmp);
	}

	//给所有语句打上标签
	P2 program_1;
	program_1.labledStatements(statements);
	string label_code=program_1.out_result(statements);

	//输出逻辑公式
	P3 program_2;
	vector<vector<FirstOrderLogical>> lgss;
	string logic_code;//-------------------------------输出逻辑公式-----------------
	lgss = program_2.to_logic(statements,logic_code);
	cout << "\n第三步结果：逻辑公式" << endl;
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

	cout << "\n第四步数据：KS状态机描述" << endl;
	cout << result<< endl;

	
}