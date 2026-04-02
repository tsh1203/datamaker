#ifndef FUNCTION_HPP
#define FUNCTION_HPP

// ============================================================
//  function.hpp  —  Data Generation Toolkit  v3.1.0
//  Cross-platform: Windows / Linux / macOS
//
//  Sections:
//    1. Error reporting
//    2. RNG
//    3. Graph / Tree
//    4. DSU
//    5. Multi-dimensional vectors / sequences
//    6. Function-expression sequences
//    7. Random polygons
//    8. String / word / sentence generation
//    9. File / directory utilities
//   10. Output helpers
// ============================================================

#include <bits/stdc++.h>

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
//  1. Error reporting
//
//  _ERR(fn, msg)  — fatal error, prints function name + message, exits
//  _WARN(fn, msg) — non-fatal warning, prints and continues
// ============================================================

#define _ERR(fn, msg) \
    do { \
        cerr << "\033[31m[ERROR]\033[0m " << (fn) << ": " << (msg) << "\n"; \
        exit(1); \
    } while(0)

#define _WARN(fn, msg) \
    do { \
        cerr << "\033[33m[WARN] \033[0m " << (fn) << ": " << (msg) << "\n"; \
    } while(0)

// ============================================================
//  2. RNG  (mt19937_64)
// ============================================================
static mt19937_64 rng(
    chrono::steady_clock::now().time_since_epoch().count()
);

// [l, r] uniform integer
inline long long rd(long long l, long long r) {
    if (l > r) {
        _WARN("rd", "l=" + to_string(l) + " > r=" + to_string(r) + ", swapped");
        swap(l, r);
    }
    return uniform_int_distribution<long long>(l, r)(rng);
}

// [l, r) uniform double
inline double rd_double(double l = 0.0, double r = 1.0) {
    if (l > r) {
        _WARN("rd_double", "l > r, swapped");
        swap(l, r);
    }
    return uniform_real_distribution<double>(l, r)(rng);
}

// random bool with probability p
inline bool rd_bool(double p = 0.5) {
    if (p < 0.0 || p > 1.0)
        _ERR("rd_bool", "p=" + to_string(p) + " out of [0,1]");
    return bernoulli_distribution(p)(rng);
}

template<typename T>
void shuffle_vec(vector<T>& v) { shuffle(v.begin(), v.end(), rng); }

// ============================================================
//  3. Graph / Tree
// ============================================================

// ---- 3.1  Tree ----

enum class TreeShape { RANDOM, CHAIN, FLOWER, BALANCED };

// Build a tree of n nodes.
//   shape    : RANDOM / CHAIN / FLOWER / BALANCED
//   strength : [0.0, 1.0], only used for RANDOM shape
//              0.0 = deep random tree
//              1.0 = very shallow (near-star / flower)
vector<pair<long long,long long>>
BuildTree(long long n,
          TreeShape shape = TreeShape::RANDOM,
          double strength = 0.0)
{
    static const char* FN = "BuildTree";
    if (n < 1)
        _ERR(FN, "n=" + to_string(n) + " must be >= 1");
    if (strength < 0.0 || strength > 1.0)
        _ERR(FN, "strength=" + to_string(strength) + " must be in [0.0, 1.0]");

    vector<pair<long long,long long>> edges;
    edges.reserve((size_t)(n - 1));

    for (long long i = 2; i <= n; ++i) {
        long long parent;
        switch (shape) {
        case TreeShape::CHAIN:
            parent = i - 1;
            break;
        case TreeShape::FLOWER:
            parent = 1;
            break;
        case TreeShape::BALANCED:
            parent = i / 2;
            break;
        case TreeShape::RANDOM:
        default:
            if (strength <= 0.0) {
                parent = rd(1, i - 1);
            } else {
                long long shallow_max = max(1LL, (long long)ceil((i-1) * strength));
                parent = rd_bool(strength) ? rd(1, shallow_max) : rd(1, i - 1);
            }
            break;
        }
        edges.emplace_back(parent, i);
    }
    return edges;
}

// Weighted tree — returns (u, v, w)
vector<tuple<long long,long long,long long>>
BuildWeightedTree(long long n, long long minW, long long maxW,
                  TreeShape shape = TreeShape::RANDOM,
                  double strength = 0.0)
{
    static const char* FN = "BuildWeightedTree";
    if (n < 1)  _ERR(FN, "n=" + to_string(n) + " must be >= 1");
    if (minW > maxW)
        _ERR(FN, "minW=" + to_string(minW) + " > maxW=" + to_string(maxW));

    auto base = BuildTree(n, shape, strength);
    vector<tuple<long long,long long,long long>> edges;
    edges.reserve(base.size());
    for (auto& [u, v] : base)
        edges.emplace_back(u, v, rd(minW, maxW));
    return edges;
}

