#include <iostream>
#include <fstream>
#include <string>
#include <cctype> //para usar toupper
#include <locale> 

// Alunas: Flávia Schnaider e Helena Becker Piazera

using namespace std;
#define tam_maximo 10
#define tam_matriz 20
/*
PARTE II
    - Seu programa deverá ler as listas de palavras e armazená-las em memória principal (usando structs)
    - O programa irá sortear uma das listas armazenadas e elaborar com ela um jogo de caça-palavras
    - Para isso, criar uma matriz de caracteres, com dimensões mínimas de 20x20,
    - As palavras da lista devem ser colocadas na vertical e horizontal, no sentido normal ou invertido (diagonal é opcional)
    - Após isso, ele irá preencher o restante da matriz com caracteres aleatórios.
    - O usuário deve buscar as palavras da lista.
    - O programa irá apresentar a matriz e a lista de palavras escondidas, e permitir localizar cada uma delas
    - Quando achar uma palavra, ela deve ser escondida na matriz (que será apresentada novamente), até que todas as palavras sejam encontradas
    - Para a busca das palavras, você deve simplificar o processo. Solicite, por exemplo, ao usuário qual é a palavra 
    e a posição da primeira letra. Depois você verifica todas as possibilidades (vertical, horizonta, normal, invertida) 
    a partir daquela posição. Achando em uma delas, você deve excluir a palavra localizada da busca. 
*/

#pragma region listas

struct Lista {
    string palavras[tam_maximo];
    int tam = 0;
    bool deletar = false;
};

int opcao_invalida(string frase, int min, int max) {
    int op;
    do {
        cout << frase << " (" << min << " - " << max << "): ";
        cin >> op;
        if (op < min || op > max) {
            cout << "\nOpcao invalida. Tente novamente.\n" << endl;
        }
        if (cin.fail()) {
            cin.clear();
            cin.ignore();
            op = min - 1;
        }
    } while (op < min || op > max);
    return op;
}

bool verificar_caracteres(string palavra) {
    locale local;
    for (char c : palavra)
        if (!isalpha(c, local) || c == 'ç' || c == 'Ç')
            return false; // caracter inválido encontrado
    return true; // todos os caracteres são válidos
}

template<typename T>
T* aumentar_vetor(T* listas, int& tam, int pos) {
    T* aux = new T[tam + pos]; //aumenta o vetor com a quantidade de posições desejadas
    for (int i = 0; i < tam; i++)
        aux[i] = listas[i];

    delete[] listas;
    tam += pos;
    return aux;
}

int contar_linhas_arquivo() {
    ifstream arquivo;
    arquivo.open("listaPalavras.csv");
    int contador = 0;
    string linha;

    if (arquivo.is_open()) {
        while (getline(arquivo, linha)) {
            if (linha.empty() == false) {
                contador++;
            }
        }
        arquivo.close();
        return contador;
    }
    else return -1;
}

void escrever_nova_lista(Lista listas[], int tam) {
    int cont = 0;
    string palavra;
    cin.ignore(); //para não pegar o ultimo enter do int main
    do {
        cout << "\n\t" << cont + 1 << ") ";
        getline(cin, palavra);
        if (verificar_caracteres(palavra)) { //verifica se as palavras digitadas sao validas (sem caracteres especiais)
            listas[tam - 1].palavras[cont] = palavra; //se for valido salva a palavra na lista
            cont++;
        }
        else
            cout << "\tPalavra com caracter invalido, digite novamente";
    } while (cont < listas[tam - 1].tam);
    cout << "\nPalavras adicionadas a lista com sucesso!" << endl;
}

void mostrar_listas(Lista listas[], int tam) {
    for (int i = 0; i < tam; i++) {
        if (!listas[i].deletar) {
            cout << "\n\tLista " << i + 1 << ": ";

            for (int j = 0; j < listas[i].tam; j++) {
                if (j == listas[i].tam - 1) // se for a ultima palavra da lista não coloca a virgula no final
                    cout << listas[i].palavras[j];
                else
                    cout << listas[i].palavras[j] << ", ";
            }
            cout << endl;
        }
    }
}

