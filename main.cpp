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
typedef vector<vector<int>> Mat_indice; //Matrice contenant des indices de vector

//Définition des structures
struct Couleur{
	int r;
	int v;
	int b;
};


constexpr int MAX(255);
constexpr double EPSILON(0.001);
constexpr Couleur coul_bord={0, 0, 0};
constexpr int ind_coul_bord(0);
constexpr double seuil_min(0.);
constexpr double seuil_max(1.);
constexpr int pixels_voisins_min(6); //Nombre de pixels voisins pour le filtrage


struct Donnees{
	vector<vector<Couleur>> image;
	int li=0;              //Nombre de lignes du tableau
	int col=0;             //Nombre de colonnes du tableau
	int nbR=0;       //Nombre de couleurs réduites
	int nbF=0;       //Nombre de filtrage
	Coul_list coul_redu={coul_bord};    //Liste des couleurs réduites
	vector<double> seuils={seuil_min};  //Liste des seuils
};

//Fonctions d'erreurs
void error_nbR(int nbR);
void error_color(int id);
void error_threshold(double invalid_val);
void error_nb_filter(int nb_filter);

//Fonctions d'importation
void import(Donnees &donnees);
void import_couleurs(Donnees &donnees);
void import_filtrages(Donnees &donnees);
void import_nbR(Donnees &donnees);
void import_seuils(Donnees &donnees);
void import_image(Donnees &donnees);
void is_color(int color, int numero);


//Autres fonction
Mat_indice seuillage(const Donnees &donnees);
double int_normalisee(Couleur couleur);
void filtrage(const Donnees &donnees, Mat_indice &entree);
void bord_noir(Donnees &donnees, Mat_indice &image);
void exportation(const Donnees &donnees, const Mat_indice &indice_redu);


int main(){
	Donnees donnees;
	import(donnees);
	Mat_indice indices_redu=seuillage(donnees);
	filtrage(donnees, indices_redu);
	if(donnees.nbF>0)
		bord_noir(donnees, indices_redu);
	exportation(donnees, indices_redu);
	return 0;
}

void import(Donnees &donnees){
	import_nbR(donnees);
	import_couleurs(donnees);
	import_seuils(donnees);
	import_filtrages(donnees);
	import_image(donnees);
}

void import_nbR(Donnees &donnees){
	cin>>donnees.nbR; //Nombre réduit de couleurs
	if(donnees.nbR<2||donnees.nbR>MAX){
		error_nbR(donnees.nbR);
		exit(0);
	}
}

void import_couleurs(Donnees &donnees){
	for(int i=1; i<=donnees.nbR; i++){
		int rouge, vert, bleu;
		cin>>rouge>>vert>>bleu;
		is_color(rouge, i);
		is_color(vert, i);
		is_color(bleu, i);
		donnees.coul_redu.push_back({rouge, vert, bleu});
	}
}

void import_seuils(Donnees &donnees){
	for(int i=0; i<donnees.nbR-1; ++i){
		double transfert;
		cin>>transfert;
		if(transfert<=0||transfert>=1){
			error_threshold(transfert);
			exit(0);
		}
		donnees.seuils.push_back(transfert);
		if(donnees.seuils[i+1]-donnees.seuils[i]<EPSILON){
			error_threshold(donnees.seuils[i+1]);
			exit(0);
		}
	}
	donnees.seuils.push_back(seuil_max);
}

void import_filtrages(Donnees &donnees){
	cin>>donnees.nbF;
	if(donnees.nbF<0){
		error_nb_filter(donnees.nbF);
		exit(0);
	}
}

Mat_indice seuillage(const Donnees &donnees){
	Mat_indice img_coul_redu(donnees.li, vector<int>(donnees.col));
	for(int li=0; li<donnees.li; ++li){
		for(int col=0; col<donnees.col; ++col){
			double intensite(int_normalisee(donnees.image[li][col]));
			bool seuil_trouve(false);
			for(int i=1; i<=donnees.nbR&&!seuil_trouve; ++i){
				if(donnees.seuils[i-1]<=intensite&&
					 donnees.seuils[i]>intensite){
					img_coul_redu[li][col]=i;
					seuil_trouve=true;
				}
			}
			if(!seuil_trouve)
				img_coul_redu[li][col]=donnees.nbR;
		}
	}
	return img_coul_redu;
}

