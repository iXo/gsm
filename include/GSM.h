////////////////////////////////////////////////////////////////////////////
/**
 *      Copyright (c) 2008   THALES e-Transactions CGA
 *
 ***************************************************************************
 *
 * @file GSM.h
 *
 * @brief
 *          Permet d'envoyer et de recevoir des messages SMS
 *          (Modem compatible aux standards ETSI GSM 07.07, ETSI GSM 07.05)
 *
 *<p><b>CSCI name    :          </b></p>
 *<p><b>CSCI version :          </b></p>
 *
 *
 * @package GSM
 *
 * @version 1.0
 *
 * @author LEBEGUE Sebastien
 *
 * @date 12/08/2008
 *
 * @see
 * @todo
 * @warning
 *
 * @bug     Version History :                                       \n <CODE>
 *|-Version-+ ---Date--- + ---Author--- + Short description         \n
 *|---------+------------+--------------+------------------         \n
 *| 1.0     | 12/08/2008 | LEBEGUE      | Creation du Fichier       \n </CODE>
 *
 */
////////////////////////////////////////////////////////////////////////////

#ifndef __GSM__
#define __GSM__

#include "SMS.h"

#ifdef WIN32
#include <gsmlib/gsm_win32_serial.h>
#else
#include <gsmlib/gsm_unix_serial.h>
#endif
#include <gsmlib/gsm_util.h>
#include <gsmlib/gsm_event.h>
#include <gsmlib/gsm_me_ta.h>

#include <list>
#include <string>
#include <iostream>
#include <pthread.h>
using namespace std;
using namespace gsmlib;

typedef struct
{
    string                      storeName;          /*!< M�moire o� sont stocker les messages SMS */
    unsigned int                index;              /*!< Index en m�moire o� se situe le message SMS */
    GsmEvent::SMSMessageType    messageType;        /*!< Type du message (Ex: Accus� de reception, ...) */
} TSMSReceptionIndications;

////////////////////////////////////////////////////////////////////////////
/**
 * @class CGSM
 * @brief
 *
 * @pre
 * @post
 * @invariant
 *
 * @see
 * @todo
 * @warning
 *
 * @bug
 *
 */
////////////////////////////////////////////////////////////////////////////

/** Type de la fonction qu'il faut appeler pour faire la notification de la reception d'un message SMS */
typedef void (*TpfNotifierReceptionMessageSMS) (CSMS*, void*);

class CGSM : public GsmEvent
{
public:

    // Constructeurs & Destructeur
    // ---------------------------

    CGSM();
    // CGSM(const CGSM& rGSM);      // Laisse le compilateur generer le constructeur par copie

    virtual ~CGSM();


    // Accesseurs
    // ----------

    /** Retourne une description sur la derni�re erreur connu si l'execution
    d'une fonction de cette classe se traduit par un �chec */
    const string&   GetDerniereErreur() const;

    /** Retourne le port de communication utilis� pour dialoguer avec le modem GSM */
    const string&   GetPortCommunication() const;

    /** Retourne la vitesse de communication utilis� pour dialoguer avec le modem GSM */
    const string&   GetVitesseCommunication() const;

    /** Retourne la s�quence d'initialisation du Modem GSM */
    const string&   GetSequenceInitialisation() const;

    /** Retourne 'Vrai' si le controle de Flux 'XonXoff' (Logiciel) est activ� */
    bool            GetControleFluxXonXoff() const;

    /** Retourne le code PIN utilis� pour initialiser le Modem GSM */
    const string&   GetCodePIN() const;


    /** Permet de sp�cifier le port de Communication � utiliser pour dialoguer avec le modem GSM */
    void    SetPortCommunication(const string& rPortCommunication);

    /** Permet de sp�cifier la vitesse de communication avec le modem GSM */
    void    SetVitesseCommunication(const string& rVitesseCommunication);

    /** Permet de sp�cifier la s�quence d'initialisation qu'il faut utiliser pour initialiser le modem GSM */
    void    SetSequenceInitialisation(const string& rSequenceInitialisation);

    /** Permet de sp�cifier si l'on souhaite utilis� le controle de flux "XonXoff" (Logiciel) avec le modem GSM */
    void    SetControleFluxXonXoff(bool controleFluxXonXoff);

    /** Permet de sp�cifier le code PIN � utiliser pour initialiser le Modem GSM */
    void    SetCodePIN(const string& rCodePIN);

