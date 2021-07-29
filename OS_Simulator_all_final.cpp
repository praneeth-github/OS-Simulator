#include <iostream>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include <bits/stdc++.h>
#include <iomanip>
using namespace std;

//Structure defining a producer
struct Producer{
    int id; //producer number
    float AT; // Arrival Time
    float BT; // Burst Time
    int flag=0; //flag to check whether a producer is waiting , producing or produced.0=Waiting,1=Producing,2=Produced
}*p;

//Structure define a consumer
struct Consumer{
    int id; //consumer number
    float AT; // Arrival Time
    float BT; // Burst Time
    int flag=0; //flag to check whether a consumer is waiting , consuming or consumed.0=Waiting,1=consuming,2=consumed
}*c;


vector<int> mvt_process;
int tot_size;
int remaining;
vector<int> part; //partition
vector<int> stat; //status of partition


//Global variables declaration
float tot_time=0;
float buffer_size;
float buffer_check=0;
int n;
int m;
int i=0;
int j=0;
int wait_chairs;
int reader_reading=0;
int flag_pc=0;
int flag_rw=0;

sem_t mutex1;

void reset()
{
    tot_time=0;
    buffer_size;
    buffer_check=0;
    i=0;
    j=0;
    wait_chairs;
    reader_reading=0;
    flag_pc=0;
    flag_rw=0;
}

//Flag to check whether the current process is producer or consumer.0=Producer 1=Consumer

void swap(int* a, int* b) 
{ 
    int t = *a; 
    *a = *b; 
    *b = t; 
} 
  
/* This function takes last element as pivot, places 
   the pivot element at its correct position in sorted 
    array, and places all smaller (smaller than pivot) 
   to left of pivot and all greater elements to right 
   of pivot */
int partition (int pid[], int at[], int bt[], int low, int high) 
{ 
    int pivot = at[high];    // pivot 
    int i = (low - 1);  // Index of smaller element 
    int j;
  
    for ( j = low; j <= high- 1; j++) 
    { 
        // If current element is smaller than the pivot 
        if (at[j] < pivot) 
        { 
            i++;    // increment index of smaller element
			swap(&pid[i],&pid[j]); 
            swap(&at[i], &at[j]); 
            swap(&bt[i], &bt[j]); 
        } 
    } 
    swap(&at[i + 1], &at[high]); 
    swap(&bt[i + 1], &bt[high]); 
    swap(&pid[i + 1], &pid[high]);
    return (i + 1); 
} 
  
/* The main function that implements QuickSort 
 arr[] --> Array to be sorted, 
  low  --> Starting index, 
  high  --> Ending index */
void quickSort(int pid[], int at[], int bt[], int low, int high) 
{ 
    if (low < high) 
    { 
        /* pi is partitioning index, arr[p] is now 
           at right place */
        int pi = partition(pid, at, bt, low, high); 
  
        // Separately sort elements before 
        // partition and after partition 
        quickSort(pid, at, bt, low, pi - 1); 
        quickSort(pid, at, bt, pi + 1, high); 
    } 
}

//Main logic function using semaphore
void* thread_pc(void* arg) 
{ 
    //wait 
    sem_wait(&mutex1); 
    //Entering Critical area
    cout<<endl;
    //Checking whether producer or consumer is curently accessing critical area
    if(flag_pc==0)
    {
        p[i].flag=1;
        buffer_check++;
        for(int a=0;a<p[i].BT;a++)
        {    
            cout<<"At time = "<<tot_time<<" seconds"<<endl;
            for(int z=0;z<n;z++)
            {
                if(p[z].flag==0)
                {
                    if(p[z].AT<=tot_time)
                    {
                        cout<<"Producer "<<p[z].id<<" is waiting"<<endl;
                    }
                }
                else if(p[z].flag==1)
                {
                    cout<<"Producer "<<p[z].id<<" is being processed"<<endl;
                }
            }         
            for(int z=0;z<m;z++)
            {
                if(c[z].flag==0)
                {
                    if(c[z].AT<=tot_time)
                    {
                        cout<<"Consumer "<<c[z].id<<" is waiting"<<endl;
                    }
                }
                else if(c[z].flag==1)
                {
                    cout<<"Consumer "<<c[z].id<<" is being processed"<<endl;   
                }
            }
            tot_time++;
            cout<<endl;
        
        }
        cout<<"Producer "<<p[i].id<<" has finished producing at time = "<<tot_time<<endl;
        p[i].flag=2;
    }
    else 
    {
        c[j].flag=1;
        buffer_check--;
        for(int a=0;a<c[j].BT;a++)
        {    
            cout<<"At time = "<<tot_time<<" seconds"<<endl;
            for(int z=0;z<n;z++)
            {
                if(p[z].flag==0)
                {
                    if(p[z].AT<=tot_time)
                    {
                        cout<<"Producer "<<p[z].id<<" is waiting"<<endl;
                    }
                }
                else if(p[z].flag==1)
                {
                    cout<<"Producer "<<p[z].id<<" is being processed"<<endl;
                }
            }         
            for(int z=0;z<m;z++)
            {
                if(c[z].flag==0)
                {
                    if(c[z].AT<=tot_time)
                    {
                        cout<<"Consumer "<<c[z].id<<" is waiting"<<endl;
                    }
                }
                else if(c[z].flag==1)
                {
                    cout<<"Consumer "<<c[z].id<<" is being processed"<<endl;   
                }
                
            }
            tot_time++;
            cout<<endl;
        }
        cout<<"Consumer "<<c[j].id<<" has finished consuming at time = "<<tot_time<<endl;
        c[j].flag=2;
    }
    
    cout<<endl;
    //Exiting Critical area
    sem_post(&mutex1); 
    //Semaphore free for next iteration
    return 0;
}

void* thread_sleeping_barber(void* arg) 
{ 
    //wait
    i++; 
    sem_wait(&mutex1); 
    //Entering Critical area
    cout<<"At time t = "<<tot_time<<" .Barber has started serving customer "<<i<<endl;
    while(p[i].BT!=0)
    {
        while(p[i].flag==3)
        i++;
        p[i].flag=2;
        tot_time++;
        p[i].BT--;
        for(int j=0;j<n;j++)
        {
            if(p[j].flag==0)
            {
                if(tot_time==p[j].AT)
                {
                    if(wait_chairs>0)
                    {
                        wait_chairs--;
                        p[j].flag=1;
                        cout<<"At time t = "<<tot_time<<" ,customer "<<j<<" occupied a waiting seat"<<endl;
                    }
                    else
                    {
                        p[j].flag=3;
                        cout<<"At time t = "<<tot_time<<" ,customer "<<j<<" left shaking his head"<<endl;
                    }
                }
            }
        }

    }
    cout<<"Barber finished serving customer "<<i<<" at t = "<<tot_time<<endl;
    for(int a=0;a<n;a++)
    {
        if(p[a].flag==1)
        {
            wait_chairs++;
            break;
        }
    }
    //Exiting Critical area
    sem_post(&mutex1); 
    //Semaphore free for next iteration
    return 0;
}

void producer_consumer(){
    sem_init(&mutex1, 0, 1);
    cout<<"Enter buffer size"<<endl;
    cin>>buffer_size;
    cout<<"Enter the number of producers"<<endl;
    cin>>n;
    cout<<"Enter number of consumers"<<endl;
    cin>>m;
    p = new struct Producer[n+1];
    c = new struct Consumer[m+1];
    //An extra producer and consumer just for protection.Used in some conditions later
    p[n].AT=100000;
    c[m].AT=100000;
    cout<<"Enter producer number, Arrival time and Burst time of each process"<<endl;
    for(int i=0;i<n;i++)
    {
        p[i].id=i+1;
        cout<<"Enter details of producer "<<p[i].id<<endl;
        cout<<"Enter its arrival time"<<endl;
        cin>>p[i].AT;
        cout<<"Enter its burst time"<<endl;
        cin>>p[i].BT;
    }
    cout<<"Enter consumer number, Arrival time and Burst time of each process"<<endl;
    for(int i=0;i<m;i++)
    {
        c[i].id=i+1;
        cout<<"Enter details of consumer "<<c[i].id<<endl;
        cout<<"Enter its arrival time"<<endl;
        cin>>c[i].AT;
        cout<<"Enter its burst time"<<endl;
        cin>>c[i].BT;
    }
    //Sorting producers
    //Quicksort_producer(0,n-1);
    for(int a=0;a<n;a++)
    {
        for(int b=a+1;b<n-a-1;b++)
        {
            if(p[b].AT>p[b+1].AT)
            {
                struct Producer temp=p[b];
                p[b]=p[b+1];
                p[b+1]=temp;
            }
        }
    }
    //Sorting consumers
    for(int a=0;a<m;a++)
    {
        for(int b=a+1;b<m-a-1;b++)
        {
            if(p[b].AT>p[b+1].AT)
            {
                struct Consumer temp=c[b];
                c[b]=c[b+1];
                c[b+1]=temp;
            }
        }
    }
    for(int a=0;a<m;a++)
    cout<<c[a].id<<endl;
    //Quicksort_c(0,m-1);
    // for(int i=0;i<n;i++)
    // cout<<p[i].id<<" "<<p[i].AT<<" "<<p[i].BT<<endl;
    // for(int i=0;i<m;i++)
    // cout<<c[i].id<<" "<<c[i].AT<<" "<<c[i].BT<<endl;
    i=0;
    j=0;
    //Declaring two arrays of threads.One for each process
    pthread_t t_p[n];
    pthread_t t_c[m];
    //Flag to check overflow and underflow conditions
    int flag_pc_overflow=0;
    //1=Waiting for producer and 2=Waiting for consumer 0=Not waiting

    //Main logic Code
    if(p[i].AT!=0&&c[j].AT!=0)
    {
        cout<<"CPU idle for some time"<<endl;
        if(p[i].AT<=c[j].AT)
        tot_time=p[i].AT;
        else tot_time=c[j].AT;
    }
    while(1)
    {
        if(i<n)
        {   
            if(p[i].AT<=tot_time&&(c[j].AT>=p[i].AT||flag_pc_overflow==1))
            {
                flag_pc_overflow=0;
                //Checking overflow
                if(buffer_size==buffer_check)
                {
                    cout<<"Overflow . Can't handle process right now. Waiting for a consumer"<<endl;
                    flag_pc_overflow=2;
                    if(c[j].AT>tot_time)
                    {
                        cout<<"CPU idle for some time"<<endl;
                        tot_time=c[j].AT;
                    }
                }
                else
                {
                    flag_pc=0;
                    pthread_create(&t_p[i],NULL,thread_pc,NULL);
                    pthread_join(t_p[i],NULL);
                    i++;
                } 
            }
        }
        if(j<m)
        {
            if(c[j].AT<=tot_time&&(p[i].AT>c[j].AT||flag_pc_overflow==2))
            {
                flag_pc_overflow=0;
                //Checking underflow
                if(buffer_check==0)
                {
                    //c[j].flag=0;
                    cout<<"Underflow. Can't handle process right now. Waiting for a producer"<<endl;
                    flag_pc_overflow=1;
                    if(p[i].AT>tot_time)
                    {
                        cout<<"CPU idle for some time"<<endl;
                        tot_time=p[i].AT;
                    }
                }
                else
                {
                    flag_pc=1;
                    pthread_create(&t_c[j],NULL,thread_pc,NULL);
                    pthread_join(t_c[j],NULL);
                    j++;
                } 
            }
        }
        if(i>=n&&j>=m)
        break;
        if(n<m&&i>=n)
        {
            cout<<"All producers done. Left consumers will be aborted"<<endl;
            break;
        }
        if(buffer_check>=buffer_size&&j>=m)
        {
            cout<<"Buffer is full and all consumers are processed. Left producers will be aborted"<<endl;
            break;
        }
        // if(p[i].AT>tot_time&&c[j].AT>tot_time)
        // {
        //     cout<<"CPU idle for some time"<<endl;
        //     if(p[i].AT<=c[j].AT)
        //     tot_time=p[i].AT;
        //     else tot_time=c[j].AT;
        // }
    }
    cout<<"All processes processed"<<endl;
    //Destroying semaphore
    sem_destroy(&mutex1);
    delete p;
    delete c;
}

