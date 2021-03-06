#include "Record/TeamRecord.h"
#include "Record/SFRecord.h"
#include "Record/HRecord.h"
#include "SequentialFile/SequentialFile.h"
#include "Record/IncidentRecord.h"

int main() {
    SequentialFile<Team> sq("../data/Teams.csv", "Teams");
    std::vector<str> splitLine = {"Hola FC", "11", "0", "2", "2", "2", "-4", "1", "5", "-4"};
    SFRecord<Team> testTeam(splitLine);

    sq.insertRecord(testTeam);


    std::cout << "________________________________________\n";

    //auto searchResult = sq.rangeSearch("Arsenal", "Wolves");
    sq.deleteRecord("Hola FC");

    auto efe = sq.load();
    for (auto item : efe)
        item.print();

    return 0;
}
