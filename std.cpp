#include<bits/stdc++.h>
using namespace std;
typedef long long ll;
const int N=1e5+5;
ll n;
ll f[N],g[N],fg[N];
vector<ll> w[N];
void dfs(ll id,ll fa){
	f[id]=1;
	ll ag=0;
	for(ll i=0;i<w[id].size();i++){
		ll son=w[id][i];
		if(son==fa) continue;
		dfs(son,id);
		ag=max(ag,min(f[son],g[son])-fg[son]);
		f[id]+=min(f[son],g[son]);
		fg[id]+=min(f[son],g[son]);
		g[id]+=f[son];
	}
	f[id]-=ag;
	return;
}
int main(){
	cin>>n;
	for(ll i=1;i<n;i++){
		ll u,v;
		cin>>u>>v;
		w[u].push_back(v);
		w[v].push_back(u);
	}
	dfs(1,-1);
	cout<<min(f[1],g[1]);
	return 0;
}
