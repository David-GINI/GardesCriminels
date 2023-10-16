#include <iostream>
#include <thread>
#include <mutex>
#include <barrier>

using namespace std;

barrier placesAssises(4);
mutex aBord;
int gardes = 0;
int criminels = 0;
mutex fileGardes;
mutex fileCriminels;
bool chauffeurPret = false;

void initMutex() {
    aBord.lock();
}


int main()
{
    cout << "Hello World!" << endl;
    return 0;
}