void sleeping_barber(){
    i=-1;
    //Initialising Semaphore
    sem_init(&mutex1, 0, 1);
    cout<<"Enter the number of processes"<<endl;
    cin>>n;
    cout<<"Enter number of waiting chairs"<<endl;
    cin>>wait_chairs;
    p = new struct Producer[n];
    cout<<"Enter customer number, Arrival time and Burst time of each process"<<endl;
    for(int i=0;i<n;i++)
    {
        cout<<"Enter customer number"<<endl;
        cin>>p[i].id;
        cout<<"Enter its arrival time"<<endl;
        cin>>p[i].AT;
        cout<<"Enter its burst time"<<endl;
        cin>>p[i].BT;
        p[i].flag=0;
    }
    //Quicksort(0,n-1);
    for(int a=0;a<n;a++)
    {
        for(int b=a+1;b<n-a-1;b++)
        {
            if(p[b].AT>p[b+1].AT)
            {
                struct Producer temp=p[b];
                p[b]=p[b+1];
                p[b+1]=temp;
            }
        }
    }
    pthread_t t_p[n];
    p[0].flag=1;
    int flag1=0;
    tot_time=p[0].AT;
    for(int a=0;a<n;a++)
    {
        if(p[a].flag==1)
        {
            flag1=1;
            pthread_create(&t_p[a],NULL,thread_sleeping_barber,NULL);
            pthread_join(t_p[a],NULL);
        }
        if(flag1==0)
        {
            for(int b=0;b<n;b++)
            {
                if(p[b].flag==0)
                {
                    tot_time=p[b].AT;
                    i=b;
                    pthread_create(&t_p[b],NULL,thread_sleeping_barber,NULL);
                    pthread_join(t_p[b],NULL);
                    break;
                }
            }
        }
    }
    sem_destroy(&mutex1);
    delete p;
    
}

void dining_philosopher(){
    struct Process{
    int id; //person id
    float AT; // Arrival Time
    float BT; // Burst Time
    int flag=0; //flag to check whether a person is waiting , consuming or consumed.0=Waiting,1=In processing,2=processed
    }*p;
    n=0;
    int process_complete=0;
    cout<<"Enter the number of people in round table"<<endl;
    cin>>n;
    p = new struct Process[n];
    cout<<"Enter Arrival time and Burst time of each person"<<endl;
    for(int i=0;i<n;i++)
    {
        p[i].id=i+1;
        cout<<"Enter the arrival time of person "<<i+1<<endl;
        cin>>p[i].AT;
        cout<<"Enter its burst time"<<endl;
        cin>>p[i].BT;
    }
    //Sorting
    for(int a=0;a<n;a++)
    {
        for(int b=a+1;b<n-a-1;b++)
        {
            if(p[b].AT>p[b+1].AT)
            {
                struct Process temp=p[b];
                p[b]=p[b+1];
                p[b+1]=temp;
            }
        }
    }
    int forks[n];
    for(int i=0;i<n;i++)
    forks[i]=0;
    while(process_complete<n)
    {
        cout<<"At time = "<<tot_time<<" seconds"<<endl;
        for(int i=0;i<n;i++)
        {
            if(p[i].flag==1)
            {
                
                if(p[i].BT>0)
                {
                    cout<<"Person "<<p[i].id<<" is consuming"<<endl;
                }
                else
                {
                    p[i].flag=2;
                    if(p[i].id==1)
                    {
                        forks[p[i].id-1]=0;
                        forks[n-1]=0;
                    }
                    else 
                    {
                        forks[p[i].id-1]=0;
                        forks[p[i].id-2]=0;
                    }
                    cout<<"Person "<<p[i].id<<" finished consuming"<<endl;
                    process_complete++;
                    
                }
                p[i].BT--;
            }
        }
        for(int i=0;i<n;i++)
        {
            if(p[i].flag==0)
            {
                if(p[i].AT<=tot_time)
                {
                    if(p[i].id==1)
                    {
                        if(forks[0]==0&&forks[n-1]==0)
                        {
                            cout<<"Person "<<p[i].id<<" has started consuming "<<endl;
                            p[i].flag=1;
                            forks[0]=1;
                            forks[n-1]=1;
                            
                            if(p[i].BT==0)
                            {
                                p[i].flag=2;
                                forks[0]=0;
                                forks[n-1]=0;
                                cout<<"Person "<<p[i].id<<" has finished consuming "<<endl;
                                process_complete++;
                                
                            }
                            p[i].BT--;
                        }
                        else
                        {
                            cout<<"Person "<<p[i].id<<" is waiting"<<endl;
                        }
                        
                    }
                    else
                    {
                        if(forks[p[i].id-1]==0&&forks[p[i].id-2]==0)
                        {
                            cout<<"Person "<<p[i].id<<" has started consuming "<<endl;
                            p[i].flag=1;
                            forks[p[i].id-1]=1;
                            forks[p[i].id-2]=1;
                            
                            if(p[i].BT==0)
                            {
                                p[i].flag=2;
                                forks[p[i].id-1]=0;
                                forks[p[i].id-2]=0;
                                process_complete++;
                            }
                            p[i].BT--;
                        }
                        else
                        {
                            cout<<"Person "<<p[i].id<<" is waiting"<<endl;
                        }
                    }
                }
            }
            
        }
        if(process_complete==n)
        cout<<"All people have consumed "<<endl;
        tot_time++;
        cout<<endl;
    }
    delete p;
}


void* thread_rw(void* arg) 
{ 
    //wait 
    sem_wait(&mutex1); 
    //Entering Critical area
    cout<<endl;
    //Checking whether reader or writer is curently accessing critical area
    if(flag_rw==0)
    {
        p[i].flag=1;
        buffer_check++;
        for(int a=0;a<p[i].BT;a++)
        {    
            cout<<"At time = "<<tot_time<<" seconds"<<endl;
            for(int z=0;z<n;z++)
            {
                if(p[z].flag==0)
                {
                    if(p[z].AT<=tot_time)
                    {
                        cout<<"Writer "<<p[z].id<<" is waiting"<<endl;
                    }
                }
                else if(p[z].flag==1)
                {
                    cout<<"Writer "<<p[z].id<<" is being processed"<<endl;
                }
            }         
            for(int z=0;z<m;z++)
            {
                if(c[z].flag==0)
                {
                    if(c[z].AT<=tot_time)
                    {
                        cout<<"Reader "<<c[z].id<<" is waiting"<<endl;
                    }
                }
                else if(c[z].flag==1)
                {
                    cout<<"Reader "<<c[z].id<<" is being processed"<<endl;   
                }
            }
            tot_time++;
            cout<<endl;
        }
        cout<<"Writer "<<p[i].id<<" finished writing at time = "<<tot_time<<" seconds"<<endl;
        p[i].flag=2;
    }
    else 
    {
        c[j].flag=1;
        while(reader_reading!=0)
        {
            cout<<"At time = "<<tot_time<<" seconds"<<endl;
            for(int z=0;z<m;z++)
            {
                if(c[z].flag==0)
                {
                    if(c[z].AT<=tot_time)
                    {
                        reader_reading++;
                        c[z].flag=1;
                        cout<<"Reader "<<c[z].id<<" is reading"<<endl;
                        c[z].BT--;
                    }
                }
                else if(c[z].flag==1)
                {
                    if(c[z].BT==0)
                    {
                        reader_reading--;
                        cout<<"Reader "<<c[z].id<<" has completed reading at time = "<<tot_time<<" seconds"<<endl;
                        c[z].flag=2;
                        j++;

                    }
                    else
                    {
                        cout<<"Reader "<<p[z].id<<" is reading"<<endl;
                        c[z].BT--; 
                    }    
                } 
            }
            if(reader_reading==0)
            {
                //tot_time++;
                cout<<endl;
                break;
            }
            for(int z=0;z<n;z++)
            {
                if(p[z].flag==0)
                {
                    if(p[z].AT<=tot_time)
                    {
                        cout<<"Writer "<<p[z].id<<" is waiting"<<endl;
                    }
                }
                else if(p[z].flag==1)
                {
                    cout<<"Writer "<<p[z].id<<" is being processed"<<endl;
                }
            }
            tot_time++;     
            cout<<endl;
        }
    } 
    cout<<endl;
    //Exiting Critical area
    sem_post(&mutex1); 
    //Semaphore free for next iteration
    return 0;
}

void reader_writer(){
    
    //Initialising Semaphore
    sem_init(&mutex1, 0, 1);
    cout<<"Enter maximum number of writers who can wrirte"<<endl;
    cin>>buffer_size;
    cout<<"Enter the number of writers"<<endl;
    cin>>n;
    cout<<"Enter number of readers"<<endl;
    cin>>m;
    p = new struct Producer[n+1];
    c = new struct Consumer[m+1];
    //An extra writer and reader just for protection.Used in some conditions later
    p[n].AT=100000;
    c[m].AT=100000;
    cout<<"Enter writer's number, Arrival time and Burst time of each process"<<endl;
    for(int i=0;i<n;i++)
    {
        p[i].id=i+1;
        cout<<"Enter details of writer "<<p[i].id<<endl;
        cout<<"Enter its arrival time"<<endl;
        cin>>p[i].AT;
        cout<<"Enter its burst time"<<endl;
        cin>>p[i].BT;
    }
    cout<<"Enter reader's number, Arrival time and Burst time of each process"<<endl;
    for(int i=0;i<m;i++)
    {
        c[i].id=i+1;
        cout<<"Enter details of reader "<<c[i].id<<endl;
        cout<<"Enter its arrival time"<<endl;
        cin>>c[i].AT;
        cout<<"Enter its burst time"<<endl;
        cin>>c[i].BT;
    }
    //Sorting writers
    for(int a=0;a<n;a++)
    {
        for(int b=a+1;b<n-a-1;b++)
        {
            if(p[b].AT>p[b+1].AT)
            {
                struct Producer temp=p[b];
                p[b]=p[b+1];
                p[b+1]=temp;
            }
        }
    }
    //Sorting readers
    for(int a=0;a<m;a++)
    {
        for(int b=a+1;b<m-a-1;b++)
        {
            if(p[b].AT>p[b+1].AT)
            {
                struct Consumer temp=c[b];
                c[b]=c[b+1];
                c[b+1]=temp;
            }
        }
    }
    // for(int i=0;i<n;i++)
    // cout<<p[i].id<<" "<<p[i].AT<<" "<<p[i].BT<<endl;
    // for(int i=0;i<m;i++)
    // cout<<c[i].id<<" "<<c[i].AT<<" "<<c[i].BT<<endl;
    i=0;
    j=0;
    //Declaring two arrays of threads.One for each process
    pthread_t t_p[n];
    pthread_t t_c[m];
    if(p[i].AT!=0&&c[j].AT!=0)
    {
        cout<<"CPU idle for some time"<<endl;
        if(p[i].AT<=c[j].AT)
        tot_time=p[i].AT;
        else tot_time=c[j].AT;
    }
    while(1)
    {
        if(i<n)
        {   
            if(p[i].AT<=tot_time&&c[j].AT>=p[i].AT)
            {
                //Checking overflow
                if(buffer_size==buffer_check)
                {
                    cout<<"Buffer filled . Only read operation possible now."<<endl;
                    i=n+1;
                }
                else
                {
                    flag_rw=0;
                    pthread_create(&t_p[i],NULL,thread_rw,NULL);
                    pthread_join(t_p[i],NULL);
                    i++;
                } 
            }
        }
        if(j<m)
        {
            if(c[j].AT<=tot_time&&p[i].AT>c[j].AT)
            {               
                flag_rw=1;
                reader_reading++;
                pthread_create(&t_c[j],NULL,thread_rw,NULL);
                pthread_join(t_c[j],NULL);      
            }
        }
        if(i>=n&&j>=m)
        break;
        if(p[i].AT>tot_time&&c[j].AT>tot_time)
        {
            cout<<"CPU idle for some time"<<endl;
            if(p[i].AT<=c[j].AT)
            tot_time=p[i].AT;
            else tot_time=c[j].AT;
        }

    }

    cout<<"Program executed successfully"<<endl;
    //Destroying semaphore
    sem_destroy(&mutex1);
    delete p;
    delete c;
}

void synchronization(){
    int choice_sync=1;
    while(choice_sync!=5)
    {
        reset();
        cout<<"\nYou have the following choices\n 1) Producer Consumer \n 2) Reader-writer \n 3) Dining Philosopher \n 4) Sleeping barber \n 5) exit"<<endl;
        cout<<"Enter choice ";
        cin>>choice_sync;
        switch(choice_sync)
        {
            case 1: producer_consumer();
                    break;
            case 2: reader_writer();
                    break;
            case 3: dining_philosopher();
                    break;
            case 4: sleeping_barber();
                    break;
            case 5: 
                    break;
            default: cout<<"Wrong choice"<<endl;
        }
        
    }
}



