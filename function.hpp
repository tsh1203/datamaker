#ifndef TREE_UTILS_HPP
#define TREE_UTILS_HPP 

#include<bits/stdc++.h>
#include<direct.h>
#include<windows.h>
#define int long long
using namespace std;

//随机数
int rd(int l,int r) {
	int x,t=rand()%4;
	if(t==1) x=l+((rand()<<15)+rand())%(r-l+1); 
	else if(t==2) x=l+rand()%(r-l+1);
	else if(t==3) x=l+rand()*rand()%(r-l+1);
	else if(t==0) x=l+rand()*(0235)%(r-l+1);
	return x; 
}

//生成树
vector<pair<int, int>> BuildTree(int n) {//无权树
	vector<pair<int, int>> edges; 
	for (int i = 2; i <= n; ++i) { 
		int parent = rand() % (i - 1) + 1; 
		edges.emplace_back(parent,  i); 
	} 
	return edges; 
}
vector<pair<int, pair<int, int>>> BuildWeightedTree(int n, int minW, int maxW) { //带权树
	vector<pair<int, pair<int, int>>> edges; 
	for (int i = 2; i <= n; ++i) { 
		int parent = rand() % (i - 1) + 1;
		int weight = rd(minW, maxW);
		edges.emplace_back(weight,  make_pair(parent, i));
	} 
	return edges; 
} 

//并查集
int find(vector<int>& parent, int x) {
	if (parent[x] != x) {
		parent[x] = find(parent, parent[x]);
	}
	return parent[x];
}

void unite(vector<int>& parent, int x, int y) {
	int rootX = find(parent, x);
	int rootY = find(parent, y);
	if (rootX != rootY) {
		parent[rootX] = rootY;
	}
}

//文件名生成
string st(string name,int n)
{
	name="data/"+name+"/"+name;
	name+=to_string(n);
	return name;
}

//文件存在判断
bool isFileExists_ifstream(const string& name) {
	ifstream f(name.c_str());
	return f.good();
}

// 创建文件夹函数 
void createDirectory(const string& path) { 
	if (_mkdir(path.c_str()) == -1) { 
		if (errno != EEXIST) { 
			cerr << "Failed to create directory: " << path << endl; 
		} 
	} 
} 
#endif
