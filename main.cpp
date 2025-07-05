#include<bits/stdc++.h> 
using namespace std; 
ifstream fin("data.txt");
int cnt; 
string st(string name,int n)
{
	name="data/"+name+"/"+name;
	name+=to_string(n);
	return name;
}
bool isFileExists_ifstream(const string& name) {
	ifstream f(name.c_str());
	return f.good();
}
int main() 
{ 
	system("@echo off"); 
	//system("Cd !cd!");//填入文件目录
	string name;
	fin>>name;
	if (system("g++ -O3 -o std.exe   std.cpp") != 0){ 
		cerr<<"Error: Compilation of std.cpp  failed."<< endl; 
		return 1; 
	} 
	cout<<"std.cpp  Running completed\n"; 
	
	// 编译数据生成器 
	if (system("g++ -O3 -o data.exe   data.cpp") != 0){ 
		cerr<<"Error: Compilation of data.cpp  failed."<<endl; 
		return 1; 
	} 
	cout<<"data.cpp Running completed\n"; 
	system("data.exe");
	int cnt=0;
	while(1)
	{
		string filename = st(name,++cnt);
		if (isFileExists_ifstream((filename+".in").c_str()))
		{
			string op="std.exe < "+filename+".in >"+filename+".out";
			system(op.c_str());
		}
		else break;
	}
	return 0; 
} 
