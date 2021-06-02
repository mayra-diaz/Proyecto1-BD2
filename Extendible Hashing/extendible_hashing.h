#ifndef EXTENDIBLE_HASHING_EXTENDIBLE_HASHING_H
#define EXTENDIBLE_HASHING_EXTENDIBLE_HASHING_H


#include "record.h"

template<typename record>
class extendible_hashing {
private:
    int MAX_DEPTH;
    int BLOCK_FACTOR;
    std::string INDEX_FILE;

    int hash_function(int key) {
        return key % int(pow(2, this->MAX_DEPTH));
    }

    static std::string to_binary(int number, int size) {
        std::string bin = std::bitset<64>(number).to_string();
        return bin.substr(64 - size, size);
    }

    std::string get_bucket_with_key(int hash_key) {
        std::fstream index_file(INDEX_FILE, std::ios::binary | std::ios::in);
        index_file.seekg(((MAX_DEPTH * 2) * hash_key) + MAX_DEPTH);
        char *file = new char[MAX_DEPTH];
        index_file.read(file, sizeof(char) * MAX_DEPTH);
        index_file.close();
        std::string str(file);
        return str;
    }

    void create_buckets() {
        std::fstream initial_bucket0, initial_bucket1;
        int number_of_registers = 0;
        int next_del = -1;
        initial_bucket0.open("../0.dat", std::ios::binary | std::ios::out);
        initial_bucket1.open("../1.dat", std::ios::binary | std::ios::out);
        initial_bucket0.write((char *) &number_of_registers, sizeof(int));
        initial_bucket1.write((char *) &number_of_registers, sizeof(int));
        initial_bucket0.write((char *) &next_del, sizeof(int));
        initial_bucket1.write((char *) &next_del, sizeof(int));
        initial_bucket0.close();
        initial_bucket1.close();
    }

    void clear_file(std::string file_name) {
        std::ofstream ofs;
        ofs.open(file_name, std::ofstream::out | std::ofstream::trunc);
        int number_of_registers = 0;
        int next_del = -1;
        ofs.write((char *) &number_of_registers, sizeof(int));
        ofs.write((char *) &next_del, sizeof(int));
        ofs.close();
    }

    void add_records_to_new_files(std::string bucket_name) {
        std::string old_file_name = "../" + bucket_name + ".dat";
        std::ifstream old_file(old_file_name, std::ifstream::binary);
        int number_of_records;
        int next_del;
        record rec;
        old_file.read((char *) &number_of_records, sizeof(int));
        old_file.read((char *) &next_del, sizeof(int));
        int beg = old_file.tellg();
        old_file.seekg(0, std::ios::end);
        int end = old_file.tellg();
        old_file.seekg(beg);
        int size = end - beg;
        int i = sizeof(record);
        while (i <= size) {
            old_file.read((char *) &rec, sizeof(record));
            if (rec.nextDel == -2) add_record(rec);
            i += sizeof(record);
        }
        old_file.close();
        clear_file(old_file_name);
    }

    void rebuild_index_file(std::string bucket_name) {
        std::fstream index_file(INDEX_FILE, std::ios::binary | std::ios::in | std::ios::out);
        char *name = new char[MAX_DEPTH];
        std::string new_name;
        for (int i = 0; i < pow(2, MAX_DEPTH); ++i) {
            index_file.seekg(0, std::ios::beg);
            index_file.seekg((i * MAX_DEPTH * 2));
            index_file.read(name, sizeof(char) * MAX_DEPTH);
            std::string str(name);
            if (str.substr(str.length() - bucket_name.length()) == bucket_name) {
                new_name = std::string(1, str[str.length() - bucket_name.length() - 1]) + bucket_name;
                index_file.seekg((i * MAX_DEPTH * 2) + MAX_DEPTH);
                char array[MAX_DEPTH];
                strcpy(array, new_name.c_str());
                index_file.write(array, sizeof(char) * MAX_DEPTH);
            }
        }
        index_file.close();
    }

