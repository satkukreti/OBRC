#include <iostream>
#include <chrono>

using namespace std;

int main() {
  //Keeping track of start time
  chrono::time_point<chrono::system_clock> start, end;
  start = chrono::system_clock::now();
  
  cout << "Hello World!" << endl;

  //End time
  end = chrono::system_clock::now();
  chrono::duration<double> elapsed_seconds = end - start;
  cout << "Elapsed time: " << elapsed_seconds.count() << "s" << endl;


  return 0;
}
