#ifndef EXTENDIBLE_HASHING_RECORD_H
#define EXTENDIBLE_HASHING_RECORD_H

#include <iostream>
#include <fstream>
#include <utility>
#include <bitset>
#include <cmath>
#include <vector>
#include <cstring>


struct Team {
    int id;
    char name[30];
    int overturns;
    int leadingToGoalsFor;
    int leadingToGoalsAgainst;
    int disallowedGoalsFor;
    int disallowedGoalsAgainst;
    int subjectiveDecisionsFor;
    int subjectiveDecisionsAgainst;
    int netGoalScore;
    int netSubjectiveScore;
    int nextDel;

    Team() = default;

    Team(int id,
           std::string name_,
           int overturns,
           int leadingToGoalsFor,
           int leadingToGoalsAgainst,
           int disallowedGoalsFor,
           int disallowedGoalsAgainst,
           int subjectiveDecisionsFor,
           int subjectiveDecisionsAgainst,
           int netGoalScore,
           int netSubjectiveScore){
        this->id = id;
        strcpy(name, name_.c_str());
        this->overturns = overturns;
        this->leadingToGoalsFor = leadingToGoalsFor;
        this->leadingToGoalsAgainst = leadingToGoalsAgainst;
        this->disallowedGoalsFor = disallowedGoalsFor;
        this->disallowedGoalsAgainst = disallowedGoalsAgainst;
        this->subjectiveDecisionsFor = subjectiveDecisionsFor;
        this->subjectiveDecisionsAgainst = subjectiveDecisionsAgainst;
        this->netGoalScore = netGoalScore;
        this->netSubjectiveScore = netSubjectiveScore;
        this->nextDel = -2;
    }

    void print_record(){
        std::cout<<id<<" "<<name<<" "<<overturns<<" "<<leadingToGoalsFor<<" "<<leadingToGoalsAgainst<<" "<<disallowedGoalsFor<<" "<<disallowedGoalsAgainst<<" "<<subjectiveDecisionsFor<<" "<<subjectiveDecisionsAgainst<<" "<<netGoalScore<<" "<<netSubjectiveScore<<" "<<nextDel<<"\n";
    }
};

struct Incident{
    int id;
    char team[30];
    char opponentTeam[30];
    char date[8];
    char site;
    char incident[200];
    int time;
    char var[8];
    int nextDel;

    Incident() = default;

    Incident(int id,
             std::string team_,
             std::string opponentTeam_,
             std::string date_,
             char site_,
             std::string incident_,
             int time,
             std::string var_){
        this->id = id;
        strcpy(team, team_.c_str());
        strcpy(opponentTeam, opponentTeam_.c_str());
        strcpy(date, date_.c_str());
        this->site = site_;
        strcpy(incident, incident_.c_str());
        this->time = time;
        strcpy(var, var_.c_str());
        this->nextDel = -2;
    }
    void print_record(){
        std::cout<<id<<" "<<team<<" "<<opponentTeam<<" "<<date<<" "<<site<<" "<<incident<<" "<<time<<" "<<var<<" "<<nextDel<<"\n";
    }
};

#endif //EXTENDIBLE_HASHING_RECORD_H
