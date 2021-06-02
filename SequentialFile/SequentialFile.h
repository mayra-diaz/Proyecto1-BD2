#ifndef PROYECTO1_BD2_SEQUENTIALFILE_H
#define PROYECTO1_BD2_SEQUENTIALFILE_H

#include <sys/stat.h>
#include <algorithm>
#include "../Record/SFRecord.h"

#define AUX_FACTOR 1

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

    bool static compare(SFRecord<RecordType> a, SFRecord<RecordType> b) {
        return (strcmp(a.getKey().c_str(), b.getKey().c_str()) <= 0);
    }

    void createBinFromCSV(const str &baseFileName) {
        std::ifstream inputFile(baseFileName, std::ios::in);
        std::vector<SFRecord<RecordType>> recordsVector;
        long currentNext = 1;
        long currentPrev = -1;

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
            //sequentialFile.write((char *) &record, sizeof(record));
            totalOrderedRecords++;
            //this->insert(record);
            splitLine.clear();
            i = 0;
            var = "";
            recordsVector.push_back(record);
        }
        inputFile.close();
        std::ofstream sequentialFile(dataFileName, std::ios::out | std::ios::binary);
        sequentialFile.seekp(0, std::ios::beg);

        std::sort(recordsVector.begin(), recordsVector.end(), compare);
        for (auto record : recordsVector) {
            record.nextReg = currentNext++;
            record.prevReg = currentPrev++;
            sequentialFile.write((char *) &record, sizeof(record));
        }

        long nullNext = -2;
        sequentialFile.seekp(-2 * sizeof(long), std::ios::end);
        sequentialFile.write((char *) &nullNext, sizeof(nullNext));
        sequentialFile.close();
        totalUnorderedRecords = 0;
    }

    unsigned long getFileSize(const std::string &newFileName) {
        std::ifstream file(newFileName, std::ios::ate | std::ios::binary);
        unsigned long size = file.tellg();
        file.close();
        return size;
    }

    /*void initializeSequentialFile() {
        std::ifstream inputFile(this->inputFileName, std::ios::in);
        std::ofstream sequentialFile(this->dataFileName, std::ios::out);

        SFRecord<RecordType> record;
        long currentNext = 1;
        long currentPrev = -1;

        unsigned long totalLines = getFileSize(this->inputFileName) / (sizeof(SFRecord<RecordType>) - 2 * sizeof(long));
        while (inputFile.read((char *) &record, sizeof(SFRecord<RecordType>) - 2 * sizeof(long))) {
            record.nextReg = totalLines == currentNext ? -2 : currentNext++;
            record.prevReg = currentPrev++;
            this->insert(record);
        }

        totalOrderedRecords = totalLines;
        totalUnorderedRecords = 0;
    }*/

    void initializeFreeList() {
        std::fstream header(this->headerFileName, std::ios::out);
        long headerPointer = -1;
        header.seekp(0);
        header.write((char *) &headerPointer, sizeof(headerPointer));
        header.close();
    }

    SFRecord<RecordType> getPrevRecord(SFRecord<RecordType> record) {
        if (record.nextReg == -2) {
            record = this->read(this->dataFileName, totalOrderedRecords - 2);
        } else {
            record = this->read(this->dataFileName, record.nextReg);
            record = this->read(this->dataFileName, record.prevReg - 1);
        }
        return record;
    }

    void simpleInsert(SFRecord<RecordType> baseRecord, SFRecord<RecordType> record) {
        SFRecord<RecordType> baseRecordNext = this->read(this->dataFileName, baseRecord.nextReg);

        long currentRecordLogPos = this->findWhereToInsert();
        long baseRecordLogPos = baseRecordNext.prevReg;
        long baseRecordNextLogPos = baseRecord.nextReg;

        baseRecord.nextReg = currentRecordLogPos;
        record.prevReg = baseRecordLogPos;
        record.nextReg = baseRecordNextLogPos;
        baseRecordNext.prevReg = currentRecordLogPos;

        this->write(baseRecord, this->dataFileName, baseRecordLogPos);
        this->write(record, this->dataFileName, currentRecordLogPos);
        this->write(baseRecordNext, this->dataFileName, baseRecordNextLogPos);
    }

    // insert "toInsert" to the left of "currentRecord"
    void insertUpdatingPointers(SFRecord<RecordType> toInsert, SFRecord<RecordType> currentRecord) {
        // get previous record
        long prevRecordLogPos = currentRecord.prevReg;
        SFRecord<RecordType> prevRecord = this->read(this->dataFileName, prevRecordLogPos);

        // get logical position of currentRecord
        long currentRecLogPos = prevRecord.nextReg;
        // calculate logical position of toInsert
        long toInsertLogPos = this->findWhereToInsert();

        // update prevRecord.nextReg
        prevRecord.nextReg = toInsertLogPos;
        this->write(prevRecord, this->dataFileName, prevRecordLogPos);

        // set toInsert pointers
        toInsert.nextReg = currentRecLogPos;
        toInsert.prevReg = prevRecordLogPos;
        this->write(toInsert, this->dataFileName, toInsertLogPos);

        // update currentRecord.prevReg
        currentRecord.prevReg = toInsertLogPos;
        this->write(currentRecord, this->dataFileName, currentRecLogPos);
    }

    void insertAtFirstPosition(SFRecord<RecordType> record) {
        SFRecord<RecordType> firstRecord = this->read(this->dataFileName, this->getFirstRecordLogPos());
        SFRecord<RecordType> firstRecordNext = this->read(this->dataFileName, firstRecord.nextReg);

        long toInsertLogPos = this->findWhereToInsert();

        record.nextReg = toInsertLogPos;
        firstRecordNext.prevReg = toInsertLogPos;
        record.prevReg = -1;

        firstRecord.prevReg = 0;

        this->write(record, this->dataFileName, this->getFirstRecordLogPos());
        this->write(firstRecord, this->dataFileName, toInsertLogPos);
        this->write(firstRecordNext, this->dataFileName, firstRecord.nextReg);
    }

    void insertAtLastPosition(SFRecord<RecordType> record) {
        SFRecord<RecordType> lastRecord = this->read(this->dataFileName, totalOrderedRecords - 1);

        record.nextReg = -2;
        record.prevReg = totalOrderedRecords - 1;

        long toInsertLogPos = this->findWhereToInsert();

        lastRecord.nextReg = toInsertLogPos;

        this->write(lastRecord, this->dataFileName, totalOrderedRecords - 1);
        this->write(record, this->dataFileName, toInsertLogPos);
    }

    void insertAfterNull(SFRecord<RecordType> current, SFRecord<RecordType> record) {
        SFRecord<RecordType> currentRecordPrev = this->read(this->dataFileName, current.prevReg);

        record.nextReg = -2;
        record.prevReg = currentRecordPrev.nextReg;

        long toInsertLogPos = this->findWhereToInsert();

        current.nextReg = toInsertLogPos;

        this->write(current, this->dataFileName, currentRecordPrev.nextReg);
        this->write(record, this->dataFileName, toInsertLogPos);
    }

    void rebuildAfterInsert() {
        unsigned long totalLines = getFileSize(this->dataFileName) / sizeof(SFRecord<RecordType>);

        this->rebuild(totalLines);

        totalOrderedRecords += AUX_FACTOR;
        totalUnorderedRecords = 0;
    }

    SFRecord<RecordType> searchInOrderedRecords(KeyType key) {
        long low = 0, high = totalOrderedRecords - 1, mid;

        SFRecord<RecordType> currentRecord;

        while (low <= high) {
            mid = (low + high) / 2;
            currentRecord = this->read(this->dataFileName, mid);
            KeyType currentID = currentRecord.getKey();

            if (currentID < key) {
                low = mid + 1;
            } else if (currentID > key) {
                high = mid - 1;
            } else {
                return currentRecord;
            }
        }

        return currentRecord;
    }

    void updatePointersDelete(SFRecord<RecordType> toDelete, long toDeleteLogPos) {
        if (toDelete.prevReg == -1) { // first register
            SFRecord<RecordType> toDeleteNext = this->read(this->dataFileName, toDelete.nextReg);
            toDeleteNext.prevReg = -1;
            this->write(toDeleteNext, this->dataFileName, toDelete.nextReg);
        } else if (toDelete.nextReg == -2) { // register whose next is null
            SFRecord<RecordType> toDeletePrev;
            toDeletePrev = this->read(this->dataFileName, toDelete.prevReg);
            toDeletePrev.nextReg = -2;
            this->write(toDeletePrev, this->dataFileName, toDelete.prevReg);
        } else { // normal case
            SFRecord<RecordType> toDeletePrev, toDeleteNext;
            toDeletePrev = this->read(this->dataFileName, toDelete.prevReg);
            toDeleteNext = this->read(this->dataFileName, toDelete.nextReg);
            toDeletePrev.nextReg = toDelete.nextReg;
            toDeleteNext.prevReg = toDelete.prevReg;
            this->write(toDeletePrev, this->dataFileName, toDelete.prevReg);
            this->write(toDeleteNext, this->dataFileName, toDelete.nextReg);
        }
    }

    void deleteOrderedRecord(long toDeleteLogPos) {
        unsigned long totalLines = getFileSize(this->dataFileName) / sizeof(SFRecord<RecordType>);

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
        SFRecord<RecordType> toDelete("-1", headerTemp); // mark as deleted with key -1
        this->write(toDelete, this->dataFileName, toDeleteLogPos);
    }

    long getFirstRecordLogPos() {
        long currentRecordLogPos = 0;
        SFRecord<RecordType> currentRecord = this->read(this->dataFileName, currentRecordLogPos);
        while (currentRecord.prevReg == -1) {
            currentRecord = this->read(this->dataFileName, ++currentRecordLogPos);
        }
        return currentRecordLogPos - 1;
    }

    void rebuild(unsigned long totalLines) {
        std::fstream sequentialFile(this->dataFileName);
        std::fstream auxFile(this->auxFileName, std::ios::out);

        SFRecord<RecordType> record = this->read(this->dataFileName, this->getFirstRecordLogPos());
        while (record.nextReg != -2) {
            auxFile.write((char *) &record, sizeof(SFRecord<RecordType>));
            record = this->read(this->dataFileName, record.nextReg);
        }
        auxFile.write((char *) &record, sizeof(SFRecord<RecordType>));

        sequentialFile.close();
        auxFile.close();

        sequentialFile.open(this->dataFileName, std::ios::out);
        auxFile.open(this->auxFileName);

        long currentNext = 1;
        long currentPrev = -1;

        while (auxFile.read((char *) &record, sizeof(SFRecord<RecordType>))) {
            record.nextReg = currentNext++;
            record.prevReg = currentPrev++;
            sequentialFile.write((char *) &record, sizeof(SFRecord<RecordType>));
        }
        long var = -2;
        sequentialFile.seekg(-2 * sizeof(long), std::ios::cur);
        sequentialFile.write((char *) &var, sizeof(var));
        sequentialFile.close();
        auxFile.close();
    }

    long getLogicalPosition(SFRecord<RecordType> record) {
        if (record.prevReg == -1) {
            return 0;
        } else {
            SFRecord<RecordType> prevRecord = this->read(this->dataFileName, record.prevReg);
            return prevRecord.nextReg;
        }
    }

    // finds where to insert in unordered records, considering free list
    long findWhereToInsert() {
        long currentHeader = readHeader();
        if (currentHeader == -1) {
            return totalOrderedRecords + totalUnorderedRecords;
        } else {
            SFRecord<RecordType> deleted = read(this->dataFileName, currentHeader);
            writeHeader(deleted.nextReg);
            return currentHeader;
        }
    }

    SFRecord<RecordType> read(const std::string &fileName, long position) {
        std::fstream file(fileName);
        SFRecord<RecordType> record;
        file.seekg(position * sizeof(SFRecord<RecordType>));
        file.read((char *) &record, sizeof(SFRecord<RecordType>));
        file.close();
        return record;
    }

    void write(SFRecord<RecordType> record, const std::string &fileName, long position) {
        std::fstream file(fileName);
        file.seekp(position * sizeof(SFRecord<RecordType>));
        file.write((char *) &record, sizeof(SFRecord<RecordType>));
        file.close();
    }

    void insertBetweenUnorderedRecords(SFRecord<RecordType> baseRecord, SFRecord<RecordType> toInsert) {
        SFRecord<RecordType> current;

        long baseRecordLogPos;
        SFRecord<RecordType> baseRecordNext = this->read(this->dataFileName, baseRecord.nextReg);
        baseRecordLogPos = baseRecordNext.prevReg;

        current = this->read(this->dataFileName, baseRecordLogPos);
        while (current.nextReg > totalOrderedRecords - 1 && current.nextReg != -2 &&
               current.getKey() < toInsert.getKey()) { // find where to insert
            if (current.getKey() == toInsert.getKey()) {
                throw std::out_of_range("User attempted to insert an already existing key");
            }
            current = this->read(this->dataFileName, current.nextReg);
        }
        if (current.getKey() == toInsert.getKey()) { // check last register key
            throw std::out_of_range("User attempted to insert an already existing key");
        }
        if (current.nextReg < totalOrderedRecords) { // if current points to ordered records
            if (current.getKey() > toInsert.getKey()) {
                this->insertUpdatingPointers(toInsert, current);
            } else {
                if (current.nextReg == -2) {
                    this->insertAfterNull(current, toInsert); // special case
                } else {
                    long currentNextLogPos = current.nextReg;
                    SFRecord<RecordType> currentNext = this->read(this->dataFileName, currentNextLogPos);
                    this->insertUpdatingPointers(toInsert, currentNext); // insert record to the left of currentNext
                }
            }
        } else { // if current points to an unordered record
            this->insertUpdatingPointers(toInsert, current); // insert record to the left of current
        }
    }

