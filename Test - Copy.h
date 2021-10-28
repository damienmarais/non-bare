// fichier monModule.h
#ifndef _MON_MODULE_H
#define _MON_MODULE_H
// pour connaitre le type LISTE_T déclaré dans liste.h
// et utilisé dans les prototypes des fonctions de monModule.h
#include "liste.h"
/* declaration d'existence de variables globales
 * publiques de monModule
 */
// Notez que ce n'est pas une instantiation,
// mais, du fait de l’utilisation du mot clé "extern"
// juste une déclaration d'existence de la variable.
// L'instanciation est, elle, faite dans le .c du module
extern int uneVariableGlobale;
extern LISTE_T uneVariableListeGlobale;
/* declaration des types publics de monModule
 */
typedef struct complex {
float re;
float im;
} COMPLEXE_T;
/* prototype des fonctions publiques de monModule
 */
 
/**
* @wi Geosat/GEOS-194 Returns author of this book
*/

public String getAuthor() {
return this.author;
}  
 
void trier_liste(LISTE_T liste);
void conjuguer_complexe(COMPLEXE_T * p_unComplexe);
#endif