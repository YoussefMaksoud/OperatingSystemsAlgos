// this is the ONLY file you should edit and submit to D2L

#include "deadlock_detector.h"
#include "common.h"
#include <string.h>
#include <unordered_map>
#include <vector>
#include <iostream>

using namespace std;

/// this is the function you need to implement
/// parameter edges[] contains a list of request and assignment edges
///   example of a request edge, process "p1" resource "r1"
///     "p1 -> r1"
///   example of an assignment edge, process "XYz" resource "XYz"
///     "XYz <- XYz"
/// Parameters edge_index and cycle[] are used to return
/// results back to the caller.
///
/// You need to process edges[] one edge at a time, and run a deadlock
/// detection after each edge.
///
/// As soon as you detecte a deadlock, you need to set edge_index to the index of the edge
/// in edges[] that caused the deadlock. For example, edges[7] caused the deadlock,
/// then set edge_index=7. You must also populete cycle[] with the names of
/// processes that are in a deadlock. You can then return from the function without
/// processing any other edges.
///
/// To indicate deadlock was detected after processing all edges, you must
/// set edge_index = -1 and clear cycle[] e.g. by calling cycle.clear()
///

class Graph {
private:
    unordered_map<string, vector<string>> adj_list;
    unordered_map<string, int> out_counts;

public:
    unordered_map<string, vector<string>> & getAdjList() { return adj_list; }
    unordered_map<string, int> & getOutCounts() { return out_counts; }
};

void detect_deadlock(
    const std::vector<std::string> & edges, int & edge_index, std::vector<std::string> & cycle)
{
    Graph graph;
    cycle.clear();

    edge_index = -1;

    for (auto it = edges.begin(); it != edges.end(); it++) {

        vector<string> spl = split(*it);
        spl[0] += 'P';
        spl[2] += 'R';

        if (spl[1] == "<-") { // assignment
            graph.getAdjList()[spl[0]].push_back(spl[2]);
            graph.getAdjList()[spl[2]];
            graph.getOutCounts()[spl[2]] ++;
            graph.getOutCounts()[spl[0]];
        }
        else if (spl[1] == "->") { // request
            graph.getAdjList()[spl[2]].push_back(spl[0]);
            graph.getAdjList()[spl[0]];
            graph.getOutCounts()[spl[0]] ++;
            graph.getOutCounts()[spl[2]];
        }

        unordered_map<string, int> out = graph.getOutCounts();
        vector<string> zeros;

        for (auto it = out.begin(); it != out.end(); it++) {
            if (it->second == 0) {
                zeros.push_back(it->first);
            }
        }

        while (zeros.size() > 0) {
            string n = zeros.back();
            zeros.pop_back();
            for (auto n2 = graph.getAdjList()[n].begin(); n2 != graph.getAdjList()[n].end(); n2++) {
                out[*n2]--;
                if (out[*n2] == 0) {
                    zeros.push_back(*n2);
                }
            }   
        }
        
        for(auto n = out.begin(); n != out.end(); n++){
            if(n->second > 0 && n->first.at(n->first.length() - 1) == 'P'){
                cycle.push_back(n->first.substr(0, n->first.length() - 1));
            }
        }

        edge_index ++;
        if(cycle.size() > 1){
            return;
        }     
    }



    cycle.clear();
    edge_index = -1;
    return;
}
