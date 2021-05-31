#ifndef PROYECTO1_BD2_SFRECORD_H
#define PROYECTO1_BD2_SFRECORD_H

#include <BaseRecord.h>

template <typename RecordType>
struct SFRecord : public BaseRecord {
    typedef typename RecordType::KeyType KeyType;
    long prevDeleted = -2;
};

#endif  //PROYECTO1_BD2_SFRECORD_H
