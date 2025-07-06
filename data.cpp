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

int v_rd(string s) {
	int l=v[s].first,r=v[s].second;
	return rd(l,r); 
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
			
			for(int j=1;j<=m;j++) { 
				vv[s[j]]=v_rd(s[j]);
			}
			freopen((st(name,cnt)+".in").c_str(),"w",stdout); 
			//只需要修改以下内容------------------------------ 
			cout<<vv["t"]<<endl;
			for(int i=1;i<=vv["t"];i++)
			{
				int l=v_rd("l"),r=v_rd("r");
				while(l>r)r=v_rd("r");
				cout<<l<<" "<<r<<endl;
			}
			//------------------------------------------------- 
			fclose(stdout); 
		} 
	} 
	return 0; 
} 
