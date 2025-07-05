#ifndef TREE_UTILS_HPP
#define TREE_UTILS_HPP 

#include<bits/stdc++.h>
#define int long long
using namespace std;

//生成树
vector<pair<int, int>> generateTree(int n) {
	vector<pair<int, int>> edges; 
	for (int i = 2; i <= n; ++i) { 
		int parent = rand() % (i - 1) + 1; 
		edges.emplace_back(parent,  i); 
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



#endif
