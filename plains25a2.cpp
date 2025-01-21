// You can edit anything you want in this file.
// However you need to implement all public Plains function, as provided below as a template

#include "plains25a2.h"


Plains::Plains(): jockeyTable(), teamTable(), gameUnion(),teamRecord(){}

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
    teamNode.location = gameUnion.m_counter;
    // insert the team to the union find
   if (gameUnion.makeSet(gameNode(teamId))==nullptr) return StatusType::ALLOCATION_ERROR;
    return status;
}

StatusType Plains::add_jockey(int jockeyId, int teamId)
{
    //validity check
    if(jockeyId<=0 || teamId<=0) return StatusType::INVALID_INPUT;
    // check if the jockey or team are already exists
    if(jockeyTable.search(jockeyId) != nullptr||teamTable.search(teamId) != nullptr )
        return StatusType::FAILURE;
    // insert the jockey to the table data is record
    gameNode jockeyNode(jockeyId);
    try {
    jockeyTable.insert(jockeyId,jockeyNode);
    } catch (std::bad_alloc&) {
        return StatusType::ALLOCATION_ERROR;
    }
    jockeyNode.location = gameUnion.m_counter;
    // insert the jockey to the union find -jockey is a game node.
    if (gameUnion.makeSet(gameNode(jockeyId))==nullptr) return StatusType::ALLOCATION_ERROR;

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
    auto victoriousTeam = teamTable.search(victoriousJockey->data.m_Id);
    auto losingTeam = teamTable.search(losingJockey->data.m_Id);
    if(victoriousTeam == losingTeam)
        return StatusType::FAILURE;

    // delete the records of the jockeys from the team record hash table
    teamRecord.remove(victoriousTeam->data.m_record,victoriousTeam->data.m_Id);
    teamRecord.remove(losingTeam->data.m_record,losingTeam->data.m_Id);

    // update the records of the jockeys
    victoriousJockey->data.m_record++;
    losingJockey->data.m_record--;

    //update the team records
    victoriousTeam->data.m_record++;
    losingTeam->data.m_record--;

    // update the team record hash table
    teamRecord.insert(victoriousTeam->data.m_record,victoriousTeam->data.m_Id);
    teamRecord.insert(losingTeam->data.m_record,losingTeam->data.m_Id);

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
    auto gameNode1 = gameUnion.m_dynamicArray[team1->data.location];
    auto gameNode2 = gameUnion.m_dynamicArray[team2->data.location];

    // the id of the team with the bigger record will be the id of the new team
    int newTeamId = team1->data.m_record>team2->data.m_record?team1->data.m_Id:team2->data.m_Id;

    // delete the less recorded team from the team record hash table
    if(newTeamId==team1->data.m_Id)
        teamRecord.remove(team2->data.m_record,team2->data.m_Id);
    else
        teamRecord.remove(team1->data.m_record,team1->data.m_Id);

    // if they have the same record the id of the first team will be the new team id
    if(team1->data.m_record==team2->data.m_record)
        newTeamId = team1->data.m_Id;

    //unite the teams
    auto new_root = (gameUnion.unionSets(gameNode1,gameNode2));
    // update the new_root id to be the new team id
    new_root->data.m_Id = newTeamId;


    return StatusType::SUCCESS;
}

StatusType Plains::unite_by_record(int record)
{
    //validity check
    if(record<=0)
        return StatusType::INVALID_INPUT;
    auto team1 = teamRecord.search(record);
    auto team2 = teamRecord.search(-record);
    if(team1 == nullptr || team2 == nullptr)
        return StatusType::FAILURE;
    if(team1->next||team2->next)
        return StatusType::FAILURE;

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
