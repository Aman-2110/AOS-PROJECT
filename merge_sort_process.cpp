#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <iostream>
#include <fstream>
#include <chrono>
#include <vector>
#include <iomanip>
using namespace std::chrono;
using namespace std;
void sel_sort(int vec[],int s,int e)
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
void merge(int vec[],int s,int m,int e)
{
    int size=e-s+1;
    if(size<=5)
    {
        sel_sort(vec,s,e);
        return;
    }
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
void merge_sort_process(int vec[],int s,int e)
{
    if(s<e)
    {
        int m=s+(e-s)/2;
        int status;
        pid_t left,right;
        left=fork();
        if(left==0)
        {
            merge_sort_process(vec,s,m);
            _exit(0);
        }
        else
        {
            right=fork();
            if(right==0)
            {
                merge_sort_process(vec,m+1,e);
                _exit(0);
            }
        }
        waitpid(left,&status,0);
        waitpid(right,&status,0);
        merge(vec,s,m,e);
    }
}
int main()
{
    auto start=high_resolution_clock::now();
    int n,shmid;
    key_t key=IPC_PRIVATE;
    int *shm_arr;
    ifstream in("input.txt");
    ofstream out("output.txt");
    in >> n;
    size_t SHM_SIZE = sizeof(int)*n;
    shmid=shmget(key,SHM_SIZE,IPC_CREAT | 0666);
    shm_arr=(int *)shmat(shmid,NULL,0);
    for(int i=0;i<n;i++)
    {
        in >> shm_arr[i];
    }
    in.close();
    merge_sort_process(shm_arr,0,n-1);
    for(int i=0;i<n;i++)
    {
        out << shm_arr[i] <<" ";
    }
    shmdt(shm_arr);
    shmctl(shmid,IPC_RMID,NULL);
    out <<"\n";
    out.close();
    auto stop=high_resolution_clock::now();
    auto diff=stop-start;
    cout << fixed << setprecision(6) << chrono::duration <double> (diff).count() << " s" << "\n";
}