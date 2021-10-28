
/* declaration d'existence de variables globales
 * publiques de monModule
 */
// Notez que ce n'est pas une instantiation,
// mais, du fait de l’utilisation du mot clé "extern"
// juste une déclaration d'existence de la variable.
// L'instanciation est, elle, faite dans le .c du module
 
 #include <stdio.h>
#include <stdlib.h>
 
/*
* @wi.implements CMI_project/CMI-588 Returns author of this book
* http://149.202.163.112/polarion/#/project/CMI_project/workitems?query=NOT%20HAS_VALUE%3Aresolution
*/

public String getAuthor() {
return this.author;
}  
 


// La ligne suivante est le prototype de la fonction aireRectangle :
double aireRectangle(double largeur, double hauteur);

int main(int argc, char *argv[])
{   
    printf("Rectangle de largeur 5 et hauteur 10. Aire = %f\n", aireRectangle(5, 10));
    printf("Rectangle de largeur 2.5 et hauteur 3.5. Aire = %f\n", aireRectangle(2.5, 3.5));
    printf("Rectangle de largeur 4.2 et hauteur 9.7. Aire = %f\n", aireRectangle(4.2, 9.7));
   
    return 0;
}

// Notre fonction aireRectangle peut maintenant être mise n'importe où dans le code source :
double aireRectangle(double largeur, double hauteur)
{
    return largeur * hauteur;
}