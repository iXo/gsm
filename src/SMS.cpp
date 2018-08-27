// SMS.cpp: impl�mentation de la classe CSMS.
//
//////////////////////////////////////////////////////////////////////

#include "SMS.h"

//////////////////////////////////////////////////////////////////////
// Initialisation des Constantes
//////////////////////////////////////////////////////////////////////

unsigned int CSMS::LONGUEUR_MAX_MESSAGE_SMS = 160;


//////////////////////////////////////////////////////////////////////
// Constructeurs & Destructeur
//////////////////////////////////////////////////////////////////////

/** Constructeur par d�faut */
CSMS::CSMS()
    : m_numeroDestinataire("")
    , m_texte("")
{}

/** */
CSMS::CSMS(const string& rNumeroDestinataire, const string& rTexte)
    : m_numeroDestinataire(rNumeroDestinataire)
    , m_texte(rTexte)
{}

/** Destructeur */
CSMS::~CSMS()
{}


//////////////////////////////////////////////////////////////////////
// Accesseurs
//////////////////////////////////////////////////////////////////////

const string&
CSMS::GetNumeroDestinataire() const
{
    return m_numeroDestinataire;
}

const string&
CSMS::GetTexte() const
{
    return m_texte;
}

void
CSMS::SetNumeroDestinataire(const string& rNumeroDestinataire)
{
    m_numeroDestinataire = rNumeroDestinataire;
}

void
CSMS::SetTexte(const string& rTexte)
{
    m_texte = rTexte;
}


//////////////////////////////////////////////////////////////////////
// Procedures et Fonctions Public membres de la Classe
//////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
/**
 * @brief
 *          V�rifie que le message ne d�passe pas le nombre de caract�res
 *          max
 *
 *
 *
 *
 * @return  'FAUX' si le nombre de caract�res qui compose le message est
 *          ou �gale � LONGUEUR_MAX_MESSAGE_SMS.
 *
 */
////////////////////////////////////////////////////////////////////////////
bool
CSMS::LongueurMessageValide()
{
    return m_texte.size() <= LONGUEUR_MAX_MESSAGE_SMS;
}
