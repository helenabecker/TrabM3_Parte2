#include <iostream>
#include <fstream>
#include <string>
#include <cstdio> 
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
            contador++;
        }
        arquivo.close();
        return contador;
    }
    else return 0;
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

void verifica_temp() {
    ifstream temp_arquivo("listaPalavras.temp");
    if (temp_arquivo.good()) {
        ofstream csv_arquivo("listaPalavras.csv");
        csv_arquivo << temp_arquivo.rdbuf();
        csv_arquivo.close();
        temp_arquivo.close();
        remove("listaPalavras.temp");

    }
}

void popular_lista(Lista listas[], int tam) {
    ifstream arquivo;
    arquivo.open("listaPalavras.csv");
    int contador = 0;
    string linha;
    contador = contar_linhas_arquivo();

    if (arquivo.is_open()) { // atualiza a lista de palavras
        while (getline(arquivo, linha)) {
            listas[tam].palavras[contador++];
        }
        arquivo.close();
    }
}
#pragma endregion listas

#pragma region jogo

int sortear_lista(int tam) {
    srand(time(NULL));
    int lista_sorteada = rand() % tam;
    return lista_sorteada;
}

//fazer função de mostrar matriz

//fazer função que preenche matriz com as palavras da lista sorteada

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
    int escolha, tam = 1, lista_sorteada;
    Lista* listas = new Lista[tam];
    char matriz[tam_matriz][tam_matriz];

    verifica_temp(); //arquivo de backup
    //Sleep(1000);
    popular_lista(listas, tam);
    
    int cont_linhas = contar_linhas_arquivo(); // contando linhas depois de popular

    if (cont_linhas > 1) 
        listas = aumentar_vetor(listas, tam, cont_linhas - 1);

    if (cont_linhas > 0)
        pegar_listas_do_arquivo(listas, tam);

    while (true) {
        exibirMenu();
        escolha = opcao_invalida("\nDigite sua escolha", 0, 5);

        switch (escolha)
        {
        case 0: // SALVAR ARQUIVO E SAIR
            escrever_no_arquivo(listas, "listaPalavras.csv", tam);
            delete[] listas;
            remove("listaPalavras.temp");
            return 0;
            break;

        case 1: // VISUALIZAR LISTA
            system("cls");
            if (cont_linhas == 0) {
                cout << "\n\tNao existem listas registradas no momento, volte para o menu e escreva uma lista" << endl;
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
            escrever_no_arquivo(listas, "listaPalavras.temp", tam);
            cont_linhas++;
            system("pause");
            system("cls");
            break;

        case 3: // DELETAR UMA LISTA
            system("cls");
            if (cont_linhas == 0) {
                cout << "\n\tNao existem listas registradas no momento, volte para o menu e escreva uma lista" << endl;
                system("pause");
                system("cls");
                break;
            }
            cont_linhas = cont_linhas - 1;
            deletar_lista(listas, tam);
            escrever_no_arquivo(listas, "listaPalavras.temp", tam);
            system("cls");
            break;

        case 4: //ALTERAR LISTA
            system("cls");
            if (cont_linhas == 0) {
                cout << "\n\tNao existem listas registradas no momento, volte para o menu e escreva uma lista" << endl;
                break;
            }
            alterar_lista(listas, tam);
            escrever_no_arquivo(listas, "listaPalavras.temp", tam);
            system("cls");
            break;

        case 5: // JOGAR
            system("cls");
            if (cont_linhas == 0) {
                cout << "\n\tNao existem listas registradas no momento, volte para o menu e escreva uma lista" << endl;
                system("pause");
                system("cls");
                break;
            }
            lista_sorteada = sortear_lista(cont_linhas);
            cout << lista_sorteada;
            
            system("pause");
            system("cls");
            break;
        }
    }
}