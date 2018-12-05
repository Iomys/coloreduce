/** \author Sandoz Valentin
 *  \date	dec 2018
 */
#include <iostream>
#include <vector>
#include <array>
#include <cmath>


using namespace std;
struct Couleur;
struct Donnees;


//Définition des types

typedef vector<Couleur> Coul_list;

//Définition des structures
struct Couleur{
    int r;
    int v;
    int b;
};
constexpr int MAX(255);
constexpr Couleur noir = {0,0,0};
constexpr double seuil_min(0.);
constexpr double seuil_max(1.);

struct Donnees{
    vector<vector<Couleur>> image;
    size_t li;              //Nombre de lignes du tableau
    size_t col;             //Nombre de colonnes du tableau
    unsigned int nbR;       //Nombre de couleurs réduites
    unsigned int nbF;       //Nombre de filtrage
    Coul_list coul_redu={noir};    //Liste des couleurs réduites
    vector<double> seuils={seuil_min};  //Liste des seuils
};


//Fonctions d'erreurs
void error_nbR(int nbR);
void error_color(int id);
void error_threshold(double invalid_val);
void error_nb_filter(int nb_filter);

//Fonctions d'importation
void import(Donnees& donnees);
void import_couleurs(Donnees& donnees);
void import_filtrages(Donnees& donnees);
void import_nbR(Donnees& donnees);
void import_seuils(Donnees& donnees) ;
void import_image(Donnees& donnees);

//Fonctions de modification de l'image
void seuillage(Donnees& donnees);
double int_normalisee(Couleur couleur);
void filtrage(Donnees& donnees);


void exportation(Donnees& donnees);
//Fonctions de tests
bool is_color(int color);

int main() {
    //Variables globales
    Donnees donnees;
    import(donnees);
    seuillage(donnees);
    exportation(donnees);
    return 0;
}
void import(Donnees& donnees) {
    import_nbR(donnees);
    import_couleurs(donnees);
    import_seuils(donnees);
    import_filtrages(donnees);
    import_image(donnees);


}
void import_nbR(Donnees& donnees){
    cin >> donnees.nbR; //Nombre réduit de couleurs
    if (donnees.nbR < 2 || donnees.nbR > MAX) {
        error_nbR(donnees.nbR);
        exit(0);
    }
}
void import_couleurs(Donnees& donnees){
    for(int i=1;i<=donnees.nbR;i++){
    int rouge, vert, bleu;
    cin >> rouge >> vert >> bleu;
        donnees.coul_redu.push_back({rouge,vert,bleu});
    }
}
void import_seuils(Donnees& donnees){
    for(int i=0; i<donnees.nbR-1;++i){
        double transfert;
        cin >> transfert;
        if(transfert<=0 || transfert >=1){
            error_threshold(transfert);
            exit(0);
        }
        donnees.seuils.push_back(transfert);
    }
    donnees.seuils.push_back(seuil_max);
}
void import_filtrages(Donnees& donnees){
    cin >> donnees.nbF;
    if(donnees.nbF < 0){
        error_nb_filter(donnees.nbF);
        exit(0);
    }
}

void seuillage(Donnees& donnees){
    for(size_t li = 0; li<donnees.li; ++li){
        for(size_t col = 0; col<donnees.col;++col){
            double intensite(int_normalisee(donnees.image[li][col]));
            bool seuil_trouve(false);
            for(size_t i = 0; i < donnees.nbR && !seuil_trouve ;++i){
                if(donnees.seuils[i] <= intensite && donnees.seuils[i+1]> intensite){
                    donnees.image[li][col]=donnees.coul_redu[i+1];
                    seuil_trouve=true;
                }
                if(!seuil_trouve)
                    donnees.image[li][col]=donnees.coul_redu.back();
            }
        }
    }

}

void filtrage(Donnees& donnees){
    for(size_t li=1; li < donnees.li-1; ++li){
        for(size_t col=1; col < donnees.col-1; ++col){

        }
    }
}

void import_image(Donnees& donnees){
    char format1(0), format2(0);
    int couleur_max(0);
    cin >> format1 >> format2 >> donnees.col >> donnees.li >> couleur_max; //Manque les tests pour savoir si image compatible
    donnees.image.resize(donnees.li);
    for(int li(0); li < donnees.li; ++li){
        for(int col(0); col < donnees.col; ++col){
            Couleur transfert;
            cin >> transfert.r >> transfert.v >> transfert.b;
            donnees.image[li].push_back(transfert);
        }
    }
}

bool is_color(int color) {
    if(color>MAX || color <0){
        error_color(color);
        exit(0);
    }
}

double int_normalisee(Couleur couleur){
    return sqrt(
            pow(couleur.r,2)
            + pow(couleur.v,2)
            + pow(couleur.b,2)
    )
    /(sqrt(3)*MAX);
}

void exportation(Donnees& donnees){
    for(size_t li = 0; li < donnees.li; ++li){
        for(size_t col = 0; col < donnees.col; ++col){
        cout << donnees.image[li][col].r << " " << donnees.image[li][col].v << " "
            << donnees.image[li][col].b << " ";
        }
        cout << endl;
    }
}
void error_nbR(int nbR) {
    cout << "Invalid number of colors: " << nbR << endl;
}
void error_color(int id) {
    cout << "Invalid color value " << id << endl;
}
void error_threshold(double invalid_val) {
    cout << "Invalid threshold value: " << invalid_val << endl;
}
void error_nb_filter(int nb_filter) {
    cout << "Invalid number of filter: " << nb_filter << endl;
}
