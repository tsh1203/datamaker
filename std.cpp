#include<iostream>
#include<vector>
#include<cstring>
using namespace std;
vector<int> a[200001],b[200001];
int last[200001],u,v,nxt[200001];
long long dis[200001],mmm[200001],op;
bool vv[200001];
void dfs1(int o,long long p,int q)
{
	if(p>op){op=p;u=o;}
	for(int i=0;i<a[o].size();i++)
		if((!vv[a[o][i]])&&(a[o][i]!=q))
		{
			vv[a[o][i]]=true;
			dfs1(a[o][i],p+b[o][i],o);
		}
}
void dfs2(int o,long long p,int q)
{
	last[o]=q;
	dis[o]=p;
	if(p>op){op=p;v=o;}
	for(int i=0;i<a[o].size();i++)
		if((!vv[a[o][i]])&&(a[o][i]!=q))
		{
			vv[a[o][i]]=true;
			dfs2(a[o][i],p+b[o][i],o);
		}
}
int main()
{
	int n;
	cin>>n;
	for(int i=1;i<n;i++)
	{
		int x,y,z;
		cin>>x>>y>>z;
		a[x].push_back(y);
		b[x].push_back(z);
		a[y].push_back(x);
		b[y].push_back(z);
	}
	memset(vv,0,sizeof(vv));op=0;
	dfs1(1,0,0);
	memset(vv,0,sizeof(vv));op=0;
	dfs2(u,0,0);
	int distance=dis[v];
	cout<<dis[v]<<endl;
	memset(vv,0,sizeof(vv));
	for(int i=v;i!=0;i=last[i]) vv[i]=true;
	for(int i=v;i!=0;i=last[i])
	{
		op=0;
		dfs1(i,0,0);
		mmm[i]=op;
	}
	int j=v;
	for(int i=last[v];i!=0;i=last[i]) nxt[i]=j,j=i;
	int ans=0;
	int i;
	for(i=j;i!=0;i=nxt[i])
		if(dis[v]-dis[i]==mmm[i]) break;
	for(;i!=0;i=last[i])
	{
		if(dis[i]==mmm[i]) break;
		ans++;
	}
	cout<<ans<<endl;
	return 0;
}
