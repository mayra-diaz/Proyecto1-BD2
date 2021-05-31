#ifndef PROYECTO1_BD2_BASERECORD_H
#define PROYECTO1_BD2_BASERECORD_H

template <typename keyType>
struct BaseRecord {
    keyType key;
    BaseRecord(keyType key) : key(key) {}
    BaseRecord()=default;
};

template <typename keyType>
struct Team: public Record<keyType> {
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
};

template <typename keyType>
struct TeamIncident: public Record<keyType> {
    char team[30],
        opponentTeam[30],
        date[8],
        site,
        incident[80],
        var[8];
    int time;
};

/*
with open('Incidents.csv', 'r') as file:
    reader = csv.reader(file, quoting=csv.QUOTE_ALL, skipinitialspace=True)
    f = open('in.csv', "w")
    for row in reader:
        temp = row;
        temp[5] = temp[5][:2]
        f.write(temp[0]+','+temp[1]+','+temp[2]+','+temp[3]+','+temp[4]+','+temp[5]+','+temp[6]+'\n')
    f.close()*/


#endif //PROYECTO1_BD2_BASERECORD_H
