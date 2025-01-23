// You can edit anything you want in this file.
// However you need to implement all public Plains function, as provided below as a template

#include "plains25a2.h"
#include <iostream>
using namespace std;

Plains::Plains(): jockeyTable(), teamTable(), gameUnion(),teamRecord(),teamRecordNeg(){}

Plains::~Plains() = default;

StatusType Plains::add_team(int teamId)
{
    // check if the input is valid
    if(teamId<=0) return StatusType::INVALID_INPUT;
    // check if the team already exists
    if(teamTable.search(teamId) != nullptr) return StatusType::FAILURE;
    // data in our case is the record of the team
    // insert the team to the table
    gameNode teamNode(teamId);
    StatusType status = teamTable.insert(teamId,teamNode);

    // update the team location on the hash table array
   teamTable.search(teamId)->data.m_location=gameUnion.m_counter;

    // insert the team to the union find
   if (gameUnion.makeSet(gameNode(teamId))==nullptr) return StatusType::ALLOCATION_ERROR;
    return status;
}

StatusType Plains::add_jockey(int jockeyId, int teamId)
{
    //validity check
    if(jockeyId<=0 || teamId<=0) return StatusType::INVALID_INPUT;
    // check if the jockey or team are already exists
    if(jockeyTable.search(jockeyId) != nullptr||teamTable.search(teamId) == nullptr )
        return StatusType::FAILURE;
    // insert the jockey to the table data is record
    gameNode jockeyNode(jockeyId);
    try {
    jockeyTable.insert(jockeyId,jockeyNode);
    } catch (std::bad_alloc&) {
        return StatusType::ALLOCATION_ERROR;
    }
  // update the jockey location on the hash table array
    jockeyTable.search(jockeyId)->data.m_location=gameUnion.m_counter;
    // insert the jockey to the union find -jockey is a game node.
    if (gameUnion.makeSet(gameNode(jockeyId))==nullptr) return StatusType::ALLOCATION_ERROR;

    //update the parent of the jockey to be the team
    gameUnion.m_dynamicArray[jockeyTable.search(jockeyId)->data.m_location]->parent =
            gameUnion.m_dynamicArray[teamTable.search(teamId)->data.m_location];

    return StatusType::SUCCESS;
}

StatusType Plains::update_match(int victoriousJockeyId, int losingJockeyId)
{
    //validity check
    if(victoriousJockeyId<=0 || losingJockeyId<=0|| victoriousJockeyId==losingJockeyId)
        return StatusType::INVALID_INPUT;
    // check if the jockeys are in the table
    auto victoriousJockey = jockeyTable.search(victoriousJockeyId);
    auto losingJockey = jockeyTable.search(losingJockeyId);
    if(victoriousJockey == nullptr || losingJockey == nullptr)
        return StatusType::FAILURE;
    // check if the jockeys are in the same team
    auto victoriousTeam =
            gameUnion.find(gameUnion.m_dynamicArray[victoriousJockey->data.m_location]);
    auto losingTeam =
            gameUnion.find(gameUnion.m_dynamicArray[losingJockey->data.m_location]);

    int victoriousTeamId = victoriousTeam->data.m_Id;
    int losingTeamId = losingTeam->data.m_Id;

    if(victoriousTeam == losingTeam)
        return StatusType::FAILURE;
    // End of checking validity


    // delete the records of the jockeys from the team record hash table
    int victoriousTeamRecord = victoriousTeam->data.m_record;
    int losingTeamRecord = losingTeam->data.m_record;
    if(victoriousTeamRecord>=0) {
        teamRecord.remove(victoriousTeamRecord, victoriousTeamId);
       // std::cout<<"------- flag 1, winning record is: "<< victoriousTeam->data.m_record<< std::endl;
    }
    else {
        teamRecordNeg.remove(-victoriousTeamRecord, victoriousTeamId);
       // std::cout<<" flag 2"<<std::endl;
    }
    if(losingTeamRecord>=0) {
        teamRecord.remove(losingTeamRecord, losingTeamId);
      //  std::cout<<" ------flag 3, the record is: "<< losingTeam->data.m_record<< std::endl;
    }
    else {
        teamRecordNeg.remove(-losingTeamRecord, losingTeamId);
       // std::cout<<" flag 4"<<std::endl;
    }

    // update the records of the jockeys
    victoriousJockey->data.m_record++;
    losingJockey->data.m_record--;

    //update the record of the union find nodes of the team
    victoriousTeam->data.m_record++;
    losingTeam->data.m_record--;




    //update the team records in the team hash table+ // update the team record hash table
    teamTable.search(victoriousTeamId)->data.m_record++;
    victoriousTeamRecord = teamTable.search(victoriousTeamId)->data.m_record;
   // std::cout<<"-------victorious team id: "<<victoriousTeamId<<std::endl;
 //   std::cout<<"-------victorious team record: "<<victoriousTeamRecord<<std::endl;

    teamTable.search(losingTeamId)->data.m_record--;
    losingTeamRecord = teamTable.search(losingTeamId)->data.m_record;
 //   std::cout<<"-------losing team id: "<<losingTeamId<<std::endl;
  //  std::cout<<"-------losing team record: "<<losingTeamRecord<<std::endl;

    if(victoriousTeamRecord>=0) {
        teamRecord.insert(victoriousTeamRecord, victoriousTeamId);
    }
    else {
        teamRecordNeg.insert(-victoriousTeamRecord, victoriousTeamId);
    }
  //  teamRecord.insert(victoriousTeam->data.m_record,victoriousTeam->data.m_Id);
    if(losingTeamRecord>=0) {
        teamRecord.insert(losingTeamRecord, losingTeamId);
    }
    else {
        teamRecordNeg.insert(-losingTeamRecord, losingTeamId);

    }
    //teamRecord.insert(losingTeam->data.m_record,losingTeam->data.m_Id);

    return StatusType::SUCCESS;
}

