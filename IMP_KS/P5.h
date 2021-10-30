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
	bool equal(len res)
	{
		if (from == res.from&&to == res.to)
		{
			return true;
		}
		return false;
	}
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
void insert(vector<len> &ways,len now)
{
	int yes=false;
	for (auto& v : ways)
	{
		if (v.equal(now) == true)
		{
			yes = true;
		}
	}
	if (yes == false)
	{
		ways.push_back(now);
	}
}
class P5 {
public://pc0=L0_1 �� pc0'=L0_2 �� (t=0) �� SAME(V\{t}) �� SAME(PC{pc0})
	void nextVars(const Variables& src, FirstOrderLogical &dst) {
		dst.vars.clear();
		dst.vars = src;
		pair<char, int> pir = dst.assign();

		if (!(pir.first == NULL))
			changeValue(dst.vars, pir.first, pir.second);
	}
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
			vars.push_back(Variable{ Variable::Int, var, value });
	}
	void bfs(node beg, vector<vector<FirstOrderLogical>> &in)
	{
		int origin_node = find(nodes,beg);
		for (int i=0;i<in.size();i++)
		{
			for (int j=0;j<in[i].size();j++)
			{
				if (in[i][j].preLable==beg.pcs[i])//��i�����̵�pc==logic��preLable
				{
					FirstOrderLogical temp = in[i][j];
					temp.vars = beg.all_var;//�������������ж�
					if (temp.condition.empty()==true)//�����жϾ�ת��
					{
						pair<char, int> newvar = temp.assign();
						changeValue(temp.vars,newvar.first,newvar.second);//赋值新的值
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
						insert(ways,a2b);
						//ways.push_back(a2b);
					}
					else//��Ҫ�ж�ת��
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
							insert(ways, a2b);
							//ways.push_back(a2b);
						}
					}
				}
			}
		}
	}
	int first_ok = 0;
	void dfs_search(node &beg, vector<vector<FirstOrderLogical>> in,int deep)
	{
		if (deep==beg.all_var.size())//�������һ��������ʼ��ʽ����
		{
			first_ok++;
			if (first_ok == 1)
			{
				node star = beg;
				star.all_var.clear();
				for (int i = 0; i < star.pcs.size(); i++)
				{
					star.pcs[i] = "null";
				}
				nodes.push_back(star);//放入第0个节点
			}
			node ver = beg;
			if (find(nodes,ver)==-1)
			{
				nodes.push_back(ver);
				int end_no = find(nodes, ver);
				len temp;
				temp.from = 0;
				temp.to = end_no;
				insert(ways,temp);
				que.push(ver);
				while (!que.empty())
				{
					node temp = que.front();
					que.pop();
					bfs(temp, in);
				}
			}
			else
			{
				int end_no = find(nodes, ver);
				len temp;
				temp.from = 0;
				temp.to = end_no;
				insert(ways, temp);
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
				Variables tempvs;
				nextVars(tempvs,in[i][j]);
				char newy='0';
				char newz='0';
				bool yes=in[i][j].conditionval(newy,newz);
				if (yes == true)
				{
					if(!(newy>='0'&&newy<='9'))
					changeValue(all_var,newy,0);
					if (!(newz >= '0'&&newz <= '9'))
					changeValue(all_var, newy, 0);
				}
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
			pc.push_back("L"+to_string(i)+"_1");//���е�pc��ǩ
		}
		node begin;
		begin.all_var = all_var;
		begin.pcs = pc;
		dfs_search(begin,in,0);
		//cout << nodes.size()<<"--------"<<ways.size() <<endl;

		ostringstream sout;
		sout << "var nodeDataArray = " << endl;

		sout << "[";
		for (int i = 0; i < nodes.size(); i++)
		{
			if (i != 0)
				sout << ",";
			sout << "{";
			sout << "\"id\":" + to_string(i) << ",\"text\":\"";
			string flag_null = "";
			for (const auto&u : nodes[i].pcs)
			{
				sout << u << " ";
				flag_null = u;
			}
			sout << ",";
			for (const auto&u : nodes[i].all_var)
			{
				sout << u.name << "=" << u.value << " ";
			}
			if (flag_null == "null") {
				sout << "\",\"category\":\"Start\"";
			} else {
				sout << "\"";
			}
			sout << "}" << endl;
		}
		sout << "]\n";
		sout << "var linkDataArray = " << endl;
		sout << "[";
		for (int i = 0; i < ways.size(); i++)
		{
			if (i != 0)
				sout << ",";
			sout << "{\"from\":" + to_string( ways[i].from)+",\"to\":"+ to_string(ways[i].to)+"}\n";
		}
		sout << "]";
	return sout.str();
	}
};