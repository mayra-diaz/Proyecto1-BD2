#ifndef PROYECTO1_BD2_BASERECORD_H
#define PROYECTO1_BD2_BASERECORD_H

template <typename RecordType>
struct BaseRecord {
    typedef typename RecordType::KeyType KeyType;
    RecordType fileRecord;
};

struct Team {
    typedef KeyType = char[30];

    char name[30];
    int overturns,
        leadingToGoalsFor,
        leadingToGoalsAgainst,
        disallowedGoalsFor,
        disallowedGoalsAgainst,
        subjectiveDecisionsFor,
        subjectiveDecisionsAgainst,
        netGoalScore,
        netSubjectiveScore;
    char[30] getKey() {
        // TODO: convertir a string
        return name;
    }
};

struct TeamIncident {
    typedef KeyType = std::string;
    char team[30],
        opponentTeam[30],
        date[8],
        site,
        incident[80],
        var[8];
    int time;
    char[30] getKey() {
        // TODO: convertir a string
        return team;
    }
};

#endif  //PROYECTO1_BD2_BASERECORD_H