StatusType Plains::merge_teams(int teamId1, int teamId2)
{
    //validity check
    if(teamId1<=0 || teamId2<=0 || teamId1==teamId2)
        return StatusType::INVALID_INPUT;
    // check if the teams are in the table
    auto team1 = teamTable.search(teamId1);
    auto team2 = teamTable.search(teamId2);
    if(team1 == nullptr || team2 == nullptr)
        return StatusType::FAILURE;

// the roots of the teams
    auto gameNode1 = gameUnion.m_dynamicArray[team1->data.m_location];
    auto gameNode2 = gameUnion.m_dynamicArray[team2->data.m_location];

    // the id of the team with the bigger record will be the id of the new team
    int newTeamId = team1->data.m_record>team2->data.m_record?team1->data.m_Id:team2->data.m_Id;

    // delete the records of the teams from the team record hash table
        if (team2->data.m_record >= 0)
            teamRecord.remove(team2->data.m_record, team2->data.m_Id);
        else
            teamRecordNeg.remove(-(team2->data.m_record), team2->data.m_Id);
        //teamRecord.remove(team2->data.m_record,team2->data.m_Id);
        if (team1->data.m_record >= 0)
            teamRecord.remove(team1->data.m_record, team1->data.m_Id);
        else
            teamRecordNeg.remove(-(team1->data.m_record), team1->data.m_Id);

      //  teamRecord.remove(team1->data.m_record,team1->data.m_Id);
    // if they have the same record the id of the first team will be the new team id
    if(team1->data.m_record==team2->data.m_record)
        newTeamId = team1->data.m_Id;

    //unite the teams
    auto new_root = (gameUnion.unionSets(gameNode1,gameNode2));
    // update the new_root id to be the new team id
    new_root->data.m_Id = newTeamId;




    // update the new team record
    new_root->data.m_record = team1->data.m_record+team2->data.m_record;

    // update the team record hash table
    if(new_root->data.m_record>=0) {
        teamRecord.insert(new_root->data.m_record, newTeamId);
    }
    else {
        teamRecordNeg.insert(-new_root->data.m_record, newTeamId);
    }

    teamTable.search(newTeamId)->data.m_record = new_root->data.m_record;



    return StatusType::SUCCESS;
}

StatusType Plains::unite_by_record(int record)
{
    //validity check
    if(record<=0)
        return StatusType::INVALID_INPUT;
    auto team1 = teamRecord.search(record);
    auto team2 = teamRecordNeg.search(record);
    if(team1 == nullptr || team2 == nullptr) {
        return StatusType::FAILURE;
    }
    if(team1->next||team2->next) {
     //   std::cout<<"more than one team with the same record"<<std::endl;
        return StatusType::FAILURE;
    }
    int teamId1 = team1->data;
    int teamId2 = team2->data;

    //now merge the teams
    return merge_teams(teamId1,teamId2);


}

output_t<int> Plains::get_jockey_record(int jockeyId)
{
    //validity check
    if(jockeyId<=0)
        return StatusType::INVALID_INPUT;
    // check if the jockey is in the table
    auto jockey = jockeyTable.search(jockeyId);
    if(jockey == nullptr)
        return StatusType::FAILURE;
     int record = jockey->data.m_record;
    return output_t<int>(record);
}

output_t<int> Plains::get_team_record(int teamId)
{
    //validity check
    if(teamId<=0)
        return StatusType::INVALID_INPUT;
    // check if the team is in the table
    auto team = teamTable.search(teamId);
    if(team == nullptr)
        return StatusType::FAILURE;
    int record = team->data.m_record;
    return output_t<int>(record);

}