void diskSchedulingFCFS(int input[],int head,int request_count){
    //code for FCFS
    int seek_time = 0;
    cout << "The FCFS scheduling is as follows: "<<endl;
    cout << "\n";
    cout << head << "->";
    for (int i = 0; i < request_count;i++){
        if(i==request_count-1){
            cout << input[i] << endl;
        }
        else{
            cout << input[i] << "->";
        }
        seek_time+=abs(head - input[i]);
        head = input[i];
    }
    cout << "\n";
    cout<<"Total seek time is "<<seek_time<<endl;
    float seek_time_average=(float)seek_time/request_count;
    cout<<"The average seek time is "<<seek_time_average<<endl;
    cout << "\n";
}
void diskSchedulingLOOK(int input[], int head, int request_count, int disk_size,set<int> input_set)
{
    //code for LOOK
    int seek_time = 0;
    vector<int> schedule;
    for(int i=head;i<disk_size;i++)
    {
        if(input_set.find(i)!=input_set.end())
        {
            schedule.push_back(i);
            input_set.erase(i);
        }
    }
    for(int i=head-1;i>=0;i--)
    {
        if(input_set.find(i)!=input_set.end())
        {
            schedule.push_back(i);
            input_set.erase(i);
        }    
    }
    cout<<"The LOOK scheduling is as follows: "<<endl;
    cout << "\n";
    cout<<head<<"->";
    for(auto a=schedule.begin();a!=schedule.end();a++)
    {
        if(a==schedule.end()-1)
        cout<<*a<<endl;
        else
        cout<<*a<<"->";
        seek_time+=abs(*(a)-head);
       
        head=*a;
    }
    cout << "\n";
    cout<<"Total seek time is "<<seek_time<<endl;
    float seek_time_average=(float)seek_time/request_count;
    cout<<"The average seek time is "<<seek_time_average<<endl;
    cout << "\n";

}
void diskSchedulingSCAN(int input[], int head, int request_count, int disk_size,set<int> input_set){
    //code for SCAN
    int seek_time = 0;
    vector<int> schedule;
    input_set.insert(disk_size-1);
    for(int i=head;i<disk_size;i++)
    {
        if(input_set.find(i)!=input_set.end())
        {
            schedule.push_back(i);
            input_set.erase(i);
        }
    }
    for(int i=head-1;i>=0;i--)
    {
        if(input_set.find(i)!=input_set.end())
        {
            schedule.push_back(i);
            input_set.erase(i);
        }    
    }
    int flag=0;
    cout<<"The SCAN scheduling is as follows "<<endl;
    cout << "\n";
    cout<<head<<"->";
    for(auto a=schedule.begin();a!=schedule.end();a++)
    {
        if(a==schedule.end()-1)
        cout<<*a<<endl;
        else
        cout<<*a<<"->";
        seek_time+=abs(*(a)-head);
        if(head>*a && flag==0)
        {

            seek_time+=2*(disk_size-1-head);
            flag=1;
        }
        head=*a;
    }
    cout << "\n";
    cout<<"Total seek time is "<<seek_time<<endl;
    float seek_time_average=(float)seek_time/request_count;
    cout<<"The average seek time is "<<seek_time_average<<endl;
    cout << "\n";
}
void diskSchedulingCLOOK(int input[], int head, int request_count, int disk_size,set<int> input_set){
    //code for CLOOK
    int seek_time = 0;
    vector<int> schedule;
    for(int i=head;i<disk_size;i++)
    {
        if(input_set.find(i)!=input_set.end())
        {
            schedule.push_back(i);
            input_set.erase(i);
        }
    }
    for(int i=0;i<head;i++)
    {
        if(input_set.find(i)!=input_set.end())
        {
            schedule.push_back(i);
            input_set.erase(i);
        }    
    }
    int flag=0;
    cout<<"The CLOOK scheduling is as follows "<<endl;
    cout << "\n";
    cout<<head<<"->";
    for(auto a=schedule.begin();a!=schedule.end();a++)
    {
        if(a==schedule.end()-1)
        cout<<*a<<endl;
        else
        cout<<*a<<"->";
        seek_time+=abs(*(a)-head);
        head=*a;
    }
    cout << "\n";
    cout<<"Total seek time is "<<seek_time<<endl;
    float seek_time_average=(float)seek_time/request_count;
    cout<<"The average seek time is "<<seek_time_average<<endl;
    cout << "\n";
}
void diskSchedulingCSCAN(int input[], int head, int request_count, int disk_size,set<int> input_set){
    //code for CSCAN
    int seek_time = 0;
    vector<int> schedule;
    input_set.insert(0);
    input_set.insert(disk_size - 1);
    for (int i = head; i < disk_size; i++)
    {
        if(input_set.find(i)!=input_set.end())
        {
            schedule.push_back(i);
            input_set.erase(i);
        }
    }
    
    for(int i=0;i<head;i++)
    {
        if(input_set.find(i)!=input_set.end())
        {
            schedule.push_back(i);
            input_set.erase(i);
        }    
    }
    int flag=0;
    cout<<"The CSCAN scheduling is as follows "<<endl;
    
    cout << "\n";
    for(auto a=schedule.begin();a!=schedule.end();a++)
    {
        if(a==schedule.end()-1)
        cout<<*a<<endl;
        else
        cout<<*a<<"->";
        
        if(head>*a&&flag==0)
        {
            seek_time+=(disk_size-1-head)+(disk_size-1);
            flag=1;
            head=0;
        }
        seek_time+=abs(*(a)-head);
        head=*a;
    }
    cout << "\n";
    cout<<"Total seek time is "<<seek_time<<endl;
    float seek_time_average=(float)seek_time/request_count;
    cout<<"The average seek time is "<<seek_time_average<<endl;
    cout << "\n";

}

int findMinimum(int dist[],bool vis[],int n){
    int idx = -1, largest = 1e9;
    for (int i = 0; i < n;i++){
        if(dist[i]<largest && !vis[i]){
            idx = i;
            largest = dist[i];
        }
    }
    return idx;
}

void findDifference(int input[],int dist[],int head,int n){
    for (int i = 0; i < n;i++){
        dist[i] = abs(input[i] - head);
    }
}

void diskSchedulingSSTF(int input[],int head,int request_count){
    int dist[request_count];
    bool vis[request_count];
    for (int i = 0; i < request_count;i++){
        
        vis[i] = false;
    }
    vector<int> seq;
    int seek_time = 0;
    for (int i = 0; i < request_count;i++){
        seq.push_back(head);
        findDifference(input, dist, head, request_count);
        int idx = findMinimum(dist, vis, request_count);
        vis[idx] = true;
        seek_time += dist[idx];
        head = input[idx];
    }
    seq.push_back(head);
    
    for (int i = 0; i < request_count+1;i++){
        if(i==request_count)
            cout << seq[i]<<endl;
        else
            cout << seq[i] << "->";
    }
    cout <<"Total seek time is "<<seek_time << endl;
    float seek_time_average=(float)seek_time/request_count;
    cout<<"The average seek time is "<<seek_time_average<<endl;
    cout << "\n";
}






void DiskSchedulingCallerFunction(){
    int request_count;
    int disk_choice;
    int disk_size;
    int head;
    set <int> input_set;
    cout << "\nWelcome to disk scheduling\n";

   //Taking the inputs 
    cout<<"Enter disk size ";
    cin>>disk_size;
    
    cout<<"Enter number of requests ";
    cin>>request_count;
    
    cout<<"Enter the starting location of head ";
    cin>>head;
    int inputsForDiskScheduling[request_count];
    
    cout<<"Enter requests one by one "<<endl;
    for(int i=0;i<request_count;i++)
    {
        cout<<"Enter request "<<i+1<<" ";
        cin >> inputsForDiskScheduling[i];
        if(inputsForDiskScheduling[i]>=disk_size)
        {
            cout<<"Page entered is wrong. Please renter the request"<<endl;
            i--;
            continue;
        }
        if(input_set.find(inputsForDiskScheduling[i])!=input_set.end())
        {
            cout<<"Page entered already present. Please renter the request"<<endl;
            i--;
            continue;
        }
        input_set.insert(inputsForDiskScheduling[i]);

    }
    while(disk_choice!=7)
    {
        cout << "The scheduling algorithm available:\n 1) FCFS\n 2) Look\n 3) SCAN\n 4) CLOOK\n 5) CSCAN\n 6) SSTF\n 7) Exit\n";
        cout << "Enter the desired algorithm option number ";
        cin >> disk_choice;
        switch(disk_choice){
            case 1 :
                diskSchedulingFCFS(inputsForDiskScheduling, head,request_count);
                break;
            case 2 :
                diskSchedulingLOOK(inputsForDiskScheduling,head,request_count,disk_size,input_set);
                break;
            case 3 :
                diskSchedulingSCAN(inputsForDiskScheduling,head,request_count,disk_size,input_set);
                break;
            case 4 :
                diskSchedulingCLOOK(inputsForDiskScheduling,head,request_count,disk_size,input_set);
                break;
            case 5 :
                diskSchedulingCSCAN(inputsForDiskScheduling,head,request_count,disk_size,input_set);
                break;
            case 6 :
                diskSchedulingSSTF(inputsForDiskScheduling,head,request_count);
                break;
            case 7 : break;
            default :
                cout << "Wrong input entered" << endl;
        }
    }

}


const int N = 1e5 +1;

void pageReplacementFIFO(){
    int fsize,pfault=0,cnt=0,filled=0;
    vector<int> v(N,-1);
    map<int,int> m;
    queue<int> q;
    cout<<"Enter the number of frames : ";
    cin>>fsize;
    int ch;
    while(true){
        cout<<"1.Add page\t2.Stop\n";
        cin>>ch;
        if(ch==2){
            cout<<"Number of page faults "<<pfault<<"\nPage fault ratio = "<<(float)pfault/cnt<<"\n";
            return;
        }
        cnt++;
        cout<<"Enter page number \t";
        int pno;
        cin>>pno;
        if(!m[pno]){
            pfault++;
            if(filled < fsize){
                v[filled]=pno;
                m[pno] = ++filled;
                cout<<"Page no "<<pno<<" is succesfully accomodated in frame "<<m[pno]-1<<'\n';
                q.push(pno);
                continue;
            }
            int victim=q.front();
            q.pop();
            v[m[victim]-1] = pno;
            m[pno] = m[victim];
            m[victim] = 0;
            q.push(pno);
            cout<<"Page no "<<pno<<" is accomodated in frame "<<m[pno]-1<<" after removing page "<<victim<<'\n';
        }
        else cout<<"Page "<<pno<<" is already there in frame "<<m[pno]-1<<'\n';
    }
}

void pageReplacementMRU(){
    int fsize,pfault=0,cnt=0,filled=0;
    vector<int> v(N,-1);
    map<int,int> m;
    map<int,int> lu;
    set<pair<int,int>> s;
    cout<<"Enter the number of frames : ";
    cin>>fsize;
    int ch;
    while(true){
        cout<<"1.Add page\t2.Stop\n";
        cin>>ch;
        if(ch==2){
            cout<<"Number of page faults "<<pfault<<"\nPage fault ratio = "<<(float)pfault/cnt<<"\n";
            return;
        }
        cnt++;
        cout<<"Enter page number \t";
        int pno;
        cin>>pno;
        if(!m[pno]){
            pfault++;
            if(filled < fsize){
                v[filled]=pno;
                m[pno] = ++filled;
                lu[pno] = cnt;
                s.insert({-cnt,pno});
                cout<<"Page no "<<pno<<" is succesfully accomodated in frame "<<m[pno]-1<<'\n';
                continue;
            }
            auto victim = s.begin()->second;
            s.erase(s.begin());
            v[m[victim]-1] = pno;
            m[pno] = m[victim];
            m[victim] = 0;
            lu[pno] =cnt;
            s.insert({-cnt,pno});
            cout<<"Page no "<<pno<<" is accomodated in frame "<<m[pno]-1<<" after removing page "<<victim<<'\n';
        }
        else{
            cout<<"Page "<<pno<<" is already there in frame "<<m[pno]-1<<'\n';
            s.erase(s.find({-lu[pno],pno}));
            lu[pno] = cnt;
            s.insert({-cnt,pno});
        }
    }
}

void pageReplacementLRU(){
    int fsize,pfault=0,cnt=0,filled=0;
    vector<int> v(N,-1);
    map<int,int> m;
    map<int,int> lu;
    set<pair<int,int>> s;
    cout<<"Enter the number of frames : ";
    cin>>fsize;
    int ch;
    while(true){
        cout<<"1.Add page\t2.Stop\n";
        cin>>ch;
        if(ch==2){
            cout<<"Number of page faults "<<pfault<<"\nPage fault ratio = "<<(float)pfault/cnt<<"\n";
            return;
        }
        cnt++;
        cout<<"Enter page number \t";
        int pno;
        cin>>pno;
        if(!m[pno]){
            pfault++;
            if(filled < fsize){
                v[filled]=pno;
                m[pno] = ++filled;
                lu[pno] = cnt;
                s.insert({cnt,pno});
                cout<<"Page no "<<pno<<" is succesfully accomodated in frame "<<m[pno]-1<<'\n';
                continue;
            }
            auto victim = s.begin()->second;
            s.erase(s.begin());
            v[m[victim]-1] = pno;
            m[pno] = m[victim];
            m[victim] = 0;
            lu[pno] =cnt;
            s.insert({cnt,pno});
            cout<<"Page no "<<pno<<" is accomodated in frame "<<m[pno]-1<<" after removing page "<<victim<<'\n';
        }
        else{
            cout<<"Page "<<pno<<" is already there in frame "<<m[pno]-1<<'\n';
            s.erase(s.find({lu[pno],pno}));
            lu[pno] = cnt;
            s.insert({cnt,pno});
        }
    }
}