public:

    explicit SequentialFile(const str &baseFileName, const str &bname) {
        str name = "../SequentialFile/data/" + bname;
        this->headerFileName = name + "Header.bin";
        this->dataFileName = name + "Data.bin";
        this->auxFileName = name + "Aux.bin";

        this->createBinFromCSV(baseFileName);
        this->initializeFreeList();
    }

    SequentialFile() = default;

    std::vector<SFRecord<RecordType>> load() {
        std::vector<SFRecord<RecordType>> records;
        std::fstream sequentialFile(this->dataFileName, std::ios::in);

        SFRecord<RecordType> record = SFRecord<RecordType>();
        while (sequentialFile.read((char *) &record, sizeof(record))) {
            records.push_back(record);
        }

        return records;
    }

    SFRecord<RecordType> search(KeyType key) {
        SFRecord<RecordType> baseRecord = this->searchInOrderedRecords(key);

        if (baseRecord.getKey() == key) {
            return baseRecord;
        }

        if ((baseRecord.prevReg == -1 && key < baseRecord.getKey()) ||
            (baseRecord.nextReg == -2 && key > baseRecord.getKey())) {
            throw std::out_of_range("Search out of range. Key: " + key);
        }

        if (baseRecord.getKey() > key) {
            baseRecord = this->getPrevRecord(baseRecord);
        }

        SFRecord<RecordType> current = baseRecord;

        current = this->read(this->dataFileName, current.nextReg);
        while (current.getKey() <= key) {
            if (current.getKey() == key) {
                return current;
            } else {
                current = this->read(this->dataFileName, current.nextReg);
            }
        }

        throw std::out_of_range("Search out of range. key: " + key);
    }

    std::vector<SFRecord<RecordType>> searchByRanges(KeyType begin, KeyType end) {
        if (begin > end) {
            std::swap(begin, end);
        }

        SFRecord<RecordType> current = this->searchInOrderedRecords(begin);

        if (current.prevReg != -1) {
            if (current.getKey() > begin) {
                current = this->getPrevRecord(current);
            }
        }

        std::vector<SFRecord<RecordType>> searchResult;

        while (true) {
            if (current.getKey() >= begin && current.getKey() <= end) {
                searchResult.push_back(current);
            }
            if (current.getKey() > end || current.nextReg == -2) {
                return searchResult;
            }
            current = this->read(this->dataFileName, current.nextReg);
        }
    }

    void deleteRecord(KeyType key) {
        SFRecord<RecordType> toDelete = this->search(key);

        if (toDelete.getKey() != key) {
            throw std::out_of_range("Record with key " + key + " not found.");
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

    void insert(SFRecord<RecordType> toInsert) {
        SFRecord<RecordType> baseRecord = this->searchInOrderedRecords(toInsert.getKey());

        if (baseRecord.getKey() == toInsert.getKey()) {
            throw std::out_of_range("User attempted to insert an already existing key");
        }

        if (baseRecord.prevReg == -1 && toInsert.getKey() < baseRecord.getKey()) { // insert at the beginning
            this->insertAtFirstPosition(toInsert);
        } else if (baseRecord.nextReg == -2) { // insert at last position
            this->insertAtLastPosition(toInsert);
        } else {
            if (baseRecord.getKey() > toInsert.getKey()) {
                baseRecord = this->getPrevRecord(baseRecord);
            }
            if (baseRecord.nextReg < totalOrderedRecords) {
                this->simpleInsert(baseRecord,
                                   toInsert); // when it's not necessary to insert "between" unordered registers
            } else { // when baseRecord points to unordered records
                this->insertBetweenUnorderedRecords(baseRecord, toInsert);
            }
        }
        if (++totalUnorderedRecords == AUX_FACTOR) {
            this->rebuildAfterInsert();
        }
    }

    long getTotalOrderedRecords() {
        return this->totalOrderedRecords;
    }

};

#endif  //PROYECTO1_BD2_SEQUENTIALFILE_H
