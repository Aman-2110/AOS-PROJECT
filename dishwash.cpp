#include <iostream>
#include <thread>
#include <unistd.h>
#include <vector>
#include <fstream>
#include <algorithm>
#include <semaphore.h>
#include <memory>
#include <chrono>
using namespace std;

int n,m;
sem_t mtx, mty;
sem_t choosing;
sem_t log_m;
vector<int>scotch_brite;
std::chrono::time_point<std::chrono::high_resolution_clock> t_start;

float wash_time[] = {4, 3, 2};  //0th is plate time, 1st is glass time, 2nd index is spoon time
float sb_time[] = {5, 3, 1};

string utensil[] = {"plate", "glass", "spoon"};

class Logger{
    public:
        Logger(const string &path) : filePath(path){
            this->logFile.open(this->filePath);
        }

        void write(const string &msg){
            this->logFile << msg;
        }

    private:
        string filePath;
        ofstream logFile;
};

void remove_at(vector<int>& v, int n){
    swap(v[n], v.back());
    v.pop_back();
}

void disp(vector<int>& taps){
    for (auto i:taps)
        cout<<" "<<i;
    cout<<endl;
}

void wash_utensils(vector<int>& scotch_brite, vector<int>& taps, int e_num, shared_ptr<Logger> &logger){
    int sb;
    
    sem_wait(&choosing);
    sem_wait(&mtx); //give taps until empty
    int value; 
    sem_getvalue(&mtx, &value); 
    int index = rand()%(value+1);
    int tapnum = taps[index]; //choose tap
    

    // 0 1 3 4

    remove_at(taps, index);
    // disp(taps);
    sem_post(&choosing);

    sem_wait(&log_m);
    auto t_mid = std::chrono::high_resolution_clock::now();
    int elapsed = std::chrono::duration_cast<std::chrono::seconds>(t_mid-t_start).count();
    logger->write("At time "+to_string(elapsed)+", User "+to_string(e_num)+" has taken tap "+to_string(tapnum+1)+"\n");
    cout<<"At t = "<<elapsed<<"s, user "<<e_num<<" has taken tap "<<tapnum+1<<endl;
    sem_post(&log_m);
    int counter = 3;
    while (counter>0){
        sem_wait(&mty);
        if (tapnum==0){
            while (scotch_brite[0]!=-1) ;
            scotch_brite[0] = 0;
            sb = 0;
        }
        else if (tapnum==n-1){
            while (scotch_brite[n-2]!=-1) ;
            scotch_brite[n-2] = n-1;
            sb = n-2;
        }
        else{
            while (scotch_brite[tapnum-1]!=-1 && scotch_brite[tapnum]!=-1) ;
            if (scotch_brite[tapnum-1] == -1){
                scotch_brite[tapnum-1] = tapnum;
                sb = tapnum-1;
            }
            else{
                scotch_brite[tapnum] = tapnum;
                sb = tapnum;
            }
        }        
        sem_post(&mty);
        sleep(sb_time[3-counter]);
        scotch_brite[sb] = -1; //release scotch brite
        // Washed 1 utensil ...
        sleep(wash_time[3-counter]);
        sem_wait(&log_m);
        auto t_utensil = std::chrono::high_resolution_clock::now();
        elapsed = std::chrono::duration_cast<std::chrono::seconds>(t_utensil-t_start).count();
        logger->write("User "+to_string(e_num)+" finished washing "+utensil[3-counter]+" at tap "+to_string(tapnum+1)+" at t = "+to_string(elapsed)+"\n");
        cout<<"User "<<e_num<<" finished washing "<<utensil[3-counter]<<" at tap "<<tapnum+1<<endl;
        sem_post(&log_m);
        counter--;
    }
    taps.push_back(tapnum);
    sem_post(&mtx);
     //release tap

    auto t_end = std::chrono::high_resolution_clock::now();
    elapsed = std::chrono::duration_cast<std::chrono::seconds>(t_end-t_start).count();
    cout<<"User "<<e_num<<" has finished washing at T="<<elapsed<<endl;
}

int main(){
    srand(time(0));
    
    cout << "Enter the number of taps : \n"; 
    cin>>n;
    cout << "Enter the number of students : \n"; 
    cin>>m;
    
    //static std::chrono::time_point
    thread users[m];
    auto logger = make_shared<Logger>("dishwash.log");

    // accepted.resize(n);

    vector<int> vec;

    t_start = std::chrono::high_resolution_clock::now();
    sem_init(&mtx, 0, n); //taps-- semaphore until taps are available
    sem_init(&mty, 0, 1);
    sem_init(&choosing, 0, 1);
    sem_init(&log_m, 0, 1);
    vector<short> dish(m,0);  //0 unwashed, 1-3 dishes
    
    for (int i=0;i<n-1;i++)
        scotch_brite.push_back(-1);
    vector<int> taps;
    for (int i=0;i<n;i++)
        taps.push_back(i);
    // cout<<"init ";
    // disp(taps);
    
    for (int i=0;i<m;i++)
        users[i] = thread(&wash_utensils, ref(scotch_brite), ref(taps), i, ref(logger));

    for (int i=0;i<m;i++){
        // cout<<"joining "<<i<<endl;
        users[i].join();
    }
    cout<<"Done!"<<endl;
    //OUTPUT
    return 0;
}