void pageReplacementOptimal(){
    map<int,vector<int>> in;
    map<int,int> m;
    set<pair<int,int>> s;
    int n,pfault=0,fsize,filled = 0;
    vector<int> pages(N),v(N,-1);
    cout<<"Enter number of frames : ";
    cin>>fsize;
    cout<<"Enter number of page references : ";
    cin>>n;
    cout<<"Enter sequence of pages referenced \n";
    for(int i=0;i<n;i++){
        cin>>pages[i];
        if(!in[pages[i]].size()) in[pages[i]].push_back(INT_MAX);
    }
    for(int i=n-1;i>=0;i--) in[pages[i]].push_back(i);
    for(int i=0;i<n;i++){
        if(m[pages[i]]){
            cout<<"Page number "<<pages[i]<<" is already in frame "<<m[pages[i]]-1<<"\n";
            s.erase(s.find({-in[pages[i]].back(),pages[i]}));
            in[pages[i]].pop_back();
            s.insert({-in[pages[i]].back(),pages[i]});
            continue;
        }
        pfault++;
        if(filled  < fsize){
            v[filled] = pages[i];
            m[pages[i]] = ++filled;
            in[pages[i]].pop_back();
            s.insert({-in[pages[i]].back(),pages[i]});
            cout<<"Page number "<<pages[i]<<" is successfully accommodated in frame "<<m[pages[i]]-1<<"\n";
            continue;
        }
        auto victim = s.begin()->second;
        s.erase(s.begin());
        v[m[victim]-1] = pages[i];
        m[pages[i]] = m[victim];
        m[victim] = 0;
        in[pages[i]].pop_back();
        s.insert({-in[pages[i]].back(),pages[i]});
        cout<<"Page number "<<pages[i]<<" is accommodated in frame "<<m[pages[i]]-1<<" after removing page "<<victim<<"\n";
    }
    cout<<"Number of page faults "<<pfault<<"\nPage fault ratio ="<<(float)pfault/n<<"\n";
}

void pageReplacement(){
    int ch;
    while(true){
        cout<<"\nYou have the following choices:\n 1) Optimal\n 2) LRU\n 3) MRU\n 4) FIFO\n 5) Quit\nEnter choice ";
        cin>>ch;
        switch(ch){
            case 1 : {
                pageReplacementOptimal();
                break;
            }
            case 2 : {
                pageReplacementLRU();
                break;
            }
            case 3 : {
                pageReplacementMRU();
                break;
            }
            case 4 : {
                pageReplacementFIFO();
                break;
            }
            case 5 : return;
        }
    }
}

void bankersAlgo(){
  //cout<<"Im here"<<endl;
  cout<<"\nBANKER'S ALGORITHM\n";
  int r,n;
  int *maxresources,*totalalloc,*remaining;
  int *sequence,*completed;
  int **maxneed,**allocated,**required;
  int cnt,i,j,retry,match,update;
  retry=0;
  again:  //label to come back to
  if(retry){
    //freeing all pointers
    free(maxresources); 
    free(totalalloc); 
    free(remaining);
    for(i=0;i<n;i++){
      free(maxneed[i]);
      free(allocated[i]);
      free(required[i]);
    }
    free(sequence);
    free(completed);
  }
  
  //TAKING INPUTS
  printf("\nEnter number of resouces: ");
  scanf("%d",&r);
  printf("Enter number of processes: ");
  scanf("%d",&n);
        
  //allocating memory according to requirements
  maxresources = (int*) malloc(r*sizeof(int));
  totalalloc = (int*) malloc(r*sizeof(int));
  remaining = (int*) malloc(r*sizeof(int));
  sequence = (int*) malloc(n*sizeof(int));
  completed = (int*) malloc(n*sizeof(int));
  
  maxneed = (int**) malloc(n*sizeof(int*));
  for(i=0;i<n;i++) maxneed[i] = (int*) malloc(r*sizeof(int));
  allocated = (int**) malloc(n*sizeof(int*));
  for(i=0;i<n;i++) allocated[i] = (int*) malloc(r*sizeof(int));
  required = (int**) malloc(n*sizeof(int*));
  for(i=0;i<n;i++) required[i] = (int*) malloc(r*sizeof(int));
  
  printf("\n");
  for(i=0;i<r;i++){
    printf("Enter max availability of resource %d: ",i+1);
    scanf("%d",&maxresources[i]);
    totalalloc[i]=0;
  }
  for(i=0;i<n;i++){
    printf("\nEnter details of process %d:\n",i+1);
    for(j=0;j<r;j++){
      printf("\tMax. need for resource %d: ",j+1);
      scanf("%d",&maxneed[i][j]);
      printf("\tAllocation of resource %d: ",j+1);
      scanf("%d",&allocated[i][j]);
      totalalloc[j]+=allocated[i][j];
      required[i][j]=maxneed[i][j]-allocated[i][j];
    }
  }
  printf("\n");
  
  //CHECKING VALIDITY OF INPUT
  retry=-1;
  for(i=0;i<r;i++){
    if(totalalloc[i]>maxresources[i]){
      printf("Invalid input. Total resource %d allocated is greater than its max availibity.\n",i+1);
      while(retry!=1){
        printf("Would you like to try another input? (0-No/1-Yes)\t");
        scanf("%d",&retry);
        if(retry==0) return;
      }
      goto again;
    }
  }  
  for(i=0;i<n;i++){
    for(j=0;j<r;j++){
      if(maxneed[i][j]>maxresources[j]){
        printf("Invalid input. Max need of resource %d for process %d is greater than its max availibility.\n",j+1,i+1);
        while(retry!=1){
          printf("Would you like to try another input? (0-No/1-Yes)\t");
          scanf("%d",&retry);
          if(retry==0) return;
        }
        goto again;
      }
      if(allocated[i][j]>maxneed[i][j]){
        printf("Invalid input. Resource %d allocated to process %d is greater than its max need.\n",j+1,i+1);
        while(retry!=1){
          printf("Would you like to try another input? (0-No/1-Yes)\t");
          scanf("%d",&retry);
          if(retry==0) return;
        }
        goto again;
      }
    }
  }

  printf("Valid input.\n");
  for(i=0;i<r;i++){
    remaining[i]=maxresources[i]-totalalloc[i]; 
  }
  //CHECKING FOR SAFE SEQUENCE
  for(i=0;i<n;i++) completed[i]=0;
  while(cnt<n){
    update=0;
    for(i=0;i<n;i++){
      if(completed[i]==1) continue;
      match=0;
      for(j=0;j<r;j++){
        if(required[i][j]<=remaining[j]) match++;
      }
      if(match==r){ //enough resources available
        completed[i]=1;
        sequence[cnt++]=i;
        for(j=0;j<r;j++){
          remaining[j]+=allocated[i][j];
        }
        update++;
      }
    }
    if(update==0){ //no updation in an iteration
      printf("No possible safe sequence. Enters deadlock.\n");
      break;
    }
  }
  if(cnt==n){
    printf("The safe sequence of execution is:\n");
    for(i=0;i<n;i++)
      printf("Process %d | ",sequence[i]+1);
  }
  printf("\n");  
  while(retry!=1){
    printf("Would you like to try another input? (0-No/1-Yes)\t");
    scanf("%d",&retry);
    if(retry==0) return;
  }
  goto again;
}

void process_sch_sjf()
{
	//Declaring a variable to store no of processes
	int n;
	cout<<"\nEnter the no of processes:";
	cin>>n;
	
	//Declaring arrays for storing Arrival, Burst, Completion, Turn Around and Waiting times
	int at[n],bt[n],ct[n],tat[n],wt[n],pid[n];
	cout<<"\nEnter the Arrival Time and Burst Time of all processes:\n";
	
	int i,j;
	//Here i and j are used to run for loops
	
	//Collect Arrival times and Burst times of all processes and store in respective arrays
	for(i=0;i<n;i++)
	{
		cout<<"\nProcess ID:";
		cin>>pid[i];
		cout<<"\nArrival time:";
		cin>>at[i];
		cout<<"Burst time:";
		cin>>bt[i];
	}
	
	int t,c,l;
	//Here t is used to store time
	//c is used to store no of processes are completed and
	//Initializing them to zero
	//l is used to store index of process which has minimum burst time
	//Initializing l to -1
	l=-1;
	t=0;
	c=0;
	
	//Initializing completion times of all processes to -1 for reference
	for(i=0;i<n;i++)
	{
		ct[i]=-1;
	}
	
	//Calculate completion times of all processes and store in ct array
	int q;
	q=1;
	while(c<n)
	{
		for(j=0;j<n;j++)
		{
			if(ct[j]==-1&&at[j]<=t)
			{
				l=j;
				break;
			}
		}
		for(i=l+1;i<n;i++)
		{
			if(at[i]<=t)
			{
				if(ct[i]==-1)
				{
					if(bt[i]<bt[l])
					{
						l=i;
					}
				}
			}
		}
		if(at[l]<=t&&ct[l]==-1)
		{
			q=1;
			cout<<t<<" ";
			cout<<"P"<<pid[l]<<" ";
			t=t+bt[l];
			ct[l]=t;
			c++;
		}
		else
		{
			if(q==1)
			{
				cout<<t<<" ";
				cout<<"   ";
			}
			q=0;
			t++;
		}
	}
	cout<<t<<"\n";
	
	//Declaring variables to store total and average turn around times and waiting times
	float ttat,twt,atat,awt;
	
	//Calculate turn around times and waiting times of all processes
	for(i=0;i<n;i++)
	{
		tat[i]=ct[i]-at[i];
		wt[i]=tat[i]-bt[i];
	}
	
	//Initialize total turn around time and total waiting time to zero
	ttat=0;
	twt=0;
	
	//Calculating total turn around time and total waiting time
	for(i=0;i<n;i++)
	{
		ttat=ttat+tat[i];
		twt=twt+wt[i];
	}
	
	//Calculating average turn around time and average waiting time
	atat=(float)ttat/n;
	awt=(float)twt/n;
	
	cout<<"\nID\tAT\tBT\tCT\tTAT\tWT\n";
    for(i=0; i<n; i++)
	{
        cout<<"\n"<<pid[i]<<"\t"<<at[i]<<"\t"<<bt[i]<<"\t"<<ct[i]<<"\t"<<tat[i]<<"\t"<<wt[i];         
    }
    cout<<"\n";
	
	//Printing average turn around time and average waiting time
	cout<<"\nAverage Turn Around Time = "<<atat;
	cout<<"\nAverage Waiting Time = "<<awt<<endl;
}

