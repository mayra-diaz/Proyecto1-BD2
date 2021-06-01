#include "Record/BaseRecord.h"
#include "Record/SFRecord.h"
#include "Record/HRecord.h"
#include "SequentialFile/SequentialFile.h"

int main() {
    SequentialFile<Team> sq("../data/Teams.csv");
    return 0;
}