void bord_noir(Donnees &donnees, Mat_indice &image){
	for(int li(0); li<donnees.li; ++li){
		for(int col(0); col<donnees.col; ++col){
			if(li==0||li==donnees.li-1||col==0||
				 col==donnees.col-1){
				image[li][col]=ind_coul_bord;
			}
		}
	}
}

void filtrage(const Donnees &donnees, Mat_indice &entree){
	Mat_indice destination(entree);
	for(int n_filtr(1); n_filtr<=donnees.nbF; ++n_filtr){
		for(int li(1); li<donnees.li-1; ++li){ //Parcours des lignes
			for(int col(1); col<donnees.col-1; ++col){ //Parcours de colonnes
				vector<int> nb_pixels_voisins(donnees.nbR+1, 0);
				//Comptage des couleurs environnantes
				for(int i=-1;
						i<=1; ++i){ //On parcours les pixels voisins (lignes)
					for(int j=-1; j<=1; ++j){ //idem (colonnes)
						if(!(j==0&&i==0)){ //On exclu le pixel de base
							int voisin=entree[li+i][col+j];
							if(voisin!=ind_coul_bord)
								++nb_pixels_voisins[voisin-1]; //Ajout du pixel voisin
						}
					}
				}
				//On regarde  si une couleur a au moins 6 pixels voisins
				for(int i=0; i<donnees.nbR; ++i){
					if(nb_pixels_voisins[i]>=pixels_voisins_min){
						destination[li][col]=i+1;
						break;
					}else if(i==donnees.nbR-1){
						destination[li][col]=ind_coul_bord;

					}
				}
			}
		}
		entree.swap(destination);
	}
}

void import_image(Donnees &donnees){
	char format1(0), format2(0);
	int couleur_max(0);
	cin>>format1>>format2>>donnees.col>>donnees.li
		 >>couleur_max; //Manque les tests pour savoir si image compatible
	donnees.image.resize(donnees.li);
	for(int li(0); li<donnees.li; ++li){
		donnees.image[li].reserve(donnees.col);
		for(int col(0); col<donnees.col; ++col){
			Couleur transfert;
			cin>>transfert.r>>transfert.v>>transfert.b;
			donnees.image[li].push_back(transfert);
		}
	}
}

void is_color(int color, int numero){
	if(color>MAX||color<0){
		error_color(numero);
		exit(0);
	}
}

double int_normalisee(Couleur couleur){
	return sqrt(
			pow(couleur.r, 2)
			+pow(couleur.v, 2)
			+pow(couleur.b, 2)
	)
				 /(sqrt(3)*MAX);
}

void exportation(const Donnees &donnees, const Mat_indice &indice_redu){
	cout<<"P3"<<endl<<donnees.col<<" "<<donnees.li<<endl<<MAX<<endl;
	for(int li=0; li<donnees.li; ++li){
		for(int col=0; col<donnees.col; ++col){
			cout<<donnees.coul_redu[indice_redu[li][col]].r<<" "
					<<donnees.coul_redu[indice_redu[li][col]].v<<" "
					<<donnees.coul_redu[indice_redu[li][col]].b<<" ";
		}
		cout<<endl;
	}
	cout<<endl;
}

void error_nbR(int nbR){
	cout<<"Invalid number of colors: "<<nbR<<endl;
}

void error_color(int id){
	cout<<"Invalid color value "<<id<<endl;
}

void error_threshold(double invalid_val){
	cout<<"Invalid threshold value: "<<invalid_val<<endl;
}

void error_nb_filter(int nb_filter){
	cout<<"Invalid number of filter: "<<nb_filter<<endl;
}
