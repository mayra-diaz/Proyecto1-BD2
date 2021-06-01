#ifndef PROYECTO1_BD2_SEQUENTIALFILE_H
#define PROYECTO1_BD2_SEQUENTIALFILE_H

#include "../Record/SFRecord.h"

template<typename RecordType>
class SequentialFile {
public:
    typedef typename RecordType::KeyType KeyType;

private:
    str headerFileName;  // index.bin
    str dataFileName;    // data.bin
    str auxFileName;     // aux.bin

    unsigned long totalOrderedRecords{};
    unsigned long totalUnorderedRecords{};

    unsigned long getFileSize(const str &newFileName) {
        std::ifstream file(newFileName, std::ios::ate | std::ios::binary);
        unsigned long size = file.tellg();
        file.close();
        return size;
    }

    void initializeSequentialFile(const str &baseFileName) {
        std::ifstream inputFile(baseFileName, std::ios::in);
        std::ofstream sequentialFile(this->dataFileName, std::ios::out | std::ios::binary);
        sequentialFile.seekp(0, std::ios::beg);

        if (!inputFile.is_open())
            throw std::runtime_error("Could not open " + baseFileName);

        str line, var;
        std::vector<str> splitLine;
        char c;
        int i = 0;
        // Read data, line by line
        std::getline(inputFile, line);
        while (std::getline(inputFile, line)) {
            while (i < line.size()) {
                if (line[i] == ',') {
                    splitLine.push_back(var);
                    i++;
                    var.clear();
                    continue;
                }
                var += line[i];
                i++;
            }
            splitLine.push_back(var);
            SFRecord<RecordType> record(splitLine);
            sequentialFile.write((char *) &record, sizeof(record));

            splitLine.clear();
            i = 0;
            var = "";
        }

        inputFile.close();

        //totalOrderedRecords = totalLines;
        //totalUnorderedRecords = 0;
    }

    void initializeFreeList() {
        std::fstream header(this->headerFileName, std::ios::out | std::ios::binary);
        if (!header.is_open())
            throw std::runtime_error("Could not open " + headerFileName);

        long headerPointer = -1;
        header.seekp(0, std::ios::beg);
        header.write((char *) &headerPointer, sizeof(headerPointer));
        header.close();
    }

    RecordType getPrevRecord(RecordType record) {
        if (record.nextDel == -2) {
            record = this->read(this->dataFileName, totalOrderedRecords - 2);
        } else {
            record = this->read(this->dataFileName, record.nextDel);
            record = this->read(this->dataFileName, record.prevDel - 1);
        }
        return record;
    }

    void simpleInsert(RecordType baseRecord, RecordType record) {
        RecordType baseRecordNext = this->read(this->dataFileName, baseRecord.nextDel);

        long currentRecordLogPos = this->findWhereToInsert();
        long baseRecordLogPos = baseRecordNext.prevDel;
        long baseRecordNextLogPos = baseRecord.nextDel;

        baseRecord.nextDel = currentRecordLogPos;
        record.prevDel = baseRecordLogPos;
        record.nextDel = baseRecordNextLogPos;
        baseRecordNext.prevDel = currentRecordLogPos;

        this->write(baseRecord, this->dataFileName, baseRecordLogPos);
        this->write(record, this->dataFileName, currentRecordLogPos);
        this->write(baseRecordNext, this->dataFileName, baseRecordNextLogPos);
    }

    // insert "toInsert" to the left of "currentRecord"
    void insertUpdatingPointers(RecordType toInsert, RecordType currentRecord) {
        // get previous record
        long prevRecordLogPos = currentRecord.prevDel;
        RecordType prevRecord = this->read(this->dataFileName, prevRecordLogPos);

        // get logical position of currentRecord
        long currentRecLogPos = prevRecord.nextDel;
        // calculate logical position of toInsert
        long toInsertLogPos = this->findWhereToInsert();

        // update prevRecord.nextDel
        prevRecord.nextDel = toInsertLogPos;
        this->write(prevRecord, this->dataFileName, prevRecordLogPos);

        // set toInsert pointers
        toInsert.nextDel = currentRecLogPos;
        toInsert.prevDel = prevRecordLogPos;
        this->write(toInsert, this->dataFileName, toInsertLogPos);

        // update currentRecord.prevDel
        currentRecord.prevDel = toInsertLogPos;
        this->write(currentRecord, this->dataFileName, currentRecLogPos);
    }