    void split_buckets(std::string bucket_name) {
        std::string first_file_name = "../0" + bucket_name + ".dat";
        std::string second_file_name = "../1" + bucket_name + ".dat";
        std::fstream first_file(first_file_name, std::ios::binary | std::ios::out);
        std::fstream second_file(second_file_name, std::ios::binary | std::ios::out);
        rebuild_index_file(bucket_name);
        int number_of_registers = 0;
        int next_del = -1;
        first_file.write((char *) &number_of_registers, sizeof(int));
        second_file.write((char *) &number_of_registers, sizeof(int));
        first_file.write((char *) &next_del, sizeof(int));
        second_file.write((char *) &next_del, sizeof(int));
        first_file.close();
        second_file.close();
    }

    int search_pos_to_delete(int key, std::string bucket) {
        int pos = 1;
        std::ifstream is(bucket, std::ifstream::binary);
        int number_of_records;
        int next_del;
        record rec;
        is.read((char *) &number_of_records, sizeof(int));
        is.read((char *) &next_del, sizeof(int));
        int beg = is.tellg();
        is.seekg(0, std::ios::end);
        int end = is.tellg();
        is.seekg(beg);
        int size = end - beg;
        int i = sizeof(record);
        while (i <= size) {
            is.read((char *) &rec, sizeof(record));
            if (rec.id == key && rec.nextDel == -2) {
                is.close();
                return pos;
            }
            i += sizeof(record);
            pos += 1;
        }
        is.close();
        return -1;
    }

    void undo_indexes(std::string bucket, std::string other_bucket, std::string new_bucket) {
        std::fstream index_file(INDEX_FILE, std::ios::binary | std::ios::in | std::ios::out);
        char *index = new char[MAX_DEPTH];
        char *buck = new char[MAX_DEPTH];
        for (int i = 0; i < pow(2, MAX_DEPTH); ++i) {
            index_file.read(index, sizeof(char) * MAX_DEPTH);
            index_file.read(buck, sizeof(char) * MAX_DEPTH);
            std::string str1(index);
            std::string str2(buck);
            if (str1.substr(str1.length() - new_bucket.length()) == new_bucket) {
                index_file.seekg((i * MAX_DEPTH * 2) + MAX_DEPTH);
                char array[MAX_DEPTH];
                strcpy(array, new_bucket.c_str());
                index_file.write(array, sizeof(char) * MAX_DEPTH);
            }
        }
        index_file.close();
    }

    void undo_split(std::string bucket, std::string other_bucket) {
        std::string new_file = bucket.substr(1);
        std::string bucket_name = "../" + bucket + ".dat";
        std::string other_bucket_name = "../" + other_bucket + ".dat";
        undo_indexes(bucket, other_bucket, new_file);

        int number_of_records;
        int next_del;
        int beg, end, size, i;
        record rec;

        // Second File
        std::ifstream other_bucket_file(other_bucket_name, std::ios::binary);
        other_bucket_file.read((char *) &number_of_records, sizeof(int));
        other_bucket_file.read((char *) &next_del, sizeof(int));
        beg = other_bucket_file.tellg();
        other_bucket_file.seekg(0, std::ios::end);
        end = other_bucket_file.tellg();
        other_bucket_file.seekg(beg);
        size = end - beg;
        i = sizeof(record);
        while (i <= size) {
            other_bucket_file.read((char *) &rec, sizeof(record));
            if (rec.nextDel == -2) add_record(rec);
            i += sizeof(record);
        }
        other_bucket_file.close();
        clear_file(other_bucket_name);

        // First File
        std::ifstream bucket_file(bucket_name, std::ios::binary);
        bucket_file.read((char *) &number_of_records, sizeof(int));
        bucket_file.read((char *) &next_del, sizeof(int));
        beg = bucket_file.tellg();
        bucket_file.seekg(0, std::ios::end);
        end = bucket_file.tellg();
        bucket_file.seekg(beg);
        size = end - beg;
        i = sizeof(record);
        while (i <= size) {
            bucket_file.read((char *) &rec, sizeof(record));
            if (rec.nextDel == -2) add_record(rec);
            i += sizeof(record);
        }
        bucket_file.close();
        clear_file(bucket_name);
    }

