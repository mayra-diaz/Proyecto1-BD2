#ifndef PROYECTO1_BD2_BASERECORD_H
#define PROYECTO1_BD2_BASERECORD_H

#include "../types.h"

template<typename RecordType>
struct BaseRecord {
    RecordType fileRecord;
    explicit BaseRecord(std::vector<str> &splitLine) : fileRecord(splitLine) {}
    BaseRecord() = default;
};

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


struct TeamIncident {
    char team[30]{},
            opponentTeam[30]{},
            date[8]{},
            site{},
            incident[200]{},
            var[8]{};
    int time{};

    TeamIncident() = default;

    explicit TeamIncident(std::vector<str> &splitLine) {
        strcpy(this->team, splitLine[0].c_str());
        strcpy(this->opponentTeam, splitLine[1].c_str());
        strcpy(this->date, splitLine[2].c_str());
        this->site = splitLine[3][0];
        strcpy(this->incident, splitLine[4].c_str());
        this->time = std::stoi(splitLine[5]);
        strcpy(this->var, splitLine[6].c_str());
    }

    str getKey() {
        return std::string(team);
    }

    void print() {
        std::cout << "team:\t" << std::string(team) << std::endl;
        std::cout << "opponentTeam:\t" << std::string(opponentTeam) << std::endl;
        std::cout << "date:\t" << std::string(date) << std::endl;
        std::cout << "site:\t" << site << std::endl;
        std::cout << "incident:\t" << std::string(incident) << std::endl;
        std::cout << "var:\t" << std::string(var) << std::endl;
        std::cout << "time:\t" << time << std::endl;

    }
};


#endif  //PROYECTO1_BD2_BASERECORD_H