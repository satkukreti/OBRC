#include <sys/types.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <iostream>
#include <chrono>
#include <fstream>
#include <cstring>
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <set>
#include <thread>
#include <mutex>
#include <cstddef>
#include "flat_hash_map.hpp"

struct City{
  double high;
  double low;
  double sum;
  int num;
  City(){
    high = 0.0;
    low = 0.0;
    sum = 0.0;
    num = 0;
  }
};

using namespace std;

ska::flat_hash_map<string, City> umap;
set<string> ordered;
mutex tsafe;

void processChunk(char* filemem, size_t start, size_t end, size_t sb){
  //Get file input, line by line
  if(start != 0){
    size_t l = strcspn(filemem + start, "\n");
    start += l;
    start++;
  }

  size_t m = strcspn(filemem + end, "\n");
  end += m;
  if(end >= sb){ end = sb-1;}

  lock_guard<mutex> lock(tsafe);
  size_t currentPos = start;
  while(currentPos < end){
    const char* line = filemem + currentPos;

    size_t lineLength = strcspn(line, "\n");
    if(lineLength == 0){currentPos++; continue;}
	
    size_t cnamepos = strcspn(line, ";");
    if(cnamepos == lineLength){currentPos += lineLength+1; continue;}

    string cname(line, cnamepos);
    string temp(line + cnamepos + 1, lineLength - cnamepos);
    double ctemp = stod(temp);
    {
        ordered.insert(cname);
        City& c = umap[cname];
        c.sum += ctemp;
        c.num++;
        if (ctemp < c.low) { c.low = ctemp; }
        if (ctemp > c.high) { c.high = ctemp; }
     }

    currentPos += lineLength + 1;
  }
}

int main(int argc, char* argv[]) {
  if(argc != 2){
    cerr << "Error: ./obrc <input_file>" << endl;
    exit(1);
  }
  
  //Keeping track of start time
  chrono::time_point<chrono::system_clock> s, e;
  s = chrono::system_clock::now();

  char* fname = argv[1];
  int fd = open(fname, O_RDONLY, S_IRUSR | S_IWUSR);
  struct stat sb;

  if(fstat(fd, &sb) == -1){
    cerr << "Error: could not get file size" << endl;
    exit(1);
  }

  //allocating memory for the input file
  char* filemem = static_cast<char *>(mmap(nullptr, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0));

  int nThreads = thread::hardware_concurrency();
  size_t chunkSize = sb.st_size/nThreads;

  vector<thread> threads;
  for (int i = 0; i < nThreads - 1; i++) {
      size_t start = i * chunkSize;
      size_t end = (i + 1) * chunkSize;
      threads.emplace_back(processChunk, filemem, start, end, sb.st_size);
  }

  size_t start = (nThreads - 1) * chunkSize;
  size_t end = sb.st_size;
  threads.emplace_back(processChunk, filemem, start, end, sb.st_size);

  for(auto& thread: threads){
    thread.join();
  }
  
  for (string str : ordered) {
      cout << str << endl;
      double calculate = umap[str].sum / umap[str].num;
      cout << calculate << " " << umap[str].low << " " << umap[str].high << endl;
  }
  
  //End time
  e = chrono::system_clock::now();
  chrono::duration<double> elapsed_seconds = e - s;
  cout << "Elapsed time: " << elapsed_seconds.count() << "s" << endl;

  munmap(filemem, sb.st_size);
  close(fd);

  return 0;
}
