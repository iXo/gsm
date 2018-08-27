////////////////////////////////////////////////////////////////////////////
/**
 *      Copyright (c) 2008   THALES e-Transactions CGA
 *
 ***************************************************************************
 *
 * @file SMS.h
 *
 * @brief
 *          Represente un message SMS (Contient le message + N° du destinataire)
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

#ifndef __SMS__
#define __SMS__

#include <string>
using namespace std;

////////////////////////////////////////////////////////////////////////////
/**
 * @class CSMS
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

class CSMS
{
public:

    // Constantes
    // ----------

    static unsigned int LONGUEUR_MAX_MESSAGE_SMS;            /*!< Longueur max que peut faire un message SMS */


    // Constructeurs & Destructeur
    // ---------------------------

    CSMS();
    CSMS(const string& rNumeroDestinataire, const string& rTexte);
    // CSMS(const CSMS& rSMS);      // Laisse le compilateur generer le constructeur par copie

    virtual ~CSMS();


    // Accesseurs
    // ----------

    /** Retourne le numéro de téléphone du destinataire */
    const string&   GetNumeroDestinataire() const;

    /** Retourne le contenu du message SMS */
    const string&   GetTexte() const;

    /** Permet de spécifier le numéro de téléphone du destinataire */
    void    SetNumeroDestinataire(const string& rNumeroDestinataire);

    /** Permet de spécifier le contenu du message SMS */
    void    SetTexte(const string& rTexte);


    // Procedures et Fonctions Public membres de la Classe
    // ---------------------------------------------------

    /** Determine si la longueur du message SMS est correcte */
    bool LongueurMessageValide();


private:

    // Attributs
    // ---------

    string  m_numeroDestinataire;
    string  m_texte;

};

#endif // __GSM__
