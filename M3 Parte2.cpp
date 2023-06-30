#include <iostream>
#include <fstream>
#include <string>
#include <cctype> //para usar toupper
#include <locale> 
#include <Windows.h> // para funcao de cor

// Alunas: Flávia Schnaider e Helena Becker Piazera

using namespace std;
#define tam_maximo 10
#define tam_matriz 20

struct ListaSorteada { //lista de palavras que aparece durante o jogo
    string palavra;
    bool esconder = false; //quando a palavra for encontrada, não aparecerá mais na tela
};

struct Lista {
    string palavras[tam_maximo];
    int tam = 0; //posições sendo utilizadas no vetor
    bool deletar = false;
};

struct Matriz {
    char letra; //letra de cada posição da matriz
    bool encontrada = false; //se a letra pertence a uma palavra encontrada ou não
};

void set_color(int color) {
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    COLORREF cor;
    SetConsoleTextAttribute(handle, color);
}

#pragma region listas

int opcao_invalida(string frase, int min, int max, bool mostrar_min_max) {
    int op;
    do {
        if (mostrar_min_max) {
            cout << frase << " (" << min << " - " << max << "): ";
            cin >> op;
        }
        else {
            cout << frase << ": ";
            cin >> op;
        }

        if (op < min || op > max) {
            set_color(4);
            cout << "\nOpcao invalida. Tente novamente.\n" << endl;
            set_color(7);
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
    int escolha = opcao_invalida("\nQual lista deseja deletar?", 0, tam, true);
    if (escolha == 0) return; // volta ao Menu
    else
        listas[escolha - 1].deletar = true;
}

void alterar_lista(Lista listas[], int tam) {
    cout << "Listas atuais: \n" << endl;
    mostrar_listas(listas, tam);

    int escolha = opcao_invalida("\n\nPara voltar ao Menu, digite 0.\nQual lista deseja alterar?", 0, tam, true);
    if (escolha == 0) {
        system("cls");
        return; // volta ao menu
    }
    listas[escolha - 1].tam = opcao_invalida("\n\nInforme quantas palavras deseja escrever", 0, tam_maximo, true);
    escrever_nova_lista(listas, escolha);
}

#pragma endregion listas

#pragma region montar jogo

int sortear_lista(int tam) {
    srand(time(NULL));
    int lista_sorteada = rand() % tam;
    return lista_sorteada;
}

void matriz_vazia(Matriz matriz[][tam_matriz]) {
    for (int i = 0; i < tam_matriz; i++) {
        for (int j = 0; j < tam_matriz; j++) {
            matriz[i][j].letra = '\0';
        }
    }
}

void mostrar_matriz(Matriz matriz[tam_matriz][tam_matriz], ListaSorteada lista_auxiliar[], int tam_lista, int palavras_encontradas) {
    cout << "\n\t";
    for (int i = 0; i < tam_matriz; i++) { // representa as colunas
        if (i < 10)
            cout << i << "   ";
        else if (i >= 10)
            cout << i << "  ";
    }
    cout << endl;
    for (int i = 0; i < tam_matriz; i++) { //representa o inicio de cada linha
        if (i <= 9)
            cout << "\n   " << i << " |";
        else
            cout << "\n  " << i << " |";

        for (int j = 0; j < tam_matriz; j++) {
            if (matriz[i][j].letra != '\0') {
                if (matriz[i][j].encontrada == true) {
                    set_color(13);
                    cout << "  " << matriz[i][j].letra << " ";
                    set_color(7);
                }
                else
                    cout << "  " << matriz[i][j].letra << " ";
            }
        }
        if (i == 0) {
            set_color(11);
            cout << "\t   Encontre as palavras:";
            set_color(7);
        }
        else if (i > 0 && i <= tam_lista) {
            if (lista_auxiliar[i - 1].esconder) {
                set_color(13);
                cout << "\t   " << lista_auxiliar[i - 1].palavra; //mostra ao lado do caça palavras a lista de palavras da lista sorteada
                set_color(7);
            }
            else 
                cout << "\t   " << lista_auxiliar[i - 1].palavra;
        }
        else if (i == tam_lista + 2)
            cout << "\t   Palavras encontradas: " << palavras_encontradas;
        cout << endl;
    }
}

bool eh_possivel_vertical(Matriz matriz[][tam_matriz], int linha, int coluna, int qtd_letras, int direcao, int inversao) {
    //verficadores para ver se a palavra caberá na vertical
    if (linha == 0 && inversao == 2)
        return false;
    else if (linha == (tam_maximo - 1) && inversao == 1)
        return false;
    else if (qtd_letras > (tam_maximo - 1 - linha) && inversao == 1)
        return false;
    else if (qtd_letras > (linha - 1) && inversao == 2)
        return false;

    //verfificador para ver se a palavra não ocupará uma posição onde já existe outra letra
    if (inversao == 1) {
        for (int i = linha; i < linha + qtd_letras; i++) {
            if (matriz[i][coluna].letra != '\0')
                return false;
        }
    }
    if (inversao == 2) {
        for (int i = linha; i > linha - qtd_letras; i--) {
            if (matriz[i][coluna].letra != '\0')
                return false;
        }
    }
    return true; //se for permitido colocar a palavra nas posições e direções sorteadas
}

bool eh_possivel_horizontal(Matriz matriz[][tam_matriz], int linha, int coluna, int qtd_letras, int direcao, int inversao) {
    //verficadores para ver se a palavra caberá na horizontal
    if (coluna == 0 && inversao == 2)
        return false;
    else if (coluna == tam_maximo - 1 && inversao == 1)
        return false;
    else if (qtd_letras > (tam_maximo - 1 - coluna) && inversao == 1)
        return false;
    else if (qtd_letras > (coluna - 1) && inversao == 2)
        return false;

    //verfificador para ver se a palavra não ocupará uma posição onde já existe outra letra
    if (inversao == 1) {
        for (int i = coluna; i < coluna + qtd_letras; i++) {
            if (matriz[linha][i].letra != '\0')
                return false;
        }
    }
    if (inversao == 2) {
        for (int i = coluna; i > coluna - qtd_letras; i--) {
            if (matriz[linha][i].letra != '\0')
                return false;
        }
    }
    return true; //se for permitido colocar a palavra nas posições e direções sorteadas
}

void palavra_vertical_na_matriz(Matriz matriz[][tam_matriz], string palavra, int inversao, int linha, int coluna) {
    if (inversao == 1) { //palavra fica normal
        for (char c : palavra) { //percorre cada caracter da palavra
            matriz[linha][coluna].letra += c;
            linha++;
        }
    }
    else { //palavra fica invertida
        for (char c : palavra) {
            matriz[linha][coluna].letra += c;
            linha--;
        }
    }
}

void palavra_horizontal_na_matriz(Matriz matriz[][tam_matriz], string palavra, int inversao, int linha, int coluna) {
    if (inversao == 1) { //palavra fica normal
        for (char c : palavra) { //percorre cada caracter da palavra
            matriz[linha][coluna].letra += c;
            coluna++;
        }
    }
    else { //palavra fica invertida
        for (char c : palavra) {
            matriz[linha][coluna].letra += c;
            coluna--;
        }
    }
}

string converter_letra_maiuscula(string palavra, int& qtd_letras) {
    string convertida;
    for (char c : palavra) {
        c = toupper(c); //converte a palavra para letras maiusculas
        convertida += c;
        qtd_letras++;
    }
    return convertida;
}

void preencher_matriz_com_lista(Matriz matriz[][tam_matriz], string palavra) {
    int linha = 0, coluna = 0, qtd_letras = 0, direcao = 0, inversao = 0;
    bool valido = false, pos_valida = false;

    palavra = converter_letra_maiuscula(palavra, qtd_letras);

    srand(time(NULL));
    while (pos_valida == false) {
        do { //sorteia linha e coluna para a primeira letra da palavra
            linha = rand() % tam_matriz;
            coluna = rand() % tam_matriz;
            if (matriz[linha][coluna].letra == '\0') valido = true;
            else valido = false;
        } while (valido == false); //enquanto não sortear uma posição vazia da matriz

        inversao = 1 + rand() % 2; //1 - normal, 2 - invertido
        direcao = 1 + rand() % 2; //1 - vertical, 2 - horizontal

        if (direcao == 1)
            pos_valida = eh_possivel_vertical(matriz, linha, coluna, qtd_letras, direcao, inversao);
        if (direcao == 2)
            pos_valida = eh_possivel_horizontal(matriz, linha, coluna, qtd_letras, direcao, inversao);
    }

    if (pos_valida) {
        if (direcao == 1)
            palavra_vertical_na_matriz(matriz, palavra, inversao, linha, coluna);
        if (direcao == 2)
            palavra_horizontal_na_matriz(matriz, palavra, inversao, linha, coluna);
    }
}

void preencher_matriz_com_caracteres(Matriz matriz[][tam_matriz]) {
    srand(time(NULL));

    for (int i = 0; i < tam_matriz; i++) {
        for (int j = 0; j < tam_matriz; j++) {
            char letra = 'A' + (rand() % 26); //gera um número de 0 a 25, somado com o valor ASCII de 'A' (65) para ter letras maiusculas aleatorias
            if (matriz[i][j].letra == '\0')
                matriz[i][j].letra = letra;
        }
    }
}

#pragma endregion montar jogo

#pragma region verificar jogada

bool verificar_vertical_normal(Matriz matriz[][tam_matriz], string palavra, int linha, int coluna, int qtd_letras, int& inversao) {
    int letras_compativeis = 0;
    for (char c : palavra) {
        if (c == matriz[linha][coluna].letra && matriz[linha][coluna].encontrada == false) // só valida palavra q ainda não foi encontrada
            letras_compativeis++;
        linha++;
    }
    if (letras_compativeis == qtd_letras) {
        inversao = 1; //salva o valor da inversao por referencia
        return true;
    }
    else return false;
}

bool verificar_vertical_inversa(Matriz matriz[][tam_matriz], string palavra, int linha, int coluna, int qtd_letras, int& inversao) {
    int letras_compativeis = 0;
    for (char c : palavra) {
        if (c == matriz[linha][coluna].letra && matriz[linha][coluna].encontrada == false) // só valida palavra q ainda não foi encontrada
            letras_compativeis++;
        linha--;
    }
    if (letras_compativeis == qtd_letras) {
        inversao = 2; //salva o valor da inversao por referencia
        return true;
    }
    else return false;
}

bool verificar_horizontal_normal(Matriz matriz[][tam_matriz], string palavra, int linha, int coluna, int qtd_letras, int& inversao) {
    int letras_compativeis = 0;
    for (char c : palavra) {
        if (c == matriz[linha][coluna].letra && matriz[linha][coluna].encontrada == false) // só valida palavra q ainda não foi encontrada
            letras_compativeis++;
        coluna++;
    }
    if (letras_compativeis == qtd_letras) {
        inversao = 1; //salva o valor da inversao por referencia
        return true;
    }
    else return false;
}

bool verificar_horizontal_inversa(Matriz matriz[][tam_matriz], string palavra, int linha, int coluna, int qtd_letras, int& inversao) {
    int letras_compativeis = 0;
    for (char c : palavra) {
        if (c == matriz[linha][coluna].letra && matriz[linha][coluna].encontrada == false) //só valida palavra q ainda não foi encontrada
            letras_compativeis++;
        coluna--;
    }
    if (letras_compativeis == qtd_letras) {
        inversao = 2; //salva o valor da inversao por referencia
        return true;
    }
    else return false;
}

bool encontrou_palavra(Matriz matriz[][tam_matriz], string palavra, int linha, int coluna, int direcao, int& qtd_letras, int& inversao) {
    int letras_compativeis = 0;

    palavra = converter_letra_maiuscula(palavra, qtd_letras); //coloca a palavra encontrada em letras maiusculas para poder fazer a comparação

    if (direcao == 1) { //verificar palavra na vertical
        if (verificar_vertical_normal(matriz, palavra, linha, coluna, qtd_letras, inversao))
            return true;
        if (verificar_vertical_inversa(matriz, palavra, linha, coluna, qtd_letras, inversao))
            return true;
        else
            return false;
    }
    if (direcao == 2) { //verificar palavra na vertical
        if (verificar_horizontal_normal(matriz, palavra, linha, coluna, qtd_letras, inversao))
            return true;
        if (verificar_horizontal_inversa(matriz, palavra, linha, coluna, qtd_letras, inversao))
            return true;
        else
            return false;
    }
}

void atualizar_matriz(Matriz matriz[][tam_matriz], int linha, int coluna, int direcao, int qtd_letras, int inversao) {
    if (direcao == 1) { //vertical
        if (inversao == 1) { //sentido normal 
            for (int i = linha; i < linha + qtd_letras; i++)
                matriz[i][coluna].encontrada = true;
        }
        if (inversao == 2) { //sentido inverso 
            for (int i = linha; i > linha - qtd_letras; i--)
                matriz[i][coluna].encontrada = true;
        }
    }
    if (direcao == 2) { //horizontal
        if (inversao == 1) { //sentido normal 
            for (int i = coluna; i < coluna + qtd_letras; i++)
                matriz[linha][i].encontrada = true;
        }
        if (inversao == 2) { //sentido inverso 
            for (int i = coluna; i > coluna - qtd_letras; i--)
                matriz[linha][i].encontrada = true;
        }
    }
}

#pragma endregion verificar jogada

int exibirMenu() {
    set_color(11);
    cout << "\n-----------------------------------------------------------\n";
    cout << "                          MENU                                 ";
    cout << "\n-----------------------------------------------------------\n" << endl;
    set_color(7);
    cout << "          [1] - Visualizar Lista Completa               \n"
        << "          [2] - Escrever nova lista                     \n"
        << "          [3] - Deletar uma lista                       \n"
        << "          [4] - Alterar uma Lista                       \n"
        << "          [5] - Jogar                                   \n"
        << "          [0] - Salvar e Sair                           \n"
        << "                                                        \n";
    int escolha = opcao_invalida("\t  Digite uma opcao do menu ", 0, 5, true);
    return escolha;
}

int main()
{
    string palavra;
    int escolha, tam = 1, pos_sorteada, linha = 0, coluna = 0, direcao = 0, encontradas = 0, inversao = 0, qtd_letras = 0;
    bool palavra_valida = false;
    Lista* listas = new Lista[tam];

    int cont_linhas = contar_linhas_arquivo(); //contando linhas do arquivo csv

    if (cont_linhas > 1) //se tiver mais que uma linha no arquivo, aumenta o tamamho do vetor de listas
        listas = aumentar_vetor(listas, tam, cont_linhas - 1);

    if (cont_linhas > 0) //se o arquivo não estiver vazio, pega as listas e salva no vetor
        pegar_listas_do_arquivo(listas, tam);

    while (true) {
        system("cls");
        escolha = exibirMenu();

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

            listas[tam - 1].tam = opcao_invalida("\n\tDigite 0 para voltar para o Menu.\n\nInforme quantas palavras deseja escrever", 0, tam_maximo, true);
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
            encontradas = 0; //reinicia palavras encontradas como zero
            
            Matriz matriz[tam_matriz][tam_matriz]; //criado dentro do case para reiniciar todas as variáveis do struct a cada novo jogo
            matriz_vazia(matriz); //limpa a matriz
            
            if (cont_linhas == 0) {
                cout << "\n\tNao existem listas registradas no momento, volte para o menu e escreva uma lista\n" << endl;
                system("pause");
                break;
            }
            pos_sorteada = sortear_lista(cont_linhas); //sorteia uma das listas de palavras para o jogo
            
            ListaSorteada* lista_auxiliar = new ListaSorteada[listas[pos_sorteada].tam]; //lista auxiliar para manter o controle da lista de palavras que aparece na tela
            for (int i = 0; i < listas[pos_sorteada].tam; i++) {
                lista_auxiliar[i].palavra = listas[pos_sorteada].palavras[i]; //copia as palavras da lista original poara a lista auxiliar
            }

            for (int i = 0; i < listas[pos_sorteada].tam; i++) {
                palavra = listas[pos_sorteada].palavras[i];
                preencher_matriz_com_lista(matriz, palavra); //coloca uma palavra de cada vez na matriz
            }
            preencher_matriz_com_caracteres(matriz);

            do {
                system("cls");
                qtd_letras = 0, inversao = 0;
                cout << "\n";
                mostrar_matriz(matriz, lista_auxiliar, listas[pos_sorteada].tam, encontradas); //mostra matriz completamente preenchida (palavras + caracteres aleatorios)

                cout << "\n\tPalavra encontrada ('fim' para voltar ao menu): ";
                cin >> palavra;

                if (palavra == "fim")  //volta ao menu no meio do jogo
                    break;

                palavra_valida = false;
                for (int i = 0; i < listas[pos_sorteada].tam; i++) {
                    if (palavra == listas[pos_sorteada].palavras[i]) { //verifica se a palavra encontrada pertence a lista sorteada
                        palavra_valida = true;
                        break;
                    }
                }
                cout << "\n\t\tLinha da primeira letra: ";
                cin >> linha;
                cout << "\n\t\tColuna da primeira letra: ";
                cin >> coluna;

                direcao = opcao_invalida("\n\t\tPalavra esta na vertical (1) ou horizontal (2)", 1, 2, false);

                //verifica se a palavra encontrada esta dentro das posições indicadas na matriz
                palavra_valida = false;
                palavra_valida = encontrou_palavra(matriz, palavra, linha, coluna, direcao, qtd_letras, inversao);
                
                if (!palavra_valida) {
                    set_color(12);
                    cout << "\n\tPalavra Invalida\n\n";
                    set_color(7);
                }
                else if (palavra_valida) {
                    set_color(2);
                    cout << "\n\tPalavra Valida\n\n";
                    set_color(7);

                    for (int i = 0; i < listas[pos_sorteada].tam; i++) {
                        if (lista_auxiliar[i].palavra == palavra) {
                            lista_auxiliar[i].esconder = true; //atualiza a variavel esconder
                            break;
                        }
                    }
                    encontradas++; //aumenta quantidade de palavras encontradas  
                    atualizar_matriz(matriz, linha, coluna, direcao, qtd_letras, inversao); //define as posições da matriz correspondentes a palavra como encontradas = true;
                }
                
                if (encontradas == listas[pos_sorteada].tam) { //se a qtd de palavras encontradas for igual a qtd de palavras da lista, houve vitoria
                    set_color(10);
                    cout << "\n\tParabens! Voce conseguiu encontrar todas as palavras\n\n";
                    set_color(7);
                }
                system("pause");
            } while (encontradas != listas[pos_sorteada].tam || palavra == "fim");
            
            delete[]lista_auxiliar;
            break;
        }
    }
}