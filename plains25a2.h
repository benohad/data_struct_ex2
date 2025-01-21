// 
// 234218 Data Structures 1.
// Semester: 2025A (Winter).
// Wet Exercise #1.
// 
// The following header file contains all methods we expect you to implement.
// You MAY add private methods and fields of your own.
// DO NOT erase or modify the signatures of the public methods.
// DO NOT modify the preprocessors in this file.
// DO NOT use the preprocessors in your other code files.
// 

#ifndef PLAINS25WINTER_WET1_H_
#define PLAINS25WINTER_WET1_H_

#include "wet2util.h"
#include "HashTable.h"
#include "UnionFind.h"

class Plains {
private:
    //
    // Here you may add anything you want
    //

    struct gameNode{
        int m_Id;
        int m_record;
        int location;
       explicit gameNode(int id): m_Id(id), m_record(0),location(-1){}
    };

    HashTable <gameNode> jockeyTable;
    HashTable <gameNode> teamTable;
    UnionFind <gameNode> gameUnion;
    //key is the record , value: is the team id  to the team node
    HashTable<int> teamRecord;


public:
    // <DO-NOT-MODIFY> {
    Plains();

    ~Plains();

    StatusType add_team(int teamId);

    StatusType add_jockey(int jockeyId, int teamId);

    StatusType update_match(int victoriousJockeyId, int losingJockeyId);

    StatusType merge_teams(int teamId1, int teamId2);

    StatusType unite_by_record(int record);

    output_t<int> get_jockey_record(int jockeyId);

    output_t<int> get_team_record(int teamId);
    // } </DO-NOT-MODIFY>
};

#endif // PLAINS25WINTER_WET1_H_
