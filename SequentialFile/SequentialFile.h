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

    unsigned long mainFileRecords{};
    unsigned long auxFileRecords{};

    bool static compare(SFRecord<RecordType> a, SFRecord<RecordType> b) {
        return (strcmp(a.getKey().c_str(), b.getKey().c_str()) <= 0);
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
            //sequentialFile.writeFile((char *) &record, sizeof(record));
            mainFileRecords++;
            //this->insertRecord(record);
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
        auxFileRecords = 0;
    }

    unsigned long getLenghtOfFile(const std::string &newFileName) {
        std::ifstream file(newFileName, std::ios::ate | std::ios::binary);
        unsigned long size = file.tellg();
        file.close();
        return size;
    }

    void createFreeList() {
        std::fstream file(this->headerFileName, std::ios::out);
        long headerPointer = -1;
        file.seekp(0);
        file.write((char *) &headerPointer, sizeof(headerPointer));
        file.close();
    }

    SFRecord<RecordType> getPrevRecord(SFRecord<RecordType> record) {
        if (record.nextReg != -2) {
            record = this->readFile(this->dataFileName, record.nextReg);
            record = this->readFile(this->dataFileName, record.prevReg - 1);
        } else {
            record = this->readFile(this->dataFileName, mainFileRecords - 2);
        }
        return record;
    }

    void pointersUpdate(SFRecord<RecordType> newRecord, SFRecord<RecordType> currentRecord) {
        long prevRecordPos = currentRecord.prevReg;
        SFRecord<RecordType> prevRecord = this->readFile(this->dataFileName, prevRecordPos);

        long currentPos = prevRecord.nextReg;
        long newRecordPos = this->insertHere();

        prevRecord.nextReg = newRecordPos;
        this->writeFile(prevRecord, this->dataFileName, prevRecordPos);

        newRecord.nextReg = currentPos;
        newRecord.prevReg = prevRecordPos;
        this->writeFile(newRecord, this->dataFileName, newRecordPos);

        currentRecord.prevReg = newRecordPos;
        this->writeFile(currentRecord, this->dataFileName, currentPos);
    }

    void pointerUpdateDeletition(SFRecord<RecordType> badRecord, long toDeleteLogPos) {
        switch (badRecord.prevReg) {
            case -1: {
                SFRecord<RecordType> next = this->readFile(this->dataFileName, badRecord.nextReg);
                next.prevReg = -1;
                this->writeFile(next, this->dataFileName, badRecord.nextReg);
                break;
            }
            case -2: {
                SFRecord<RecordType> prev;
                prev = this->readFile(this->dataFileName, badRecord.prevReg);
                prev.nextReg = -2;
                this->writeFile(prev, this->dataFileName, badRecord.prevReg);
                break;
            }
            default: {
                SFRecord<RecordType> prev, next;
                prev = this->readFile(this->dataFileName, badRecord.prevReg);
                next = this->readFile(this->dataFileName, badRecord.nextReg);
                prev.nextReg = badRecord.nextReg;
                next.prevReg = badRecord.prevReg;
                this->writeFile(prev, this->dataFileName, badRecord.prevReg);
                this->writeFile(next, this->dataFileName, badRecord.nextReg);
                break;
            }
        }
    }

    long getFirstRecordPos() {
        long currentRecordLogPos = 0;
        SFRecord<RecordType> currentRecord = this->readFile(this->dataFileName, currentRecordLogPos);
        while (currentRecord.prevReg == -1) {
            currentRecord = this->readFile(this->dataFileName, ++currentRecordLogPos);
        }
        return currentRecordLogPos - 1;
    }

    void insertAtTheBeginning(SFRecord<RecordType> newRecord) {
        SFRecord<RecordType> firstRecord = this->readFile(this->dataFileName, this->getFirstRecordPos());
        SFRecord<RecordType> secondRecord = this->readFile(this->dataFileName, firstRecord.nextReg);

        long newRecordPos = this->insertHere();

        newRecord.nextReg = newRecordPos;
        secondRecord.prevReg = newRecordPos;
        newRecord.prevReg = -1;
        firstRecord.prevReg = 0;

        this->writeFile(newRecord, this->dataFileName, this->getFirstRecordPos());
        this->writeFile(firstRecord, this->dataFileName, newRecordPos);
        this->writeFile(secondRecord, this->dataFileName, firstRecord.nextReg);
    }

    void nullBeginningInsertion(SFRecord<RecordType> current, SFRecord<RecordType> record) {
        SFRecord<RecordType> prev = this->readFile(this->dataFileName, current.prevReg);

        record.nextReg = -2;
        record.prevReg = prev.nextReg;

        long newPos = this->insertHere();
        current.nextReg = newPos;

        this->writeFile(current, this->dataFileName, prev.nextReg);
        this->writeFile(record, this->dataFileName, newPos);
    }

    void insertAtTheEnd(SFRecord<RecordType> newRecord) {
        SFRecord<RecordType> lastRecord = this->readFile(this->dataFileName, mainFileRecords - 1);

        newRecord.nextReg = -2;
        newRecord.prevReg = mainFileRecords - 1;
        long newPos = this->insertHere();
        lastRecord.nextReg = newPos;

        this->writeFile(lastRecord, this->dataFileName, mainFileRecords - 1);
        this->writeFile(newRecord, this->dataFileName, newPos);
    }

    void basicInsert(SFRecord<RecordType> base, SFRecord<RecordType> record) {
        SFRecord<RecordType> baseNext = this->readFile(this->dataFileName, base.nextReg);

        long currentPos = this->insertHere();
        long basePos = baseNext.prevReg;
        long baseNextPos = base.nextReg;

        base.nextReg = currentPos;
        record.prevReg = basePos;
        record.nextReg = baseNextPos;
        baseNext.prevReg = currentPos;

        this->writeFile(base, this->dataFileName, basePos);
        this->writeFile(record, this->dataFileName, currentPos);
        this->writeFile(baseNext, this->dataFileName, baseNextPos);
    }

    void rebuildCall() {
        this->rebuild();
        mainFileRecords += AUX_FACTOR;
        auxFileRecords = 0;
    }

    void rebuild() {
        std::fstream sequentialFile(this->dataFileName);
        std::fstream auxFile(this->auxFileName, std::ios::out);

        SFRecord<RecordType> record = this->readFile(this->dataFileName, this->getFirstRecordPos());
        while (record.nextReg != -2) {
            auxFile.write((char *) &record, sizeof(SFRecord<RecordType>));
            record = this->readFile(this->dataFileName, record.nextReg);
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

    SFRecord<RecordType> searchInOrderedRecords(KeyType key) {
        long low = 0, high = mainFileRecords - 1, mid;

        SFRecord<RecordType> current;
        while (low <= high) {
            mid = (low + high) / 2;
            current = this->readFile(this->dataFileName, mid);
            KeyType currentID = current.getKey();

            if (currentID < key) {
                low = mid + 1;
            } else if (currentID > key) {
                high = mid - 1;
            } else {
                return current;
            }
        }
        return current;
    }

    void deleteInMainFile(long toDeleteLogPos) {
        this->rebuild();
        mainFileRecords = mainFileRecords + auxFileRecords;
        auxFileRecords = 0;
    }

    void deleteInAuxFile(long toDeleteLogPos) {
        long headerTemp = readHeader();
        writeHeader(toDeleteLogPos);
        SFRecord<RecordType> toDelete("-1", headerTemp);
        this->writeFile(toDelete, this->dataFileName, toDeleteLogPos);
    }

    long getPos(SFRecord<RecordType> record) {
        if (record.prevReg == -1) {
            return 0;
        } else {
            SFRecord<RecordType> prevRecord = this->readFile(this->dataFileName, record.prevReg);
            return prevRecord.nextReg;
        }
    }

    long insertHere() {
        long currentHeader = readHeader();
        if (currentHeader == -1) {
            return mainFileRecords + auxFileRecords;
        } else {
            SFRecord<RecordType> deleted = readFile(this->dataFileName, currentHeader);
            writeHeader(deleted.nextReg);
            return currentHeader;
        }
    }

    SFRecord<RecordType> readFile(const std::string &fileName, long position) {
        std::fstream file(fileName);
        SFRecord<RecordType> record;
        file.seekg(position * sizeof(SFRecord<RecordType>));
        file.read((char *) &record, sizeof(SFRecord<RecordType>));
        file.close();
        return record;
    }

    void writeFile(SFRecord<RecordType> record, const std::string &fileName, long position) {
        std::fstream file(fileName);
        file.seekp(position * sizeof(SFRecord<RecordType>));
        file.write((char *) &record, sizeof(SFRecord<RecordType>));
        file.close();
    }

    void insertMiddleAuxFile(SFRecord<RecordType> baseRecord, SFRecord<RecordType> toInsert) {
        SFRecord<RecordType> current;

        long baseRecordLogPos;
        SFRecord<RecordType> baseRecordNext = this->readFile(this->dataFileName, baseRecord.nextReg);
        baseRecordLogPos = baseRecordNext.prevReg;

        current = this->readFile(this->dataFileName, baseRecordLogPos);
        while (current.nextReg > mainFileRecords - 1 && current.nextReg != -2 &&
               current.getKey() < toInsert.getKey()) {
            if (current.getKey() == toInsert.getKey()) {
                throw std::out_of_range("User attempted to insertRecord an already existing key");
            }
            current = this->readFile(this->dataFileName, current.nextReg);
        }
        if (current.getKey() == toInsert.getKey()) {
            throw std::out_of_range("User attempted to insertRecord an already existing key");
        }
        if (current.nextReg < mainFileRecords) {
            if (current.getKey() > toInsert.getKey()) {
                this->pointersUpdate(toInsert, current);
            } else {
                if (current.nextReg == -2) {
                    this->nullBeginningInsertion(current, toInsert);
                } else {
                    long currentNextLogPos = current.nextReg;
                    SFRecord<RecordType> currentNext = this->readFile(this->dataFileName, currentNextLogPos);
                    this->pointersUpdate(toInsert, currentNext);
                }
            }
        } else {
            this->pointersUpdate(toInsert, current);
        }
    }

public:

    explicit SequentialFile(const str &baseFileName, const str &bname) {
        str name = "../SequentialFile/data/" + bname;
        this->headerFileName = name + "Header.bin";
        this->dataFileName = name + "Data.bin";
        this->auxFileName = name + "Aux.bin";

        this->createBinFromCSV(baseFileName);
        this->createFreeList();
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

    SFRecord<RecordType> punctualSearch(KeyType key) {
        SFRecord<RecordType> baseRecord = this->searchInOrderedRecords(key);

        if (baseRecord.getKey() == key)
            return baseRecord;
        if ((baseRecord.prevReg == -1 && key < baseRecord.getKey()) ||
            (baseRecord.nextReg == -2 && key > baseRecord.getKey())) {
            throw std::out_of_range("Search out of range. Key: " + key);
        }

        if (baseRecord.getKey() > key)
            baseRecord = this->getPrevRecord(baseRecord);

        SFRecord<RecordType> current = baseRecord;
        current = this->readFile(this->dataFileName, current.nextReg);
        while (current.getKey() <= key) {
            if (current.getKey() == key)
                return current;
            current = this->readFile(this->dataFileName, current.nextReg);
        }
        throw std::out_of_range("Search out of range. key: " + key);
    }

    std::vector<SFRecord<RecordType>> rangeSearch(KeyType begin, KeyType end) {
        if (begin > end)
            std::swap(begin, end);
        SFRecord<RecordType> current = this->searchInOrderedRecords(begin);
        if (current.prevReg != -1) {
            if (current.getKey() > begin)
                current = this->getPrevRecord(current);
        }

        std::vector<SFRecord<RecordType>> searchResult;

        while (true) {
            if (current.getKey() >= begin && current.getKey() <= end)
                searchResult.push_back(current);
            if (current.getKey() > end || current.nextReg == -2)
                return searchResult;
            if(current.nextReg == -2)
                break;
            current = this->readFile(this->dataFileName, current.nextReg);
        }
    }

    void deleteRecord(KeyType key) {
        SFRecord<RecordType> badRecord = this->punctualSearch(key);

        if (badRecord.getKey() != key) {
            throw std::out_of_range("Record with key " + key + " not found.");
        }

        long deletePos = this->getPos(badRecord);
        this->pointerUpdateDeletition(badRecord, deletePos);

        if (deletePos < mainFileRecords) {
            this->deleteInMainFile(deletePos);
            --mainFileRecords;
        } else {
            this->deleteInAuxFile(deletePos);
            --auxFileRecords;
        }
    }

    void insertRecord(SFRecord<RecordType> toInsert) {
        SFRecord<RecordType> baseRecord = this->searchInOrderedRecords(toInsert.getKey());

        if (baseRecord.getKey() == toInsert.getKey())
            throw std::out_of_range("User attempted to insertRecord an already existing key");

        if (baseRecord.prevReg == -1 && toInsert.getKey() < baseRecord.getKey())
            this->insertAtTheBeginning(toInsert);
        else if (baseRecord.nextReg == -2)
            this->insertAtTheEnd(toInsert);
        else {
            if (baseRecord.getKey() > toInsert.getKey())
                baseRecord = this->getPrevRecord(baseRecord);
            if (baseRecord.nextReg < mainFileRecords)
                this->basicInsert(baseRecord, toInsert);
            else
                this->insertMiddleAuxFile(baseRecord, toInsert);
        }
        if (++auxFileRecords == AUX_FACTOR)
            this->rebuildCall();
    }
};

#endif  //PROYECTO1_BD2_SEQUENTIALFILE_H
