// ============================================================
//  data.cpp  —  数据生成器  v2.1.0
//  用法：编译后直接运行，读取 data.txt 生成测试数据
// ============================================================

#include <bits/stdc++.h>
#include "function.hpp"
#define int long long
#define endl "\n"
using namespace std;

// ============================================================
//  全局变量表（供生成逻辑使用）
//  v[name]  → {左界, 右界}
//  vv[name] → 本组随机值
// ============================================================
map<string, pair<int,int>> v;
map<string, int>           vv;

int v_rd(const string& s) {
	if (!v.count(s)) {
		cerr << "[ERROR] 未定义的变量：" << s << "\n";
		exit(1);
	}
	return rd(v[s].first, v[s].second);
}

// ============================================================
//  data.txt 格式（# 开头为注释，空行忽略）
//
//    problem <题目名>
//
//    group
//      count  <组数>
//      <变量名>  <左界>  <右界>
//      ...
//    end
//
//  示例：
//    problem price
//
//    # 小数据
//    group
//      count 10
//      t  1    50
//      l  1    1000000
//      r  1    1000000
//    end
//
//    # 大数据（极限）
//    group
//      count 3
//      t  1          5
//      l  300000     1000000000
//      r  300000     1000000000
//    end
// ============================================================

struct Group {
	int count = 0;
	vector<string>             varOrder;
	map<string, pair<int,int>> ranges;
};

string trim(const string& s) {
	int lo = 0, hi = (int)s.size() - 1;
	while (lo <= hi && isspace((unsigned char)s[lo])) ++lo;
	while (hi >= lo && isspace((unsigned char)s[hi])) --hi;
	return lo > hi ? "" : s.substr(lo, hi - lo + 1);
}

pair<string, vector<Group>> parseConfig(const string& filename) {
	ifstream fin(filename);
	if (!fin) { cerr << "[ERROR] cannot open " << filename << "\n"; exit(1); }
	
	string problemName;
	vector<Group> groups;
	string line;
	bool inGroup = false;
	Group cur;
	
	while (getline(fin, line)) {
		line = trim(line);
		if (line.empty() || line[0] == '#') continue;
		
		istringstream iss(line);
		string token;
		iss >> token;
		
		if (token == "problem") {
			iss >> problemName;
		} else if (token == "group") {
			if (inGroup) { cerr << "[ERROR] nested group\n"; exit(1); }
			inGroup = true;
			cur = Group{};
		} else if (token == "end") {
			if (!inGroup) { cerr << "[ERROR] unexpected end\n"; exit(1); }
			if (cur.count <= 0) { cerr << "[ERROR] group missing count or count<=0\n"; exit(1); }
			groups.push_back(cur);
			inGroup = false;
		} else if (token == "count") {
			iss >> cur.count;
		} else if (inGroup) {
			int l, r;
			if (!(iss >> l >> r)) {
				cerr << "[ERROR] bad variable line: " << line << "\n"; exit(1);
			}
			if (l > r) {
				cerr << "[WARN]  " << token << ": l>r, swapped\n";
				swap(l, r);
			}
			if (!cur.ranges.count(token)) cur.varOrder.push_back(token);
			cur.ranges[token] = {l, r};
		} else {
			cerr << "[WARN]  unknown line: " << line << "\n";
		}
	}
	
	if (inGroup)             { cerr << "[ERROR] group not closed by end\n"; exit(1); }
	if (problemName.empty()) { cerr << "[ERROR] missing problem line\n";    exit(1); }
	return {problemName, groups};
}

signed main() {
	auto [name, groups] = parseConfig("data.txt");
	
	createDirectory("data");
	createDirectory("data/" + name);
	
	int totalCnt = 0;
	cerr << "[INFO] problem: " << name
	<< "  groups: " << (int)groups.size() << "\n";
	
	for (int gi = 0; gi < (int)groups.size(); ++gi) {
		auto& g = groups[gi];
		
		cerr << "[INFO] group " << gi + 1
		<< "  count=" << g.count;
		for (auto& vname : g.varOrder) {
			auto [l, r] = g.ranges[vname];
			cerr << "  " << vname << "=[" << l << "," << r << "]";
		}
		cerr << "\n";
		
		// update global variable table
		v = g.ranges;
		
		for (int k = 0; k < g.count; ++k) {
			++totalCnt;
			
			for (auto& vname : g.varOrder)
				vv[vname] = v_rd(vname);
			
			string filepath = st(name, totalCnt);
			if (!freopen((filepath + ".in").c_str(), "w", stdout)) {
				cerr << "[ERROR] cannot write " << filepath << ".in\n";
				exit(1);
			}
			
			// ======================================================
			//  ↓↓↓ 在这里编写数据生成逻辑 ↓↓↓
			// ======================================================
			
			cout << vv["t"] << endl;
			for (int q = 1; q <= vv["t"]; ++q) {
				int l = v_rd("l"), r = v_rd("r");
				if (l > r) swap(l, r);
				cout << l << " " << r << endl;
			}
			
			// ======================================================
			//  ↑↑↑ 数据生成逻辑结束 ↑↑↑
			// ======================================================
			
			fclose(stdout);
		}
	}
	
	cerr << "[INFO] done, total " << totalCnt << " case(s).\n";
	return 0;
}