    void insertAtFirstPosition(RecordType record) {
        RecordType firstRecord = this->read(this->dataFileName, this->getFirstRecordLogPos());
        RecordType firstRecordNext = this->read(this->dataFileName, firstRecord.nextDel);

        long toInsertLogPos = this->findWhereToInsert();

        record.nextDel = toInsertLogPos;
        firstRecordNext.prevDel = toInsertLogPos;
        record.prevDel = -1;

        firstRecord.prevDel = 0;

        this->write(record, this->dataFileName, this->getFirstRecordLogPos());
        this->write(firstRecord, this->dataFileName, toInsertLogPos);
        this->write(firstRecordNext, this->dataFileName, firstRecord.nextDel);
    }

    void insertAtLastPosition(RecordType record) {
        RecordType lastRecord = this->read(this->dataFileName, totalOrderedRecords - 1);

        record.nextDel = -2;
        record.prevDel = totalOrderedRecords - 1;

        long toInsertLogPos = this->findWhereToInsert();

        lastRecord.nextDel = toInsertLogPos;

        this->write(lastRecord, this->dataFileName, totalOrderedRecords - 1);
        this->write(record, this->dataFileName, toInsertLogPos);
    }

    void insertAfterNull(RecordType current, RecordType record) {
        RecordType currentRecordPrev = this->read(this->dataFileName, current.prevDel);

        record.nextDel = -2;
        record.prevDel = currentRecordPrev.nextDel;

        long toInsertLogPos = this->findWhereToInsert();

        current.nextDel = toInsertLogPos;

        this->write(current, this->dataFileName, currentRecordPrev.nextDel);
        this->write(record, this->dataFileName, toInsertLogPos);
    }

    void rebuildAfterInsert() {
        unsigned long totalLines = getFileSize(this->dataFileName) / sizeof(RecordType);

        this->rebuild(totalLines);

        totalOrderedRecords += 5;
        totalUnorderedRecords = 0;
    }

    RecordType searchInOrderedRecords(KeyType ID) {
        long low = 0, high = totalOrderedRecords - 1, mid;

        RecordType currentRecord;

        while (low <= high) {
            mid = (low + high) / 2;
            currentRecord = this->read(this->dataFileName, mid);
            KeyType currentID = currentRecord.ID;

            if (currentID < ID) {
                low = mid + 1;
            } else if (currentID > ID) {
                high = mid - 1;
            } else {
                return currentRecord;
            }
        }

        return currentRecord;
    }

    void updatePointersDelete(RecordType toDelete, long toDeleteLogPos) {
        if (toDelete.prevDel == -1) {  // first register
            RecordType toDeleteNext = this->read(this->dataFileName, toDelete.nextDel);
            toDeleteNext.prevDel = -1;
            this->write(toDeleteNext, this->dataFileName, toDelete.nextDel);
        } else if (toDelete.nextDel == -2) {  // register whose nextDel is null
            RecordType toDeletePrev;
            toDeletePrev = this->read(this->dataFileName, toDelete.prevDel);
            toDeletePrev.nextDel = -2;
            this->write(toDeletePrev, this->dataFileName, toDelete.prevDel);
        } else {  // normal case
            RecordType toDeletePrev, toDeleteNext;
            toDeletePrev = this->read(this->dataFileName, toDelete.prevDel);
            toDeleteNext = this->read(this->dataFileName, toDelete.nextDel);
            toDeletePrev.nextDel = toDelete.nextDel;
            toDeleteNext.prevDel = toDelete.prevDel;
            this->write(toDeletePrev, this->dataFileName, toDelete.prevDel);
            this->write(toDeleteNext, this->dataFileName, toDelete.nextDel);
        }
    }

    void deleteOrderedRecord(long toDeleteLogPos) {
        unsigned long totalLines = getFileSize(this->dataFileName) / sizeof(RecordType) - 1;

        this->rebuild(totalLines);

        totalOrderedRecords = totalOrderedRecords + totalUnorderedRecords;
        totalUnorderedRecords = 0;
    }

    long readHeader() {
        std::fstream header(this->headerFileName);
        long headerValue;
        header.seekg(0);
        header.read((char *) &headerValue, sizeof(headerValue));
        header.close();
        return headerValue;
    }

