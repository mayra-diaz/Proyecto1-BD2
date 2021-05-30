#ifndef PROYECTO1_BD2_HRECORD_H
#define PROYECTO1_BD2_HRECORD_H

#include "BaseRecord.h"

template <typename keyType>
struct HRecord: public BaseRecord {
    long prevDeleted=-2;
    HRecord(keyType key) : BaseRecord(key) {}
    HRecord()=default;
};

#endif //PROYECTO1_BD2_HRECORD_H
