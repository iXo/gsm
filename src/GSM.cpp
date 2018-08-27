// GSM.cpp: implémentation de la classe CGSM.
//
//////////////////////////////////////////////////////////////////////

#include "GSM.h"
#include <gsmlib/gsm_at.h>
#include <gsmlib/gsm_sms.h>
#include <gsmlib/gsm_sms_store.h>

#include <sys/time.h>

#include "Portable.h"


//////////////////////////////////////////////////////////////////////
// Constructeurs & Destructeur
//////////////////////////////////////////////////////////////////////

CGSM::CGSM()
    : m_derniereErreur("")
#ifdef WIN32
    , m_portCommunication("COM1")
#else
    , m_portCommunication("/dev/modem")
#endif
    , m_vitesseCommunication("9600")
    , m_sequenceInitialisation("E0")
    , m_controleFluxXonXoff(false)
    , m_codePIN("0000")
    , m_pPortSerie(NULL)
    , m_pMeTa(NULL)
    , m_etatConnexion(false)
    , m_idThreadEmissionReceptionSMS()
    , m_etatThreadEmissionReceptionSMS(false)
    , m_pInstanceClasse(NULL)
    , m_listeSMSAEnvoyer()
    , m_listeSMSAReceptionner()
    , m_pfNotifierReceptionMessageSMS(NULL)
{
    // Initialisation du Mutex 'm_mutexListeSMSAEnvoyer'
    pthread_mutex_init(&m_mutexListeSMSAEnvoyer, NULL);
}

CGSM::~CGSM()
{
    // Si on a pas désalouer la mémoire pour 'm_pMeta'
    if(m_pMeTa != NULL)
    {
        delete m_pMeTa;
    }

    // Suppression du Mutex 'm_mutexListeSMSAEnvoyer'
    pthread_mutex_destroy(&m_mutexListeSMSAEnvoyer);
}


//////////////////////////////////////////////////////////////////////
// Accesseurs
//////////////////////////////////////////////////////////////////////

const string&
CGSM::GetDerniereErreur() const
{
    return m_derniereErreur;
}

const string&
CGSM::GetPortCommunication() const
{
    return m_portCommunication;
}

const string&
CGSM::GetVitesseCommunication() const
{
    return m_vitesseCommunication;
}

const string&
CGSM::GetSequenceInitialisation() const
{
    return m_sequenceInitialisation;
}

bool
CGSM::GetControleFluxXonXoff() const
{
    return m_controleFluxXonXoff;
}

const string&
CGSM::GetCodePIN() const
{
    return m_codePIN;
}


void
CGSM::SetPortCommunication(const string& rPortCommunication)
{
    m_portCommunication = rPortCommunication;
}

void
CGSM::SetVitesseCommunication(const string& rVitesseCommunication)
{
    m_vitesseCommunication = rVitesseCommunication;
}

void
CGSM::SetSequenceInitialisation(const string& rSequenceInitialisation)
{
    m_sequenceInitialisation = rSequenceInitialisation;
}

void
CGSM::SetControleFluxXonXoff(bool controleFluxXonXoff)
{
    m_controleFluxXonXoff = controleFluxXonXoff;
}

void
CGSM::SetCodePIN(const string& rCodePIN)
{
    m_codePIN = rCodePIN;
}

void
CGSM::SetInstanceClasse(void* pInstanceClasse)
{
    m_pInstanceClasse = pInstanceClasse;
}

void
CGSM::SetNotifierArriveSMS(const TpfNotifierReceptionMessageSMS pf)
{
    m_pfNotifierReceptionMessageSMS = pf;
}


//////////////////////////////////////////////////////////////////////
// Procedures et Fonctions Public membres de la Classe
//////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
/**
 * @brief
 *          Etablie la connexion avec le Modem GSM
 *
 *
 *
 *
 * @return
 *          'VRAI' si on est bien connecté au modem GSM
 *
 */
