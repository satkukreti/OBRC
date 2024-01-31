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

using namespace std;

int main(int argc, char* argv[]) {
  if(argc != 3){
    cerr << "Error: ./obrc <input_file> <output_file>" << endl;
    exit(1);
  }
  
  //Keeping track of start time
  chrono::time_point<chrono::system_clock> start, end;
  start = chrono::system_clock::now();

  char* fname = argv[1];
  int fd = open(fname, O_RDONLY, S_IRUSR | S_IWUSR);
  struct stat sb;

  if(fstat(fd, &sb) == -1){
    cerr << "Error: could not get file size" << endl;
    exit(1);
  }

  //allocating memory for the input file
  char* filemem = static_cast<char *>(mmap(nullptr, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0));

  
  //Get file input, line by line
  size_t currentPos = 0;
  while (currentPos < static_cast<size_t>(sb.st_size)) {
    const char* line = filemem + currentPos;
    size_t lineLength = strcspn(line, "\n");

    cout.write(line, lineLength);
    cout << endl;

    //moves on to the next line
    currentPos += lineLength + 1;
  }

  /*FILE* fp = fopen(argv[2], "w");

    fprintf(fp, "%d,%d;\n", c, total);*/
    

  //End time
  end = chrono::system_clock::now();
  chrono::duration<double> elapsed_seconds = end - start;
  cout << "Elapsed time: " << elapsed_seconds.count() << "s" << endl;

  munmap(filemem, sb.st_size);
  close(fd);

  return 0;
}