    /** Permet de sp�cifier une instance de classe qu'il faut donn�e � la fonction 'm_pfNotifierReceptionMessageSMS' */
    void    SetInstanceClasse(void* pInstanceClasse);

    /** Permet de sp�cifier la proc�dure qu'il faut appeler pour faire la notification de l'arriv� d'un SMS */
    void    SetNotifierArriveSMS(const TpfNotifierReceptionMessageSMS pf);


    // Procedures et Fonctions Public membres de la Classe
    // ---------------------------------------------------

    // Etablie la connexion avec le Modem GSM
    bool Connecter();

    // Se deconnecte du Modem GSM
    // bool Deconnecter() const;

	bool TesteCarteSimPresente();

	bool TesteCodeSimOK();


    // Initialise le Modem GSM
    bool Initialiser();

    // Permet d'envoyer un message SMS
    bool EnvoyerSMS(const CSMS& rMessageSMS);

    // Lance le Thread qui s'occupe d'envoyer et de recevoir les SMS.
    bool StartThreadEmissionReceptionSMS();

    // Arr�te le Thread qui s'occupe d'envoyer et de recevoir les SMS
    bool StopThreadEmissionReceptionSMS();


private:

    // Attributs
    // ---------

    string              m_derniereErreur;                           /*!< Contient une description sur la derni�re erreur connu */
    string              m_portCommunication;                        /*!< Contient le port de communication qui est utilis� pour dialoguer avec le modem GSM */
    string              m_vitesseCommunication;                     /*!< Contient la vitesse utilis�e pour dialoguer avec le modem GSM */
    string              m_sequenceInitialisation;                   /*!< Contient la s�quence d'initialisation qu'il faut envoyer au modem GSM */
    bool                m_controleFluxXonXoff;                      /*!< 'VRAI' si il faut utiliser le controle de flux 'Logiciel' */
    string              m_codePIN;                                  /*!< Contient le code PIN utilis� pour initialiser le modem GSM */

    Ref<Port>           m_pPortSerie;                               /*!< Port s�rie utilis� pour dialoguer avec le modem GSM (Ref<> --> Reference Counting) */
    MeTa*               m_pMeTa;                                    /*!< Mobile Equipment / Terminal Adaptor */
    bool                m_etatConnexion;                            /*!< 'VRAI' si la connexion avec le Modem GSM est �tablie */

    pthread_t           m_idThreadEmissionReceptionSMS;             /*!< ID du Thread pour l'emissions et la receptions des SMS */
    bool                m_etatThreadEmissionReceptionSMS;           /*!< 'VRAI' si le Thread pour l'emissions et la receptions des SMS est actif */

    void*               m_pInstanceClasse;                          /*!< Contient une instance de classe qu'il faut donn�e � la fonction 'm_pfNotifierReceptionMessageSMS' */

    list<CSMS>          m_listeSMSAEnvoyer;                         /*!< Contient la liste des SMS � envoyer */
    pthread_mutex_t     m_mutexListeSMSAEnvoyer;                    /*!< Mutex sur 'm_listeSMSAEnvoyer' */
    list<TSMSReceptionIndications>  m_listeSMSAReceptionner;        /*!< Contient les emplacements m�moires et les types des nouveaux SMS */


    // Procedures et Fonctions Privees membres de la Classe
    // ----------------------------------------------------

    // H�rit� de la classe 'GsmEvent'
//    void SMSReception(SMSMessageRef newMessage, SMSMessageType messageType);
//    void CBReception(CBMessageRef newMessage);
    void SMSReceptionIndication(string storeName, unsigned int index, SMSMessageType messageType);

    /** Proc�dure qu'il faut appeler pour faire la notification de la reception d'un message SMS */
    TpfNotifierReceptionMessageSMS    m_pfNotifierReceptionMessageSMS;

    // Fonction qui execute la commande AT et retourne 'Vrai' si le r�sultat de cette commande correspond � celle attendu
    bool ExecuteCommandeAT(const string& rCommande, const string& rReponseAttendue);
    void ExecuteCommandeAT(const string& rCommande, string& rReponse);
    string m_lastResponse;
    // Thread qui s'occupe d'envoyer et de recevoir les messages SMS
    static void* ThreadEmissionReceptionSMS(void* pThis);

};

#endif // __GSM__
