#ifndef PROYECTO1_BD2_SFRECORD_H
#define PROYECTO1_BD2_SFRECORD_H

#include "TeamRecord.h"

//nextDel = {-2:active record, -1:no deleted records, 0+: next deleted record}

template<typename RecordType>
struct SFRecord {
    RecordType fileRecord;
    long nextReg;
    long prevReg;

    typedef typename RecordType::KeyType KeyType;

    explicit SFRecord(std::vector<str> &splitLine) : fileRecord(splitLine), nextReg(0), prevReg(0) {};

    SFRecord(KeyType key, long next) : nextReg(next), prevReg(0) {};

    SFRecord() : nextReg(0), prevReg(0) {};

    KeyType getKey() {
        return this->fileRecord.getKey();
    }

    void print() {
        this->fileRecord.print();
        std::cout << "NextReg: " << nextReg << '\n' << "PrevReg: " << prevReg << "\n\n";
    }
};


/*
template<typename  RecordType>
struct SFRecord {
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
    long nextDel{};
    long prevDel{};

    SFRecord() = default;
    explicit SFRecord(std::vector<str> &splitLine) {
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
        nextDel = -2; prevDel = -2;
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
        std::cout << "NextDel: " << nextDel << '\n' << "PrevDel: " << prevDel << "\n\n";
    }
};
*/

#endif  //PROYECTO1_BD2_SFRECORD_H
