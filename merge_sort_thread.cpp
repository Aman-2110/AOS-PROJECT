#include <iostream>
#include <bits/stdc++.h>
#include <thread>
#include <fstream>
#include<chrono>
#include<iomanip>
using namespace std::chrono;
using namespace std;
void sel_sort(vector<int>& vec,int s,int e)
{
    for(int i=s;i<e;i++)
    {
        int min_in=i;
        for(int j=i+1;j<=e;j++)
        {
            if(vec[j]<vec[min_in])
            {
                min_in=j;
            }
        }
        int temp=vec[min_in];
        vec[min_in]=vec[i];
        vec[i]=temp;
    }
}
void merge(vector<int>& vec,int s,int m,int e)
{
    vector<int> temp;
    int i=s,j=m+1;
    while(i<=m && j<=e)
    {
        if(vec[i]<vec[j])
        {
            temp.push_back(vec[i++]);
        }
        else
        {
            temp.push_back(vec[j++]);
        }
    }
    while(i<=m)
    {
        temp.push_back(vec[i++]);
    }
    while(j<=e)
    {
        temp.push_back(vec[j++]);
    }
    for(int i=s;i<=e;i++)
    {
        vec[i]=temp[i-s];
    }
}
void merge_sort_thread(vector<int>& vec,int s,int e)
{
    int size=e-s+1;
    if(size<=5)
    {
        sel_sort(vec,s,e);
        return;
    }
    if(s<e)
    {
        int m=s+(e-s)/2;
        thread t1(merge_sort_thread,std::ref(vec),s,m);
        thread t2(merge_sort_thread,std::ref(vec),m+1,e);
        t1.join();
        t2.join();
        merge(vec,s,m,e);
    }
}
int main()
{
    int n;
    auto start=high_resolution_clock::now();
    ifstream in("input.txt");
    ofstream out("output.txt");
    in >> n;
    vector<int> vec(n);
    for(int i=0;i<n;i++)
    {
        in >> vec[i];
    }
    in.close();
    thread t(merge_sort_thread,std::ref(vec),0,n-1);
    t.join();
    for(int x:vec)
    {
        out << x <<" ";
    }
    out <<"\n";
    out.close();
    auto stop=high_resolution_clock::now();
    auto diff=stop-start;
    cout << fixed << setprecision(6) << chrono::duration <double> (diff).count() << " s" << "\n";
}