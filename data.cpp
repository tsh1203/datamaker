#include<bits/stdc++.h> 
#include "function.hpp"  
#include <direct.h>  // 用于 Windows 系统创建文件夹 
#define int long long 
#define endl "\n" 
using namespace std; 
ifstream fin("data.txt");  
map<string,pair<int,int>>v; 
map<string,int>vv; 
string s[100005]; 

int rd(int l,int r) { 
	int x=l+rand()%(r-l+1); 
	return x; 
} 

string st(string name,int n) { 
	name="data/"+name+"/"+name; 
	name+=to_string(n); 
	name+=".in"; 
	return name; 
} 

// 创建文件夹函数 
void createDirectory(const string& path) { 
	if (_mkdir(path.c_str()) == -1) { 
		if (errno != EEXIST) { 
			cerr << "Failed to create directory: " << path << endl; 
		} 
	} 
} 

signed main() { 
	struct _timeb T; 
	_ftime(&T); 
	srand(T.millitm);  
	string name; 
	int n,m,cnt=0; 
	if (!fin) { 
		cout << "Failed to open data.txt"   << endl; 
		return 1; 
	} 
	fin>>name>>n>>m; 
	for(int i=1;i<=n;i++) { 
		int t; 
		fin>>t; 
		for(int j=1;j<=m;j++) { 
			int l,r; 
			fin>>s[j]>>l>>r; 
			v[s[j]]={l,r}; 
		} 
		while(t-- >0) { 
			++cnt; 
			string dirPath = "data/" + name; 
			createDirectory("data");  // 创建 data 文件夹 
			createDirectory(dirPath);  // 创建 data/name 文件夹 
			
			freopen(st(name,cnt).c_str(),"w",stdout); 
			for(int j=1;j<=m;j++) { 
				vv[s[j]]=rd(v[s[j]].first,v[s[j]].second); 
			} 
			while(vv["n"]<=vv["m"])vv["n"]=rd(v["n"].first,v["n"].second); 
			//只需要修改以下内容------------------------------ 
			for(int j=1;j<=m;j++) { 
				vv[s[j]]=rd(v[s[j]].first,v[s[j]].second); 
			} 
			cout<<vv["n"]<<endl; 
			vector<pair<int, int>> roads = generateTree(vv["n"]); 
			for (const auto& road : roads) { 
				cout << road.first   << " " << road.second  <<" "<<rd(1,1000) << endl; 
			} 
			//------------------------------------------------- 
			fclose(stdout); 
		} 
	} 
	return 0; 
} 
