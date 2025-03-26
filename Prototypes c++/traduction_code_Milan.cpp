#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <sstream>

using namespace std;

struct Pixel {
    int r, g, b;
};

std::ostream& operator<<(std::ostream& flux, const Pixel& pixel) {
	flux <<"(" << pixel.r << "," << pixel.g << "," << pixel.b << ")";
	return flux;
}

// Ignorer les commentaires pour accéder plus facilement aux infos du fichiers (résolution et valeurs des pixels)
void ignorerCommentairesEtEspaces(std::ifstream &file) {
    while (file.peek() == '#' || std::isspace(file.peek())) {
        if (file.peek() == '#') {
            file.ignore(256, '\n'); 
        } else {
            file.get(); 
        }
    }
}

int main() {
    std::ifstream file("imageAvecUnSegment.ppm");

    std::string format;
    int largeur, hauteur, maxValeur;

    ignorerCommentairesEtEspaces(file);
    file >> format;

    ignorerCommentairesEtEspaces(file);
    file >> largeur >> hauteur;

    ignorerCommentairesEtEspaces(file);
    file >> maxValeur;
    
    ignorerCommentairesEtEspaces(file);

    std::cout << "Format : " << format << std::endl;
    std::cout << "Largeur : " << largeur << ", Hauteur : " << hauteur << std::endl;
    std::cout << "Valeur max : " << maxValeur << std::endl;

    std::vector<std::vector<Pixel>> image(hauteur,std::vector<Pixel>(largeur));

    // enregistrement des pixels
    for (int i = 0; i < hauteur; ++i){
        for (int j=0; j<largeur; ++j){
            file >> image[i][j].r >> image[i][j].g >> image[i][j].b;
        }
    }

    // lecture pour voir
    for(int i=0;i<hauteur;++i){
        for(int j=0;j<largeur;++j){
            std::cout<<image[i][j]<<std::endl;
        } 
    }

    double thetaStep = M_PI / 180;
    vector<double> theta;
    for (int i = 0; i < 100; ++i) {
        theta.push_back(i * thetaStep);
    }
    
    int rhomax = int(ceil(sqrt(largeur*largeur+hauteur*hauteur)));
    vector<vector<int>> accumulator(2 * rhomax, vector<int>(theta.size(), 0));
    
    vector<int> indicesX, indicesY;
    for (int i = 0; i < hauteur; i++) {
        for (int j = 0; j < largeur; j++) {
            if ((image[i][j].r!=0)&&(image[i][j].g!=0)&&(image[i][j].b!=0)) {
                indicesX.push_back(j);
                indicesY.push_back(i);
            }
        }
    }
    
    for (size_t i = 0; i < indicesX.size(); i++) {
        int x = indicesX[i];
        int y = indicesY[i];
        for (size_t j = 0; j < theta.size(); j++) {
            int rho = round(x * cos(theta[j]) + y * sin(theta[j])) + rhomax;
            if (rho >= 0 && rho < 2 * rhomax) {
                accumulator[rho][j] += 1;
            }
        }
    }

    int max=0;
    for(int i=0;i<accumulator.size();++i){
        for(int j=0;j<accumulator[0].size();++j){
            if(accumulator[i][j]>max){
                max=accumulator[i][j];
            }
        }
    }
    std::cout<<"max accumulateur : "<<max<<std::endl;
    
    // seuil
    vector<int> thresholds = {5,6};
    for (int threshold : thresholds) {
        vector<pair<int, double>> linesDetected;
        for (int i = 0; i < 2 * rhomax; i++) {
            for (int j = 0; j < theta.size(); j++) {
                if (accumulator[i][j] >= threshold) {
                    int rho = i - rhomax;
                    linesDetected.push_back({rho, theta[j]});
                }
            }
        }
        
        // traage des droites sur image initiale
        for (const auto& line : linesDetected) {
            int rho = line.first;
            double t = line.second;
            for (int x = 0; x < largeur; x++) {
                int y = (rho - x * cos(t)) / sin(t);
                if (y >= 0 && y < hauteur) {
                    image[y][x].r=255;
                    image[y][x].g=0;
                    image[y][x].b=0;
                }
            }
        }
        
        // sauvegarde dans un fichier ppm
        std::ofstream outFile("resultatSegmentPolaire"+to_string(threshold)+".ppm");
        if (!outFile) {
            std::cerr << "Erreur : Impossible de créer le fichier resultatSegmentPolaire.ppm !" << std::endl;
            return 1;
        }

        outFile << "P3\n" << largeur << " " << hauteur << "\n255\n";
        for (int i = 0; i < hauteur; ++i) {
            for (int j = 0; j < largeur; ++j) {
                outFile << image[i][j].r << " " << image[i][j].g << " " << image[i][j].b << " ";
            }
            outFile << "\n";
        }

        outFile.close();
        std::cout << "Image enregistrée sous resultatSegmentPolaire"+to_string(threshold)+".ppm"<< std::endl;
    }
    


}
