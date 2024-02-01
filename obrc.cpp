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

using namespace std;

int main(int argc, char* argv[]) {
  if(argc != 3){
    cerr << "Error: ./obrc <input_file> <output_file>" << endl;
    exit(1);
  }
  
  //Keeping track of start time
  chrono::time_point<chrono::system_clock> start, end;
  start = chrono::system_clock::now();

  unordered_map<string, vector<double>> umap;
  unordered_map<string, vector<double>>::iterator itr;
  vector<string> ordered;

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
    //Delimiters
    size_t lineLength = strcspn(line, "\n");
    size_t cnamepos = strcspn(line, ";");

    //Parsing out the name and temp
    string cname (line, cnamepos);
    string temp (line+cnamepos+1, lineLength-cnamepos);
    double ctemp = stod(temp);

    ordered.push_back(cname);

    if(umap.find(cname) == umap.end()){
      vector<double> dtemp (4);
      dtemp[0] = ctemp;
      dtemp[1] = 1;
      dtemp[2] = ctemp;
      dtemp[3] = ctemp;
      
      umap[cname] = dtemp;
    } else {
      umap[cname][0] += ctemp;
      umap[cname][1]++;
      if(ctemp < umap[cname][2]){
	umap[cname][2] = ctemp;
      }
      if(ctemp > umap[cname][3]){
	umap[cname][3] = ctemp;
      }
    }
    
    //cout << cname << endl;
    //cout << ctemp << endl;
    //cout.write(line, lineLength);
    //cout << endl;

    //moves on to the next line
    currentPos += lineLength + 1;
  }

  /*for(itr = umap.begin(); itr != umap.end(); itr++){
    cout << itr->first << endl;
    double calculate = itr->second[0]/itr->second[1];
    cout << calculate << endl;
    }*/
  sort(ordered.begin(), ordered.end());
  for(string str: ordered){
    cout << str << endl;
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
