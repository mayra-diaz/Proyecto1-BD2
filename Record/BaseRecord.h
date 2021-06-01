#ifndef PROYECTO1_BD2_BASERECORD_H
#define PROYECTO1_BD2_BASERECORD_H

#include "../types.h"

template<typename RecordType>
struct BaseRecord {
    typedef typename RecordType::KeyType KeyType;
    RecordType fileRecord;

    explicit BaseRecord(std::vector <str> &splitLine) : fileRecord(splitLine) {};
};

struct Team {
    char name[30]{};
    int overturns,
            leadingToGoalsFor,
            leadingToGoalsAgainst,
            disallowedGoalsFor,
            disallowedGoalsAgainst,
            subjectiveDecisionsFor,
            subjectiveDecisionsAgainst,
            netGoalScore,
            netSubjectiveScore;

    typedef std::string KeyType;

    Team(const str &name, const str &overturns, const str &leadingToGoalsFor, const str &leadingToGoalsAgainst,
         const str &disallowedGoalsFor, const str &disallowedGoalsAgainst, const str &subjectiveDecisionsFor,
         const str &subjectiveDecisionsAgainst,
         const str &netGoalScore, const str &netSubjectiveScore)
            : overturns(std::stoi(overturns)), leadingToGoalsFor(std::stoi(leadingToGoalsFor)),
              leadingToGoalsAgainst(std::stoi(leadingToGoalsAgainst)),
              disallowedGoalsFor(std::stoi(disallowedGoalsFor)),
              disallowedGoalsAgainst(std::stoi(disallowedGoalsAgainst)),
              subjectiveDecisionsFor(std::stoi(subjectiveDecisionsFor)),
              subjectiveDecisionsAgainst(std::stoi(subjectiveDecisionsAgainst)), netGoalScore(std::stoi(netGoalScore)),
              netSubjectiveScore(std::stoi(netSubjectiveScore)) {
        strcpy(this->name, name.c_str());
    }

    explicit Team(std::vector <str> &splitLine) {
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
};
/*
struct TeamIncident {
    char team[30],
            opponentTeam[30],
            date[8],
            site,
            incident[80],
            var[8];
    int time;

    typedef std::string KeyType;

    TeamIncident(str team, str opponentTeam, str date, str site, str incident, str var) {
        strcpy(this->team, team.c_str());
        strcpy(this->opponentTeam, opponentTeam.c_str());
        strcpy(this->date, date.c_str());
        strcpy(this->site, site[0]);
        strcpy(this->incident, incident.c_str());
        strcpy(this->var, var.c_str());
        this->time = std::stoi(time);
    }

    explicit Team(std::vector<str> splitLine) {
        strcpy(this->team, splitLine[0].c_str());
        strcpy(this->opponentTeam, splitLine[1].c_str());
        strcpy(this->date, splitLine[2].c_str());
        strcpy(this->site, splitLine[3][0]);
        strcpy(this->incident, splitLine[4].c_str());
        strcpy(this->var, splitLine[5].c_str());
        this->time = std::stoi(splitLine[6].c_str());
    }

    str getKey() {
        return std::string(team);
    }
};*/

#endif  //PROYECTO1_BD2_BASERECORD_H