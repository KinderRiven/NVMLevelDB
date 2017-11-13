#include <iostream>
#include <sstream>
#include "leveldb/db.h"
#include "leveldb/table_builder.h"

uint64_t clflush_cnt = 0;
uint64_t WRITE_LATENCY_IN_NS = 0;


int main(int argc, char** argv) {
  if (argc < 2) {
    printf("file arg\n");
    return 1;
  }
  struct timespec start, end;
  int numData = 10000000;
  leveldb::DB* db;
  leveldb::Options options;
  options.filter_policy = NULL;
  options.create_if_missing = true;
  options.compression = leveldb::kNoCompression;
  options.max_file_size = 8 << 20;
  char *c = argv[1];
  std::string dbpath(c);
  leveldb::Status status = leveldb::DB::Open(options, dbpath, &db);
  assert(status.ok());
  clock_gettime(CLOCK_MONOTONIC, &start);
  for (auto i = 0; i < numData; i++) {
    std::stringstream key;
    key << "key0key" << i ;
    std::stringstream value;
    value << "value" << i ;
    status = db->Put(leveldb::WriteOptions(), key.str(), value.str());
  }
  clock_gettime(CLOCK_MONOTONIC, &end);
  int64_t elapsed = (end.tv_sec - start.tv_sec)*1000000000 + (end.tv_nsec - start.tv_nsec);
  std::cout << elapsed/1000 << "\tusec\t" << (uint64_t)(1000000*(numData/(elapsed/1000.0))) << "\tOps/sec\tInsertion" << std::endl;

  clock_gettime(CLOCK_MONOTONIC, &start);
  for (auto i = 0; i < numData; i++) {
    std::string v = "value" + std::to_string(i);
    std::stringstream key;
    key << "key0key" << i;
    std::string value;
    status = db->Get(leveldb::ReadOptions(), key.str(), &value);
    if (v.compare(value) != 0) {
      printf("%s %s %s\n", key.str().data(), v.data(), value.data());
    }
  }
  clock_gettime(CLOCK_MONOTONIC, &end);
  elapsed = (end.tv_sec - start.tv_sec)*1000000000 + (end.tv_nsec - start.tv_nsec);
  std::cout << elapsed/1000 << "\tusec\t" << (uint64_t)(1000000*(numData/(elapsed/1000.0))) << "\tOps/sec\tRead" << std::endl;
}