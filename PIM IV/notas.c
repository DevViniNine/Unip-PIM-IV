#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <glib.h>
#include <limits.h>

#ifdef _WIN32
#include <windows.h>
#include <shellapi.h>
#else
#include <unistd.h>
#endif
#define MAX_INDUSTRIAS 100
#define MAX_USUARIOS 100
#define DADOS_CSV "DADOS.csv"



typedef struct {
    char tipo[10];
    int id;
    char login[50];
    char senha[50];
} Usuario;

Usuario usuarios[MAX_USUARIOS];
int total_usuarios = 0;

typedef struct {
    char nome_empresa[100];
    char razao_social[100];
    char nome_fantasia[100];
    char cnpj[20];
    char endereco[200];
    char cidade[100];
    char estado[100];
    char cep[9];
    char data_abertura[11];
    float custos;
    float residuos;
} Industria;


void carregar_dados(GtkListStore *liststore) {
    FILE *file = fopen("industrias.csv", "r");
    if (file == NULL) {
        g_print("Erro ao abrir o arquivo CSV.\n");
        return;
    }

    // Variáveis para armazenar os dados de cada linha
    char nome[100], cnpj[20], data_abertura[20];
    GtkTreeIter iter;

    // Ignora a primeira linha do arquivo (cabeçalho)
    fscanf(file, "%*[^\n]\n");

    // Lê o arquivo linha por linha
    while (fscanf(file, "%99[^,],%19[^,],%19[^\n]\n", nome, cnpj, data_abertura) == 3) {
        // Adiciona uma nova linha na ListStore
        gtk_list_store_append(liststore, &iter);
        // Insere os dados lidos na ListStore
        gtk_list_store_set(liststore, &iter,
                           0, nome,            // Coluna 0: Nome
                           1, cnpj,            // Coluna 1: CNPJ
                           2, data_abertura,   // Coluna 2: Data de Abertura
                           -1);                 // Finaliza a lista de colunas
    }

    // Fecha o arquivo após a leitura
    fclose(file);

    g_print("Dados carregados com sucesso.\n");
}
//------------------------------------FUNÇÃO PARA GERAR RELATORIOS---------------

void on_relatorios_clicked(GtkWidget *widget, gpointer data) {
    GtkBuilder *builder = (GtkBuilder *)data;

    // Pegando a janela principal e a GtkStack
    GtkWidget *main_window = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));
    GtkWidget *main_stack = GTK_WIDGET(gtk_builder_get_object(builder, "main_stack"));

    // Verificando se a GtkStack foi encontrada
    if (GTK_IS_STACK(main_stack)) {
        g_print("Alterando para a página de relatórios...\n");

        // Alternando para a página de relatórios dentro da GtkStack
        gtk_stack_set_visible_child_name(GTK_STACK(main_stack), "view_relatorio");

        // Carregar dados na ListStore
        GtkListStore *liststore = GTK_LIST_STORE(gtk_builder_get_object(builder, "liststore1"));
        if (liststore) {
            g_print("ListStore encontrada. Limpando e carregando os dados...\n");
            gtk_list_store_clear(liststore);  // Limpa a ListStore antes de carregar os novos dados
            g_print("Iniciando o carregamento dos dados...\n");
            carregar_dados(liststore);  // Carrega os dados no liststore
            g_print("Dados carregados com sucesso.\n");
        } else {
            g_print("Erro ao localizar a ListStore.\n");
        }
    } else {
        g_print("Erro: 'main_stack' não é um GtkStack.\n");
    }
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    GtkBuilder *builder = gtk_builder_new_from_file("user_interface.glade");
    GtkWidget *main_window = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));

    // Obtenha o widget da janela view_relatorio

    // Conecta os sinais aos callbacks
    gtk_builder_add_callback_symbols(
        builder,
        "on_confirma_cadastro_usuario_clicked", G_CALLBACK(on_confirma_cadastro_usuario_clicked),
        "on_geral_voltar_clicked", G_CALLBACK(on_geral_voltar_clicked),
        "on_botao_login_clicked", G_CALLBACK(on_botao_login_clicked),
        "on_botao_sair_clicked", G_CALLBACK(on_botao_sair_clicked),
        "on_cadastro_de_usuario_clicked", G_CALLBACK(on_cadastro_de_usuario_clicked),
        "on_cadastro_de_industria_clicked", G_CALLBACK(on_cadastro_de_industria_clicked),
        "on_confirmar_cadastro_industria_clicked", G_CALLBACK(on_confirmar_cadastro_industria_clicked),
        "on_abrir_local_relatorios_clicked", G_CALLBACK(on_abrir_local_relatorios_clicked),
        "on_relatorios_clicked", G_CALLBACK(on_relatorios_clicked),
        NULL
    );

    // Conecta os sinais da interface
    gtk_builder_connect_signals(builder, builder);

    // Conectar o sinal de "show" da janela view_relatorio

    // Exibe a janela principal
    gtk_widget_show_all(main_window);

    gtk_main();

    // Libera o builder
    g_object_unref(builder);
    return 0;
}