void escrever_no_arquivo(Lista listas[], string nome_arq, int tam) {
    ofstream arquivo;
    arquivo.open(nome_arq);
    if (arquivo.is_open()) {
        for (int i = 0; i < tam; i++) {
            if (!listas[i].deletar) { //só permite salvar as listas que onde deletar é false
                for (int j = 0; j < listas[i].tam; j++) {
                    arquivo << listas[i].palavras[j] << ","; //salva as palavras no arquivo com a virgula como separador
                }
                arquivo << endl;
            }
        }
        arquivo.close();
        cout << "Arquivo salvo com sucesso!" << endl;
    }
    else cout << "\nErro ao abrir o arquivo\n";
}

void pegar_listas_do_arquivo(Lista listas[], int tam) {
    ifstream arquivo;
    arquivo.open("listaPalavras.csv");
    string palavra, linha;

    if (arquivo.is_open()) {
        for (int i = 0; i < tam; i++) {
            int pos = 0; //coloca a posição inicial de cada vetor "palavras" como 0
            getline(arquivo, linha); //pega uma linha do arquivo

            for (char c : linha) { //percorre cada caracter da linha
                if (c != ',') //pega todos os caracteres ate encontrar uma virgula
                    palavra += c; //salva cada caracter na string
                else {
                    listas[i].palavras[pos] = palavra;
                    palavra.clear();
                    pos++;
                }
            }
            listas[i].tam = pos; //posições ocupadas do vetor palavras
        }
        arquivo.close();
    }
    else //se nao abrir o arquivo corretamente
        cout << "\nErro ao abrir o arquivo\n";
}

void deletar_lista(Lista listas[], int tam) {
    cout << "Listas atuais: \n" << endl;
    mostrar_listas(listas, tam);

    cout << "\nPara voltar ao Menu, digite 0.";
    int escolha = opcao_invalida("\nQual lista deseja deletar?", 0, tam);
    if (escolha == 0) return; // volta ao Menu
    else
        listas[escolha - 1].deletar = true;
}

void alterar_lista(Lista listas[], int tam) {
    cout << "Listas atuais: \n" << endl;
    mostrar_listas(listas, tam);

    int escolha = opcao_invalida("\n\nPara voltar ao Menu, digite 0.\nQual lista deseja alterar?", 0, tam);
    if (escolha == 0) {
        system("cls");
        return; // volta ao menu
    }
    listas[escolha - 1].tam = opcao_invalida("\n\nInforme quantas palavras deseja escrever", 0, tam_maximo);
    escrever_nova_lista(listas, escolha);
}

#pragma endregion listas

#pragma region jogo

int sortear_lista(int tam) {
    srand(time(NULL));
    int lista_sorteada = rand() % tam;
    return lista_sorteada;
}

void matriz_vazia(char matriz[][tam_matriz]) {
    for (int i = 0; i < tam_matriz; i++) {
        for (int j = 0; j < tam_matriz; j++) {
            matriz[i][j] = '\0';
        }
    }
}

void mostrar_matriz(char matriz[tam_matriz][tam_matriz]) {
    cout << "\n\t";
	for (char letra = 'a'; letra <= 't'; letra++) { // Percorre o alfabeto de 'a' a 'z' para representar as colunas
        if (letra == 'a') 
            cout << "      " << letra << "   ";
        else 
            cout << letra << "   ";
	}
    cout << endl;
	for (int i = 0; i < tam_matriz; i++) {
        if (i <= 9)
		    cout << "\n\t " << i << " |";
        else 
            cout << "\n\t" << i << " |";

		for (int j = 0; j < tam_matriz; j++) {
			cout << "  " << matriz[i][j] << " "; //mostra a letra de cada posição da matriz
		}
		cout << endl;
	}
}
bool eh_possivel_vertical(char matriz[][tam_matriz], int linha, int coluna, int qtd_letras, int direcao, int inversao) {
    //verficadores para ver se a palavra caberá na vertical
    if (linha == 0 && inversao == 2)
        return false;
    else if (linha == (tam_maximo - 1) && inversao == 1)
        return false;
    else if (qtd_letras > (tam_maximo - 1 - linha) && inversao == 1)
        return false;
    else if (qtd_letras > (linha - 1) && inversao == 2)
        return false;

    //verfificador para ver se a apalvra não ocupará uma posição onde já existe outra letra
    if (inversao == 1) {
        for (int i = linha; i < linha + qtd_letras; i++) {
            if (matriz[i][coluna] != '\0') {
                return false;
            }
        }
    }
    if (inversao == 2) {
        for (int i = linha; i > linha - qtd_letras; i--) {
            if (matriz[i][coluna] != '\0') {
                return false;
            }
        }
    }
    return true; //se for permitido colocar a palavra nas posições e direções sorteadas
}

