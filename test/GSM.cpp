#include <iostream>
#include "GSM.h"
#include "SMS.h"
using namespace std;

int main(int argc, char* argv[])
{
    // D�clarations et Initialisations des Variables
    CGSM* pGSM = NULL;


    // R�cup�ration d'une Instance de GSM
    pGSM = CGSM::GetInstance();

    // Si on arrive � se connecter au Modem GSM
    if(pGSM->Connecter())
    {
        cout << "Connexion etablie" << endl;

        // Si l'initialisation du Modem �choue
        if(! pGSM->Initialiser())
        {
            // Affichage du message d'erreur
            cout << pGSM->GetDerniereErreur() << endl;
        }
        else
        {
            cout << "Initialisation du Modem GSM reussi" << endl;

            // Creation d'un message SMS � envoyer
            CSMS monMessageSMS("0620930491","Module de receptions et d'envoies de messages SMS");

            // Si on arrive � envoyer le Message SMS
            if(pGSM->EnvoyerSMS(monMessageSMS))
            {
                cout << "Message SMS envoyer" << endl;
            }
            else
            {
                cout << "Erreur pendant l'envoie du Message SMS" << endl;
                cout << pGSM->GetDerniereErreur() << endl;
            }
        }
    }
    else
    {
        cout << "Connexion non etablie: " << pGSM->GetDerniereErreur() << endl;
    }


    return 0;
}
