#ifndef PROYECTO1_BD2_TEAMRECORD_H
#define PROYECTO1_BD2_TEAMRECORD_H

#include "../types.h"

struct Team {
    typedef std::string KeyType;

    char name[30]{};
    int overturns{},
            leadingToGoalsFor{},
            leadingToGoalsAgainst{},
            disallowedGoalsFor{},
            disallowedGoalsAgainst{},
            subjectiveDecisionsFor{},
            subjectiveDecisionsAgainst{},
            netGoalScore{},
            netSubjectiveScore{};

    Team() = default;

    explicit Team(std::vector<str> &splitLine) {
        strcpy(this->name, splitLine[0].c_str());
        overturns = std::stoi(splitLine[1]);
        leadingToGoalsFor = std::stoi(splitLine[2]);
        leadingToGoalsAgainst = std::stoi(splitLine[3]);
        disallowedGoalsFor = std::stoi(splitLine[4]);
        disallowedGoalsAgainst = std::stoi(splitLine[5]);
        subjectiveDecisionsFor = std::stoi(splitLine[6]);
        subjectiveDecisionsAgainst = std::stoi(splitLine[7]);
        netGoalScore = std::stoi(splitLine[8]);
        netSubjectiveScore = std::stoi(splitLine[9]);
    }

    str getKey() {
        return std::string(name);
    }

    void print() {
        std::cout << "name:\t" << std::string(name) << std::endl;
        std::cout << "Overturns:\t" << overturns << std::endl;
        std::cout << "leadingToGoalsFor:\t" << leadingToGoalsFor << std::endl;
        std::cout << "leadingToGoalsAgainst:\t" << leadingToGoalsAgainst << std::endl;
        std::cout << "disallowedGoalsFor:\t" << disallowedGoalsFor << std::endl;
        std::cout << "disallowedGoalsAgainst:\t" << disallowedGoalsAgainst << std::endl;
        std::cout << "subjectiveDecisionsFor:\t" << subjectiveDecisionsFor << std::endl;
        std::cout << "subjectiveDecisionsAgainst:\t" << subjectiveDecisionsAgainst << std::endl;
        std::cout << "netGoalScore:\t" << netGoalScore << std::endl;
        std::cout << "netSubjectiveScore:\t" << netSubjectiveScore << std::endl;

    }
};

#endif  //PROYECTO1_BD2_TEAMRECORD_H