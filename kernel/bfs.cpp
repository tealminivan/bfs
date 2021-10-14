#include <cassert>
#include <iostream>
#include "csr.h"
#include <list>
#include <vector>
#include <omp.h>
#include <string.h>

using std::cout;
using std::endl;


void run_bfs(graph_t& g, vid_t root)
{
    csr_t* csr = &g.csr;
    csr_t* csc = &g.csc;
    vid_t* offset_list = csr-> offset;
    vid_t* nebrs_list = csr-> nebrs;

    bool* visited = new bool[csr->v_count];
    for (int i = 0; i < csr->v_count - 1; i++){
        visited[i] = false;
    } 

    int level[csr->v_count];
    list<int> queue;
    visited[root] = true;
    level[root] = 0;
    queue.push_back(root);

    while (!queue.empty()){
        int currVertex = queue.front();
        queue.pop_front();

        for( int neighbor=offset_list[currVertex]; neighbor<offset_list[currVertex+1]; neighbor++){
            if (!visited[nebrs_list[neighbor]]){
                visited[nebrs_list[neighbor]] = true;
                queue.push_back(nebrs_list[neighbor]);
                level[nebrs_list[neighbor]] = level[currVertex]+1;
            }
        }
    }

    
    vector<vector<int>>prntList(csr->v_count);
    for(int i=0; i<csr->v_count; i++){
        prntList[level[i]].push_back(i);
    }
    for (int i=0; i<csr->v_count; i++){
        if (prntList[i].size() != 0){
            cout << "level" << i << ": "<< prntList[i].size()<<" verticies"<<endl;
        }
    }


    // code to print out nodes 
    // for (int i=0; i<csr->v_count; i++){
    //     if (prntList[i].size() != 0){
    //         cout << "level" << i << ": "<< prntList[i].size()<<" verticies  Nodes: [";
    //         for (int j=0; j<prntList[i].size(); j++){
    //             if (j == prntList[i].size()-1){
    //                 cout << prntList[i][j] <<"";
    //             }
    //             else{
    //             cout << prntList[i][j] <<", ";
    //             }
    //         }
    //         cout << "]" << endl;
    //     }
    // }
}


void run_bfs_parallel(graph_t& g, vid_t root)
{
    csr_t* csr = &g.csr;
    csr_t* csc = &g.csc;
    vid_t* offset_list = csr-> offset;
    vid_t* nebrs_list = csr-> nebrs;
    
    bool* visited = new bool[csr->v_count];
    for (int i = 0; i < csr->v_count; i++){
        visited[i] = false;
    } 
    int level[csr->v_count];
    int queue[csr->v_count][csr->v_count];
    int list[csr->v_count];


    visited[root] = true;
    level[root] = 0;
    queue[0][0] = root;
    list[0]=1;
    int currQueue = 0;
    int currVertex;
    int next_queue;
    while(1){
        next_queue=(currQueue+1)%2;
        list[next_queue]=0;
        omp_set_dynamic(0);
        #pragma omp parallel private(currVertex)
        {
            #pragma omp for
                for(int i=0; i<list[currQueue]; i++){
                    currVertex = queue[currQueue][i];
                    for( int neighbor=offset_list[currVertex]; neighbor<offset_list[currVertex+1]; neighbor++){
                        if(!visited[nebrs_list[neighbor]]){
                            visited[nebrs_list[neighbor]] = true;
                            level[nebrs_list[neighbor]] = level[currVertex]+1;
                            #pragma omp critical
                                queue[next_queue][list[next_queue]++] = nebrs_list[neighbor];
                        }
                    }    
                }
        }
        if(list[next_queue]==0)
            break;
        else  
            currQueue = next_queue;
    }


    vector<vector<int>>prntList(csr->v_count);
    for(int i=0; i<csr->v_count; i++){
        prntList[level[i]].push_back(i);
    }
    for (int i=0; i<csr->v_count; i++){
        if (prntList[i].size() != 0){
            cout << "level" << i << ": "<< prntList[i].size()<<" verticies"<<endl;
        }
    }

    //code to print out nodes 
    // for (int i=0; i<csr->v_count; i++){
    //     if (prntList[i].size() != 0){
    //         cout << "level" << i << ": "<< prntList[i].size()<<" verticies  Nodes: [";
    //         for (int j=0; j<prntList[i].size(); j++){
    //             if (j == prntList[i].size()-1){
    //                 cout << prntList[i][j] <<"";
    //             }
    //             else{
    //             cout << prntList[i][j] <<", ";
    //             }
    //         }
    //         cout << "]" << endl;
    //     }
    // }

}



