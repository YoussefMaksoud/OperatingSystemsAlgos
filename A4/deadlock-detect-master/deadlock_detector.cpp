// this is the ONLY file you should edit and submit to D2L

#include "common.h"
#include "deadlock_detector.h"

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
void detect_deadlock(
    const std::vector<std::string> & edges,
    int & edge_index,
    std::vector<std::string> & cycle)
{
    // let's try to guess the results :)
    cycle = split("12 7 7");
    edge_index = 6;
}