void process_sch_lrjf()
{
	//Declaring a variable to store no of processes
	int n;
	cout<<"\nEnter the no of processes:";
	cin>>n;
	
	//Declaring arrays for storing Arrival, Burst, Completion, Turn Around and Waiting times
	int at[n],bt[n],obt[n],ct[n],tat[n],wt[n],rt[n],pid[n];
	cout<<"\nEnter the Arrival Time and Burst Time of all processes:\n";
	
	int i,j;
	//Here i and j are used to run for loops
	
	//Collect Arrival times and Burst times of all processes and store in respective arrays
	for(i=0;i<n;i++)
	{
		cout<<"\nProcess ID:";
		cin>>pid[i];
		cout<<"\nArrival time:";
		cin>>at[i];
		cout<<"Burst time:";
		cin>>bt[i];
		obt[i]=bt[i];
	}
	
	//here t is used to store time
	//c is used to keep track of how many processes have completed
	//l is used to store index of process which has longest remaaining time
	//m is used to store the index of process which was excuted in the last sec 
	int t,c,l,m;
	t=0;
	c=0;
	l=-1;
	m=-1;
	
	//Initializing completion times of all processes to -1 for reference
	for(i=0;i<n;i++)
	{
		ct[i]=-1;
	}
	

	cout<<"\nGantt chart:\n";
	
	//while loop to calculate response time and print gantt chart
	//will end when c=n i.e., all processes have completed
	int q;
	q=1;
	while(c<n)
	{
		
		for(j=0;j<n;j++)
		{
			if(ct[l]==-1 && l!=-1)
			{
				break;
			}
			if(ct[j]==-1 && at[j]<=t)
			{
				l=j;
				break;
			}
			else
			{
				l=-1;
			}
		}
		if(l!=-1)
		{
			for(i=0;i<n;i++)
			{
				if(at[i]<=t)
				{
					if(ct[i]==-1)
					{
						if(bt[i]>bt[l])
						{
							l=i;
						}
					}
				}
			}
		}
		
		if(l!=-1)
		{
			q=1;
			if(bt[l]==obt[l])
			{
				rt[l]=t;
			}
			if(m!=l)
			{
				cout<<t<<" ";
				cout<<"P"<<pid[l]<<" ";
			}
			t++;
			bt[l]--;
			m=l;

			if(bt[l]==0)
			{
				ct[l]=t;
				c++;
			}
		}
		else
		{
			if(q==1)
			{
				cout<<t<<" ";
				cout<<"   ";
			}
			q=0;
			t++;
		}
	}
	
	cout<<t;
	
	//deleclaring variables to store total response time of all processes and average response time
	float trt,art;
	trt=0;
	
	//calculating total response time of all processes
	for(i=0;i<n;i++)
	{
		trt=trt+rt[i];
	}
	
	for(i=0;i<n;i++)
	{
		tat[i]=ct[i]-at[i];
		wt[i]=tat[i]-obt[i];
	}
	
	float ttat,twt,atat,awt;
	ttat=0;
	twt=0;
	
	//Calculating total turn around time and total waiting time
	for(i=0;i<n;i++)
	{
		ttat=ttat+tat[i];
		twt=twt+wt[i];
	}
	
	//Calculating average turn around time and average waiting time
	atat=(float)ttat/n;
	awt=(float)twt/n;
	
	cout<<"\nID\tAT\tBT\tCT\tTAT\tWT\n";
    for(i=0; i<n; i++)
	{
        cout<<"\n"<<pid[i]<<"\t"<<at[i]<<"\t"<<obt[i]<<"\t"<<ct[i]<<"\t"<<tat[i]<<"\t"<<wt[i];         
    }
    cout<<"\n";
	
	//calculating average response time
	art=(float)trt/n;
	
	cout<<"\nAverage Response time: "<<art;
	cout<<"\nAverage Turn Around Time = "<<atat;
	cout<<"\nAverage Waiting Time = "<<awt<<endl;
}

void process_sch_rr()
{
	//Declaring a variable to store no of processes
	int n;
	cout<<"\nEnter the no of processes:";
	cin>>n;
	
	//Declaring arrays for storing Arrival, Burst, Completion, Turn Around and Waiting times
	int at[n],bt[n],obt[n],ct[n],tat[n],wt[n],pid[n];
	cout<<"\nEnter the Arrival Time and Burst Time of all processes:\n";
	
	int i;
	//Here i is used to run for loops
	
	//Collect Arrival times and Burst times of all processes and store in respective arrays
	for(i=0;i<n;i++)
	{
		cout<<"\nProcess ID:";
		cin>>pid[i];
		cout<<"\nArrival time:";
		cin>>at[i];
		cout<<"Burst time:";
		cin>>bt[i];
		//obt[i]=bt[i];

	}
	
	//Declaring a duplicate array for storing actual burst values since they keep changing
	int ibt[n];
	for(i=0;i<n;i++)
	{
		ibt[i]=bt[i];
	}

	//Declaring a variable to store time slice
	int s;
	cout<<"\nEnter the time slice:";
	cin>>s;
	
	int t,c;
	//Here t is used to store time
	//And c is used to store no of processes are completed
	//Initializing them to zero
	t=0;
	c=0;
	quickSort(pid, at, bt, 0, n-1);
    for(i=0;i<n;i++)
	{
		obt[i]=bt[i];
	}
	//Calculate completion times of all processes and store in ct array
	int q;
	q=1;
	while(c<n)
	{
		for(i=0;i<n;i++)
		{
			if(at[i]<=t)
			{
				if(bt[i]!=0)
				{
					if(bt[i]>2)
					{
						q=1;
						cout<<t<<" ";
						cout<<"P"<<pid[i]<<" ";
						t=t+2;
						bt[i]=bt[i]-2;
					}
					else
					{
						q=1;
						cout<<t<<" ";
						cout<<"P"<<pid[i]<<" ";
						t=t+bt[i];
						bt[i]=0;
						ct[i]=t;
						c++;
					}
				}
			}
			else
			{
				if(q==1)
				{
					cout<<t<<" ";
					cout<<"   ";
				}
				q=0;
				t++;
			}
		}
	}
	
    cout<<t<<endl;
	//Declaring variables to store total and average turn around times and waiting times
	float ttat,twt,atat,awt;
	
	//Calculate turn around times and waiting times of all processes
	for(i=0;i<n;i++)
	{
		tat[i]=ct[i]-at[i];
		wt[i]=tat[i]-obt[i];
	}
	

	//Initialize total turn around time and total waiting time to zero
	ttat=0;
	twt=0;
	
	//Calculating total turn around time and total waiting time
	for(i=0;i<n;i++)
	{
		ttat=ttat+tat[i];
		twt=twt+wt[i];
	}
	
	//Calculating average turn around time and average waiting time
	atat=(float)ttat/n;
	awt=(float)twt/n;
	
	cout<<"\nID\tAT\tBT\tCT\tTAT\tWT\n";
    for(i=0; i<n; i++)
	{
        cout<<"\n"<<pid[i]<<"\t"<<at[i]<<"\t"<<obt[i]<<"\t"<<ct[i]<<"\t"<<tat[i]<<"\t"<<wt[i];         
    }
    cout<<"\n";
	
	//Printing average turn around time and average waiting time
	printf("\nAverage Turn Around Time = %f",atat);
	printf("\nAverage Waiting Time = %f\n",awt);
}

void process_sch_srjf()
{
	//Declaring a variable to store no of processes
	int n;
	cout<<"\nEnter the no of processes:";
	cin>>n;
	
	//Declaring arrays for storing Arrival, Burst, Completion, Turn Around and Waiting times
	int at[n],bt[n],obt[n],ct[n],tat[n],wt[n],rt[n],pid[n];
	cout<<"\nEnter the Arrival Time and Burst Time of all processes:\n";
	
	int i,j;
	//Here i and j are used to run for loops
	
	//Collect Arrival times and Burst times of all processes and store in respective arrays
	for(i=0;i<n;i++)
	{
		cout<<"\nProcess ID:";
		cin>>pid[i];
		cout<<"\nArrival time:";
		cin>>at[i];
		cout<<"Burst time:";
		cin>>bt[i];
		obt[i]=bt[i];
	}
	
	//here t is used to store time
	//c is used to keep track of how many processes have completed
	//l is used to store index of process which has longest remaaining time
	//m is used to store the index of process which was excuted in the last sec 
	int t,c,l,m;
	t=0;
	c=0;
	l=-1;
	m=-1;
	
	//Initializing completion times of all processes to -1 for reference
	for(i=0;i<n;i++)
	{
		ct[i]=-1;
	}
	

	cout<<"\nGantt chart:\n";
	
	//while loop to calculate response time and print gantt chart
	//will end when c=n i.e., all processes have completed
	int q;
	q=1;
	while(c<n)
	{
		
		for(j=0;j<n;j++)
		{
			if(ct[l]==-1 && l!=-1)
			{
				break;
			}
			if(ct[j]==-1 && at[j]<=t)
			{
				l=j;
				break;
			}
			else
			{
				l=-1;
			}
		}
		if(l!=-1)
		{
			for(i=0;i<n;i++)
			{
				if(at[i]<=t)
				{
					if(ct[i]==-1)
					{
						if(bt[i]<bt[l])
						{
							l=i;
						}
					}
				}
			}
		}
		
		if(l!=-1)
		{
			q=1;
			if(bt[l]==obt[l])
			{
				rt[l]=t;
			}
			if(m!=l)
			{
				cout<<t<<" ";
				cout<<"P"<<pid[l]<<" ";
			}
			t++;
			bt[l]--;
			m=l;

			if(bt[l]==0)
			{
				ct[l]=t;
				c++;
			}
		}
		else
		{
			if(q==1)
			{
				cout<<t<<" ";
				cout<<"   ";
			}
			q=0;
			t++;
		}
	}
	
	cout<<t;
	
	//deleclaring variables to store total response time of all processes and average response time
	float trt,art;
	trt=0;
	
	//calculating total response time of all processes
	for(i=0;i<n;i++)
	{
		trt=trt+rt[i];
	}
	
	for(i=0;i<n;i++)
	{
		tat[i]=ct[i]-at[i];
		wt[i]=tat[i]-obt[i];
	}
	
	float ttat,twt,atat,awt;
	ttat=0;
	twt=0;
	
	//Calculating total turn around time and total waiting time
	for(i=0;i<n;i++)
	{
		ttat=ttat+tat[i];
		twt=twt+wt[i];
	}
	
	//Calculating average turn around time and average waiting time
	atat=(float)ttat/n;
	awt=(float)twt/n;
	
	cout<<"\nID\tAT\tBT\tCT\tTAT\tWT\n";
    for(i=0; i<n; i++)
	{
        cout<<"\n"<<pid[i]<<"\t"<<at[i]<<"\t"<<obt[i]<<"\t"<<ct[i]<<"\t"<<tat[i]<<"\t"<<wt[i];         
    }
    cout<<"\n";
	
	//calculating average response time
	art=(float)trt/n;
	
	cout<<"\nAverage Response time: "<<art;
	cout<<"\nAverage Turn Around Time = "<<atat;
	cout<<"\nAverage Waiting Time = "<<awt<<endl;
}

void process_sch_priority_pemp()
{
	//Declaring a variable to store no of processes
	int n;
	cout<<"\nEnter the no of processes:";
	cin>>n;
	
	//Declaring arrays for storing Arrival, Burst, Completion, Turn Around and Waiting times
	int at[n],bt[n],obt[n],p[n],ct[n],tat[n],wt[n],pid[n];
	cout<<"\nEnter the Arrival Time, Burst Time and Priority of all processes:\n";
	
	int i,j;
	//Here i is used to run for loops
	
	//Collect Arrival times and Burst times of all processes and store in respective arrays
	for(i=0;i<n;i++)
	{
		cout<<"\nProcess ID:";
		cin>>pid[i];
		cout<<"\nArrival time:";
		cin>>at[i];
		cout<<"Burst time:";
		cin>>bt[i];
		obt[i]=bt[i];
		cout<<"Priority:";
		cin>>p[i];
	}
	
	//here t is used to store time
	//c is used to keep track of how many processes have completed
	//l is used to store index of process which has highest priority (lowest priority number)
	//m is used to store the index of process which was excuted in the last sec 
	int t,c,l,m;
	t=0;
	c=0;
	l=-1;
	m=-1;
	
	//Initializing completion times of all processes to -1 for reference
	for(i=0;i<n;i++)
	{
		ct[i]=-1;
	}
	
	cout<<"\nGantt Chart\n";

	//while loop to calculate completion time and print gantt chart
	//will end when c=n i.e., all processes have completed
	int q;
	q=1;
	while(c<n)
	{
		for(j=0;j<n;j++)
		{
			if(ct[l]==-1 && l!=-1)
			{
				break;
			}
			if(ct[j]==-1 && at[j]<=t)
			{
				l=j;
				break;
			}
			else
			{
				l=-1;
			}
		}
		if(l!=-1)
		{
			for(i=0;i<n;i++)
			{
				if(at[i]<=t)
				{
					if(ct[i]==-1)
					{
						if(p[i]<p[l])
						{
							l=i;
						}
					}
				}
			}
		}
		
		if(l!=-1)
		{
			q=1;
			if(m!=l)
			{
				cout<<t<<" ";
				cout<<"P"<<pid[l]<<" ";
			}
			t++;
			bt[l]--;

			m=l;
			if(bt[l]==0)
			{
				ct[l]=t;
				c++;
			}
		}
		else
		{
			if(q==1)
			{
				cout<<t<<" ";
				cout<<"   ";
			}
			q=0;
			t++;
		}
	}
	cout<<t<<" ";

	//Calculate turn around times and waiting times of all processes
	for(i=0;i<n;i++)
	{
		tat[i]=ct[i]-at[i];
		wt[i]=tat[i]-obt[i];
	}
	
	//Declaring variables to store total and average turn around times and waiting times
	float ttat,twt,atat,awt;
	ttat=0;
	twt=0;
	
	//Calculating total turn around time and total waiting time
	for(i=0;i<n;i++)
	{
		ttat=ttat+tat[i];
		twt=twt+wt[i];
	}
	
	//Calculating average turn around time and average waiting time
	atat=(float)ttat/n;
	awt=(float)twt/n;
	
	cout<<"\nID\tAT\tBT\tCT\tTAT\tWT\n";
    for(i=0; i<n; i++)
	{
        cout<<"\n"<<pid[i]<<"\t"<<at[i]<<"\t"<<obt[i]<<"\t"<<ct[i]<<"\t"<<tat[i]<<"\t"<<wt[i];         
    }
    cout<<"\n";
	
	cout<<"\nAverage Turn Around Time = "<<atat;
	cout<<"\nAverage Waiting Time = "<<awt<<endl;
}