    void writeHeader(long toDeleteLogPos) {
        std::fstream header(this->headerFileName);
        header.seekp(0);
        header.write((char *) &toDeleteLogPos, sizeof(toDeleteLogPos));
        header.close();
    }

    void deleteUnorderedRecord(long toDeleteLogPos) {
        long headerTemp = readHeader();
        writeHeader(toDeleteLogPos);
        RecordType toDelete(-1, headerTemp);  // mark as deleted with ID -1
        this->write(toDelete, this->dataFileName, toDeleteLogPos);
    }

    long getFirstRecordLogPos() {
        long currentRecordLogPos = 0;
        RecordType currentRecord = this->read(this->dataFileName, currentRecordLogPos);
        while (currentRecord.prevDel == -1) {
            currentRecord = this->read(this->dataFileName, ++currentRecordLogPos);
        }
        return currentRecordLogPos - 1;
    }

    void rebuild(unsigned long totalLines) {
        std::fstream sequentialFile(this->dataFileName);
        std::fstream auxFile(this->auxFileName, std::ios::out);

        RecordType record = this->read(this->dataFileName, this->getFirstRecordLogPos());
        while (record.nextDel != -2) {
            auxFile.write((char *) &record, sizeof(RecordType));
            record = this->read(this->dataFileName, record.nextDel);
        }
        auxFile.write((char *) &record, sizeof(RecordType));

        sequentialFile.close();
        auxFile.close();

        sequentialFile.open(this->dataFileName, std::ios::out);
        auxFile.open(this->auxFileName);

        long currentNext = 1;
        long currentPrev = -1;

        while (auxFile.read((char *) &record, sizeof(RecordType))) {
            record.nextDel = totalLines == currentNext ? -2 : currentNext++;
            record.prevDel = currentPrev++;
            sequentialFile.write((char *) &record, sizeof(RecordType));
        }

        sequentialFile.close();
        auxFile.close();
    }

    long getLogicalPosition(RecordType record) {
        if (record.prevDel == -1) {
            return 0;
        } else {
            RecordType prevRecord = this->read(this->dataFileName, record.prevDel);
            return prevRecord.nextDel;
        }
    }

    // finds where to insert in unordered records, considering free list
    long findWhereToInsert() {
        long currentHeader = readHeader();
        if (currentHeader == -1) {
            return totalOrderedRecords + totalUnorderedRecords;
        } else {
            RecordType deleted = read(this->dataFileName, currentHeader);
            writeHeader(deleted.nextDel);
            return currentHeader;
        }
    }

    RecordType read(const str &fileName, long position) {
        std::fstream file(fileName);
        RecordType record;
        file.seekg(position * sizeof(RecordType));
        file.read((char *) &record, sizeof(RecordType));
        file.close();
        return record;
    }

    void write(RecordType record, const str &fileName, long position) {
        std::fstream file(fileName);
        file.seekp(position * sizeof(RecordType));
        file.write((char *) &record, sizeof(RecordType));
        file.close();
    }

    void insertBetweenUnorderedRecords(RecordType baseRecord, RecordType toInsert) {
        RecordType current;

        long baseRecordLogPos;
        RecordType baseRecordNext = this->read(this->dataFileName, baseRecord.nextDel);
        baseRecordLogPos = baseRecordNext.prevDel;

        current = this->read(this->dataFileName, baseRecordLogPos);
        while (current.nextDel > totalOrderedRecords - 1 && current.nextDel != -2 &&
               current.ID < toInsert.ID) {  // find where to insert
            if (current.ID == toInsert.ID) {
                throw std::out_of_range("User attempted to insert an already existing ID");
            }
            current = this->read(this->dataFileName, current.nextDel);
        }
        if (current.ID == toInsert.ID) {  // check last register ID
            throw std::out_of_range("User attempted to insert an already existing ID");
        }
        if (current.nextDel < totalOrderedRecords) {  // if current points to ordered records
            if (current.ID > toInsert.ID) {
                this->insertUpdatingPointers(toInsert, current);
            } else {
                if (current.nextDel == -2) {
                    this->insertAfterNull(current, toInsert);  // special case
                } else {
                    long currentNextLogPos = current.nextDel;
                    RecordType currentNext = this->read(this->dataFileName, currentNextLogPos);
                    this->insertUpdatingPointers(toInsert, currentNext);  // insert record to the left of currentNext
                }
            }
        } else {                                              // if current points to an unordered record
            this->insertUpdatingPointers(toInsert, current);  // insert record to the left of current
        }
    }

public:
    explicit SequentialFile(const str &baseFileName) {
        str name = baseFileName.substr(0, baseFileName.find(".csv"));
        this->headerFileName = name + "Header.bin";
        this->dataFileName = name + "Data.bin";
        this->auxFileName = name + "Aux.bin";

        this->initializeSequentialFile(baseFileName);
        this->initializeFreeList();
    }

