#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cmath> // Pour std::abs

struct Pixel {
    int r, g, b; // composantes rouge, vert et bleu
};

std::ostream& operator<<(std::ostream& flux, const Pixel& pixel) {
	flux <<"(" << pixel.r << "," << pixel.g << "," << pixel.b << ")";
	return flux;
}

void ignorerCommentairesEtEspaces(std::ifstream &file) {
    while (file.peek() == '#' || file.peek() == '\n') {
        file.ignore(256, '\n');
        }
    }

void detecterContours(const std::vector<std::vector<Pixel>>& image, int hauteur, int largeur, std::vector<std::vector<int>>& contours) {
    // Initialiser tableau des contours avec des zéros
    contours = std::vector<std::vector<int>>(hauteur, std::vector<int>(largeur, 0));

    // Initialiser une image vide pour stocker notre image grise
    std::vector<std::vector<int>> image_grise(hauteur, std::vector<int>(largeur, 0));
    
    for (int i = 0; i < hauteur; ++i) {
        for (int j=0; j<largeur; ++j){
            image_grise[i][j] = (image[i][j].r + image[i][j].g + image[i][j].b)/3; // Pour obtenir du gris on somme les 3 couleurs et on divise par 3
            std::cout<<image_grise[i][j]<<std::endl;
        }
    }

    // Parcourir l'image (en évitant les bords)
    for (int i = 1; i < hauteur - 1; ++i) {
        for (int j = 1; j < largeur - 1; ++j) {
            // Calculer les gradients de Sobel
            int gx = (image_grise[i-1][j+1] + 2 * image_grise[i][j+1] + image_grise[i+1][j+1])
                   - (image_grise[i-1][j-1] + 2 * image_grise[i][j-1] + image_grise[i+1][j-1]);
            int gy = (image_grise[i+1][j-1] + 2 * image_grise[i+1][j] + image_grise[i+1][j+1])
                   - (image_grise[i-1][j-1] + 2 * image_grise[i-1][j] + image_grise[i-1][j+1]);

            // Magnitude du gradient
            int magnitude = std::sqrt(gx * gx + gy * gy);

            // Ajouter la magnitude au tableau des contours en 
            contours[i][j] = magnitude; 
        }
    }
    
}

void reduireMatriceContours(const std::vector<std::vector<int>>& contours, std::vector<std::vector<int>>& matriceReduite) {
        int hauteur = contours.size();
        int largeur = contours[0].size();

        int minLigne = hauteur, maxLigne = -1;
        int minColonne = largeur, maxColonne = -1;

        // Identifier les limites de la matrice utile
        for (int i = 0; i < hauteur; ++i) {
            for (int j = 0; j < largeur; ++j) {
                if (contours[i][j] != 0) {
                    minLigne = std::min(minLigne, i);
                    maxLigne = std::max(maxLigne, i);
                    minColonne = std::min(minColonne, j);
                    maxColonne = std::max(maxColonne, j);
                }
            }
        }

        // Si aucun élément non nul, renvoyer une matrice vide
        if (minLigne > maxLigne || minColonne > maxColonne) {
            matriceReduite = {};
            return;
        }

        // Construire la matrice réduite
        matriceReduite = std::vector<std::vector<int>>(maxLigne - minLigne + 1, std::vector<int>(maxColonne - minColonne + 1));
        for (int i = minLigne; i <= maxLigne; ++i) {
            for (int j = minColonne; j <= maxColonne; ++j) {
                matriceReduite[i - minLigne][j - minColonne] = contours[i][j];
            }
        }
    }


int main() {
    std::ifstream file("imageM1.ppm");

    std::string format;
    int largeur, hauteur, maxValeur;

    ignorerCommentairesEtEspaces(file);
    file >> format;

    ignorerCommentairesEtEspaces(file);
    file >> largeur >> hauteur;

    ignorerCommentairesEtEspaces(file);
    file >> maxValeur;
    ignorerCommentairesEtEspaces(file); // Ignore la fin de ligne après le maxValeur     

    std::cout << "Format : " << format << std::endl;
    std::cout << "Largeur : " << largeur << ", Hauteur : " << hauteur << std::endl;
    std::cout << "Valeur max : " << maxValeur << std::endl;

    std::vector<std::vector<Pixel>> image(hauteur,std::vector<Pixel>(largeur));

    // Lecture des pixels et stockage dans variable image
    for (int i = 0; i < hauteur; ++i) {
        for (int j=0; j<largeur; ++j){
            file >> image[i][j].r >> image[i][j].g >> image[i][j].b;
        }
    }

    file.close();
    
    // Détecter les contours
    std::vector<std::vector<int>> contours;
    detecterContours(image, hauteur, largeur, contours);

    // Afficher les contours
    std::cout<<"Matrice contours :"<< std::endl;
    for (int i = 0; i < hauteur; ++i) {
        for (int j = 0; j < largeur; ++j) {
            std::cout << contours[i][j] << " ";
        }
        std::cout << std::endl;
    }

    std::vector<std::vector<int>> matriceReduite;
    reduireMatriceContours(contours, matriceReduite);

    // Afficher la matrice réduite
    std::cout<<"Matrice contours réduite :"<< std::endl;
    for (const auto& ligne : matriceReduite) {
        for (int val : ligne) {
            std::cout << val << " ";
        }
        std::cout << std::endl;
    }
}