bool eh_possivel_horizontal(char matriz[][tam_matriz], int linha, int coluna, int qtd_letras, int direcao, int inversao) {
    //verficadores para ver se a palavra caberá na horizontal
    if (coluna == 0 && inversao == 2)
        return false;
    else if (coluna == tam_maximo - 1 && inversao == 1)
        return false;
    else if (qtd_letras > (tam_maximo - 1 - coluna) && inversao == 1)
        return false;
    else if (qtd_letras > (coluna - 1) && inversao == 2)
        return false;

    //verfificador para ver se a apalvra não ocupará uma posição onde já existe outra letra
    if (inversao == 1) {
        for (int i = coluna; i < coluna + qtd_letras; i++) {
            if (matriz[linha][i] != '\0') {
                return false;
            }
        }
    }
    if (inversao == 2) {
        for (int i = coluna; i < coluna - qtd_letras; i--) {
            if (matriz[linha][i] != '\0') {
                return false;
            }
        }
    }
    return true; //se for permitido colocar a palavra nas posições e direções sorteadas
}

void palavra_vertical_na_matriz(char matriz[][tam_matriz], string palavra, int inversao, int linha, int coluna) {
    if (inversao == 1) { //palavra fica normal
        for (char c : palavra) { //percorre cada caracter da palavra
            matriz[linha][coluna] += c;
            linha++;
        }
    }
    else { //palavra fica invertida
        for (char c : palavra) {
            matriz[linha][coluna] += c;
            linha--;
        }
    }
}

void palavra_horizontal_na_matriz(char matriz[][tam_matriz], string palavra, int inversao, int linha, int coluna) {
    if (inversao == 1) { //palavra fica normal
        for (char c : palavra) { //percorre cada caracter da palavra
            matriz[linha][coluna] += c;
            coluna++;
        }
    }
    else { //palavra fica invertida
        for (char c : palavra) {
            matriz[linha][coluna] += c;
            coluna--;
        }
    }
}

void preencher_matriz_com_lista(char matriz[][tam_matriz], string palavra) {
    int linha = 0, coluna = 0, qtd_letras = 0, direcao = 0, inversao = 0;
    bool valido = false, pos_valida = false;
    
    string convertida;
    for (char c : palavra) {
        c = toupper(c); //converte a palavra para letras maiusculas
        convertida += c;
        qtd_letras++;
    }

    srand(time(NULL));
    while (pos_valida == false) {
        do { //sorteia linha e coluna para a primeira letra da palavra
            linha = rand() % tam_matriz;
            coluna = rand() % tam_matriz;
            if (matriz[linha][coluna] == '\0') valido = true;
            else valido = false;
        } while (valido == false); //enquanto não sortear uma posição vazia da matriz

        inversao = 1 + rand() % 2; //1 - normal, 2 - invertido
        direcao = 1 + rand() % 2; //1 - vertical, 2 - horizontal, 3 - diagonal SEM DIAGONAL POR ENQUANTO

        if (direcao == 1)
            pos_valida = eh_possivel_vertical(matriz, linha, coluna, qtd_letras, direcao, inversao);
        if (direcao == 2)
            pos_valida = eh_possivel_horizontal(matriz, linha, coluna, qtd_letras, direcao, inversao);
    }

    if (pos_valida) {
        if (direcao == 1)
            palavra_vertical_na_matriz (matriz, convertida, inversao, linha, coluna);
        if (direcao == 2)
            palavra_horizontal_na_matriz(matriz, convertida, inversao, linha, coluna);
    }
}