    SequentialFile() = default;

    std::vector<SFRecord<RecordType>> load() {
        std::vector<SFRecord<RecordType>> records;
        std::fstream sequentialFile(this->dataFileName, std::ios::in | std::ios::binary);
        sequentialFile.seekg(0, std::ios::beg);
        SFRecord<RecordType> record;
        while (sequentialFile.read((char *) &record, sizeof(record))) {
            records.push_back(record);
            //sequentialFile.seekg(0, std::ios::cur);
        }

        return records;
    }

    RecordType search(KeyType ID) {
        RecordType baseRecord = this->searchInOrderedRecords(ID);

        if (baseRecord.ID == ID) {
            return baseRecord;
        }

        if ((baseRecord.prevDel == -1 && ID < baseRecord.ID) || (baseRecord.nextDel == -2 && ID > baseRecord.ID)) {
            throw std::out_of_range("Search out of range. ID: " + std::to_string(ID));
        }

        if (baseRecord.ID > ID) {
            baseRecord = this->getPrevRecord(baseRecord);
        }

        RecordType current = baseRecord;

        current = this->read(this->dataFileName, current.nextDel);
        while (current.ID <= ID) {
            if (current.ID == ID) {
                return current;
            } else {
                current = this->read(this->dataFileName, current.nextDel);
            }
        }

        throw std::out_of_range("Search out of range. ID: " + std::to_string(ID));
    }

    std::vector<RecordType> searchByRanges(KeyType begin, KeyType end) {
        if (begin > end) {
            std::swap(begin, end);
        }

        RecordType current = this->searchInOrderedRecords(begin);

        if (current.prevDel != -1) {
            if (current.ID > begin) {
                current = this->getPrevRecord(current);
            }
        }

        std::vector<RecordType> searchResult;

        while (true) {
            if (current.ID >= begin && current.ID <= end) {
                searchResult.push_back(current);
            }
            if (current.ID > end || current.nextDel == -2) {
                return searchResult;
            }
            current = this->read(this->dataFileName, current.nextDel);
        }
    }

    void deleteRecord(KeyType ID) {
        RecordType toDelete = this->search(ID);

        if (toDelete.ID != ID) {
            throw std::out_of_range("Record with ID " + std::to_string(ID) + " not found.");
        }

        long toDeleteLogPos = this->getLogicalPosition(toDelete);

        this->updatePointersDelete(toDelete, toDeleteLogPos);

        if (toDeleteLogPos < totalOrderedRecords) {
            this->deleteOrderedRecord(toDeleteLogPos);
            --totalOrderedRecords;
        } else {
            this->deleteUnorderedRecord(toDeleteLogPos);
            --totalUnorderedRecords;
        }
    }

    void insert(RecordType toInsert) {
        RecordType baseRecord = this->searchInOrderedRecords(toInsert.ID);

        if (baseRecord.ID == toInsert.ID) {
            throw std::out_of_range("User attempted to insert an already existing ID");
        }

        if (baseRecord.prevDel == -1 && toInsert.ID < baseRecord.ID) {  // insert at the beginning
            this->insertAtFirstPosition(toInsert);
        } else if (baseRecord.nextDel == -2) {  // insert at last position
            this->insertAtLastPosition(toInsert);
        } else {
            if (baseRecord.ID > toInsert.ID) {
                baseRecord = this->getPrevRecord(baseRecord);
            }
            if (baseRecord.nextDel < totalOrderedRecords) {
                this->simpleInsert(baseRecord,
                                   toInsert);  // when it's not necessary to insert "between" unordered registers
            } else {                           // when baseRecord points to unordered records
                this->insertBetweenUnorderedRecords(baseRecord, toInsert);
            }
        }
        if (++totalUnorderedRecords == 5) {
            this->rebuildAfterInsert();
        }
    }

    long getTotalOrderedRecords() {
        return this->totalOrderedRecords;
    }
};

#endif  //PROYECTO1_BD2_SEQUENTIALFILE_H