void process_sch_ljf()
{
	//Declaring a variable to store no of processes
	int n;
	cout<<"\nEnter the no of processes:";
	cin>>n;
	
	//Declaring arrays for storing Arrival, Burst, Completion, Turn Around and Waiting times
	int at[n],bt[n],ct[n],tat[n],wt[n],pid[n];
	cout<<"\nEnter the Arrival Time and Burst Time of all processes:\n";
	
	int i,j;
	//Here i and j are used to run for loops
	
	//Collect Arrival times and Burst times of all processes and store in respective arrays
	for(i=0;i<n;i++)
	{
		cout<<"\nProcess ID:";
		cin>>pid[i];
		cout<<"\nArrival time:";
		cin>>at[i];
		cout<<"Burst time:";
		cin>>bt[i];
	}
	
	int t,c,l;
	//Here t is used to store time
	//c is used to store no of processes are completed and
	//Initializing them to zero
	//l is used to store index of process which has minimum burst time
	//Initializing l to -1
	l=-1;
	t=0;
	c=0;
	
	//Initializing completion times of all processes to -1 for reference
	for(i=0;i<n;i++)
	{
		ct[i]=-1;
	}
	
	//Calculate completion times of all processes and store in ct array
	int q;
	q=1;
	while(c<n)
	{
		for(j=0;j<n;j++)
		{
			if(ct[j]==-1&&at[j]<=t)
			{
				l=j;
				break;
			}
		}
		for(i=l+1;i<n;i++)
		{
			if(at[i]<=t)
			{
				if(ct[i]==-1)
				{
					if(bt[i]>bt[l])
					{
						l=i;
					}
				}
			}
		}
		if(at[l]<=t&&ct[l]==-1)
		{
			q=1;
			cout<<t<<" ";
			cout<<"P"<<pid[l]<<" ";
			t=t+bt[l];
			ct[l]=t;
			c++;
		}
		else
		{
			if(q==1)
			{
				cout<<t<<" ";
				cout<<"   ";
			}
			q=0;
			t++;
		}
	}
	cout<<t<<"\n";
	
	//Declaring variables to store total and average turn around times and waiting times
	float ttat,twt,atat,awt;
	
	//Calculate turn around times and waiting times of all processes
	for(i=0;i<n;i++)
	{
		tat[i]=ct[i]-at[i];
		wt[i]=tat[i]-bt[i];
	}
	
	//Initialize total turn around time and total waiting time to zero
	ttat=0;
	twt=0;
	
	//Calculating total turn around time and total waiting time
	for(i=0;i<n;i++)
	{
		ttat=ttat+tat[i];
		twt=twt+wt[i];
	}
	
	//Calculating average turn around time and average waiting time
	atat=(float)ttat/n;
	awt=(float)twt/n;
	
	cout<<"\nID\tAT\tBT\tCT\tTAT\tWT\n";
    for(i=0; i<n; i++)
	{
        cout<<"\n"<<pid[i]<<"\t"<<at[i]<<"\t"<<bt[i]<<"\t"<<ct[i]<<"\t"<<tat[i]<<"\t"<<wt[i];         
    }
    cout<<"\n";
	
	//Printing average turn around time and average waiting time
	cout<<"\nAverage Turn Around Time = "<<atat;
	cout<<"\nAverage Waiting Time = "<<awt<<endl;
}



void process_sch_fcfs()
{
	//Declaring a variable to store no of processes
	int n;
	cout<<"\nEnter the no of processes:";
	cin>>n;
	
	//Declaring arrays for storing Arrival, Burst, Completion, Turn Around and Waiting times
	int at[n],bt[n],ct[n],tat[n],wt[n],pid[n];
	cout<<"\nEnter the Arrival Time and Burst Time of all processes:\n";
	
	int i,t,c;
	//Here i is used to run for loops
	//Here t is used to store time
	//And c is used to store no of processes are completed
	
	//Declaring variables to store total and average turn around times and waiting times
	float ttat,twt,atat,awt;
	
	//Collect Arrival times and Burst times of all processes and store in respective arrays
	for(i=0;i<n;i++)
	{
		cout<<"\nProcess ID:";
		cin>>pid[i];
		cout<<"\nArrival time:";
		cin>>at[i];
		cout<<"Burst time:";
		cin>>bt[i];
	}
	
	//Initialize time and no of processes completed to zero
	c=0;
	t=0;
	
	//Calling quicksort to sort
	quickSort(pid, at, bt, 0, n-1);
	
	//Calculate completion times of all processes and store in ct array
	
	cout<<"\nGantt Chart\n";
	
	int q;
	q=1;
	while(c<n)
	{
		if(at[c]<=t)
		{
			q=1;
			cout<<t<<" ";
			cout<<"P"<<pid[c]<<" ";
			t=t+bt[c];
			ct[c]=t;
			c++;
		}
		else
		{
			if(q==1)
			{
				cout<<t<<" ";
				cout<<"   ";
			}
			q=0;
			t++;
		}
	}
	cout<<t<<"\n";
	
	//Calculate turn around times and waiting times of all processes
	for(i=0;i<n;i++)
	{
		tat[i]=ct[i]-at[i];
		wt[i]=tat[i]-bt[i];
	}
	
	//Initialize total turn around time and total waiting time to zero
	ttat=0;
	twt=0;
	
	//Calculate total turn around time and total waiting time
	for(i=0;i<n;i++)
	{
		ttat=ttat+tat[i];
		twt=twt+wt[i];
	}
	
	//Calculating average turn around time and average waiting time
	atat=(float)ttat/n;
	awt=(float)twt/n;
	

    cout<<"\nID\tAT\tBT\tCT\tTAT\tWT\n";
    for(i=0; i<n; i++)
	{
        cout<<"\n"<<pid[i]<<"\t"<<at[i]<<"\t"<<bt[i]<<"\t"<<ct[i]<<"\t"<<tat[i]<<"\t"<<wt[i];         
    }
    cout<<"\n";

	
	//Printing average turn around time and average waiting time
	cout<<"\nAverage Turn Around Time = "<<atat;
	cout<<"\nAverage Waiting Time = "<<awt<<endl;
}


void HRRN()
{
   int n,i,times=0,t;
   float hrrn;
   float sum_turnaround=0,avg_turnaround,sum_waiting=0,avg_waiting;
   //number of processes
   printf("enter the number of processes");
   scanf("%d",&n);
   //declaring arrival,burst,completion,,turnaround and waiting times
   int arrival[n],burst_rem[n],completion[n],turnaround[n],wait_now[n],count=0,process_state[n];
   //inputing arrival times of processes
   float rr[n];
//    printf("enter the arrival times of processes accordingly");
//    for(i=0;i<n;i++)
//    {scanf("%d",&arrival[i]);}
//    //inputing burst times of processes
//     printf("enter the burst times of processes accordingly");
//     for(i=0;i<n;i++)
//    {scanf("%d",&burst_rem[i]);}
    // for(i=0;i<n;i++)
    // {process_state[i]=0;}
    //  for(i=0;i<n;i++)
    // {rr[i]=0;}
    cout<<"Enter information about processes one by one"<<endl;
    for(int i=0;i<n;i++)
    {
        cout<<"Enter details of process "<<i<<endl;
        cout<<"Enter its arrival time ";
        cin>>arrival[i];
        cout<<"Enter its burst time ";
        cin>>burst_rem[i];
        process_state[i]=0;
        rr[i]=0;
        cout<<endl;
    }
    int c;
    while(count!=n)
    {   c=0;
    t=0;
     for(i=0;i<n;i++)
    { if(process_state[i]==0&&times>=arrival[i])
       { rr[i]=(1.0 * (times-arrival[i]))/(1.0 * burst_rem[i]);
    if(c==0)
    {hrrn=rr[i];
        t=i;
        c++;
    }
        if(hrrn<rr[i])
        t=i;
    }}
    if(c==0)
    times++;    
        else
        {
          wait_now[t]=times-arrival[t];
          process_state[t]=1;
          rr[t]=0;
          count++;
         times=times+burst_rem[t];
         completion[t]=times;}
         
    }  
          
          

    //turnaround times
    for(i=0;i<n;i++)
    {turnaround[i]=completion[i]-arrival[i];}
    //waiting times 
    // printing the gaul chart
    printf("processnumber   arrival time   burst time   completion time   turnaround time   waiting time ");
    printf("\n");
    for(i=0;i<n;i++)
    {printf("%d               %d              %d            %d                 %d                 %d",i+1,arrival[i],burst_rem[i],completion[i],turnaround[i],wait_now[i]);
    printf("\n");}
    //finding the avg turnaround and waiting times_
    for(i=0;i<n;i++)
    { sum_turnaround=sum_turnaround+turnaround[i];}
    avg_turnaround=(sum_turnaround/n);
    printf("\n");
    printf("the average turnaround time is %f",avg_turnaround);
    for(i=0;i<n;i++)
    { sum_waiting=sum_waiting+wait_now[i];}
    avg_waiting=(sum_waiting/n);
    printf("\n");
    printf("the average waiting time is %f\n",avg_waiting);
}


void process_sch_caller()
{
    
	int choice=1;
    while(choice!=9)
    {
        cout<<endl;
        cout<<"You have the following choice :-";
        cout << "\n 1) SJF\n 2) LRTF\n 3) RR\n 4) SRTF\n 5) Priority\n 6) LJF\n 7) FCFS\n 8) HRRN\n 9) Exit\n";
        cout << "Enter the desired algorithm option number ";
        cin >> choice;
        switch(choice){
            case 1 :
                process_sch_sjf();
                break;
            case 2 :
                process_sch_lrjf();
                break;
            case 3 :
                process_sch_rr();
                break;
            case 4 :
                process_sch_srjf();
                break;
            case 5 :
                process_sch_priority_pemp();
                break;
            case 6 :
                process_sch_ljf();
                break;
            case 7 :
                process_sch_fcfs();
                break;
            case 8 : 
                HRRN();
                break;
            case 9 :
                break;
            default :
                cout << "Wrong input entered" << endl;
        }
    }
}

void paging(){
    int physical_mem_size;
    int virtual_mem_size;
    int page_size;
    int virtual_page_count=0;
    int physical_page_count=0;
    int offset_max=1024;
    cout<<"\nEnter size of virtual memory(in kB) ";
    cin>>virtual_mem_size;
    cout<<"Enter size of physical memory(in kB) ";
    cin>>physical_mem_size;
    cout<<"Enter page size(in kB) ";
    cin>>page_size;
    cout<<endl;
    offset_max*=page_size;
    offset_max--;
    virtual_page_count=virtual_mem_size/page_size;
    physical_page_count=physical_mem_size/page_size;
    cout<<"Number of pages in virtual memory are "<<virtual_page_count<<endl;
    cout<<"Number of frames in physical memory are "<<physical_page_count<<endl;
    int pages[virtual_page_count];
    for(int i=0;i<virtual_page_count;i++)
        pages[i]=-1;
    int frames[physical_page_count];
    for(int i=0;i<virtual_page_count;i++)
        frames[i]=-1;

    list <int> q;
    int flag=1;
    int page_fault_count=0;
    int page;
    int offset;
    int frame_no=0;
    int x;
    int input_count=0;
    int choice;
    while(1)  //Loop runs till choice entered is not 2
    {
        cout<<"\nPress\n1 to enter next virtual address\n2 to print page table(Virtual Memory)\n3 to print frame table(Physical Memory)\n4 to exit \nEnter Choice ";
        cin>>choice;
        if(choice==4)
        break;
        else if(choice==1)
        {
            input_count++;
            cout<<"Enter page number ";
            cin>>page;
            cout<<"Enter offset ";
            cin>>offset;
            if(offset>offset_max)
            {
                cout<<"Offset is too large"<<endl;
                input_count--;
                continue;
            }
            else if(page>=virtual_page_count)
            {
                cout<<"Page does not exist"<<endl;
                input_count--;
                continue;
            }
            flag=0;
            if(pages[page]!=-1)
            flag=1;
            if(flag==1)  //Searching for the required frame which contains the entered page
            {
                for(int i=0;i<physical_page_count;i++)
                {
                    if(frames[i]==page)
                    {
                        frame_no=i;
                        break;
                    }
                }
                cout<<"Page "<<page<<" already present in frame "<<frame_no<<endl;
                cout<<"The physical address for this virtual address is frame "<<frame_no<<" with offset "<<offset<<endl;
            }
            else if(q.size()>=physical_page_count)
            {
                page_fault_count++;
                q.push_back(page); //Inserting new page in the list
                x=*q.begin(); //This page will be replaced
                for(int i=0;i<physical_page_count;i++) //Search for the frame number in which new page will be inserted
                {
                    if(frames[i]==x)
                    {
                        frame_no=i;
                        frames[i]=page;
                        break;
                    }
                }
                pages[page]=frame_no;
                cout<<"Page "<<page<<" did not exist in physical memory so it is accomodated in frame "<<frame_no<<" after removing page "<<x<<endl;
                cout<<"The physical address for this virtual address is frame "<<frame_no<<" with offset "<<offset<<endl;
                q.erase(q.begin()); //Removing the page
                pages[x]=-1;
            }
            else //If a frame is empty
            {
                page_fault_count++;
                for(int i=0;i<physical_page_count;i++)
                {
                    if(frames[i]==-1)
                    {
                        frame_no=i;
                        frames[i]=page;
                        break;
                    }
                }
                pages[page]=frame_no;
                q.push_back(page);
                cout<<"Page "<<page<<" did not exist in physical memory so it is accomodated in frame "<<frame_no<<endl;
                cout<<"The physical address for this virtual address is frame "<<frame_no<<" with offset "<<offset<<endl;
            }
            // for(auto a=q.begin();a!=q.end();a++)
            // cout<<*a<<" ";
            // cout<<endl;
        }
        else if(choice==2)
        {
            cout<<"The state of page table is "<<endl;
            for(int i=0;i<virtual_page_count;i++)
            {
                cout<<"P"<<i<<"   "<<pages[i]<<endl;;
            }
        }
        else if(choice==3)
        {
            cout<<"The state of frame table is "<<endl;
            for(int i=0;i<physical_page_count;i++)
            {
                cout<<"F"<<i<<"   "<<frames[i]<<endl;
            }
        }
        else cout<<"Wrong Choice"<<endl;
    }
    //Printing number of page faults and page fault ratio
    cout<<"Number of page faults that occured are "<<page_fault_count<<endl;
    double page_fault_ratio = (double)page_fault_count/input_count;
    cout<<"The page fault ratio is "<<page_fault_ratio<<endl;
}




