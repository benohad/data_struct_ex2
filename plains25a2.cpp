// You can edit anything you want in this file.
// However you need to implement all public Plains function, as provided below as a template

#include "plains25a2.h"


Plains::Plains(): jockeyTable(), teamTable(), gameUnion(){}

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
    // check if the jockeys are in the table or in same team
    if(jockeyTable.search(victoriousJockeyId) == nullptr || jockeyTable.search(losingJockeyId) == nullptr)
        return StatusType::FAILURE;
    if(gameUnion.find(gameNode(victoriousJockeyId)) == gameUnion.find(gameNode(losingJockeyId)))
        return StatusType::FAILURE;

    return StatusType::FAILURE;
}

StatusType Plains::merge_teams(int teamId1, int teamId2)
{
    return StatusType::FAILURE;
}

StatusType Plains::unite_by_record(int record)
{
    return StatusType::FAILURE;
}

output_t<int> Plains::get_jockey_record(int jockeyId)
{
    return 0;
}

output_t<int> Plains::get_team_record(int teamId)
{
    return 0;
}
