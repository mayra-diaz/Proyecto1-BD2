#ifndef PROYECTO1_BD2_SFRECORD_H
#define PROYECTO1_BD2_SFRECORD_H

#include "BaseRecord.h"

//nextDel = {-2:active record, -1:no deleted records, 0+: next deleted record}
template<typename RecordType>
struct SFRecord : public BaseRecord<RecordType> {
    typedef typename RecordType::KeyType KeyType;
    long nextDel = -1;
    long prevDel = -1;

    explicit SFRecord(std::vector <str> &splitLine) : BaseRecord<RecordType>(splitLine) {};
};

#endif  //PROYECTO1_BD2_SFRECORD_H