    void check_to_undo_split(std::string bucket_name) {
        if (bucket_name.length() == 1) return;
        std::string other_bucket_name;
        if (bucket_name[0] == '1') other_bucket_name = '0' + bucket_name.substr(1);
        else other_bucket_name = '1' + bucket_name.substr(1);
        std::string other_bucket_file_name = "../" + other_bucket_name + ".dat";
        std::string bucket_file_name = "../" + bucket_name + ".dat";
        std::ifstream bucket(bucket_file_name, std::ios::binary);
        std::ifstream other_bucket(other_bucket_file_name, std::ios::binary);
        int bucket_records, other_bucket_records;
        bucket.read((char *) &bucket_records, sizeof(int));
        other_bucket.read((char *) &other_bucket_records, sizeof(int));
        bucket.close();
        other_bucket.close();
        if (bucket_records + other_bucket_records <= BLOCK_FACTOR) {
            undo_split(bucket_name, other_bucket_name);
        }
    }

public:

    void create_file(std::string file_name, int MAX_DEPTH, int BLOCK_FACTOR) {
        this->INDEX_FILE = std::move(file_name);
        this->MAX_DEPTH = MAX_DEPTH;
        this->BLOCK_FACTOR = BLOCK_FACTOR;
        std::fstream index_file;
        index_file.open(this->INDEX_FILE, std::ios::binary | std::ios::out);
        std::string index, pointer;
        create_buckets();
        for (int i = 0; i < pow(2, this->MAX_DEPTH); ++i) {
            index = to_binary(i, this->MAX_DEPTH);
            int j = i % 2;
            pointer = std::to_string(j);
            char x[MAX_DEPTH];
            char array[MAX_DEPTH];
            strcpy(array, index.c_str());
            strcpy(x, pointer.c_str());
            index_file.write((char *) &array, sizeof(char) * MAX_DEPTH);
            index_file.write((char *) &x, sizeof(char) * MAX_DEPTH);
        }
        index_file.close();
    }

    static void read_index_file(std::string file_name, int MAX_DEPTH) {
        std::ifstream is(file_name, std::ifstream::binary);
        char *buffer = new char[MAX_DEPTH];
        char *j = new char[MAX_DEPTH];
        std::cout<<"INDEX\tFILE\n";
        for (int i = 0; i < pow(2, MAX_DEPTH); ++i) {
            is.read(buffer, sizeof(char) * MAX_DEPTH);
            is.read(j, sizeof(char) * MAX_DEPTH);
            std::cout << buffer <<"\t"<< "../" << j << ".dat\n";
        }
        is.close();
    }

    static void read_bucket_file(std::string file_name) {
        std::ifstream is(file_name, std::ifstream::binary);
        int number_of_records;
        int next_del;
        record rec;
        is.read((char *) &number_of_records, sizeof(int));
        is.read((char *) &next_del, sizeof(int));
        int beg = is.tellg();
        is.seekg(0, std::ios::end);
        int end = is.tellg();
        is.seekg(beg);
        std::cout << "NUMBER OF VALID RECORDS: " << number_of_records << " NEXT RECORD DELETED: " << next_del << "\n";
        int size = end - beg;
        int i = sizeof(record);
        while (i <= size) {
            is.read((char *) &rec, sizeof(record));
            rec.print_record();
            i += sizeof(record);
        }
        is.close();
        std::cout << "END OF FILE\n";
    }