// ---- 3.2  General Graph ----

struct GraphOptions {
    bool      directed  = false; // directed graph
    bool      simple    = true;  // no self-loops / multi-edges
    bool      connected = true;  // guarantee connectivity (undirected only)
    bool      weighted  = false;
    long long minW      = 1;
    long long maxW      = 100;
};

struct Edge { long long u, v, w; };

// Build a random graph with n nodes and m edges.
vector<Edge>
BuildGraph(long long n, long long m, GraphOptions opt = {})
{
    static const char* FN = "BuildGraph";
    if (n < 1) _ERR(FN, "n=" + to_string(n) + " must be >= 1");
    if (m < 0) _ERR(FN, "m=" + to_string(m) + " must be >= 0");
    if (opt.weighted && opt.minW > opt.maxW)
        _ERR(FN, "minW=" + to_string(opt.minW) + " > maxW=" + to_string(opt.maxW));

    long long maxEdges = opt.directed
        ? (opt.simple ? n * (n - 1)     : (long long)4e18)
        : (opt.simple ? n * (n - 1) / 2 : (long long)4e18);
    if (opt.simple && m > maxEdges) {
        _WARN(FN, "m=" + to_string(m) + " exceeds max=" + to_string(maxEdges)
                  + " for simple graph, clamped");
        m = maxEdges;
    }
    if (opt.connected && !opt.directed && opt.simple && m < n - 1)
        _ERR(FN, "connected graph needs m >= n-1, but m=" + to_string(m)
                 + " n=" + to_string(n));

    vector<Edge> edges;
    edges.reserve((size_t)m);
    set<pair<long long,long long>> edgeSet;

    auto addEdge = [&](long long u, long long v) {
        long long w = opt.weighted ? rd(opt.minW, opt.maxW) : 1;
        edges.push_back({u, v, w});
        if (opt.simple) {
            edgeSet.insert({u, v});
            if (!opt.directed) edgeSet.insert({v, u});
        }
    };

    // Guarantee connectivity: random Prüfer-style spanning tree
    if (opt.connected && !opt.directed && opt.simple) {
        vector<long long> perm((size_t)n);
        iota(perm.begin(), perm.end(), 1LL);
        shuffle_vec(perm);
        for (long long i = 1; i < n; ++i) {
            long long u = perm[rd(0, i - 1)], v = perm[i];
            if (u > v) swap(u, v);
            addEdge(u, v);
        }
    }

    long long attempts = 0;
    long long maxAttempts = max(m * 30LL, (long long)2e6);
    while ((long long)edges.size() < m && attempts < maxAttempts) {
        ++attempts;
        long long u = rd(1, n), v = rd(1, n);
        if (opt.simple) {
            if (u == v) continue;
            long long ku = opt.directed ? u : min(u, v);
            long long kv = opt.directed ? v : max(u, v);
            if (edgeSet.count({ku, kv})) continue;
        } else if (!opt.directed) {
            if (u > v) swap(u, v);
        }
        addEdge(u, v);
    }

    if ((long long)edges.size() < m)
        _WARN(FN, "only generated " + to_string(edges.size())
                  + "/" + to_string(m) + " edges (range too tight?)");

    return edges;
}

// DAG: directed, acyclic (guaranteed by enforcing u < v)
vector<Edge>
BuildDAG(long long n, long long m,
         bool weighted = false, long long minW = 1, long long maxW = 100)
{
    static const char* FN = "BuildDAG";
    if (n < 1) _ERR(FN, "n=" + to_string(n) + " must be >= 1");
    if (m < 0) _ERR(FN, "m=" + to_string(m) + " must be >= 0");
    long long maxEdges = n * (n - 1) / 2;
    if (m > maxEdges)
        _ERR(FN, "m=" + to_string(m) + " exceeds max DAG edges=" + to_string(maxEdges));
    if (weighted && minW > maxW)
        _ERR(FN, "minW=" + to_string(minW) + " > maxW=" + to_string(maxW));

    set<pair<long long,long long>> used;
    vector<Edge> edges;
    edges.reserve((size_t)m);
    long long attempts = 0, maxA = max(m * 30LL, (long long)2e6);
    while ((long long)edges.size() < m && attempts < maxA) {
        ++attempts;
        long long u = rd(1, n), v = rd(1, n);
        if (u >= v) continue;
        if (used.count({u, v})) continue;
        used.insert({u, v});
        long long w = weighted ? rd(minW, maxW) : 1;
        edges.push_back({u, v, w});
    }
    if ((long long)edges.size() < m)
        _WARN(FN, "only generated " + to_string(edges.size())
                  + "/" + to_string(m) + " edges");
    return edges;
}

