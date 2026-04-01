#ifndef FUNCTION_HPP
#define FUNCTION_HPP

// ============================================================
//  function.hpp  —  数据生成工具库  v2.0.1
//  跨平台支持 (Windows / Linux / macOS)
//
//  注意：本头文件不定义 #define int long long
//        请在各 .cpp 文件中自行定义
// ============================================================

#include <bits/stdc++.h>

// ---------- 平台兼容层 ----------
#ifdef _WIN32
#include <direct.h>
#define MKDIR(p) _mkdir(p)
#else
#include <sys/stat.h>
#include <sys/types.h>
#define MKDIR(p) mkdir(p, 0755)
#endif

using namespace std;

// ============================================================
//  随机数引擎（mt19937_64）
// ============================================================
static mt19937_64 rng(
	chrono::steady_clock::now().time_since_epoch().count()
	);

// 生成 [l, r] 均匀随机整数
inline long long rd(long long l, long long r) {
	if (l > r) swap(l, r);
	return uniform_int_distribution<long long>(l, r)(rng);
}

// 生成 [l, r] 均匀随机浮点数
inline double rd_double(double l, double r) {
	return uniform_real_distribution<double>(l, r)(rng);
}

// 随机布尔值（p 为 true 的概率，默认 0.5）
inline bool rd_bool(double p = 0.5) {
	return bernoulli_distribution(p)(rng);
}

// 随机打乱 vector
template<typename T>
void shuffle_vec(vector<T>& v) {
	shuffle(v.begin(), v.end(), rng);
}

// ============================================================
//  图 / 树 生成（参数统一用 long long）
// ============================================================

// 随机无权树（n 个节点），返回 n-1 条边
vector<pair<long long, long long>> BuildTree(long long n) {
	vector<pair<long long, long long>> edges;
	edges.reserve((size_t)(n - 1));
	for (long long i = 2; i <= n; ++i) {
		long long parent = rd(1, i - 1);
		edges.emplace_back(parent, i);
	}
	return edges;
}

// 随机带权树，返回 (u, v, w)
vector<tuple<long long, long long, long long>>
BuildWeightedTree(long long n, long long minW, long long maxW) {
	vector<tuple<long long, long long, long long>> edges;
	edges.reserve((size_t)(n - 1));
	for (long long i = 2; i <= n; ++i) {
		long long parent = rd(1, i - 1);
		long long weight = rd(minW, maxW);
		edges.emplace_back(parent, i, weight);
	}
	return edges;
}

// 随机连通无向图（n 节点，m 条边），先建生成树再补边
vector<pair<long long, long long>>
BuildGraph(long long n, long long m, bool allowSelfLoop = false) {
	assert(m >= n - 1 && m <= n * (n - 1) / 2);
	set<pair<long long, long long>> edgeSet;
	vector<pair<long long, long long>> edges;
	
	for (auto& e : BuildTree(n)) {
		long long u = min(e.first, e.second);
		long long v = max(e.first, e.second);
		edgeSet.insert({u, v});
		edges.push_back({u, v});
	}
	
	long long attempts = 0;
	while ((long long)edges.size() < m && attempts < m * 20) {
		++attempts;
		long long u = rd(1, n), v = rd(1, n);
		if (!allowSelfLoop && u == v) continue;
		if (u > v) swap(u, v);
		if (edgeSet.count({u, v})) continue;
		edgeSet.insert({u, v});
		edges.push_back({u, v});
	}
	return edges;
}

// 随机 DAG（n 节点，m 条边）
vector<pair<long long, long long>>
BuildDAG(long long n, long long m) {
	assert(m <= n * (n - 1) / 2);
	set<pair<long long, long long>> edgeSet;
	vector<pair<long long, long long>> edges;
	long long attempts = 0;
	while ((long long)edges.size() < m && attempts < m * 20) {
		++attempts;
		long long u = rd(1, n), v = rd(1, n);
		if (u >= v) continue;
		if (edgeSet.count({u, v})) continue;
		edgeSet.insert({u, v});
		edges.push_back({u, v});
	}
	return edges;
}

// ============================================================
//  并查集
// ============================================================
struct DSU {
	vector<long long> parent, rank_;
	DSU(long long n) : parent((size_t)(n + 1)), rank_((size_t)(n + 1), 0) {
		iota(parent.begin(), parent.end(), 0LL);
	}
	long long find(long long x) {
		return parent[x] == x ? x : parent[x] = find(parent[x]);
	}
	bool unite(long long x, long long y) {
		x = find(x); y = find(y);
		if (x == y) return false;
		if (rank_[x] < rank_[y]) swap(x, y);
		parent[y] = x;
		if (rank_[x] == rank_[y]) rank_[x]++;
		return true;
	}
	bool connected(long long x, long long y) { return find(x) == find(y); }
};

// ============================================================
//  字符串 / 序列生成
// ============================================================

// 随机小写字母字符串
inline string rd_string(long long len, char lo = 'a', char hi = 'z') {
	string s((size_t)len, ' ');
	for (auto& c : s) c = (char)rd(lo, hi);
	return s;
}

// 随机排列 [1..n]
vector<long long> rd_permutation(long long n) {
	vector<long long> p((size_t)n);
	iota(p.begin(), p.end(), 1LL);
	shuffle_vec(p);
	return p;
}

// ============================================================
//  文件 / 目录工具
// ============================================================

inline bool fileExists(const string& path) {
	ifstream f(path);
	return f.good();
}

inline void createDirectory(const string& path) {
	if (MKDIR(path.c_str()) == -1 && errno != EEXIST)
		cerr << "[WARN] createDirectory failed: " << path << "\n";
}

// 生成数据文件路径（不含扩展名）
// 例: st("grid", 3) → "data/grid/grid3"
inline string st(const string& name, long long n) {
	return "data/" + name + "/" + name + to_string(n);
}

// ============================================================
//  输出辅助
// ============================================================

template<typename T>
void print_vec(const vector<T>& v, const string& sep = " ") {
	for (size_t i = 0; i < v.size(); ++i) {
		if (i) cout << sep;
		cout << v[i];
	}
	cout << "\n";
}

inline void print_edges(const vector<pair<long long, long long>>& edges) {
	for (auto& [u, v] : edges) cout << u << " " << v << "\n";
}

inline void print_edges(const vector<tuple<long long, long long, long long>>& edges) {
	for (auto& [u, v, w] : edges) cout << u << " " << v << " " << w << "\n";
}

#endif // FUNCTION_HPP
