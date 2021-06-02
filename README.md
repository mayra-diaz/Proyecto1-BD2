# Proyecto 1 - Organización de Archivos

**Curso: Base de datos II**
**Profesor: Heider Sánchez**
**Integrantes:**
- Joaquín Elías Ramírez Gutiérrez 201910277
- Paolo André Morey Tutiven 201910236
- Mayra Díaz Tramontana 201910

## Introducción

### Objetivo del proyecto
- Entender y aplicar algoritmos de almacenamiento de archivos físicos.
- Implementar estructuras de organización de archivos y medir su eficiencia.
- Contrastar los métodos utilizados y analizar los resultados.


### Descripción del dominio de datos a usar
Para este proyecto hemos utilizado data de [Kaggle](https://www.kaggle.com/datasets). Los archivos fueron extraídos de [FIFA VIDEO ASSISTANT REFEREES](https://www.kaggle.com/sauravjoshi23/fifa-video-assistant-referees-var?select=VAR_Team_Stats.csv), los cuales son "Teams.csv" y "Incidents.csv".

- **Teams:** 
    Consiste en un resumen personalizado respecto al balance de decisiones en las que el VAR (Video Assistant Referee) para cada uno de los veinte equipos ingleses participantes en la Premier League en la temporada 2019-20. Cada entrada cuenta con el nombre del quipo, seguido de nueve son datos numéricos entorno a goles, carácter de decisiones, entre otros. 
- **Incidents:** 
    Se trata de una bitácora de todos los incidentes en las que el VAR intervino a lo largo de la temporada 2019-20 en la Premier League de Inglaterra. Cada tupla contiene a los equipos involucrados, la fecha del evento, una descripción de los hechos, y el estado acerca de la decisión en contra del equipo (a favor o en contra).


### Resultados esperados
Dado `EH = Extendible Hashing` y `SF = Sequential File` Se espera que:
- Búsqueda puntal en SF es más costosa en tiempo que en EH.
- Búsqueda por rangos en SF menos costosa en tiempo que en EH.

## Técnicas Implementadas

### Sequential File

#### Header, Data y Aux
En la implementación optamos por manejar tres archivos. El Header es simplemente un archivo que nos indica la posición del primer archivo eliminado. Si éste es -1, significa que no hay eliminados. Si no lo es, el proceso de lógica sigue el de Free List LIFO. El archivo Data es el que contiene los registros ordenados físicamente. El Aux es un archivo temporal que se utiliza como apoyo a la hora de hacer un ordenamiento total de todos los registros (ya que los nuevos registros se almacenan al final de Data).

El constructor se encarga de convertir el archivo csv a un archivo bin, agregando el puntero siguiente y previo a cada registro.

#### Inserción
Ya data.bin está físicamente ordenado bajo un atributo `key`, por lo que las inserciones que se hacen utilizando la búsqueda binaria. Primero que todo, se verifica que la llave primaria aún no exista. Si este es el caso, se hace una búsqueda binaria, la cual se hace con facilidad gracias a los punteros previos y siguientes de cada registro. Una vez encontrada la posición lógica en la que debe ser insertado el nuevo registro, se modifican los punteros de la tupla previa y siguiente, y el nuevo registro se añade al final del archivo. Se actualiza el número de archivos desordenados, y si es que éste alcanzó el `AUX_FACTOR`, entonces hay que hacer un `rebuild` de todo el data.bin, para que esté nuevamente ordenado de manera física. Es así que el costo promedio es `O(lg(n))`, mientras que el peor caso es `O(n)`. 

```c++
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
```

#### Búsqueda
Lo primero que hacemos es buscar de manera binaria el posible candidato de registro bajo el atributo llave. Si es que lo encontramos, simplemente lo devolvemos. Si se da el caso de que la llave está fuera del rango, botamos un error. Si el candidato de registro tiene una llave mayor a la `key` de búsqueda, entonces accedemos al anterior de este. Finalmente, vamos a recorrer los registros a partir del candidato. Si es que se encontró lo devolvemos, si no, continuamos buscando mientras la llave sea menor o igual a la 
`key`. Si nunca lo encontró, se indica un error.

El costo promedio es ideal es`O(lg(n))`, mientras que el peor caso es cuando el tiempo de ejecución es `O(n)`.
```c++
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
```

#### Búsqueda por rango
Primero hay que validar que los rangos estén en orden creciente. Después buscamos `begin` y el registro en donde podría estar. Se verifica que éste key sea mayor y si lo es se cambia el registro por el de la izquierda.

Después se busca durante todo el rango, desde `begin` hasta `end`. Si es que se encuentran llaves dentro de este que verifiquen la condición y se añaden al vector de salida si es que lo cumplen. El costo de este algoritmo es `O(lg(n) + n)`, lo que en sí es `O(n)`.

```c++
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
```

#### Eliminación
Para eliminar un registro se hace búsqueda binaria bajo el atributo llave. Si es que sí se encontró, se procede a buscar la posición de este registro (puede estar en la parte ordenada o en la parte desordenada de data.bin). Luego procedemos a actualizar los punteros, de tal forma que los registros activos ya no tomen en cuenta al eliminado. Finalmente validamos si es que la posición del registro eliminado pertenece a los registros ordenados, y si es así, lo que hacemos es un `rebuild` para organizarlo de nuevo. Si no es así, solo vamos a la posición de este y sobreescribimos este registro, indicando con los punteros que ya ha sido eliminado.

El costo promedio de la eliminación es `O(lg(n))`, y en el peor de los casos es `O(n)`.

```c++
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
```
### Extendible Hashing

#### Indexación de archivos
Para la indexación de archivos, simplemente se aplicó la técnica vista en clase, en la que se iban a crear 2^MAX_DEPTH posibles llaves para la data, y a cada una de estas llaves se le iba a asignar un archivo de cierta profundidad (empezando en 1), la cual iba aumentando dependiendo de las inserciones de registros a la estructura.

El algoritmo para crear el archivo index.dat, que iba a contener toda la lógica de indexación es el siguiente:

```c++
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
```

Además, se crearon también los buckets iniciales de profundidad 1 (1 y 0) de la siguiente forma:

```c++
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
```

#### Inserción

Existen múltiples casos al momento de insertar en esta estructura, ya sea si el número de registros en un bucket excede el factor de este o no, siendo necesario hacer un split a los buckets para poder mantener la regla en el caso que no. Además, tenemos que tomar en cuenta el caso en el que no se pueden hacer mas splits ya que se excedería con la profundidad máxima. Este caso se llama overflow y la estrategia que utilizaremos será seguir insertando en el archivo una vez que este llegue a su máxima profunidad.

El algoritmo para implementar la inserción es el siguiente:

```c++
void add_record(record rec) {
        int key_to_insert = hash_function(rec.id);
        std::string bucket_name = get_bucket_with_key(key_to_insert);
        std::string bucket = "../" + bucket_name + ".dat";
        std::fstream bucket_file(bucket, std::ios::binary | std::ios::in | std::ios::out);
        int number_of_records;
        int next_del;
        bucket_file.read((char *) &number_of_records, sizeof(int));
        bucket_file.read((char *) &next_del, sizeof(int));
        if (!(number_of_records >= BLOCK_FACTOR)) {
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
    }
```

Nota: Al usar la estrategia de free list para eliminados (explicado posteriormente), se chancará el nuevo registro en el registro eliminado, si es que existe alguno.

#### Búsqueda

Para el algoritmo de búsqueda en base a un key, lo que se hizo fue buscar en donde debería estar ubicado cierto registro con la key recibida como párametro. Para buscarlo, se aplico la función hash (key módulo 2 elevado a profundidad máxima) y se localizó el archivo en nuestra indexación en base al resultado devuelto en hash. Se itero en todo el bucket y se añadió en un vector los registros que coinciden con la key.

El algoritmo para la búsqueda es el siguiente:
```c++
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
        return records_with_key;
    }
```

Nota: Si no se encuentra algún registro con la key dada, se retornará un vector vacío.

#### Búsqueda por rango
La estructura de Extendible Hashing no está ordenada mediante los keys, los registros solo se colocan en buckets dependiendo de la función hash, por lo que no están ordenados. Debido a esto, es díficil encontrar una solución óptima al problema de búsqueda por rango, incluso al de la búsqueda normal. Debido a esto, se implemento una búsqueda por key para cada key entre el rango entre una key de inicio y una key final. La función retorna un vector con todo lo encontrado en base a lo que se indicó anteriormente.

El algoritmo para la búsqueda por rango es el siguiente:

```c++
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
```


#### Eliminación 
Para la eliminación se uso la estrategia de Free List. Cada bucket tiene una cabecera, el cuál tiene el número de registros válidos en el archivo y también un puntero a la posición lógica del siguiente eliminado. Se implemento la estrategia de LIFO, para que las inserciones sean en 0(1) , sin embargo, la eliminación será O(n) ya que tenemos que encontrar el registro con la key dada en el bucket que va a ser eliminado, ya que el bucket no esta ordenado.

El algoritmo para la eliminación es el siguiente:

```c++
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
```

Nota: El valor del puntero a siguiente eliminado es -2 si es un registro que no está eliminado, es -1 si es el último eliminado y es otro valor si es que no es el último eliminado (puntero lógico al siguiente registro eliminado).

## Resultados Experimentales

### Sequential File
|       Función       | Accesos a memoria |
|:-------------------:|:-----------------:|
|       Insertar      |   lg (n) + m      |
|       Eliminar      |       lg (n)      |
|   Búsqueda puntual  |       lg(n)       |
| Búsqueda por rangos |     lg(n) + m     |

### Extendible Hashing
|       Función       | Accesos a memoria |
|:-------------------:|:-----------------:|
|       Insertar      |         C         |
|       Eliminar      |         n         |
|   Búsqueda puntual  |         1         |
| Búsqueda por rangos |         n         |

## Pruebas de uso y presentación
Las pruebas se encuentran en el video mostrado posteriormente

## Video de demostración

https://bit.ly/3vKDvCO
