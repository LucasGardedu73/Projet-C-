#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>

using namespace std;

void ignorerCommentairesEtEspaces(ifstream &file) {
    char c;
    while (file >> ws && (c = file.peek()) == '#') {
        file.ignore(256, '\n'); // Ignorer les commentaires pour accéder plus facilement aux infos du fichiers (résolution et valeurs des pixels)
    }
}

int main() {
    ifstream file("imagetetelucas.ppm", ios::binary);
    if (!file) {
        cerr << "Erreur: Impossible d'ouvrir le fichier imagetest.ppm\n";
        return 1;
    }

    string format;
    int largeur, hauteur, maxValeur;

    ignorerCommentairesEtEspaces(file);
    file >> format; // on récupère le format

    ignorerCommentairesEtEspaces(file);
    file >> largeur >> hauteur; // on récupère la résolution

    ignorerCommentairesEtEspaces(file);
    file >> maxValeur;
    file.get(); // Lire le saut de ligne après maxValeur

    if (format != "P5" && format != "P6") {
        cerr << "Format non supporté. Utilisez une image P5 (grayscale) ou P6 (couleur).\n";
        return 1;
    }

    cout << "Format : " << format << endl;
    cout << "Largeur : " << largeur << ", Hauteur : " << hauteur << endl;
    cout << "Valeur max : " << maxValeur << endl;

    vector<vector<int>> pixels(hauteur, vector<int>(largeur));

    if (format == "P5") {
        // Lecture d'une image en niveaux de gris (P5=gris)
        for (int i = 0; i < hauteur; i++) {
            for (int j = 0; j < largeur; j++) {
                unsigned char gray;
                file.read(reinterpret_cast<char*>(&gray), 1);
                pixels[i][j] = gray;
            }
        }
    } else if (format == "P6") {
        // Lecture d'une image couleur (conversion en niveaux de gris) (P6=couleurs)
        for (int i = 0; i < hauteur; i++) {
            for (int j = 0; j < largeur; j++) {
                unsigned char r, g, b;
                file.read(reinterpret_cast<char*>(&r), 1);
                file.read(reinterpret_cast<char*>(&g), 1);
                file.read(reinterpret_cast<char*>(&b), 1);
                // Conversion en niveaux de gris (luminance)
                pixels[i][j] = static_cast<int>(0.299 * r + 0.587 * g + 0.114 * b);
            }
        }
    }
    file.close();

    //  application du flou : convolution par matrice de gauss 5x5 (voir wikipedia pour la matrice)
    int gaussKernel[5][5] = { 
        {1, 4, 7, 4, 1}, 
        {4, 16, 26, 16, 4}, 
        {7, 26, 41, 26, 7}, 
        {4, 16, 26, 16, 4}, 
        {1, 4, 7, 4, 1} 
    };
    int kernelSum = 273; // Somme des coefficients

    vector<vector<int>> flou(hauteur, vector<int>(largeur, 0));

    for (int i = 2; i < hauteur - 2; i++) {
        for (int j = 2; j < largeur - 2; j++) {
            int somme = 0;
            for (int k = -2; k <= 2; k++) {
                for (int l = -2; l <= 2; l++) {
                    somme += pixels[i + k][j + l] * gaussKernel[k + 2][l + 2];
                }
            }
            flou[i][j] = somme / kernelSum;
        }
    }

    // application du filtre de Sobel (ça tu connais)
    int sobelX[3][3] = { {-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1} };
    int sobelY[3][3] = { {1, 2, 1}, {0, 0, 0}, {-1, -2, -1} };

    vector<vector<int>> result(hauteur, vector<int>(largeur, 0));

    for (int i = 1; i < hauteur - 1; i++) {
        for (int j = 1; j < largeur - 1; j++) {
            int gx = 0, gy = 0;
            for (int k = -1; k <= 1; k++) {
                for (int l = -1; l <= 1; l++) {
                    gx += flou[i + k][j + l] * sobelX[k + 1][l + 1];
                    gy += flou[i + k][j + l] * sobelY[k + 1][l + 1];
                }
            }
            result[i][j] = min(255, max(0, static_cast<int>(sqrt(gx * gx + gy * gy))));
        }
    }

    // sauvegarde dans un fichier ppm
    ofstream outFile("sobel_flou_gaussien_output.ppm", ios::binary);
    if (!outFile) {
        cerr << "Erreur: Impossible de créer le fichier sobel_flou_gaussien_output.ppm\n";
        return 1;
    }

    outFile << "P6\n" << largeur << " " << hauteur << "\n" << maxValeur << "\n";
    for (int i = 0; i < hauteur; i++) {
        for (int j = 0; j < largeur; j++) {
            unsigned char gray = static_cast<unsigned char>(result[i][j]);
            outFile.write(reinterpret_cast<char*>(&gray), 1);
            outFile.write(reinterpret_cast<char*>(&gray), 1);
            outFile.write(reinterpret_cast<char*>(&gray), 1);
        }
    }
    outFile.close();

    cout << "Image enregistrée sous sobel_flou_5x5_output.ppm" << endl;

    return 0;
}
