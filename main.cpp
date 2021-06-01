#include "Record/TeamRecord.h"
#include "Record/SFRecord.h"
#include "Record/HRecord.h"
#include "SequentialFile/SequentialFile.h"
#include "Record/IncidentRecord.h"

int main() {
    SequentialFile<TeamIncident> sq("../data/Incidents.csv", "Incidents");

    auto efe = sq.load();
    for(auto item : efe)
        item.print();
    return 0;
}
