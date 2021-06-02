#include "extendible_hashing.h"

std::vector<Team> vector_of_records_team(std::string file_name){
    std::vector<Team> vec;
    std::ifstream file(file_name, std::ios::in);
    std::string line, var;
    std::vector<std::string> split_line;
    char c;
    int i = 0;
    int id = 1;
    std::getline(file, line);
    while (std::getline(file, line)){
        while (i <= line.size()){
            if (line[i] == ','){
                split_line.push_back(var);
                i++;
                var = "";
                continue;
            }
            var += line[i];
            i++;
        }
        split_line.push_back(var);
        Team a(id, split_line[0], std::stoi(split_line[1]), std::stoi(split_line[2]), std::stoi(split_line[3]),  std::stoi(split_line[4]), std::stoi(split_line[5]), std::stoi(split_line[6]), std::stoi(split_line[7]), std::stoi(split_line[8]), std::stoi(split_line[9]));
        vec.push_back(a);
        split_line.clear();
        i = 0;
        id++;
        var = "";
    }
    file.close();
    return vec;
}

std::vector<Incident> vector_of_records_incident(std::string file_name){
    std::vector<Incident> vec;
    std::ifstream file(file_name, std::ios::in);
    std::string line, var;
    std::vector<std::string> split_line;
    char c;
    int i = 0;
    int id = 1;
    std::getline(file, line);
    while (std::getline(file, line)){
        while (i <= line.size()){
            if (line[i] == ','){
                split_line.push_back(var);
                i++;
                var = "";
                continue;
            }
            var += line[i];
            i++;
        }
        split_line.push_back(var);
        Incident a(id, split_line[0], split_line[1], split_line[2], split_line[3][0], split_line[4], std::stoi(split_line[5]), split_line[6]);
        vec.push_back(a);
        split_line.clear();
        i = 0;
        id++;
        var = "";
    }
    file.close();
    return vec;
}

int main() {
    return 0;
}
