#ifndef PROYECTO1_BD2_INCIDENTRECORD_H
#define PROYECTO1_BD2_INCIDENTRECORD_H


struct TeamIncident {
    typedef std::string KeyType;

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


#endif //PROYECTO1_BD2_INCIDENTRECORD_H