void mft_bestfit(){
    cout<<"\nFIXED PARTITIONING- BEST FIT";
    //INPUT PARTITIONS
    int total_mem,n;
    printf("\nEnter total space in main memory: ");
    scanf("%d",&total_mem);
    printf("Enter number of partitions: ");
    scanf("%d",&n);
    int used_up=0,max_part=0;
    int part_size[n],pstart[n],prip[n],in_frag[n];
    //used_up=total space taken up by partitions, max_part=max partition size
    //part_size=partition size, pstart=Where the partition starts 
    //prip=which process in which partition, in_frag=internal fragmentation in the partition
    for(int i=0;i<n;i++){
        printf("Enter size of partition %d: ", i);
        scanf("%d",&part_size[i]);
        prip[i]=-1;
        in_frag[i]=0;
        used_up+=part_size[i];
        if(used_up<=total_mem){
            pstart[i]=used_up-part_size[i];
            if(part_size[i]>max_part) max_part=part_size[i];
        }
        else{
            pstart[i]=-10000;
            prip[i]=-10000;
            used_up-=part_size[i];
            printf("\tPartition %d cannot be made as only %d space is remaining.\n",i,total_mem-used_up);
        }
    }    
    if(used_up!=total_mem) printf("\nUnused space: %d\n",total_mem-used_up);
    //ASSIGN PARTITIONS
    int choice=1,pid=0,process[100];
    int i,best,del,ex_frag;
    while(choice!=0){
        printf("\n1. Insert\t2.Delete\t0.Stop\nEnter choice: ");
        scanf("%d",&choice);
        switch(choice){
            case 1: //INSERTION USING BEST FIT
                printf("Enter size of process %d: ",pid);
                scanf("%d",&process[pid]);
                if(process[pid]>max_part){
                    printf("\tNo partition can accomodate this process.\n");
                    break;
                }
                //best fit algo
                best=-1;
                for(i=0;i<n;i++){
                    if(prip[i]!=-1) continue;
                    if(part_size[i]<process[pid]) continue;
                    if(best==-1 || part_size[i]<part_size[best]) best=i;
                }
                if(best!=-1){
                    prip[best]=pid;
                    in_frag[best]=part_size[best]-process[pid];
                    printf("\tProcess %d accomodated in partition %d.\n\tInternal fragmentation in this partition is %d.\n",pid,best,in_frag[best]);
                }
                else{
                    printf("\tProcess %d cannot be accomodated ",pid);
                    //internal or external fragmentation condition
                    ex_frag=0;
                    for(i=0;i<n;i++){
                        if(in_frag[i]>=process[pid]){
                            printf("due to internal fragmentation.\n");
                            break;
                        }
                        ex_frag+=in_frag[i];
                    }
                    if(i==n && ex_frag>=process[pid]) printf("due to external fragmentation.\n");
                    if(i==n && ex_frag<process[pid]) printf("due to no availability of partition.\n");
                }
                pid++;
                break;
            case 2: //DELETION
                printf("Enter pid of process to be deleted: ");
                scanf("%d",&del);
                for(i=0;i<n;i++){
                    if(prip[i]==del){
                        printf("\tProcess %d removed from partition %d.\n",del,i);
                        prip[i]=-1;
                        in_frag[i]=0;
                        break;
                    }
                }
                if(i==n) printf("\tProcess %d is not present in any partition.\n",del);
                break;
            case 0: 
                break;
            default: 
                printf("Invalid choice.\n"); 
                break;
        }
    }
    printf("\nStatus of the partitions:\n");
    for(i=0;i<n;i++){
        if(prip[i]>=0) printf("\tPartition %d contains process %d of size %d.\n",i,prip[i],process[prip[i]]);
        else if(prip[i]==-1) printf("\tPartition %d is empty.\n",i);
        else printf("\tPartition %d wasn't made.\n",i);
    }
}

void mft_firstfit(){
    cout<<"\nFIXED PARTITIONING- FIRST FIT";
    //INPUT PARTITIONS
    int total_mem,n;
    printf("\nEnter total space in main memory: ");
    scanf("%d",&total_mem);
    printf("Enter number of partitions: ");
    scanf("%d",&n);
    int used_up=0,max_part=0;
    int part_size[n],pstart[n],prip[n],in_frag[n];
    //used_up=total space taken up by partitions, max_part=max partition size
    //part_size=partition size, pstart=Where the partition starts 
    //prip=which process in which partition, in_frag=internal fragmentation in the partition
    for(int i=0;i<n;i++){
        printf("Enter size of partition %d: ", i);
        scanf("%d",&part_size[i]);
        prip[i]=-1;
        in_frag[i]=0;
        used_up+=part_size[i];
        if(used_up<=total_mem){
            pstart[i]=used_up-part_size[i];
            if(part_size[i]>max_part) max_part=part_size[i];
        }
        else{
            pstart[i]=-10000;
            prip[i]=-10000;
            used_up-=part_size[i];
            printf("\tPartition %d cannot be made as only %d space is remaining.\n",i,total_mem-used_up);
        }
    }    
    if(used_up!=total_mem) printf("\nUnused space: %d\n",total_mem-used_up);
    //ASSIGN PARTITIONS
    int choice=1,pid=0,process[100];
    int i,del,ex_frag;
    while(choice!=0){
        printf("\n1. Insert\t2.Delete\t0.Stop\nEnter choice: ");
        scanf("%d",&choice);
        switch(choice){
            case 1: //INSERTION USING FIRST FIT
                printf("Enter size of process %d: ",pid);
                scanf("%d",&process[pid]);
                if(process[pid]>max_part){
                    printf("\tNo partition can accomodate this process.\n");
                    break;
                }
                //first fit algo
                for(i=0;i<n;i++){
                    if(prip[i]!=-1) continue;
                    if(part_size[i]<process[pid]) continue;
                    prip[i]=pid;
                    in_frag[i]=part_size[i]-process[pid];
                    printf("\tProcess %d accomodated in partition %d.\n\tInternal fragmentation in this partition is %d.\n",pid,i,in_frag[i]);
                    break;
                }
                if(i==n){
                    printf("\tProcess %d cannot be accomodated ",pid);
                    //internal or external fragmentation condition
                    ex_frag=0;
                    for(i=0;i<n;i++){
                        if(in_frag[i]>=process[pid]){
                            printf("due to internal fragmentation.\n");
                            break;
                        }
                        ex_frag+=in_frag[i];
                    }
                    if(i==n && ex_frag>=process[pid]) printf("due to external fragmentation.\n");
                    if(i==n && ex_frag<process[pid]) printf("due to no availability of partition.\n");
                }
                pid++;
                break;
            case 2: //DELETION
                printf("Enter pid of process to be deleted: ");
                scanf("%d",&del);
                for(i=0;i<n;i++){
                    if(prip[i]==del){
                        printf("\tProcess %d removed from partition %d.\n",del,i);
                        prip[i]=-1;
                        in_frag[i]=0;
                        break;
                    }
                }
                if(i==n) printf("\tProcess %d is not present in any partition.\n",del);
                break;
            case 0: 
                break;
            default: 
                printf("Invalid choice.\n"); 
                break;
        }
    }
    printf("\nStatus of the partitions:\n");
    for(i=0;i<n;i++){
        if(prip[i]>=0) printf("\tPartition %d contains process %d of size %d.\n",i,prip[i],process[prip[i]]);
        else if(prip[i]==-1) printf("\tPartition %d is empty.\n",i);
        else printf("\tPartition %d wasn't made.\n",i);
    }
}

void mft_worstfit(){
    cout<<"\nFIXED PARTITIONING- WORST FIT";
    //INPUT PARTITIONS
    int total_mem,n;
    printf("\nEnter total space in main memory: ");
    scanf("%d",&total_mem);
    printf("Enter number of partitions: ");
    scanf("%d",&n);
    int used_up=0,max_part=0;
    int part_size[n],pstart[n],prip[n],in_frag[n];
    //used_up=total space taken up by partitions, max_part=max partition size
    //part_size=partition size, pstart=Where the partition starts 
    //prip=which process in which partition, in_frag=internal fragmentation in the partition
    for(int i=0;i<n;i++){
        printf("Enter size of partition %d: ", i);
        scanf("%d",&part_size[i]);
        prip[i]=-1;
        in_frag[i]=0;
        used_up+=part_size[i];
        if(used_up<=total_mem){
            pstart[i]=used_up-part_size[i];
            if(part_size[i]>max_part) max_part=part_size[i];
        }
        else{
            pstart[i]=-10000;
            prip[i]=-10000;
            used_up-=part_size[i];
            printf("\tPartition %d cannot be made as only %d space is remaining.\n",i,total_mem-used_up);
        }
    }    
    if(used_up!=total_mem) printf("\nUnused space: %d\n",total_mem-used_up);
    //ASSIGN PARTITIONS
    int choice=1,pid=0,process[100];
    int i,worst,del,ex_frag;
    while(choice!=0){
        printf("\n1. Insert\t2.Delete\t0.Stop\nEnter choice: ");
        scanf("%d",&choice);
        switch(choice){
            case 1: //INSERTION USING WORST FIT
                printf("Enter size of process %d: ",pid);
                scanf("%d",&process[pid]);
                if(process[pid]>max_part){
                    printf("\tNo partition can accomodate this process.\n");
                    break;
                }
                //worst fit algo
                worst=-1;
                for(i=0;i<n;i++){
                    if(prip[i]!=-1) continue;
                    if(part_size[i]<process[pid]) continue;
                    if(worst==-1 || part_size[i]>part_size[worst]) worst=i;
                }
                if(worst!=-1){
                    prip[worst]=pid;
                    in_frag[worst]=part_size[worst]-process[pid];
                    printf("\tProcess %d accomodated in partition %d.\n\tInternal fragmentation in this partition is %d.\n",pid,worst,in_frag[worst]);
                }
                else{
                    printf("\tProcess %d cannot be accomodated ",pid);
                    //internal or external fragmentation condition
                    ex_frag=0;
                    for(i=0;i<n;i++){
                        if(in_frag[i]>=process[pid]){
                            printf("due to internal fragmentation.\n");
                            break;
                        }
                        ex_frag+=in_frag[i];
                    }
                    if(i==n && ex_frag>=process[pid]) printf("due to external fragmentation.\n");
                    if(i==n && ex_frag<process[pid]) printf("due to no availability of partition.\n");
                }
                pid++;
                break;
            case 2: //DELETION
                printf("Enter pid of process to be deleted: ");
                scanf("%d",&del);
                for(i=0;i<n;i++){
                    if(prip[i]==del){
                        printf("\tProcess %d removed from partition %d.\n",del,i);
                        prip[i]=-1;
                        in_frag[i]=0;
                        break;
                    }
                }
                if(i==n) printf("\tProcess %d is not present in any partition.\n",del);
                break;
            case 0: 
                break;
            default: 
                printf("Invalid choice.\n"); 
                break;
        }
    }
    printf("\nStatus of the partitions:\n");
    for(i=0;i<n;i++){
        if(prip[i]>=0) printf("\tPartition %d contains process %d of size %d.\n",i,prip[i],process[prip[i]]);
        else if(prip[i]==-1) printf("\tPartition %d is empty.\n",i);
        else printf("\tPartition %d wasn't made.\n",i);
    }
}