////////////////////////////////////////////////////////////////////////////
bool
CGSM::Connecter()
{
	if (m_etatConnexion)
	{
		return true;	// déjà fait, ne pas refaire
	}

    // Si on n'est pas connecté
    if(m_pPortSerie.isnull())
    {
        try
        {
            // Ouverture du port série
#ifdef WIN32
            m_pPortSerie = new gsmlib::Win32SerialPort(
#else
            m_pPortSerie = new gsmlib::UnixSerialPort(
#endif
                                               m_portCommunication,
                                               baudRateStrToSpeed(m_vitesseCommunication),
                                               m_sequenceInitialisation,
                                               m_controleFluxXonXoff);
        }
        catch(GsmException& ge)
        {
            // Etablissement du message d'erreur
            m_derniereErreur = "Erreur pendant l'ouverture du port serie: ";
            m_derniereErreur += ge.what();

            return false;
        }
    }

    // Envoie la commande AT pour vérifier qu'on est bien connecté au Modem GSM
    if(! ExecuteCommandeAT("AT", "OK"))
    {
        // Etablissement du message d'erreur
        m_derniereErreur = "Erreur pendant la tentative de dialogue avec le modem GSM";

        return false;
    }

    // La connexion est donc bien établie
    m_etatConnexion = true;

    return true;
}


bool
CGSM::TesteCarteSimPresente()
{
    // Réinitialisation du Modem GSM
    if(! ExecuteCommandeAT("ATZ", "OK"))
    {
        // Etablissement du message d'erreur
        m_derniereErreur = "Erreur pendant la reinitialisation du modem GSM";

        return false;
    }

    // Si la carte SIM n'est pas présente dans le Modem GSM
    //**/ lorsque la carte Sim est introduite à la volée, le compte rendu est CMEE ERROR : 10
    string csReponse;
    ExecuteCommandeAT("AT+CPIN?", csReponse);
    if (    csReponse.empty()                           // arrive rarement
        || (csReponse.find("ERROR") != string::npos) )  // ERROR ou +CMEE ERROR
    {
        // Etablissement du message d'erreur
        m_derniereErreur = "Aucune carte SIM n'est inseree dans le Modem GSM";

        return false;
    }
    else
    {
        // Lecture (ignorée) de la suite de la réponse
        m_pPortSerie->getLine();     // ligne vide
        m_pPortSerie->getLine();     // "OK" normalement

    }
    return true;
}

bool
CGSM::TesteCodeSimOK()
{
    // Si le code PIN n'a pas été saisie
    if(! ExecuteCommandeAT("AT+CPIN?", "+CPIN: READY"))
    {
        // Si le code PIN donné échoue
        if(! ExecuteCommandeAT("AT+CPIN=" + m_codePIN, "OK"))
        {
            // Etablissement du message d'erreur
            m_derniereErreur = "Le code PIN donne n'est pas valide";

            return false;
        }
    }
    else
    {
        // Lecture (ignorée) de la suite de la réponse
        m_pPortSerie->getLine();     // ligne vide
        m_pPortSerie->getLine();     // "OK" normalement

    }

    return true;
}

////////////////////////////////////////////////////////////////////////////
/**
 * @brief
 *          Initialise le Modem GSM
 *              - Envoie de la commande 'ATZ'
 *              - Verifie et saisie le code PIN
 *
 *
 * @return
 *          'VRAI' si on a bien initialisé le modem GSM
 *
 */
////////////////////////////////////////////////////////////////////////////
bool
CGSM::Initialiser()
{
    // Laisse un délai au modem, le temps de se racorder au réseau GSM
    // et de lire la carte SIM
    Portable::AttenteMilliSecondes(3000);

    // Ouverture du ME/TA (Mobile Equipment  / Terminal Adapter)
    if(m_pMeTa == NULL)
    {
        try
        {
            m_pMeTa = new gsmlib::MeTa(m_pPortSerie);
        }
        catch(GsmException &ge)
        {
            // Etablissement du message d'erreur
            m_derniereErreur = "Erreur pendant l'ouverture du ME/TA: ";
            m_derniereErreur += ge.what();

            return false;
        }
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////
/**
 * @brief
 *          Envoie d'un message SMS
 *
 *
 * @param   rMessageSMS Represente le message SMS à envoyer
 *                      (contient le message et le N° du destinataire)
 *
 * @return
 *          'VRAI' si on réussi à envoyer le message SMS
 *
 */
////////////////////////////////////////////////////////////////////////////
bool
CGSM::EnvoyerSMS(const CSMS& rMessageSMS)
{
        // Protection de la donnée membre 'm_listeSMSAEnvoyer'
        pthread_mutex_lock(&m_mutexListeSMSAEnvoyer);

        // Ajoute le SMS à envoyer dans la liste 'm_listeSMSAEnvoyer'
        m_listeSMSAEnvoyer.push_back(rMessageSMS);

        // Suppression de la protection de la donnée membre 'm_listeSMSAEnvoyer'
        pthread_mutex_unlock(&m_mutexListeSMSAEnvoyer);

        return true;
}

////////////////////////////////////////////////////////////////////////////
/**
 * @brief
 *          Lance le Thread qui s'occupe d'envoyer et de recevoir les SMS.
 *
 *
 *
 *
 *
 * @return  'VRAI' si on a réussi à lancer le Thread
 *
 */
////////////////////////////////////////////////////////////////////////////
bool
CGSM::StartThreadEmissionReceptionSMS()
{
    // Si aucune procédure n'a été défini pour notifier la reception d'un SMS
    if(m_pfNotifierReceptionMessageSMS == NULL)
    {
        // Etablisement du message d'erreur
        m_derniereErreur = "Aucune fonction n'a ete definie pour faire la notification de la reception d'un SMS";

        return false;
    }

    // Si la creation du Thread 'EmissionReceptionSMS' échoue
    if(pthread_create(&m_idThreadEmissionReceptionSMS,
                       NULL,
                       ThreadEmissionReceptionSMS,
                       this))
    {
        // Etablisement du message d'erreur
        m_derniereErreur = "Erreur pendant la creation du Thread 'EmissionReceptionSMS'";

        return false;
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////
/**
 * @brief
 *          Arrête le Thread qui s'occupe d'envoyer et de recevoir les SMS
 *
 *
 *
 *
 *
 * @return  'VRAI' si on a réussi à arrêter le Thread
 *
 */
////////////////////////////////////////////////////////////////////////////
bool
CGSM::StopThreadEmissionReceptionSMS()
{
    // On souhaite arrêter le Thread 'EmissionReceptionSMS'
    m_etatThreadEmissionReceptionSMS = false;

    // Si l'arrêt du Thread échoue
    if(pthread_join(m_idThreadEmissionReceptionSMS, NULL))
    {
        // Etablisement du message d'erreur
        m_derniereErreur = "Erreur pendant l'arret du Thread 'EmissionReceptionSMS'";

        return false;
    }

    return true;
}


//////////////////////////////////////////////////////////////////////
// Procedures et Fonctions Privees membres de la Classe
//////////////////////////////////////////////////////////////////////
/***
void
CGSM::SMSReception(SMSMessageRef nouveauMessage, SMSMessageType messageType)
{}

void
CGSM::CBReception(CBMessageRef newMessage)
{}
***/

void
CGSM::SMSReceptionIndication(string storeName,
                             unsigned int index,
                             SMSMessageType messageType)
{
    // Déclarations et Initialisations des Variables
    TSMSReceptionIndications indicationsReceptionSMS;

    indicationsReceptionSMS.storeName = storeName;
    indicationsReceptionSMS.index = index;
    indicationsReceptionSMS.messageType = messageType;

    m_listeSMSAReceptionner.push_back(indicationsReceptionSMS);
}

////////////////////////////////////////////////////////////////////////////
/**
 * @brief
 *          Fonction qui execute la commande AT et retourne 'Vrai' si le
 *          résultat de cette commande correspond à celle attendue
 *
 *
 * @param   rCommande Commande 'AT' à executer
 * @param   rReponse  Reponse attendue
 *
 * @return  Retourne 'VRAI' si le resultat de la commande AT retourne
 *          la réponse attendue
 *
 */
////////////////////////////////////////////////////////////////////////////

void CGSM::ExecuteCommandeAT(const string& rCommande, string& rReponse)
{
    // Déclarations et Initialisations des Variables

    // Execution de la commande
    m_pPortSerie->putLine(rCommande);

    // Récupération du résultat de la commande
    rReponse = m_pPortSerie->getLine();     // Echo de la commande si 'ATE1' ou vide sinon
    rReponse = m_pPortSerie->getLine();     // Resultat de la commande qui vient d'être exécuté

    m_lastResponse = rReponse;  // pour debug


}

bool
CGSM::ExecuteCommandeAT(const string& rCommande,
                        const string& rReponseAttendue)
{
    // Déclarations et Initialisations des Variables
    string reponseModem = "";
    ExecuteCommandeAT(rCommande, reponseModem);
    // Si on obtient la reponse attendu
    if(reponseModem != rReponseAttendue)
    {
        return false;
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////
/**
 * @brief
 *          Thread qui s'occupe d'envoyer et de recevoir les messages SMS
 *
 *
 * @param   pThis   Instance de la classe 'CGSM'
 *
 * @return  Valeur NULL
 *
 */
////////////////////////////////////////////////////////////////////////////
void*
CGSM::ThreadEmissionReceptionSMS(void* pThis)
{
    // Déclarations et Initialisations des Variables
    CGSM* pMyThis = reinterpret_cast<CGSM*>(pThis);
    CSMS* pMessageSMS = NULL;


    // Le Thread est à présent actif
    pMyThis->m_etatThreadEmissionReceptionSMS = true;

    // Selection du Service de Messagerie
    pMyThis->m_pMeTa->setMessageService(1);

    // On souhaite seulement être prévenu de l'arrivé des SMS
    pMyThis->m_pMeTa->setSMSRoutingToTA(true,       // On ne s'occupe que des messages SMS
                                        false,      // On ne s'occupe pas des messages 'CellBroadcastSMS'
                                        false,      // On ne s'occupe pas des accusés de reception
                                        true);      // On souhaite seulement être notifier de l'arrivé des messages (C'est à nous de les récupérer depuis la carte SIM)

    // Enregistre l'instance 'GSM' pour etre prévenu de l'arrivés des nouveaux SMS
    pMyThis->m_pMeTa->setEventHandler(pMyThis);


    // Tant que l'on ne demande pas l'arrêt du Thread
    while(pMyThis->m_etatThreadEmissionReceptionSMS)
    {
        // On se met en attente d'un message SMS
#ifdef WIN32
        ::timeval timeoutVal;
        timeoutVal.tv_sec = 5;
        timeoutVal.tv_usec = 0;
        pMyThis->m_pMeTa->waitEvent((gsmlib::GsmTime)&timeoutVal);
#else
        struct timeval timeoutVal;
        timeoutVal.tv_sec = 5;
        timeoutVal.tv_usec = 0;
        pMyThis->m_pMeTa->waitEvent(&timeoutVal);
#endif
        // Si on souhaite arrêter le Thread
        if(! pMyThis->m_etatThreadEmissionReceptionSMS)
        {
            break;
        }

        // Protection de la donnée membre 'm_listeSMSAEnvoyer'
        pthread_mutex_lock(&pMyThis->m_mutexListeSMSAEnvoyer);

        // Tant qu'il y a des nouveaux SMS à émettres
        while(pMyThis->m_listeSMSAEnvoyer.size() > 0)
        {
            // Déclarations et Initialisations des Variables
            Ref<SMSSubmitMessage> caracteristiquesMessageSMS = new SMSSubmitMessage();

            // On ne souhaite pas reçevoir d'accusé de réception
            caracteristiquesMessageSMS->setStatusReportRequest(false);

            // Renseigne le N° du Destinataire
            Address numeroDestinataire((*(pMyThis->m_listeSMSAEnvoyer.begin())).GetNumeroDestinataire());
            caracteristiquesMessageSMS->setDestinationAddress(numeroDestinataire);

            // Envoie du message SMS
            try
            {
                pMyThis->m_pMeTa->sendSMSs(caracteristiquesMessageSMS,
                                           (*(pMyThis->m_listeSMSAEnvoyer.begin())).GetTexte(),
                                           true);
            }
            catch(GsmException &ge)
            {
#ifdef GEST_ASTR_TRACE_ERREURS
                cerr << "CGSM: Envoie du SMS a destination du: " << (*(pMyThis->m_listeSMSAEnvoyer.begin())).GetNumeroDestinataire()
                     << "\tErreur pendant l'envoie du SMS: "
                     << ge.what() << endl;
#endif
            }

            // Suppression du Message SMS
            pMyThis->m_listeSMSAEnvoyer.pop_front();
        }

        // Suppression de la protection de la donnée membre 'm_listeSMSAEnvoyer'
        pthread_mutex_unlock(&pMyThis->m_mutexListeSMSAEnvoyer);


        // Tant qu'il y a des nouveaux SMS à receptionner
        while(pMyThis->m_listeSMSAReceptionner.size() > 0)
        {
            // Récupération de l'emplacement mémoire du 1er SMS reçu
            TSMSReceptionIndications indicationsReceptionSMS = *(pMyThis->m_listeSMSAReceptionner.begin());
            pMyThis->m_listeSMSAReceptionner.pop_front();

            // Récupère le message depuis la carte SIM
            SMSStoreRef store = pMyThis->m_pMeTa->getSMSStore(indicationsReceptionSMS.storeName);
            store->setCaching(false);

            // Si il s'agit d'un simple SMS (Pas d'un CellBroadcastSMS ou d'un accusé de reception)
            if(indicationsReceptionSMS.messageType == GsmEvent::NormalSMS)
            {
                // Si il est possible qu'il s'agit d'un message provenant d'un Agent
                if((*store.getptr())[indicationsReceptionSMS.index].message().getptr()->messageType() == SMSMessage::SMS_DELIVER)
                {
                    // Récupération du N° de téléphone de l'émetteur
                    string numeroEmetteur((*store.getptr())[indicationsReceptionSMS.index].message().getptr()->address().toString());

                    // Si il s'agit d'un numero de telephone valide
                    if(numeroEmetteur.length() > 10)
                    {
                        // Création d'une nouvelle instance de 'CSMS'
                        pMessageSMS = new CSMS();

                        // Formatage du numéro de téléphone '33674649510' sous la forme '0674649510'
                        pMessageSMS->SetNumeroDestinataire("0" + numeroEmetteur.substr(3));

                        // Récupération du message de l'émetteur
                        pMessageSMS->SetTexte((*store.getptr())[indicationsReceptionSMS.index].message().getptr()->userData());

                        // On prévient le module 'TraitementAlarmes' qu'un nouveau message SMS est arrivé
                        pMyThis->m_pfNotifierReceptionMessageSMS(pMessageSMS, pMyThis->m_pInstanceClasse);
                    }
                }
            }

            // TODO: Contrôler la supression du message SMS de la carte SIM
            // Supression du Message de la Carte SIM pour ne pas saturer la mémoire
            store.getptr()->erase(store.getptr()->begin() + indicationsReceptionSMS.index);
        }
    }

    // switch off SMS routing
    try
    {
          pMyThis->m_pMeTa->setSMSRoutingToTA(false, false, false);
    }
    catch (GsmException &ge)
    {
        // some phones (e.g. Motorola Timeport 260) don't allow to switch
        // off SMS routing which results in an error. Just ignore this.
    }

    return NULL;
}