    // Insert Algorithm
    void add_record(record rec) {
        int key_to_insert = hash_function(rec.id);
        std::string bucket_name = get_bucket_with_key(key_to_insert);
        std::string bucket = "../" + bucket_name + ".dat";
        std::fstream bucket_file(bucket, std::ios::binary | std::ios::in | std::ios::out);
        int number_of_records;
        int next_del;
        bucket_file.read((char *) &number_of_records, sizeof(int));
        bucket_file.read((char *) &next_del, sizeof(int));
        if (number_of_records < BLOCK_FACTOR) {
            number_of_records += 1;
            bucket_file.seekg(0, std::ios::beg);
            bucket_file.write((char *) &number_of_records, sizeof(int));
            if (next_del == -1) {
                bucket_file.seekg(0, std::ios::end);
                bucket_file.write((char *) &rec, sizeof(record));
            } else {
                record eliminated_rec;
                bucket_file.seekg((2 * sizeof(int)) + ((next_del - 1) * sizeof(record)));
                bucket_file.read((char *) &eliminated_rec, sizeof(record));
                int new_next_del = eliminated_rec.nextDel;
                bucket_file.seekg((2 * sizeof(int)) + ((next_del - 1) * sizeof(record)));
                bucket_file.write((char *) &rec, sizeof(record));
                bucket_file.seekg(sizeof(int));
                bucket_file.write((char *) &new_next_del, sizeof(int));
            }
        } else {
            if (bucket_name.length() >= MAX_DEPTH) {
                number_of_records += 1;
                bucket_file.seekg(0, std::ios::beg);
                bucket_file.write((char *) &number_of_records, sizeof(int));
                bucket_file.seekg(0, std::ios::end);
                bucket_file.write((char *) &rec, sizeof(record));
            } else {
                split_buckets(bucket_name);
                add_records_to_new_files(bucket_name);
                add_record(rec);
            }
        }
        bucket_file.close();
    }

    // Search Algorithm
    std::vector<record> search_record(int key) {
        std::vector<record> records_with_key;
        int hash_key = hash_function(key);
        std::string bucket_to_search = "../" + get_bucket_with_key(hash_key) + ".dat";
        std::ifstream is(bucket_to_search, std::ifstream::binary);
        int number_of_records;
        int next_del;
        record rec;
        is.read((char *) &number_of_records, sizeof(int));
        is.read((char *) &next_del, sizeof(int));
        for (int i = 0; i < number_of_records; ++i) {
            is.read((char *) &rec, sizeof(record));
            if (rec.id == key && rec.nextDel == -2) records_with_key.push_back(rec);
        }
        is.close();
        return records_with_key;
    }

    // Range Search Algorithm
    std::vector<record> search_record_range(int begin_key, int end_key) {
        std::vector<record> records_in_range_with_key;
        if (end_key < begin_key) {
            std::cout << "Invalid Arguments\n";
            return records_in_range_with_key;
        }
        for (int i = begin_key; i < end_key + 1; ++i) {
            std::vector<record> records_with_key = search_record(i);
            for (auto &j: records_with_key) {
                records_in_range_with_key.push_back(j);
            }
        }
    }

    // Deletion Algorithm. Delete the first occurrence of a record with a certain key
    bool delete_record(int key) {
        int hash_key = hash_function(key);
        std::string bucket = get_bucket_with_key(hash_key);
        std::string bucket_to_delete = "../" + bucket + ".dat";
        std::fstream fs(bucket_to_delete, std::ios::binary | std::ios::in | std::ios::out);
        int number_of_records;
        int next_del;
        record rec;
        fs.read((char *) &number_of_records, sizeof(int));
        fs.read((char *) &next_del, sizeof(int));
        number_of_records -= 1;
        int pos = search_pos_to_delete(key, bucket_to_delete);
        if (pos == -1) return false;
        fs.seekg(0, std::ios::beg);
        fs.write((char *) &number_of_records, sizeof(int));
        fs.write((char *) &pos, sizeof(int));
        fs.seekg((2 * sizeof(int)) + ((pos - 1) * sizeof(record)));
        fs.read((char *) &rec, sizeof(record));
        rec.nextDel = next_del;
        fs.seekg((2 * sizeof(int)) + ((pos - 1) * sizeof(record)));
        fs.write((char *) &rec, sizeof(record));
        fs.close();
        check_to_undo_split(bucket);
    }
};

#endif //EXTENDIBLE_HASHING_EXTENDIBLE_HASHING_H