void mft_fits(){
    int choice=1;
    while(choice!=4)
    {
        cout<<"\nYou have the following choices :-\n 1) Best Fit\n 2) First Fit\n 3) Worst Fit\n 4) Exit\n";
        cout<<"Enter choice ";
        cin>>choice;
        switch (choice)
        {
            case 1: mft_bestfit();
                    break;
            case 2: mft_firstfit();
                    break;
            case 3: mft_worstfit();
                    break;
            case 4: break;
            default: cout<<"Wrong choice"<<endl;
        }
    }
}




void wf_insertProcess(int pid, int s)
{
    mvt_process.push_back(s);
    int index = -1;
    int wst_max = -1; //variable to store the biggest partition that fits the process
    for (int i = 1; i < stat.size(); i++)
    {
        int wst = part[i] - part[i - 1] - s;
        if (stat[i] == -1 && wst > wst_max && part[i] - part[i - 1] >= s) //if the partition is empty and it can accomodate that process
        {
            index = i;
            wst_max = wst;
        }
    }
    if (index != -1)
    {
        if (part[index] - part[index - 1] == s)
        { //if partitionsize=process size
            stat[index] = pid;
            remaining = remaining - s; //updating remaining
        }
        else
        {
            part.insert(part.begin() + index, part[index - 1] + s);
            stat.insert(stat.begin() + index, pid);
            stat[index + 1] = -1;
            remaining = remaining - s;
        }
        cout << "Inserted!\n";
        return;
    }
    else
    {
        //checking external framentation
        if (s <= remaining)
        {
            cout << "Process " << pid << " could not be allocated due to external fragmentation" << endl;
        }
        else
        {
            cout << "Process " << pid << " could not be allocated as space remaining = " << remaining << endl;
        }
    }
    return;
}

//deleting process in worstfit
void wf_deleteProcess(int pid)
{
    for (int i = 1; i < stat.size(); i++)
    {
        if (stat[i] == pid)
        { //if process is found
            stat[i] = -1;
            if (i < stat.size() - 1 && stat[i + 1] == -1)
            {
                stat.erase(stat.begin() + i);
                part.erase(part.begin() + i);
            }
            if (i != 0 && stat[i - 1] == -1)
            {
                stat.erase(stat.begin() + i - 1);
                part.erase(part.begin() + i - 1);
            }
            remaining = remaining + mvt_process[i - 1];
            cout << "Deleted!" << endl;
            return;
        }
    }
    cout << "Process " << pid << " has ended/isn't allocated and therfore, could not be deleted!" << endl;
}
//printing worstfit allocation
void wf_printing()
{
    cout << "\nFinal allocations: " << endl;
    cout << "\nPartition   Size   Process" << endl;
    for (int i = 1; i < stat.size(); i++)
    {
        if (stat[i] == -1)
        {
            printf("  %-8d   %-4d   empty\n", i, part[i] - part[i - 1]);
        }
        else
        {
            printf("  %-8d   %-4d   %-8d\n", i, part[i] - part[i - 1], stat[i]);
        }
    }
}
//inserting in bestfit
void bf_insertProcess(int pid, int s)
{
    mvt_process.push_back(s);
    int index = -1;
    int wst_min = tot_size;
    for (int i = 1; i < stat.size(); i++)
    {
        int wst = part[i] - part[i - 1] - s;
        if (stat[i] == -1 && wst < wst_min && part[i] - part[i - 1] >= s)
        {
            index = i;
            wst_min = wst;
        }
    }
    if (index != -1)
    {
        if (part[index] - part[index - 1] == s)
        {
            stat[index] = pid;
            remaining = remaining - s;
        }
        else
        {
            part.insert(part.begin() + index, part[index - 1] + s);
            stat.insert(stat.begin() + index, pid);
            stat[index + 1] = -1;
            remaining = remaining - s;
        }
        cout << "Inserted!\n";
        return;
    }
    else
    {
        if (s <= remaining)
        {
            cout << "Process " << pid << " could not be allocated due to external fragmentation" << endl;
        }
        else
        {
            cout << "Process " << pid << " could not be allocated as space remaining = " << remaining << endl;
        }
    }
    return;
}
//deleting process in bestfit
void bf_deleteProcess(int pid)
{
    for (int i = 1; i < stat.size(); i++)
    {
        if (stat[i] == pid)
        {
            stat[i] = -1;
            if (i < stat.size() - 1 && stat[i + 1] == -1)
            {
                stat.erase(stat.begin() + i);
                part.erase(part.begin() + i);
            }
            if (i != 0 && stat[i - 1] == -1)
            {
                stat.erase(stat.begin() + i - 1);
                part.erase(part.begin() + i - 1);
            }
            remaining = remaining + mvt_process[i - 1];
            cout << "Deleted!" << endl;
            return;
        }
    }
    cout << "Process " << pid << " has ended/isn't allocated and therfore, could not be deleted!" << endl;
}
//printing bestfit allocation
void bf_printing()
{
    cout << "\nFinal allocations: " << endl;
    cout << "\nPartition   Size   Process" << endl;
    for (int i = 1; i < stat.size(); i++)
    {
        if (stat[i] == -1)
        {
            printf("  %-8d   %-4d   empty\n", i, part[i] - part[i - 1]);
        }
        else
        {
            printf("  %-8d   %-4d   %-8d\n", i, part[i] - part[i - 1], stat[i]);
        }
    }
}

//insering process into firstfit
void ff_insertProcess(int pid, int s)
{
    mvt_process.push_back(s);
    for (int i = 1; i < stat.size(); i++)
    {
        if (stat[i] == -1 && (part[i] - part[i - 1]) >= s) //if the partition is empty and it can accomodate that process
        {
            if (part[i] - part[i - 1] == s) // if the partition size= process size
            {
                stat[i] = pid;             //updating status of partition
                remaining = remaining - s; //updating remaining size of partition
            }
            else
            {
                part.insert(part.begin() + i, part[i - 1] + s);
                stat.insert(stat.begin() + i, pid);
                stat[i + 1] = -1;
                remaining = remaining - s;
            }
            cout << "Inserted!\n";
            //cout<<remaining<<endl;
            return;
        }
    }
    //condition for external fragmentation
    if (s <= remaining)
    {
        cout << "Process " << pid << " could not be allocated due to external fragmentation" << endl;
        //condition when process cant be accomodated in the memory due to less space
    }
    else
    {
        cout << "Process " << pid << " could not be allocated as space remaining = " << remaining << endl;
    }
    return;
}
//deleting firstfit process
void ff_deleteProcess(int pid)
{
    for (int i = 1; i < stat.size(); i++)
    {
        if (stat[i] == pid) // if process is found
        {
            stat[i] = -1; //updating index
            if (i < stat.size() - 1 && stat[i + 1] == -1)
            {
                stat.erase(stat.begin() + i); //removing process
                part.erase(part.begin() + i);
            }
            if (stat[i - 1] == -1)
            {
                stat.erase(stat.begin() + i - 1);
                part.erase(part.begin() + i - 1);
            }
            remaining = remaining + mvt_process[i - 1]; //updating remaining
            cout << "Deleted!" << endl;
            return;
        }
    }
    cout << "Process " << pid << " has ended/isn't allocated and therfore, could not be deleted!" << endl;
}

//printing firstfit
void ff_printing()
{
    cout << "\nFinal allocations: " << endl;
    cout << "\nPartition   Size   Process" << endl;
    for (int i = 1; i < stat.size(); i++)
    {
        if (stat[i] == -1)
        {
            printf("  %-8d   %-4d   empty\n", i, part[i] - part[i - 1]);
        }
        else
        {
            printf("  %-8d   %-4d   %-8d\n", i, part[i] - part[i - 1], stat[i]);
        }
    }
}




void MVT_allocation()
{
    int mem_alloc_option;
    cout << "\nYou have the following choices:\n 1) First Fit \n 2) Best Fit \n 3) Worst Fit\nEnter your choice ";
    cin >> mem_alloc_option;
    switch (mem_alloc_option)
    {
    case 1:
    {
        cout << "VARIABLE PARTITIONING - FIRST FIT" << endl;
        cout << "Enter total size: ";
        cin >> tot_size;
        remaining = tot_size;
        part.push_back(0);
        part.push_back(tot_size);
        stat.push_back(0);
        stat.push_back(-1);

        int choice;
        do
        {
            cout << "\nYou have the following choices: \n 1) Insert process\n 2) Delete process\n 3) Exit\nEnter your choice ";
            cin >> choice;
            switch (choice)
            {
            case 1:
                int size;
                cout << "Process ID: " << mvt_process.size() + 1 << endl;
                cout << "Enter it's size: ";
                cin >> size;
                ff_insertProcess(mvt_process.size() + 1, size);
                break;
            case 2:
                int pid;
                cout << "Enter ID of the process to be deleted: ";
                cin >> pid;
                ff_deleteProcess(pid);
                break;
            case 3:
                break;
            default:
                cout << "Invalid choice!" << endl;
            }
        } while (choice != 3);
        ff_printing();
        break;
    }

    case 3:
    {
        cout << "VARIABLE PARTITIONING - WORST FIT" << endl;
        cout << "Enter total size: ";
        cin >> tot_size;
        remaining = tot_size;
        part.push_back(0);
        part.push_back(tot_size);
        stat.push_back(0);
        stat.push_back(-1);

        int choice1;
        do
        {
            cout << "\nYou have the following choices: \n 1) Insert process\n 2) Delete process\n 3) Exit\nEnter your choice ";
            cin >> choice1;
            switch (choice1)
            {
            case 1:
                int size;
                cout << "Process ID: " << mvt_process.size() + 1 << endl;
                cout << "Enter it's size: ";
                cin >> size;
                wf_insertProcess(mvt_process.size() + 1, size);
                break;
            case 2:
                int pid;
                cout << "Enter ID of the process to be deleted: ";
                cin >> pid;
                wf_deleteProcess(pid);
                break;
            case 3:
                break;
            default:
                cout << "Invalid choice!" << endl;
            }

        } while (choice1 != 3);
        wf_printing();
        break;
    }

    case 2:
    {
        cout << "VARIABLE PARTITIONING - BEST FIT" << endl;
        cout << "Enter total size: ";
        cin >> tot_size;
        remaining = tot_size;
        part.push_back(0);
        part.push_back(tot_size);
        stat.push_back(0);
        stat.push_back(-1);

        int choice2;
        do
        {
            cout << "\nYou have the following choices: \n 1) Insert process\n 2) Delete process\n 3) Exit\nEnter your choice ";
            cin >> choice2;
            switch (choice2)
            {
            case 1:
                int size;
                cout << "Process ID: " << mvt_process.size() + 1 << endl;
                cout << "Enter it's size: ";
                cin >> size;
                bf_insertProcess(mvt_process.size() + 1, size);
                break;
            case 2:
                int pid;
                cout << "Enter ID of the process to be deleted: ";
                cin >> pid;
                bf_deleteProcess(pid);
                break;
            case 3:
                break;
            default:
                cout << "Invalid choice!" << endl;
            }

        } while (choice2 != 3);
        bf_printing();
    }
    }
}



//Main driving Code
int main()
{
    int choice = 1;
    while(choice!=9){
        //system("CLS");
        cout<<"\nYou have the following choices:\n 1) Synchronization\n 2) Disk Scheduling\n 3) Page replacement\n 4) Banker's Algo\n 5) CPU Scheduling\n 6) Paging\n 7) MVT Fits\n 8) MFT Fits\n 9) Exit"<<endl;
        cout<<"Enter choice ";
        cin>>choice;
        switch (choice)
        {
        case 1: synchronization();
            break;
        case 2 : DiskSchedulingCallerFunction();
                break;
        case 3 : pageReplacement();
                break;
        case 4 : bankersAlgo();
                break;
        case 5 : process_sch_caller();
                break;
        case 6 : paging(); 
                break;
        case 7 : MVT_allocation();
                break;
        case 8 : mft_fits();
                break;
        case 9 : cout<<"Thank you for your time"<<endl;
                break;
        default:
            break;
        }

    }

    return 0;
}