#include <stdio.h>
#include <stdlib.h>


/* Nome da Equipe: 
   Membros:
            Aluno XYZ - RA 1234
            Aluno XYZ - RA 1234
            Aluno XYZ - RA 1234
            Aluno XYZ - RA 1234
*/


// Define uma estrututa para representar uma imagem
typedef struct {
    int rows;       // O número de linhas na imagem
    int cols;       // O número de colunas na imagem
    int** pixels;   // Matriz 2D para os pixels da imagem
} Image;

// Função para alocar memória para uma nova imagem
Image* allocateImage(int rows, int cols) {
    Image* image = (Image*)malloc(sizeof(Image));               // Aloca memória para a estrutura da imagem
    image->rows = rows;                                         // Define o número de linhas da imagem
    image->cols = cols;                                         // Define o número de colunas da imagem
    image->pixels = (int**)malloc(rows * sizeof(int*));         // Aloca memória para as linhas da imagem
    for (int i = 0; i < rows; i++) {
        image->pixels[i] = (int*)malloc(cols * sizeof(int));    // Aloca memória para as linhas da imagem
    }
    return image;
}

// Função para liberar a memória de uma imagem
void deallocateImage(Image* image) {
    for (int i = 0; i < image->rows; i++) {
        free(image->pixels[i]);
    }
    free(image->pixels);
    free(image);
}

// Função para ler uma imagem do formato PGM
// Aqui vai todo o código para abrir o arquivo, verificar se o formato é P2,
// ler as dimensões e os pixels da imagem, e alocar e preencher a estrutura da imagem.
void readPGM(const char* filename, Image** image) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Erro ao abrir o arquivo %s\n", filename);
        exit(1);
    }

    char format[3];
    fscanf(file, "%2s\n", format);
    if (format[0] != 'P' || format[1] != '2') {
        printf("Formato de imagem inválido\n");
        exit(1);
    }

    int cols, rows, maxGray;
    fscanf(file, "%d %d\n%d\n", &cols, &rows, &maxGray);
    *image = allocateImage(rows, cols);

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            fscanf(file, "%d", &((*image)->pixels[i][j]));
        }
    }

    fclose(file);
}

// Função para escrever uma imagem no formato PGM
// Aqui vai todo o código para abrir o arquivo, escrever o formato P2, 
// as dimensões e os pixels da imagem.
void writePGM(const char* filename, const Image* image) {
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        printf("Erro ao criar o arquivo %s\n", filename);
        exit(1);
    }

    fprintf(file, "P2\n");
    fprintf(file, "%d %d\n255\n", image->cols, image->rows);
    for (int i = 0; i < image->rows; i++) {
        for (int j = 0; j < image->cols; j++) {
            fprintf(file, "%d ", image->pixels[i][j]);
        }
        fprintf(file, "\n");
    }

    fclose(file);
}

// Função para calcular o histograma de uma imagem
// Aqui vai todo o código para percorrer todos os pixels da imagem
// e contar as ocorrências de cada valor de pixel no histograma.
void computeHistogram(const Image* image, int* histogram) {
    for (int i = 0; i < image->rows; i++) {
        for (int j = 0; j < image->cols; j++) {
            histogram[image->pixels[i][j]]++;
        }
    }
}

// Função para calcular o limiar ótimo de binarização de uma imagem
// Aqui vai todo o código para calcular o limiar ótimo, que maximiza 
// a variância entre as classes de pixels abaixo e acima do limiar.
int getOptimalThreshold(int* histogram) {
    int total = 0;
    for (int i = 0; i < 256; i++)
        total += histogram[i];

    float sum = 0;
    for (int i = 0; i < 256; i++)
        sum += i * histogram[i];

    float sumB = 0;
    int wB = 0;
    int wF = 0;

    float varMax = 0;
    int threshold = 0;

    for (int i = 0 ; i < 256 ; i++) {
        wB += histogram[i];               // Peso Background
        if (wB == 0)
            continue;

        wF = total - wB;                 // Peso Foreground
        if (wF == 0)
            break;

        sumB += (float) (i * histogram[i]);

        float mB = sumB / wB;            // Média do Background
        float mF = (sum - sumB) / wF;    // Média do Foreground

        // Calcula entre a variância de valores
        float varBetween = (float)wB * (float)wF * (mB - mF) * (mB - mF);

        // Checa se novo máximo foi encontrado
        if (varBetween > varMax) {
            varMax = varBetween;
            threshold = i;
        }
    }

    return threshold;
}


// Função para binarizar uma imagem com um dado limiar
// Aqui vai todo o código para alocar a imagem de saída, 
// percorrer todos os pixels da imagem de entrada e definir
// o pixel correspondente na imagem de saída como 0 ou 255
// dependendo se o pixel da imagem de entrada é abaixo ou acima do limiar.
void binarizeImage(const Image* inputImage, Image** outputImage, int threshold) {
    *outputImage = allocateImage(inputImage->rows, inputImage->cols);

    for (int i = 0; i < inputImage->rows; i++) {
        for (int j = 0; j < inputImage->cols; j++) {
            if (inputImage->pixels[i][j] >= threshold) {
                (*outputImage)->pixels[i][j] = 255;
            } else {
                (*outputImage)->pixels[i][j] = 0;
            }
        }
    }
}

// Função principal
int main() {
    Image* inputImage;
    // Trocar pela imagem a ser processada
    readPGM("castor.pgm", &inputImage);

    int histogram[256] = {0};
    computeHistogram(inputImage, histogram);

    int threshold = getOptimalThreshold(histogram);

    Image* outputImage;
    binarizeImage(inputImage, &outputImage, threshold);

    // Define o nome do arquivo como output
    writePGM("output.pgm", outputImage);

    deallocateImage(inputImage);
    deallocateImage(outputImage);

    return 0;
}
// FIM