//fazer função que preenche resto da matriz com caracteres aleatorios

#pragma endregion jogo

void exibirMenu() {
    cout << "\n - - - - - MENU - - - - -\n" << endl
        << " [1] - Visualizar Lista Completa\n"
        << " [2] - Escrever nova lista\n"
        << " [3] - Deletar uma lista\n"
        << " [4] - Alterar uma Lista\n"
        << " [5] - Jogar\n"
        << " [0] - Salvar e Sair\n";
}

int main()
{
    string palavra;
    int escolha, tam = 1, pos_sorteada;
    Lista* listas = new Lista[tam];
    
    char matriz[tam_matriz][tam_matriz];

    int cont_linhas = contar_linhas_arquivo(); //contando linhas do arquivo csv

    if (cont_linhas > 1) //se tiver mais que uma linha no arquivo, aumenta o tamamho do vetor de listas
        listas = aumentar_vetor(listas, tam, cont_linhas - 1);

    if (cont_linhas > 0) //se o arquivo não estiver vazio, pega as listas e salva no vetor
        pegar_listas_do_arquivo(listas, tam);

    while (true) {
        exibirMenu();
        escolha = opcao_invalida("\nDigite sua escolha", 0, 5);

        switch (escolha)
        {
        case 0: // SALVAR ARQUIVO E SAIR
            escrever_no_arquivo(listas, "listaPalavras.csv", tam);
            delete[] listas;
            return 0;
            break;

        case 1: // VISUALIZAR LISTA
            system("cls");
            if (cont_linhas == 0) {
                cout << "\n\tNao existem listas registradas no momento, volte para o menu e escreva uma lista\n" << endl;
                system("pause");
                system("cls");
                break;
            }
            mostrar_listas(listas, tam);
            cout << "\n";
            system("pause");
            system("cls");
            break;

        case 2: // ESCREVER LISTA
            system("cls");
            if (cont_linhas == tam)
                listas = aumentar_vetor(listas, tam, 1);

            listas[tam - 1].tam = opcao_invalida("\n\tDigite 0 para voltar para o Menu.\n\nInforme quantas palavras deseja escrever", 0, tam_maximo);
            if (listas[tam - 1].tam == 0) { //voltar ao menu
                system("cls");
                break;
            }
            escrever_nova_lista(listas, tam);
            cont_linhas++;
            system("pause");
            system("cls");
            break;

        case 3: // DELETAR UMA LISTA
            system("cls");
            if (cont_linhas == 0) {
                cout << "\n\tNao existem listas registradas no momento, volte para o menu e escreva uma lista\n" << endl;
                system("pause");
                system("cls");
                break;
            }
            cont_linhas = cont_linhas - 1;
            deletar_lista(listas, tam);
            system("cls");
            break;

        case 4: //ALTERAR LISTA
            system("cls");
            if (cont_linhas == 0) {
                cout << "\n\tNao existem listas registradas no momento, volte para o menu e escreva uma lista\n" << endl;
                break;
            }
            alterar_lista(listas, tam);
            system("cls");
            break;

        case 5: // JOGAR
            system("cls");
            matriz_vazia(matriz); //limpa a matriz
            if (cont_linhas == 0) {
                cout << "\n\tNao existem listas registradas no momento, volte para o menu e escreva uma lista\n" << endl;
                system("pause");
                system("cls");
                break;
            }
            pos_sorteada = sortear_lista(cont_linhas);

            for (int i = 0; i < listas[pos_sorteada].tam; i++) {
                palavra = listas[pos_sorteada].palavras[i];
                preencher_matriz_com_lista(matriz, palavra);
            }
            cout << "\n\n";
            mostrar_matriz(matriz);

            system("pause");
            system("cls");
            break;
        }
    }
}
