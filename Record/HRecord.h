#ifndef PROYECTO1_BD2_HRECORD_H
#define PROYECTO1_BD2_HRECORD_H

#include "BaseRecord.h"

template <typename keyType>
struct HTeam: public Team {
    long prevDeleted=-2;
    HRecord()=default;
};

struct HTeamIncident: public TeamIncident {
    long prevDeleted=-2;
    HRecord()=default;
};

#endif //PROYECTO1_BD2_HRECORD_H