// ============================================================
//  4. DSU
// ============================================================
struct DSU {
    vector<long long> parent, rank_;
    DSU(long long n) : parent((size_t)(n+1)), rank_((size_t)(n+1), 0) {
        if (n < 0) _ERR("DSU", "n=" + to_string(n) + " must be >= 0");
        iota(parent.begin(), parent.end(), 0LL);
    }
    long long find(long long x) {
        if (x < 0 || x >= (long long)parent.size())
            _ERR("DSU::find", "x=" + to_string(x) + " out of range");
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
//  5. Multi-dimensional vectors / sequences
// ============================================================

struct VecOptions {
    bool distinct = false; // all vectors must be distinct from each other
    bool sorted   = false; // each vector's elements sorted ascending
};

// Generate `count` D-dimensional integer vectors.
// lo[d], hi[d]: range for dimension d.
// Returns flat array: result[i*D + d] = element d of vector i.
// Fast enough for 1e6 vectors at range 1e9.
vector<long long>
GenVectors(long long count, long long D,
           const vector<long long>& lo,
           const vector<long long>& hi,
           VecOptions opt = {})
{
    static const char* FN = "GenVectors";
    if (count < 0) _ERR(FN, "count=" + to_string(count) + " must be >= 0");
    if (D <= 0)    _ERR(FN, "D="     + to_string(D)     + " must be >= 1");
    if ((long long)lo.size() != D || (long long)hi.size() != D)
        _ERR(FN, "lo/hi size must equal D=" + to_string(D));
    for (long long d = 0; d < D; ++d)
        if (lo[d] > hi[d])
            _ERR(FN, "lo[" + to_string(d) + "]=" + to_string(lo[d])
                     + " > hi[" + to_string(d) + "]=" + to_string(hi[d]));

    vector<uniform_int_distribution<long long>> dists;
    dists.reserve((size_t)D);
    for (long long d = 0; d < D; ++d)
        dists.emplace_back(lo[d], hi[d]);

    vector<long long> result;
    result.reserve((size_t)(count * D));

    if (!opt.distinct) {
        for (long long i = 0; i < count; ++i) {
            for (long long d = 0; d < D; ++d)
                result.push_back(dists[d](rng));
            if (opt.sorted)
                sort(result.end() - D, result.end());
        }
    } else {
        set<vector<long long>> seen;
        long long attempts = 0, maxA = count * 20 + 1000000LL;
        while ((long long)seen.size() < count && attempts < maxA) {
            ++attempts;
            vector<long long> v((size_t)D);
            for (long long d = 0; d < D; ++d) v[d] = dists[d](rng);
            if (opt.sorted) sort(v.begin(), v.end());
            if (seen.insert(v).second)
                for (auto x : v) result.push_back(x);
        }
        if ((long long)seen.size() < count)
            _WARN(FN, "distinct mode: only got " + to_string(seen.size())
                      + "/" + to_string(count) + " (range too small?)");
    }
    return result;
}

// 1-D convenience
inline vector<long long>
GenSeq(long long count, long long lo, long long hi,
       bool distinct = false, bool sorted_asc = false)
{
    static const char* FN = "GenSeq";
    if (count < 0) _ERR(FN, "count=" + to_string(count) + " must be >= 0");
    if (lo > hi)   _ERR(FN, "lo=" + to_string(lo) + " > hi=" + to_string(hi));
    VecOptions opt; opt.distinct = distinct; opt.sorted = sorted_asc;
    return GenVectors(count, 1, {lo}, {hi}, opt);
}

// Print flat result as `count` rows of D values
inline void PrintVectors(const vector<long long>& flat, long long D) {
    if (D <= 0) _ERR("PrintVectors", "D=" + to_string(D) + " must be >= 1");
    long long count = (long long)flat.size() / D;
    for (long long i = 0; i < count; ++i) {
        for (long long d = 0; d < D; ++d) {
            if (d) cout << ' ';
            cout << flat[i * D + d];
        }
        cout << '\n';
    }
}

// ============================================================
//  6. Function-expression sequences
// ============================================================

// double version: evaluates f(x) for x = x0, x0+dx, x0+2*dx, ...
template<typename F>
vector<double>
GenFuncSeq(double x0, long long count, double dx, F f)
{
    static const char* FN = "GenFuncSeq";
    if (count < 0) _ERR(FN, "count=" + to_string(count) + " must be >= 0");
    if (dx == 0.0) _WARN(FN, "dx=0, all x values will be identical");
    vector<double> result;
    result.reserve((size_t)count);
    double x = x0;
    for (long long i = 0; i < count; ++i, x += dx)
        result.push_back(f(x));
    return result;
}

// integer version: evaluates f(x) for x = x0, x0+dx, ..., casts to long long
template<typename F>
vector<long long>
GenFuncSeqInt(long long x0, long long count, long long dx, F f)
{
    static const char* FN = "GenFuncSeqInt";
    if (count < 0) _ERR(FN, "count=" + to_string(count) + " must be >= 0");
    if (dx == 0)   _WARN(FN, "dx=0, all x values will be identical");
    vector<long long> result;
    result.reserve((size_t)count);
    for (long long i = 0, x = x0; i < count; ++i, x += dx)
        result.push_back((long long)f(x));
    return result;
}

// ============================================================
//  7. Random polygons
// ============================================================

struct Point { double x, y; };

// Random convex polygon with n vertices (Valtr's algorithm, CCW order).
vector<Point>
GenConvexPolygon(long long n, double radius = 1000.0)
{
    static const char* FN = "GenConvexPolygon";
    if (n < 3)      _ERR(FN, "n=" + to_string(n) + " must be >= 3");
    if (radius <= 0) _ERR(FN, "radius=" + to_string(radius) + " must be > 0");

    vector<double> xs((size_t)n), ys((size_t)n);
    for (auto& v : xs) v = rd_double(0, radius);
    for (auto& v : ys) v = rd_double(0, radius);
    sort(xs.begin(), xs.end());
    sort(ys.begin(), ys.end());

    double xmin = xs.front(), xmax = xs.back();
    double ymin = ys.front(), ymax = ys.back();

    vector<double> dx, dy;
    double lastTop = xmin, lastBot = xmin;
    for (long long i = 1; i < n - 1; ++i) {
        if (rd_bool()) { dx.push_back(xs[i] - lastTop); lastTop = xs[i]; }
        else            { dx.push_back(lastBot - xs[i]); lastBot = xs[i]; }
    }
    dx.push_back(xmax - lastTop);
    dx.push_back(lastBot - xmax);

    double lastLeft = ymin, lastRight = ymin;
    for (long long i = 1; i < n - 1; ++i) {
        if (rd_bool()) { dy.push_back(ys[i] - lastLeft);  lastLeft  = ys[i]; }
        else            { dy.push_back(lastRight - ys[i]); lastRight = ys[i]; }
    }
    dy.push_back(ymax - lastLeft);
    dy.push_back(lastRight - ymax);

    shuffle_vec(dy);

    vector<pair<double,double>> vecs((size_t)n);
    for (long long i = 0; i < n; ++i) vecs[i] = {dx[i], dy[i]};
    sort(vecs.begin(), vecs.end(), [](auto& a, auto& b){
        return atan2(a.second, a.first) < atan2(b.second, b.first);
    });

    vector<Point> poly((size_t)n);
    double cx = 0, cy = 0;
    for (long long i = 0; i < n; ++i) {
        cx += vecs[i].first;
        cy += vecs[i].second;
        poly[i] = {cx, cy};
    }

    double minX = poly[0].x, minY = poly[0].y;
    for (auto& p : poly) { minX = min(minX, p.x); minY = min(minY, p.y); }
    for (auto& p : poly) { p.x -= minX; p.y -= minY; }

    return poly;
}

// Random simple (non-convex) polygon: points on a perturbed circle.
vector<Point>
GenSimplePolygon(long long n, double radius = 1000.0, double perturbation = 0.3)
{
    static const char* FN = "GenSimplePolygon";
    if (n < 3)           _ERR(FN, "n=" + to_string(n) + " must be >= 3");
    if (radius <= 0)     _ERR(FN, "radius=" + to_string(radius) + " must be > 0");
    if (perturbation < 0 || perturbation >= 1.0)
        _ERR(FN, "perturbation=" + to_string(perturbation) + " must be in [0, 1)");

    vector<Point> pts((size_t)n);
    for (long long i = 0; i < n; ++i) {
        double angle = 2.0 * M_PI * i / n;
        double r = radius * (1.0 + rd_double(-perturbation, perturbation));
        pts[i] = {r * cos(angle), r * sin(angle)};
    }
    return pts;
}

// Shoelace signed area (positive = CCW)
inline double PolySignedArea(const vector<Point>& poly) {
    if (poly.size() < 3)
        _ERR("PolySignedArea", "polygon must have >= 3 vertices");
    double area = 0;
    long long n = (long long)poly.size();
    for (long long i = 0; i < n; ++i) {
        long long j = (i + 1) % n;
        area += poly[i].x * poly[j].y - poly[j].x * poly[i].y;
    }
    return area / 2.0;
}

inline double PolyArea(const vector<Point>& poly) {
    return fabs(PolySignedArea(poly));
}

inline double PolyPerimeter(const vector<Point>& poly) {
    if (poly.size() < 2)
        _ERR("PolyPerimeter", "polygon must have >= 2 vertices");
    double peri = 0;
    long long n = (long long)poly.size();
    for (long long i = 0; i < n; ++i) {
        long long j = (i + 1) % n;
        double dx = poly[j].x - poly[i].x, dy = poly[j].y - poly[i].y;
        peri += sqrt(dx*dx + dy*dy);
    }
    return peri;
}

// Point-in-polygon test (ray casting)
inline bool PolyContains(const vector<Point>& poly, double px, double py) {
    long long n = (long long)poly.size();
    bool inside = false;
    for (long long i = 0, j = n-1; i < n; j = i++) {
        double xi = poly[i].x, yi = poly[i].y;
        double xj = poly[j].x, yj = poly[j].y;
        if (((yi > py) != (yj > py)) &&
            (px < (xj-xi)*(py-yi)/(yj-yi)+xi))
            inside = !inside;
    }
    return inside;
}

inline void PrintPoly(const vector<Point>& poly, int prec = 6) {
    cout << fixed << setprecision(prec);
    for (auto& p : poly) cout << p.x << ' ' << p.y << '\n';
}

// ============================================================
//  8. String / word / sentence / paragraph generation
// ============================================================

static const vector<string> _DICT = {
    "the","be","to","of","and","a","in","that","have","it",
    "for","not","on","with","he","as","you","do","at","this",
    "but","his","by","from","they","we","say","her","she","or",
    "an","will","my","one","all","would","there","their","what",
    "so","up","out","if","about","who","get","which","go","me",
    "when","make","can","like","time","no","just","him","know",
    "take","people","into","year","your","good","some","could",
    "them","see","other","than","then","now","look","only","come",
    "its","over","think","also","back","after","use","two","how",
    "our","work","first","well","way","even","new","want","because",
    "any","these","give","day","most","us","great","between","need",
    "large","often","hand","high","place","hold","turn","open","seem",
    "together","next","white","children","begin","got","walk","example",
    "ease","paper","group","always","music","those","both","mark","book",
    "letter","until","mile","river","car","feet","care","second","enough",
    "plain","girl","usual","young","ready","above","ever","red","list",
    "though","feel","talk","bird","soon","body","dog","family","direct",
    "pose","leave","song","measure","door","product","black","short","numeral",
    "class","wind","question","happen","complete","ship","area","half","rock",
    "fire","south","problem","piece","told","knew","pass","since","top",
    "whole","king","space","heard","best","hour","better","true","during",
    "hundred","five","remember","step","early","west","ground","interest",
    "reach","fast","verb","sing","listen","six","table","travel","less","morning",
    "ten","simple","several","vowel","toward","war","lay","against","pattern",
    "slow","center","love","person","money","serve","appear","road","map","rain",
    "rule","govern","pull","cold","notice","voice","unit","power","town","fine",
    "drive","fought","fall","lead","cry","dark","machine","note","wait","plan",
    "figure","star","box","noun","field","rest","able","pound","done","beauty",
    "stood","contain","front","teach","week","final","gave","green","quick",
    "develop","ocean","warm","free","minute","strong","special","mind","behind",
    "clear","tail","produce","fact","street","inch","multiply","nothing","course",
    "stay","wheel","full","force","blue","object","decide","surface","deep",
    "moon","island","foot","system","busy","test","record","boat","common",
    "gold","possible","plane","age","dry","wonder","laugh","thousands","ago",
    "ran","check","game","shape","equate","hot","miss","brought","heat","snow",
    "tire","bring","yes","distant","fill","east","paint","language","among",
    "ball","wave","drop","heart","present","heavy","dance","engine","position",
    "arm","wide","sail","material","size","vary","settle","speak","weight",
    "ice","matter","circle","pair","include","divide","syllable","felt","perhaps",
    "pick","sudden","square","reason","length","represent","art","subject",
    "region","energy","hunt","probable","bed","brother","egg","ride","cell",
    "fraction","forest","sit","race","window","store","summer","train","sleep",
    "lone","leg","exercise","wall","catch","mount","wish","sky","board","joy",
    "winter","sat","written","wild","instrument","kept","glass","grass","cow",
    "sign","visit","past","soft","fun","bright","gas","weather","month","million"
};

// Random word from built-in dictionary
inline string rd_word() {
    return _DICT[(size_t)rd(0, (long long)_DICT.size() - 1)];
}

// Random string from character range [lo, hi]
inline string rd_string(long long len, char lo = 'a', char hi = 'z') {
    static const char* FN = "rd_string";
    if (len < 0) _ERR(FN, "len=" + to_string(len) + " must be >= 0");
    if (lo > hi) _ERR(FN, "lo > hi");
    string s((size_t)len, ' ');
    for (auto& c : s) c = (char)rd((long long)lo, (long long)hi);
    return s;
}

// Random string from a custom charset string
inline string rd_string_from(long long len, const string& charset) {
    static const char* FN = "rd_string_from";
    if (len < 0)       _ERR(FN, "len=" + to_string(len) + " must be >= 0");
    if (charset.empty()) _ERR(FN, "charset must not be empty");
    string s((size_t)len, ' ');
    for (auto& c : s) c = charset[(size_t)rd(0, (long long)charset.size()-1)];
    return s;
}

// Random sentence of `words` words
string rd_sentence(long long words = 8) {
    if (words < 1) _ERR("rd_sentence", "words=" + to_string(words) + " must be >= 1");
    string s;
    for (long long i = 0; i < words; ++i) {
        if (i) s += ' ';
        string w = rd_word();
        if (i == 0) w[0] = (char)toupper((unsigned char)w[0]);
        s += w;
    }
    return s + '.';
}

// Random paragraph of `sentences` sentences
string rd_paragraph(long long sentences = 5, long long wordsPerSentence = 8) {
    static const char* FN = "rd_paragraph";
    if (sentences < 1)      _ERR(FN, "sentences must be >= 1");
    if (wordsPerSentence < 1) _ERR(FN, "wordsPerSentence must be >= 1");
    string p;
    for (long long i = 0; i < sentences; ++i) {
        if (i) p += ' ';
        p += rd_sentence(rd(wordsPerSentence / 2 + 1, wordsPerSentence * 2));
    }
    return p;
}

// Random permutation [1..n]
vector<long long> rd_permutation(long long n) {
    if (n < 0) _ERR("rd_permutation", "n=" + to_string(n) + " must be >= 0");
    vector<long long> p((size_t)n);
    iota(p.begin(), p.end(), 1LL);
    shuffle_vec(p);
    return p;
}

// ============================================================
//  9. File / directory utilities
// ============================================================

inline bool fileExists(const string& path) {
    ifstream f(path); return f.good();
}

inline void createDirectory(const string& path) {
    if (MKDIR(path.c_str()) == -1 && errno != EEXIST)
        _WARN("createDirectory", "failed to create: " + path);
}

// "data/name/nameN"
inline string st(const string& name, long long n) {
    if (name.empty()) _ERR("st", "name must not be empty");
    return "data/" + name + "/" + name + to_string(n);
}

// ============================================================
//  10. Output helpers
// ============================================================

template<typename T>
void print_vec(const vector<T>& v, const string& sep = " ") {
    for (size_t i = 0; i < v.size(); ++i) {
        if (i) cout << sep;
        cout << v[i];
    }
    cout << '\n';
}

inline void print_edges(const vector<pair<long long,long long>>& edges) {
    for (auto& [u, v] : edges) cout << u << ' ' << v << '\n';
}
inline void print_edges(const vector<tuple<long long,long long,long long>>& edges) {
    for (auto& [u, v, w] : edges) cout << u << ' ' << v << ' ' << w << '\n';
}
inline void print_edges(const vector<Edge>& edges, bool printWeight = false) {
    for (auto& e : edges) {
        cout << e.u << ' ' << e.v;
        if (printWeight) cout << ' ' << e.w;
        cout << '\n';
    }
}

#endif // FUNCTION_HPP
