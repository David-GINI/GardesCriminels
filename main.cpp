#include <iostream>
#include <thread>
#include <random>
#include <mutex>
#include <condition_variable>

using namespace std;

mutex mtx;
condition_variable detenusQueue;
condition_variable guardesQueue;
bool isCaptain = false;
int detenus = 0;
int guardes = 0;
int totalDetenus = 0;
int totalGuardes = 0;
int remaining = 4;  // variable qui comptent le nombre de personnes a attendre avant de partir

void board(int id, string type) {
    cout << type << " " << id << " monte dans la navette de transport." << endl;

}

void start() {
    cout << "La navette demarre. Une personne a appuyé sur le bouton pour déclencher l'automatisme." << endl;
}

void detenu(int id) {
    unique_lock<mutex> lock(mtx);// On cree un verrou unique pour le mutex mtx.
    detenus++;

    if (detenus == 4) {
        detenusQueue.notify_all();// On rapelle tous les detenus en attente dans la queue.
        detenus = 0;
        isCaptain = true;
    } else if (detenus == 2 && guardes >= 2) {
        detenusQueue.notify_all(); // On rapelle tous les detenus en attente dans la queue.
        guardesQueue.notify_all(); // On rapelle tous les gardes en attente dans la queue.
        guardes -= 2;
        detenus = 0;
        isCaptain = true;
    } else {
        detenusQueue.wait(lock);// sinon, attend dans la queue des detenus jusqu'à ce qu'il soit rapeller dans une des conditions ci dessus ou que le verrou soit unlock
    }


    board(id, "Le detenu"); // Le detenu actuel embarque

    if (--remaining == 0 && isCaptain) {  // si le compteur est a 0 après avoir été reduit de 1, et que isCaptain == true
        start();// Démarre la navette
        isCaptain = false;
        remaining = 4;   // reset le compteur de personnes a embarquer restantes à 4 pour le prochain tour.
    }
}

void guarde(int id) {
    unique_lock<mutex> lock(mtx);  // On cree un verrou unique pour le mutex mtx.
    guardes++;

    if (guardes == 4) {
        guardesQueue.notify_all();  // On rapelle tous les gardes en attente dans la queue.
        guardes = 0;
        isCaptain = true;
    } else if (guardes == 2 && detenus >= 2) {
        guardesQueue.notify_all();  // rapelle tous les gardes en attente dans la queue.
        detenusQueue.notify_all();  // rapelle tous les détenus en attente dans la queue.
        detenus -= 2;
        guardes = 0;
        isCaptain = true;
    } else {
        guardesQueue.wait(lock);  // sinon, attend dans la queue des gardes jusqu'à ce qu'il soit rapeller dans une des conditions ci dessus ou que le verrou soit unlock
    }

    board(id, "Le garde");  // Le garde actuel embarque

    if (--remaining == 0 && isCaptain) { // si le compteur est a 0 après avoir été reduit de 1, et que isCaptain == true
        start();  // Démarre la navette
        isCaptain = false;  // Réinitialise le statut du garde à "non-capitaine".
        remaining = 4;  // reset le compteur de personnes a embarquer restantes à 4 pour le prochain tour.
    }
}


int main() {
    thread people[12]; // ici : 12 est le nombre de personnes qui arrivent on peut modifier afin d'en faire arriver plus ou moins


    for (int i = 0; i < 12; i++) { // ici : 12 est le nombre de personnes qui arrivent on peut modifier afin d'en faire arriver plus ou moins
        int random_number = rand() % 2; // On crée un nombre aléatoire
        if (random_number== 0) { // Si nombre random est pair :
            totalDetenus++;
            people[i] = thread(detenu, totalDetenus);
            cout << "Arrivée du detenu " << totalDetenus << "." << endl;
        } else { // Si nombre random est impair
            totalGuardes++;
            people[i] = thread(guarde, totalGuardes);
            cout << "Arrivée du guarde " << totalGuardes << "." << endl;
        }
        // on sleep pour que les threads ne soient pas créés trop rapidement
        this_thread::sleep_for(chrono::milliseconds(100));
    }

    for (int i = 0; i < 12; i++) {// ici : 12 est le nombre de personnes qui arrivent on peut modifier afin d'en faire arriver plus ou moins
        people[i].join();
    }

    return 0;
}
