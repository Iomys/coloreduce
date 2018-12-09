/** \author Sandoz Valentin
 *  \date	dec 2018
 */
#include <iostream>
#include <vector>
#include <cmath>


using namespace std;
struct Couleur;
struct Donnees;


//Définition des types

typedef vector<Couleur> Coul_list;
typedef vector<vector<size_t>> Mat_indice; //Matrice contenant des indices de vector

//Définition des structures
struct Couleur{
    int r;
    int v;
    int b;
};
constexpr int MAX(255);
constexpr Couleur coul_bord = {0,0,0};
constexpr double seuil_min(0.);
constexpr double seuil_max(1.);
constexpr int pixels_voisins_min(6);
constexpr size_t ind_coul_bord(0);

struct Donnees{
    vector<vector<Couleur>> image;
    size_t li=0;              //Nombre de lignes du tableau
    size_t col=0;             //Nombre de colonnes du tableau
    unsigned int nbR=0;       //Nombre de couleurs réduites
    unsigned int nbF=0;       //Nombre de filtrage
    Coul_list coul_redu={coul_bord};    //Liste des couleurs réduites
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
void bord_noir(Donnees& donnees, Mat_indice& image);

//Fonctions de modification de l'image
Mat_indice seuillage(const Donnees& donnees);
double int_normalisee(Couleur couleur);
void filtrage(const Donnees& donnees, Mat_indice& entree, Mat_indice& destination);


void exportation(const Donnees& donnees, const Mat_indice& indice_redu);
//Fonctions de tests
void is_color(int color);

int main() {
    //Variables globales
    Donnees donnees;
    import(donnees);
    Mat_indice indices_redu=seuillage(donnees);
    Mat_indice destination(donnees.li, vector<size_t>(donnees.col, ind_coul_bord));
    destination = indices_redu;
    for (int i = 0; i < donnees.nbF; ++i) {
        filtrage(donnees, indices_redu, destination);
        indices_redu.swap(destination);
    }
    bord_noir(donnees, indices_redu);
    exportation(donnees, indices_redu);
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

Mat_indice seuillage(const Donnees& donnees){
    Mat_indice img_coul_redu(donnees.li, vector<size_t>(donnees.col));
    for(size_t li = 0; li<donnees.li; ++li){
        for(size_t col = 0; col<donnees.col;++col){
            double intensite(int_normalisee(donnees.image[li][col]));
            bool seuil_trouve(false);
            for(size_t i = 1; i <= donnees.nbR && !seuil_trouve ;++i) {
                if (donnees.seuils[i-1] <= intensite &&
                    donnees.seuils[i] > intensite) {
                    img_coul_redu[li][col] = i ;
                    seuil_trouve = true;
                }
            }
            if (!seuil_trouve)
                img_coul_redu[li][col] = donnees.nbR;
        }
    }
    return img_coul_redu;
}

void bord_noir(Donnees& donnees, Mat_indice& image){
    for(size_t li(0); li < donnees.li; ++li){
        for(size_t col(0); col < donnees.col; ++col){
           if(li==0 || li==donnees.li-1 || col==0 || col==donnees.col-1){
            image[li][col]=ind_coul_bord;
           }
        }
    }
}
void filtrage(const Donnees& donnees, Mat_indice& entree, Mat_indice& destination){
        //Début du filtrage

        for (size_t li(1); li < donnees.li - 1; ++li) { // On parcours les lignes (sans la première et la dernière)
            //cout << "\tDébut ligne n° " << li << " sur " << donnees.li << endl;
            for (size_t col(1); col < donnees.col - 1; ++col) { // On parcours les colonnes (sans la première et la dernière)
                //cout << "\tDébut colonne n° " << col << " sur " << donnees.col << endl;
                vector<int> nb_pixels_voisins(donnees.nbR+1, 0); //Variable pour compter le nombre de pixels voisins d'une même couleur
                //Comptage des couleurs environnantes
                for (int i = -1; i <= 1; ++i) { //On parcours les pixels voisins (lignes)
                    for (int j = -1; j <= 1; ++j) { //idem (colonnes)
                        if (!(j == 0 && i == 0)) { //On exclu le pixel de base
                            size_t voisin = entree[li + i][col + j];
                            //cout << voisin;
                            if(voisin != ind_coul_bord)
                                ++nb_pixels_voisins[voisin-1]; //On ajoute l'indice du pixel voisin
                        }
                    }
                }
                //cout << "";
                //On regarde  si une couleur a au moins 6 pixels voisins
                for (size_t i = 0; i < donnees.nbR; ++i) {
                    //cout << "\t\t Prédestination " << i <<": ";
                    if (nb_pixels_voisins[i] >= pixels_voisins_min) {
                        destination[li][col] = i+1;
                        //cout << "\t\tDestination (>=6)[" << li << "]["<< col << "] = " << i+1 << endl;
                        i = donnees.nbR+1; //sortie de la boucle au prochain passage
                    } else{  //if (i == donnees.nbR-1)

                        destination[li][col] = ind_coul_bord;
                        //cout << "\t\tDestination (<6)[" << li << "]["<< col << "] <- " << ind_coul_bord << endl;
                    }
                }
                //cout << "\t Destination = " << destination[li][col] << endl;
            }
            //cout << "\t ligne finie" << endl;
        }
        //cout << "\t tableau fini" << endl;
}

void import_image(Donnees& donnees){
    char format1(0), format2(0);
    int couleur_max(0);
    cin >> format1 >> format2 >> donnees.col >> donnees.li >> couleur_max; //Manque les tests pour savoir si image compatible
    donnees.image.resize(donnees.li);
    for(int li(0); li < donnees.li; ++li){
        donnees.image[li].reserve(donnees.col);
        for(int col(0); col < donnees.col; ++col){
            Couleur transfert;
            cin >> transfert.r >> transfert.v >> transfert.b;
            donnees.image[li].push_back(transfert);
        }
    }
}

void is_color(int color) {
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

void exportation(const Donnees& donnees, const Mat_indice& indice_redu){
    cout << "P3" << endl << donnees.col << " " << donnees.li << endl << MAX << endl;
    for(size_t li = 0; li < donnees.li; ++li){
        for(size_t col = 0; col < donnees.col; ++col){
        cout << donnees.coul_redu[indice_redu[li][col]].r << " "
            << donnees.coul_redu[indice_redu[li][col]].v << " "
            << donnees.coul_redu[indice_redu[li][col]].b << " ";
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
