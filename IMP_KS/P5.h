#pragma once
#include<iostream>
#include <cassert>
#include<queue>
#include<regex>
#include<sstream>
#include"basic_method.h"
#include"basicStruct.h"
using namespace std;
typedef struct node{
	vector<string> pcs;
	Variables all_var;
};

bool equal(node u,node v)
{
	bool yes = true;
	for (int i = 0; i < u.pcs.size(); i++)
	{
		if (u.pcs[i] != v.pcs[i])
		{
			yes = false;
			break;
		}
	}
	if (yes == false)
	{
		return false;
	}
	yes = true;
	for (int i = 0; i < u.all_var.size(); i++)
	{
		if (!(u.all_var[i] == v.all_var[i]))
		{
			yes = false;
			break;
		}
	}
	if (yes == false)
	{
		return false;
	}
	return true;
}

typedef struct len{
	int from;
	int to;
};
int find(vector<node> nodes,node nod)
{
	for(int i=0;i<nodes.size();i++)
	{
		if (equal(nod,nodes[i]))
		{
			return i;
		}
	}
	return -1;
}
vector<node> nodes;
vector<len> ways;
queue<node> que;
class P5 {
public://pc0=L0_1 ∧ pc0'=L0_2 ∧ (t=0) ∧ SAME(V\{t}) ∧ SAME(PC{pc0})
	void bfs(node beg, vector<vector<FirstOrderLogical>> &in)
	{
		int origin_node = find(nodes,beg);
		for (int i=0;i<in.size();i++)
		{
			for (int j=0;j<in[i].size();j++)
			{
				if (in[i][j].preLable==beg.pcs[i])//第i个进程的pc==logic的preLable
				{
					FirstOrderLogical temp = in[i][j];
					temp.vars = beg.all_var;//根据所有条件判断
					if (temp.condition.empty()==true)//不用判断就转换
					{
						temp.assign();//一个新状态
						node newnode;
						newnode.all_var = temp.vars;
						newnode.pcs = beg.pcs;
						newnode.pcs[i] = in[i][j].postLable;
						int no = find(nodes,newnode);
						if (no == -1)
						{
							no = nodes.size();
							nodes.push_back(newnode);
							que.push(newnode);
						}
						len a2b;
						a2b.from=origin_node;
						a2b.to = no;
						ways.push_back(a2b);
					}
					else//需要判断转换
					{
						if (temp.isConditionOk() == true)
						{
							node newnode;
							newnode = beg;
							newnode.pcs[i] = in[i][j].postLable;
							int no = find(nodes, newnode);
							if (no == -1)
							{
								no = nodes.size();
								nodes.push_back(newnode);
								que.push(newnode);
							}
							len a2b;
							a2b.from = origin_node;
							a2b.to = no;
							ways.push_back(a2b);
						}
					}
				}
			}
		}
	}
	void dfs_search(node &beg, vector<vector<FirstOrderLogical>> in,int deep)
	{
		if (deep==beg.all_var.size())//搜完最后一个。。开始正式搜索
		{
			node ver = beg;//找到一个开始节点
			nodes.push_back(ver);//节点加入
			que.push(ver);//加入队列
			while(!que.empty())
			{
				node temp = que.front();
				que.pop();
				bfs(temp,in);
			}
		}
		else
		{
			if (beg.all_var[deep].type==1 )//bool
			{
				beg.all_var[deep].value = 0;
				dfs_search(beg,in,deep+1);
				beg.all_var[deep].value = 1;
			}
			else//int
			{
				beg.all_var[deep].value = 0;
				dfs_search(beg,in,deep+1);
				beg.all_var[deep].value = 1;
				dfs_search(beg, in, deep + 1);
				beg.all_var[deep].value = 2;
				dfs_search(beg, in, deep + 1);
			}
		}
	}
	string change(vector<vector<FirstOrderLogical>> in)
	{
		Variables all_var;
		for (int i = 0; i < in.size(); i++)
		{
			for (int j = 0; j < in[i].size(); j++)
			{
				for(int k=0;k<in[i][j].vars.size();k++)
				{
					bool yes = false;
					for (int l = 0; l < all_var.size(); l++)
					{
						if (in[i][j].vars[k].name==all_var[l].name)
						{
							yes = true;
							break;
						}
					}
					if(yes==false)
					all_var.push_back(in[i][j].vars[k]);
				}
			}
		}
		//PC,PC
		vector<string> pc;
		for (int i=0;i<in.size();i++)
		{
			pc.push_back("L"+to_string(i)+"_1");//所有的pc标签
		}
		node begin;
		begin.all_var = all_var;
		begin.pcs = pc;
		dfs_search(begin,in,0);
		cout << nodes.size()<<"--------"<<ways.size() <<endl;

		ostringstream sout;
		sout << "{ \"class\": \"go.GraphLinksModel\",\"nodeKeyProperty\" : \"id\",\"nodeDataArray\" : " << endl;

		sout << "[";
		for (int i = 0; i < nodes.size(); i++)
		{
			if (i != 0)
				sout << ",";
			sout << "{" << endl;
			sout << " \"id\":" + to_string(i) << ",\"text\":\"";
			for (const auto&u : nodes[i].pcs)
			{
				sout << u << " ";
			}
			sout << ",";
			for (const auto&u : nodes[i].all_var)
			{
				sout << u.name << "=" << u.value << " ";
			}
			sout << "\"}\n" << endl;
		}
		sout << "]";
		sout << ",\"linkDataArray\":\n";
		sout << "[";
		for (int i = 0; i < ways.size(); i++)
		{
			if (i != 0)
				sout << ",";
			sout << "{\"from\":" + to_string( ways[i].from)+",\"to\":"+ to_string(ways[i].to)+"}\n";
		}
		sout << "]}";
		return sout.str();
	}
};