// ============================================================
//  main.cpp  —  主控程序  v2.0.1
//  功能：编译 → 生成数据 → 跑标程 → [可选] 对拍用户程序
//
//  用法：
//    g++ -O2 -o main.exe main.cpp
//    main.exe            # 普通模式（只跑标程生成答案）
//    main.exe --check    # 对拍模式（标程 vs 用户程序）
//    main.exe --time     # 计时模式（统计每组数据耗时）
// ============================================================

#include <bits/stdc++.h>
#include "function.hpp"
using namespace std;

// ---------- 彩色输出（ANSI 转义码，MinGW / Linux / macOS 均支持）----------
// 注意：不引入 <windows.h>，避免与 std::byte 产生命名冲突
void log_ok  (const string& s) { cerr << "\033[32m[ OK ] \033[0m" << s << "\n"; }
void log_err (const string& s) { cerr << "\033[31m[FAIL] \033[0m" << s << "\n"; }
void log_info(const string& s) { cerr << "\033[36m[INFO] \033[0m" << s << "\n"; }
void log_warn(const string& s) { cerr << "\033[33m[WARN] \033[0m" << s << "\n"; }

// ---------- 执行命令并返回是否成功 ----------
bool run(const string& cmd) {
	return system(cmd.c_str()) == 0;
}

// ---------- 比较两个文件内容（忽略行尾空白）----------
bool filesEqual(const string& f1, const string& f2) {
	ifstream a(f1), b(f2);
	if (!a || !b) return false;
	string la, lb;
	while (true) {
		bool ga = (bool)getline(a, la);
		bool gb = (bool)getline(b, lb);
		if (!ga && !gb) return true;
		if (!ga || !gb) return false;
		// 去掉行尾 \r（Windows 换行）
		while (!la.empty() && (la.back() == '\r' || la.back() == ' ')) la.pop_back();
		while (!lb.empty() && (lb.back() == '\r' || lb.back() == ' ')) lb.pop_back();
		if (la != lb) return false;
	}
}

// ---------- 获取当前时间戳（毫秒）----------
long long nowMs() {
	return chrono::duration_cast<chrono::milliseconds>(
		chrono::steady_clock::now().time_since_epoch()
		).count();
}

signed main(int argc, char* argv[]) {
	// 解析参数
	bool checkMode = false, timeMode = false;
	string userExe = "user.exe", userSrc = "user.cpp";
	for (int i = 1; i < argc; ++i) {
		string arg(argv[i]);
		if (arg == "--check") checkMode = true;
		if (arg == "--time")  timeMode  = true;
		if (arg == "--user" && i + 1 < argc) {
			userSrc = argv[++i];
			userExe = userSrc.substr(0, userSrc.rfind('.')) + ".exe";
		}
	}
	
	// 读取题目名（新格式：第一个非注释行为 "problem <name>"）
	ifstream fin("data.txt");
	if (!fin) { log_err("无法打开 data.txt"); return 1; }
	string name, line;
	while (getline(fin, line)) {
		int lo = 0, hi = (int)line.size() - 1;
		while (lo <= hi && isspace((unsigned char)line[lo])) ++lo;
		while (hi >= lo && isspace((unsigned char)line[hi])) --hi;
		string tl = (lo > hi) ? "" : line.substr(lo, hi - lo + 1);
		if (tl.empty() || tl[0] == '#') continue;
		istringstream iss(tl);
		string kw; iss >> kw;
		if (kw == "problem") { iss >> name; break; }
	}
	fin.close();
	if (name.empty()) { log_err("data.txt 中未找到 problem 行"); return 1; }
	
	log_info("problem: " + name);
	if (checkMode) log_info("mode: check  (std vs " + userSrc + ")");
	else           log_info("mode: normal (generate + std answer)");
	
	// compile std
	log_info("compiling std.cpp ...");
	if (!run("g++ -O2 -o std.exe std.cpp")) {
		log_err("std.cpp compile failed"); return 1;
	}
	log_ok("std.cpp compiled");
	
	// compile data generator
	log_info("compiling data.cpp ...");
	if (!run("g++ -O2 -o data.exe data.cpp")) {
		log_err("data.cpp compile failed"); return 1;
	}
	log_ok("data.cpp compiled");
	
	// compile user program (check mode)
	if (checkMode) {
		log_info("compiling " + userSrc + " ...");
		if (!run("g++ -O2 -o " + userExe + " " + userSrc)) {
			log_err(userSrc + " compile failed"); return 1;
		}
		log_ok(userSrc + " compiled");
	}
	
	// generate test data
	log_info("generating test data ...");
	if (!run("data.exe")) {
		log_err("data.exe failed"); return 1;
	}
	log_ok("data generated");
	
	// 遍历所有数据文件
	int cnt = 0, failCnt = 0;
	long long totalTime = 0;
	vector<int> failList;
	
	while (true) {
		string filepath = st(name, cnt + 1);
		string inFile   = filepath + ".in";
		string outFile  = filepath + ".out";
		string usrFile  = filepath + ".usr";
		
		if (!fileExists(inFile)) break;
		++cnt;
		
		// 运行标程
		long long t0 = nowMs();
		bool stdOk = run("std.exe < " + inFile + " > " + outFile);
		long long elapsed = nowMs() - t0;
		totalTime += elapsed;
		
		if (!stdOk) {
			log_err("std RE/TLE on case #" + to_string(cnt));
			++failCnt; failList.push_back(cnt);
			continue;
		}
		
		// check mode: run user program and compare
		if (checkMode) {
			bool usrOk = run(userExe + " < " + inFile + " > " + usrFile);
			if (!usrOk) {
				log_err("user RE on case #" + to_string(cnt));
				++failCnt; failList.push_back(cnt);
				continue;
			}
			if (!filesEqual(outFile, usrFile)) {
				log_err("WA on case #" + to_string(cnt));
				++failCnt; failList.push_back(cnt);
				continue;
			}
			log_ok("case #" + to_string(cnt) + "  AC"
				+ (timeMode ? "  (" + to_string(elapsed) + " ms)" : ""));
		} else {
			log_ok("case #" + to_string(cnt) + "  done"
				+ (timeMode ? "  (" + to_string(elapsed) + " ms)" : ""));
		}
	}
	
	// summary
	cerr << "\n";
	log_info("==================");
	log_info("total cases : " + to_string(cnt));
	if (timeMode)
		log_info("std time    : " + to_string(totalTime) + " ms total, "
			+ to_string(cnt ? totalTime / cnt : 0) + " ms avg");
	if (failCnt == 0) {
		log_ok("all passed!");
	} else {
		log_err("failed " + to_string(failCnt) + " case(s):");
		for (int id : failList) cerr << "  #" << id << "\n";
	}
	
	return failCnt > 0 ? 1 : 0;
}
