#include <iostream>
#include <thread>
#include <unistd.h>
#include <vector>
#include <algorithm>
#include <semaphore.h>
using namespace std;

// n -> number of editors  || m -> number of articles
int n, m, counter, nCount, rej=0;
sem_t mtx;
vector<vector<int>> accepted;

void read_n_decide(vector<pair<short,vector<int>>>& articles, int e_num){
    int index, start_point;
    bool wait;
    while (nCount>0){
        sem_wait(&mtx); //at a time one editor will choose
        if (counter<=0){
            // cout<<"No more articles in queue"<<endl;
            sem_post(&mtx);
            continue;
        }
        index = rand()%m;
        start_point = index;
        wait = false;
        while (articles[index].first != 0 || find(articles[index].second.begin(), articles[index].second.end(), e_num) != articles[index].second.end()){
                index = ( index + 1 )%m; // next article
                if (index == start_point){
                    // cout<<e_num<<" can't choose articles now"<<endl;
                    sleep(1);
                    sem_post(&mtx);
                    wait = true;
                    break;
                }
            }
        
        if (wait)
            continue;

        articles[index].first = 1; //reading start
        
        counter--;
        cout<<"Editor - " << e_num<<" reading article no. "<<index<<endl;
        sem_post(&mtx);
        sleep(1);
        srand(time(0));
        int accept = rand() % 4;
        if (accept < 1){
            articles[index].first = 2;
            accepted[e_num].push_back(index);
            sem_wait(&mtx);
            cout << "Editor - " << e_num << " Accepted article no. " << index << endl; 
            nCount--;
            sem_post(&mtx);
        }
        else {
            articles[index].second.push_back(e_num);
            if (articles[index].second.size()==n){
                articles[index].first = 3;
                rej++;
                sem_wait(&mtx);
                cout << "Rejected article no. " << index << endl; 
                nCount--;
                sem_post(&mtx);
            }
            else{
                articles[index].first = 0;
                sem_wait(&mtx);
                counter++;
                sem_post(&mtx);
            }

            // sem_wait(&mtx);
            // cout << "Editor - " << e_num << " rejected article no. " << index << endl; 
            // sem_post(&mtx);
        }
    }
    // sem_wait(&mtx);
    // cout<<"Editor "<<e_num<<" finished working"<<endl;
    // sem_post(&mtx);
}

int main(){

    // m -> articles, n -> editors
    cout << "Enter the number of editors : \n"; 
    cin>>n;
    cout << "Enter the number of reporters : \n"; 
    cin>>m;
    
    counter = m;
    nCount = m;
    thread editors[n];      // separate thread for each editor
    accepted.resize(n);

    sem_init(&mtx, 0, 1);
    
    // for each article pair of <status, list of editors who had rejected it> is maintained
    vector<pair<short,vector<int>>> articles(m,{0, {}});  // status -> 0 unprocessed, 1 processing, 2 accepted, 3 rejected overall
    
    for (int i = 0 ; i < n ; i++)
        editors[i] = thread(&read_n_decide, ref(articles), i); //all editors active

    for (int i = 0 ; i < n ; i++)
        editors[i].join(); // joining all thread to main thread.
    
    cout<<"Done!"<<endl;

    int tot = 0;
    int ii = 0;

    for (auto it:accepted){
        cout<<"Editor - " << ii << " has accepted - " << it.size() <<" articles (";
        int itn = it.size();
        tot += itn;
        for (int i = 0 ; i < itn ; i++)
            if (i == itn - 1)
                cout<<it[i];
            else
                cout<<it[i]<<", ";
        cout<<")" << endl;
        ii++;
    }
    
    // cout<<"Accepted "<<tot<<endl;
    // cout<<"Rejected "<<rej<<endl;
    return 0;
}