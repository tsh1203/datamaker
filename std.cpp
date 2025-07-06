#include<bits/stdc++.h>
#pragma GCC optimize("3","Ofast","inline")
#define int long long
#define endl "\n"
using namespace std;
int l,r,t;
int ma,ans;
signed main()
{
	ios::sync_with_stdio(0);
	cin.tie(0);
	cout.tie(0);
	cin>>t;
	while(t--){
		ma=INT_MAX;
		cin>>l>>r;
		while(l<=r){
			int x=l,cnt=1;
			while(x%10==0)
				x/=10,cnt*=10;
			
			int y=x,len=0;
			while(y)
				y/=10,++len;
			int an=2*len;
			if(x%10==5)an--;
			if(ma>an)ma=an,ans=l;
			l+=cnt;
		}
		cout<<ans<<endl;
	}
	return 0;
}
