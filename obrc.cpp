#include <iostream>
#include <chrono>
#include <sstream>
#include <fstream>
#include <string>

using namespace std;

int main(int argc, char* argv[]) {
  if(argc != 3){
    cerr << "Error: <input_file> <output_file>" << endl;
    exit(1);
  }
  
  //Keeping track of start time
  chrono::time_point<chrono::system_clock> start, end;
  start = chrono::system_clock::now();

  string fname = argv[1];
  ifstream inFile;
  inFile.open(fname);

  if(!inFile) {
    cerr << "Error: file cannot be opened" << endl;
    exit(1);
  }

  string line;
  while(getline(inFile, line)){
    size_t pos = line.rfind(';');
    string cityName = line.substr(0, pos);
    double cityTemp = stod(line.substr(pos+1));
    cout << cityName << endl;
    cout << cityTemp << endl;
    
  }

  /*FILE* fp = fopen(argv[2], "w");

    fprintf(fp, "%d,%d;\n", c, total);*/
    

  //End time
  end = chrono::system_clock::now();
  chrono::duration<double> elapsed_seconds = end - start;
  cout << "Elapsed time: " << elapsed_seconds.count() << "s" << endl;

  inFile.close();

  return 0;
}
