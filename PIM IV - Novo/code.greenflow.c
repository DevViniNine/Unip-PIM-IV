#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include <glib/gstdio.h>
#include <direct.h>
#include <windows.h>
#include <shellapi.h>
#include <locale.h>
#define PATH_SEP '\\'
#include <gtk/gtksearchentry.h>
#include <gtk/gtksearchentry.h>
/*
#include <ctype.h>
#include <limits.h>
*/

#define MAX_INDUSTRIAS 100
#define MAX_USUARIOS 100
#define DADOS_CSV "DADOS.csv"
#define INDUSTRIAS_CSV "industrias.csv"

GtkBuilder *builder;  // Variável global para o GtkBuilder

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
    char nome_responsavel[100];
    char cpf_responsavel[12];
    char razao_social[100];
    char nome_fantasia[100];
    char cnpj[15];
    char data_abertura[11];
    char telefone[15];
    char email[100];
    char endereco[100];
    char cidade[50];
    char estado[50];
    char cep[9];
    double residuo;
    double custo;
} Industria;


void criar_diretorio_relatorios() {
    if (!g_file_test("Relatorios Greenflow", G_FILE_TEST_IS_DIR)) {
        g_print("Diretório 'Relatorios Greenflow' não existe. Criando...\n");
        if (g_mkdir("Relatorios Greenflow", 0755) != 0) {
            g_print("Falha ao criar diretório 'Relatorios Greenflow'.\n");
        }
    }
}
//------------------------------------CRIPTOGRAFIA MAIS AVANÇADA UTILIZADA PELA NASA---------------


void inverter_senha(const char *senha, char *senha_invertida) {
    int len = strlen(senha);
    for (int i = 0; i < len; i++) {
        senha_invertida[i] = senha[len - i - 1];
    }
    senha_invertida[len] = '\0'; // Adiciona o terminador da string
}


//------------------------------------CARREGA DADOS PARA TELA DE RELATORIOS-------------------------

void carregar_dados(GtkListStore *liststore, GtkEntry *entry_total_custo, GtkEntry *entry_total_residuo) {
    FILE *file = fopen("industrias.csv", "r");
    if (!file) {
        g_print("Erro ao abrir o arquivo industrias.csv.\n");
        return;
    }

    char linha[1024];
    double total_custos = 0.0;
    double total_residuos = 0.0;

    // Ignorar a primeira linha (cabeçalho)
    fgets(linha, sizeof(linha), file);

    GtkTreeIter iter;
    while (fgets(linha, sizeof(linha), file)) {
        char razao_social[100], cnpj[20];
        double custos, residuos;

        // Lê os campos necessários do CSV
        if (sscanf(linha, "%*[^,],%*[^,],%*[^,],%99[^,],%*[^,],%19[^,],%*[^,],%*[^,],%*[^,],%*[^,],%*[^,],%*[^,],%*[^,],%lf,%lf",
                   razao_social, cnpj, &custos, &residuos) == 4) {
            // Adiciona os dados na ListStore
            gtk_list_store_append(liststore, &iter);
            gtk_list_store_set(liststore, &iter,
                               0, razao_social,
                               1, cnpj,
                               2, custos,
                               3, residuos,
                               -1);

            // Acumula os totais
            total_custos += custos;
            total_residuos += residuos;
        }
    }

    fclose(file);

// Exibe os totais nos campos correspondentes
    char buffer[50];
    snprintf(buffer, sizeof(buffer), "%.2f", total_custos);
    gtk_entry_set_text(entry_total_custo, buffer);

    snprintf(buffer, sizeof(buffer), "%.2f", total_residuos);
    gtk_entry_set_text(entry_total_residuo, buffer);
}

void on_botao_relatorios_clicked(GtkWidget *widget, gpointer data) {
    GtkBuilder *builder = (GtkBuilder *)data;

    GtkStack *main_stack = GTK_STACK(gtk_builder_get_object(builder, "main_stack"));
    gtk_stack_set_visible_child_name(main_stack, "view_relatorio");

    GtkListStore *liststore = GTK_LIST_STORE(gtk_builder_get_object(builder, "liststore1"));
    GtkEntry *entry_total_custo = GTK_ENTRY(gtk_builder_get_object(builder, "total_custo"));
    GtkEntry *entry_total_residuo = GTK_ENTRY(gtk_builder_get_object(builder, "total_residuo"));

    carregar_dados(liststore, entry_total_custo, entry_total_residuo);
}

void on_menu_atualizar_dados_clicked(GtkButton *button, gpointer data) {
    GtkBuilder *builder = (GtkBuilder *)data;
    GtkStack *main_stack = GTK_STACK(gtk_builder_get_object(builder, "main_stack"));
    GtkListStore *liststore = GTK_LIST_STORE(gtk_builder_get_object(builder, "liststore2"));

    // Limpa o liststore
    gtk_list_store_clear(liststore);

    // Recarrega os dados do arquivo CSV
    FILE *file = fopen(INDUSTRIAS_CSV, "r");
    if (!file) {
        g_printerr("Erro ao abrir o arquivo CSV para recarregar a lista.\n");
        gtk_stack_set_visible_child_name(main_stack, "atualizacao_de_dados");
        return;
    }

    char line[1024];
    while (fgets(line, sizeof(line), file)) {
        char *tokens[15];
        int i = 0;

        tokens[i++] = strtok(line, ",");
        while (tokens[i - 1] != NULL && i < 15) {
            tokens[i++] = strtok(NULL, ",");
        }

        // Adiciona os dados ao liststore
        GtkTreeIter iter;
        gtk_list_store_append(liststore, &iter);
        gtk_list_store_set(liststore, &iter,
            0, tokens[0],  // Nome da indústria
            1, tokens[1],  // Nome do responsável
            2, tokens[2],  // CPF do responsável
            3, tokens[3],  // Razão social
            4, tokens[4],  // Nome fantasia
            5, tokens[5],  // CNPJ
            6, tokens[6],  // Data de abertura
            7, tokens[7],  // Telefone
            8, tokens[8],  // Email
            9, tokens[9],  // Endereço
            10, tokens[10], // Cidade
            11, tokens[11], // Estado
            12, tokens[12], // CEP
            13, tokens[13], // Resíduo
            14, tokens[14], // Custo
            -1);
    }

    fclose(file);

    // Exibe a visualização de atualização
    gtk_stack_set_visible_child_name(main_stack, "atualizacao_de_dados");
}










//------------------------------------ATUALIZAÇÃO DE DADOS ABAIXO-------------------------------



void atualizar_treeview(GtkListStore *liststore) {
    FILE *file = fopen(INDUSTRIAS_CSV, "r");
    if (!file) {
        printf("Erro ao abrir o arquivo %s.\n", INDUSTRIAS_CSV);
        return;
    }

    char line[1024];
    gboolean first_line = TRUE;

    while (fgets(line, sizeof(line), file)) {
        if (first_line) {
            first_line = FALSE;
            continue; // Ignora a primeira linha (cabeçalho)
        }

        // Tokeniza a linha do arquivo
        char *tokens[15];
        int i = 0;
        tokens[i++] = strtok(line, ",");
        while (tokens[i - 1] != NULL && i < 15) {
            tokens[i++] = strtok(NULL, ",");
        }

        if (i < 15) {
            continue; // Erro ao processar a linha
        }

        Industria ind;
        strcpy(ind.nome_empresa, tokens[0]);
        strcpy(ind.nome_responsavel, tokens[1]);
        strcpy(ind.cpf_responsavel, tokens[2]);
        strcpy(ind.razao_social, tokens[3]);
        strcpy(ind.nome_fantasia, tokens[4]);
        strcpy(ind.cnpj, tokens[5]);
        strcpy(ind.data_abertura, tokens[6]);
        strcpy(ind.telefone, tokens[7]);
        strcpy(ind.email, tokens[8]);
        strcpy(ind.endereco, tokens[9]);
        strcpy(ind.cidade, tokens[10]);
        strcpy(ind.estado, tokens[11]);
        strcpy(ind.cep, tokens[12]);
        ind.residuo = atof(tokens[13]);
        ind.custo = atof(tokens[14]);

        GtkTreeIter iter;
        gtk_list_store_append(liststore, &iter);
        gtk_list_store_set(liststore, &iter,
                           0, ind.nome_empresa,
                           1, ind.nome_responsavel,
                           2, ind.cpf_responsavel,
                           3, ind.razao_social,
                           4, ind.nome_fantasia,
                           5, ind.cnpj,
                           6, ind.data_abertura,
                           7, ind.telefone,
                           8, ind.email,
                           9, ind.endereco,
                           10, ind.cidade,
                           11, ind.estado,
                           12, ind.cep,
                           13, ind.residuo,
                           14, ind.custo,
                           -1);
    }

    fclose(file);
}





void on_treeview_selection_changed(GtkTreeSelection *selection, gpointer builder) {
    GtkTreeModel *model;
    GtkTreeIter iter;

    if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
        gchar *nome_empresa, *cnpj;
        gdouble residuo, custo;

        gtk_tree_model_get(model, &iter,
                           0, &nome_empresa,
                           5, &cnpj,
                           13, &residuo,
                           14, &custo,
                           -1);

        GtkEntry *entry_nome = GTK_ENTRY(gtk_builder_get_object(builder, "nome_da_industria_selecionada"));
        GtkEntry *entry_cnpj = GTK_ENTRY(gtk_builder_get_object(builder, "cnpj_da_industria_selecionada"));
        GtkLabel *label_residuo = GTK_LABEL(gtk_builder_get_object(builder, "atualiza_residuo_atual"));
        GtkLabel *label_custo = GTK_LABEL(gtk_builder_get_object(builder, "atualiza_custo_atual"));

        gtk_entry_set_text(entry_nome, nome_empresa);
        gtk_entry_set_text(entry_cnpj, cnpj);

        char residuo_str[50], custo_str[50];
        snprintf(residuo_str, sizeof(residuo_str), "%.2f", residuo);
        snprintf(custo_str, sizeof(custo_str), "%.2f", custo);

        gtk_label_set_text(label_residuo, residuo_str);
        gtk_label_set_text(label_custo, custo_str);

        g_free(nome_empresa);
        g_free(cnpj);
    }
}

// Função de callback para o botão de atualizar dados
void on_confirma_atualizar_dados_clicked(GtkButton *button, gpointer data) {
    GtkBuilder *builder = (GtkBuilder *)data;
    GtkTreeView *treeview = GTK_TREE_VIEW(gtk_builder_get_object(builder, "atualizacao_treeview"));
    GtkListStore *liststore = GTK_LIST_STORE(gtk_builder_get_object(builder, "liststore2"));
    GtkLabel *status_label = GTK_LABEL(gtk_builder_get_object(builder, "status_atualiza_dados"));
    GtkEntry *novo_residuo_entry = GTK_ENTRY(gtk_builder_get_object(builder, "novo_residuo"));
    GtkEntry *novo_custo_entry = GTK_ENTRY(gtk_builder_get_object(builder, "novo_custo"));

    const gchar *novo_residuo = gtk_entry_get_text(novo_residuo_entry);
    const gchar *novo_custo = gtk_entry_get_text(novo_custo_entry);

    // Verifica se os campos estão vazios
    if (strlen(novo_residuo) == 0 || strlen(novo_custo) == 0) {
        gtk_label_set_text(status_label, "Os campos de resíduos e custos não podem ser vazios.");
        return;
    }

    // Valida se os valores inseridos são numéricos
    for (int i = 0; novo_residuo[i] != '\0'; i++) {
        if (!isdigit(novo_residuo[i]) && novo_residuo[i] != '.') {
            gtk_label_set_text(status_label, "Resíduo deve conter apenas números ou ponto decimal.");
            return;
        }
    }

    for (int i = 0; novo_custo[i] != '\0'; i++) {
        if (!isdigit(novo_custo[i]) && novo_custo[i] != '.') {
            gtk_label_set_text(status_label, "Custo deve conter apenas números ou ponto decimal.");
            return;
        }
    }

    gdouble novo_residuo_val = atof(novo_residuo);
    gdouble novo_custo_val = atof(novo_custo);

    // Verifica se os valores são positivos
    if (novo_residuo_val < 0 || novo_custo_val < 0) {
        gtk_label_set_text(status_label, "Os valores de resíduos e custos devem ser positivos.");
        return;
    }

    GtkTreeSelection *selection = gtk_tree_view_get_selection(treeview);
    GtkTreeModel *model;
    GtkTreeIter iter;

    if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
        gchar *nome_empresa;
        gchar *cnpj;
        gtk_tree_model_get(model, &iter,
                           0, &nome_empresa,
                           5, &cnpj,
                           -1);

        // Atualiza os valores de resíduos e custos no arquivo CSV
        FILE *file = fopen(INDUSTRIAS_CSV, "r");
        if (!file) {
            gtk_label_set_text(status_label, "Erro ao abrir o arquivo para leitura.");
            return;
        }

        FILE *temp_file = fopen("temp.csv", "w");
        if (!temp_file) {
            fclose(file);
            gtk_label_set_text(status_label, "Erro ao criar arquivo temporário.");
            return;
        }

        char line[1024];
        gboolean updated = FALSE;

        // Ignora a primeira linha (cabeçalho) e escreve diretamente no arquivo temporário
        if (fgets(line, sizeof(line), file)) {
            fputs(line, temp_file); // Copia o cabeçalho para o arquivo temporário
        }

        while (fgets(line, sizeof(line), file)) {
            char *tokens[15];
            int i = 0;

            tokens[i++] = strtok(line, ",");
            while (tokens[i - 1] != NULL && i < 15) {
                tokens[i++] = strtok(NULL, ",");
            }

            if (i < 15 || tokens[0] == NULL || tokens[5] == NULL) {
                continue;
            }

            for (int j = 0; j < i; j++) {
                if (tokens[j]) {
                    char *newline_pos = strchr(tokens[j], '\n');
                    if (newline_pos) *newline_pos = '\0';
                }
            }

            if (strcmp(tokens[0], nome_empresa) == 0 && strcmp(tokens[5], cnpj) == 0) {
                snprintf(tokens[13], 50, "%.2f", novo_residuo_val);
                snprintf(tokens[14], 50, "%.2f", novo_custo_val);
                updated = TRUE;
            }

            fprintf(temp_file, "%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s\n",
                    tokens[0], tokens[1], tokens[2], tokens[3], tokens[4],
                    tokens[5], tokens[6], tokens[7], tokens[8], tokens[9],
                    tokens[10], tokens[11], tokens[12], tokens[13], tokens[14]);
        }

        fclose(file);
        fclose(temp_file);

        if (updated) {
            if (remove(INDUSTRIAS_CSV) == 0 && rename("temp.csv", INDUSTRIAS_CSV) == 0) {
                gtk_label_set_text(status_label, "Dados atualizados com sucesso.");
                gtk_list_store_clear(liststore);
                atualizar_treeview(liststore);

                gtk_entry_set_text(novo_residuo_entry, "");
                gtk_entry_set_text(novo_custo_entry, "");
            } else {
                gtk_label_set_text(status_label, "Erro ao substituir o arquivo.");
            }
        } else {
            gtk_label_set_text(status_label, "Indústria não encontrada para atualização.");
            remove("temp.csv");
        }

        g_free(nome_empresa);
        g_free(cnpj);
    } else {
        gtk_label_set_text(status_label, "Nenhuma indústria selecionada.");
    }
}




void on_industria_selecionada(GtkTreeSelection *selection, gpointer data) {
    GtkBuilder *builder = (GtkBuilder *)data;
    GtkTreeModel *model;
    GtkTreeIter iter;
    GtkLabel *residuo_atual_label = GTK_LABEL(gtk_builder_get_object(builder, "residuo_atual"));
    GtkLabel *custo_atual_label = GTK_LABEL(gtk_builder_get_object(builder, "custo_atual"));
    GtkEntry *novo_residuo_entry = GTK_ENTRY(gtk_builder_get_object(builder, "novo_residuo"));
    GtkEntry *novo_custo_entry = GTK_ENTRY(gtk_builder_get_object(builder, "novo_custo"));

    if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
        gchar *nome_empresa, *nome_responsavel, *cpf_responsavel, *razao_social, *nome_fantasia, *cnpj, *data_abertura;
        gchar *telefone, *email, *endereco, *cidade, *estado, *cep;
        gdouble residuo, custo;

        gtk_tree_model_get(model, &iter, 0, &nome_empresa, 1, &nome_responsavel, 2, &cpf_responsavel,
                           3, &razao_social, 4, &nome_fantasia, 5, &cnpj, 6, &data_abertura, 7, &telefone,
                           8, &email, 9, &endereco, 10, &cidade, 11, &estado, 12, &cep, 13, &residuo, 14, &custo, -1);

        // Exibe resíduos e custos na interface
        gchar residuo_str[50], custo_str[50];
        snprintf(residuo_str, 50, "%.2f", residuo);
        snprintf(custo_str, 50, "%.2f", custo);
        gtk_label_set_text(residuo_atual_label, residuo_str);
        gtk_label_set_text(custo_atual_label, custo_str);

        // Limpa os campos de entrada
        gtk_entry_set_text(novo_residuo_entry, "");
        gtk_entry_set_text(novo_custo_entry, "");
    }
}



//------------------------------------ATUALIZAÇÃO DE DADOS ACIMA--------------------------------     */












//------------------------------------FUNÇÃO PARA GERAR RELATORIOS---------------

void salvar_relatorio(GtkTreeView *treeview, GtkLabel *status_label) {
    GtkTreeModel *model = gtk_tree_view_get_model(treeview);
    GtkTreeIter iter;
    gboolean valid;
    gchar *razao_social, *cnpj;
    gdouble residuos, custos;
    int arquivo_contador = 1;
    char caminho_base[256];
    char caminho_final[256];
    double total_residuos = 0.0, total_custos = 0.0;

    // Diretório base
    snprintf(caminho_base, sizeof(caminho_base), "Relatorios Greenflow");

    // Criar diretório se não existir
    if (!g_file_test(caminho_base, G_FILE_TEST_IS_DIR)) {
        if (g_mkdir(caminho_base, 0755) != 0) {
            gtk_label_set_text(status_label, "Erro ao criar o diretório dos relatórios.");
            return;
        }
    }

    // Procurar nome de arquivo incremental
    do {
        snprintf(caminho_final, sizeof(caminho_final), "%s/relatorio%d.csv", caminho_base, arquivo_contador);
        arquivo_contador++;
    } while (g_file_test(caminho_final, G_FILE_TEST_EXISTS));

    // Abrir arquivo para escrita
    FILE *arquivo = fopen(caminho_final, "w");
    if (!arquivo) {
        gtk_label_set_text(status_label, "Erro ao salvar o relatório!");
        return;
    }

    // Escrever cabeçalho
    fprintf(arquivo, "Razao Social,CNPJ,Residuos,Custos\n");

    // Iterar pelos dados da TreeView
    valid = gtk_tree_model_get_iter_first(model, &iter);
    while (valid) {
        gtk_tree_model_get(model, &iter,
                           0, &razao_social,
                           1, &cnpj,
                           2, &residuos,
                           3, &custos,
                           -1);

        // Escrever linha com identificadores
        fprintf(arquivo, "%s,CNPJ: %s,Residuos KG: %.2f,Custos R$: %.2f\n",
                razao_social, cnpj, residuos, custos);

        // Acumular totais
        total_residuos += residuos;
        total_custos += custos;

        // Liberar memória alocada
        g_free(razao_social);
        g_free(cnpj);

        valid = gtk_tree_model_iter_next(model, &iter);
    }

    // Escrever totais com separação
    fprintf(arquivo, "\n");
    fprintf(arquivo, "TOTAL DE RESIDUOS TRATADOS EM KG: %.2f\n", total_residuos);
    fprintf(arquivo, "TOTAL DE CUSTOS EM R$: %.2f\n", total_custos);
    fprintf(arquivo, "\nGreenFlow-2024\n");

    fclose(arquivo);

    // Atualizar status
    char mensagem_status[256];
    snprintf(mensagem_status, sizeof(mensagem_status), "Relatório salvo em:  %s", caminho_final);
    gtk_label_set_text(status_label, mensagem_status);
}

void on_salvar_relatorio_clicked(GtkWidget *widget, gpointer data) {




    GtkBuilder *builder = (GtkBuilder *)data;
    GtkTreeView *treeview = GTK_TREE_VIEW(gtk_builder_get_object(builder, "treeview_id"));
    GtkLabel *status_label = GTK_LABEL(gtk_builder_get_object(builder, "status_relatorio"));

    salvar_relatorio(treeview, status_label);
}
//------------------------------------FUNÇÃO PARA O BOTAO ABRIR O DIRETORIO------------------------


void abrir_pasta_relatorios(GtkLabel *status_label) {
    char caminho_diretorio[MAX_PATH];

    // Define o caminho para a pasta "relatorios greenflow"
    snprintf(caminho_diretorio, sizeof(caminho_diretorio), "relatorios greenflow");

    // Verifica se o diretório existe
    DWORD atributos = GetFileAttributes(caminho_diretorio);
    if (atributos == INVALID_FILE_ATTRIBUTES || !(atributos & FILE_ATTRIBUTE_DIRECTORY)) {
        // Diretório não encontrado, atualiza a label
        gtk_label_set_text(status_label, "Diretório 'relatorios greenflow' não encontrado.");
        return;
    }

    // Tenta abrir o diretório usando ShellExecute
    HINSTANCE resultado = ShellExecute(NULL, "open", caminho_diretorio, NULL, NULL, SW_SHOWNORMAL);
    if ((intptr_t)resultado <= 32) {
        // Falha ao abrir o diretório
        gtk_label_set_text(status_label, "Falha ao abrir o diretório.");
    }
}

void on_abrir_local_relatorios_clicked(GtkButton *button, gpointer user_data) {
    GtkBuilder *builder = GTK_BUILDER(user_data);

    // Obtém o label de status pela ID "status_relatorio"
    GtkLabel *status_label = GTK_LABEL(gtk_builder_get_object(builder, "status_relatorio"));
    if (status_label) {
        abrir_pasta_relatorios(status_label);
    }
}

//------------------------------------VERIFICA USUARIO NA TELA DE LOGIN----------------------------
int verificar_usuario(const char *login, const char *senha) {




    FILE *file = fopen(DADOS_CSV, "r");
    if (file == NULL) {
        return 0; // Erro ao abrir o arquivo
    }

    char linha[256];
    while (fgets(linha, sizeof(linha), file)) {
        char tipo[10], login_arquivo[50], senha_arquivo[50];
        int id;

        // Lê os dados do arquivo
        sscanf(linha, "%[^,],%d,%[^,],%s", tipo, &id, login_arquivo, senha_arquivo);

        // Verifica se o login corresponde
        if (strcmp(login, login_arquivo) == 0) {
            char senha_descriptografada[50];
            inverter_senha(senha_arquivo, senha_descriptografada); // Descriptografa a senha

            // Compara a senha
            if (strcmp(senha, senha_descriptografada) == 0) {
                fclose(file);
                return 1; // Login bem-sucedido
            } else {
                fclose(file);
                return -1; // Senha incorreta
            }
        }
    }

    fclose(file);
    return 0; // Usuário não encontrado
}


//------------------------------------FUNCAO PARA FAZER LOGIN NO SISTEMA---------------------------
void on_botao_login_clicked(GtkWidget *widget, gpointer data) {
    GtkBuilder *builder = (GtkBuilder *)data; // Recebe o builder
    GtkEntry *entry_usuario = GTK_ENTRY(gtk_builder_get_object(builder, "view_login_usuario"));
    GtkEntry *entry_senha = GTK_ENTRY(gtk_builder_get_object(builder, "view_login_senha"));
    GtkLabel *label_status = GTK_LABEL(gtk_builder_get_object(builder, "status_login"));

    const char *login = gtk_entry_get_text(entry_usuario);
    const char *senha = gtk_entry_get_text(entry_senha);

    // Verificar se os campos estão vazios
    if (strlen(login) == 0 || strlen(senha) == 0) {
        gtk_label_set_text(label_status, "Por favor, insira todos os dados.");
        return;
    }

    // Verificar o usuário
    int resultado = verificar_usuario(login, senha);
    if (resultado == 1) {
        // Login bem-sucedido, mudar para a view do menu principal
        GtkStack *main_stack = GTK_STACK(gtk_builder_get_object(builder, "main_stack"));
        gtk_stack_set_visible_child_name(main_stack, "view_menu_principal");
    } else if (resultado == -1) {
        gtk_label_set_text(label_status, "Senha incorreta.");
    } else {
        gtk_label_set_text(label_status, "Usuário não encontrado.");
    }
    gtk_entry_set_text(entry_usuario, "");
    gtk_entry_set_text(entry_senha, "");

}


//------------------------------------BOTAO PARA ABIR A TELA DE CADASTRO DE USUARIO------------------
void on_cadastro_de_usuario_clicked(GtkButton *button, gpointer user_data) {
    GtkBuilder *builder = GTK_BUILDER(user_data);
    GtkStack *main_stack = GTK_STACK(gtk_builder_get_object(builder, "main_stack"));
    gtk_stack_set_visible_child_name(main_stack, "view_cadastro_usuario");
}
//------------------------------------FUNCAO PARA VERIFICA SE USUARIO CADASTRADO JA EXISTE-----------
int usuario_existe(const char *login) {
    for (int i = 0; i < total_usuarios; i++) {
        if (strcmp(usuarios[i].login, login) == 0) {
            return 1; // Usuário já existe
        }
    }
    return 0; // Usuário não existe
}


//------------------------------------FUNCAO PARA CONFIRMAR CADASTRO DO USUARIO----------------------
void on_confirma_cadastro_usuario_clicked(GtkWidget *widget, gpointer data) {
    GtkBuilder *builder = (GtkBuilder *)data;
    GtkEntry *entry_login = GTK_ENTRY(gtk_builder_get_object(builder, "cadastro_login"));
    GtkEntry *entry_senha = GTK_ENTRY(gtk_builder_get_object(builder, "cadastro_senha"));
    GtkEntry *entry_conferir_senha = GTK_ENTRY(gtk_builder_get_object(builder, "cadastro_conferir_senha"));
    GtkLabel *label_status = GTK_LABEL(gtk_builder_get_object(builder, "status_cadastro_usuario"));

    const char *login = gtk_entry_get_text(entry_login);
    const char *senha = gtk_entry_get_text(entry_senha);
    const char *conferir_senha = gtk_entry_get_text(entry_conferir_senha);


    if (strlen(login) == 0 || strlen(senha) == 0 || strlen(conferir_senha) == 0) {
        gtk_label_set_text(label_status, "Por favor, insira todos os dados.");
        return;
    }


    if (strcmp(senha, conferir_senha) != 0) {
        gtk_label_set_text(label_status, "As senhas não são iguais.");
        return;
    }


    if (usuario_existe(login)) {
        gtk_label_set_text(label_status, "Usuário já cadastrado.");
        return;
    }


    Usuario novo_usuario;
    strcpy(novo_usuario.tipo, "USUARIO");
    novo_usuario.id = total_usuarios + 1;
    strcpy(novo_usuario.login, login);


    inverter_senha(senha, novo_usuario.senha);


    usuarios[total_usuarios] = novo_usuario;
    total_usuarios++;

    //-----SALVAR DADOS NO ARQUIVO
    FILE *file = fopen(DADOS_CSV, "a");
    if (file != NULL) {
        fprintf(file, "USUARIO,%d,%s,%s\n", novo_usuario.id, novo_usuario.login, novo_usuario.senha);
        fclose(file);
    }

    gtk_label_set_text(label_status, "Usuário cadastrado com sucesso!");
    gtk_entry_set_text(entry_login, "");
    gtk_entry_set_text(entry_senha, "");
    gtk_entry_set_text(entry_conferir_senha, "");


}


//------------------------------------BOTAO VOLTAR DA TELA RELATORIOS PARA LIMPAR OS DADOS E LABEL-----------------
void on_relatorio_voltar_clicked(GtkButton *button, gpointer user_data) {
    GtkBuilder *builder = GTK_BUILDER(user_data);

    // Obtém os objetos necessários
    GtkListStore *liststore = GTK_LIST_STORE(gtk_builder_get_object(builder, "liststore1"));
    GtkLabel *status_label = GTK_LABEL(gtk_builder_get_object(builder, "status_relatorio"));
    GtkStack *main_stack = GTK_STACK(gtk_builder_get_object(builder, "main_stack"));

    // Limpa os dados da ListStore
    if (liststore) {
        gtk_list_store_clear(liststore);
    }

    // Limpa o texto da label de status
    if (status_label) {
        gtk_label_set_text(status_label, "");
    }

    // Retorna para o menu principal
    if (main_stack) {
        gtk_stack_set_visible_child_name(main_stack, "view_menu_principal");
    }
}


//------------------------------------BOTAO VOLTAR DA TELA CADASTRO DE USUARIO PARA LIMPAR OS DADOS E LABEL--------
void on_cadastro_usuario_voltar_clicked(GtkButton *button, gpointer user_data) {
    GtkBuilder *builder = GTK_BUILDER(user_data);

    // Obtém o objeto da label de status
    GtkLabel *status_label = GTK_LABEL(gtk_builder_get_object(builder, "status_cadastro_usuario"));

    // Limpa o texto da label de status
    if (status_label) {
        gtk_label_set_text(status_label, "");
    }

    // Obtém os objetos das entradas (Entry)
    GtkEntry *entry_login = GTK_ENTRY(gtk_builder_get_object(builder, "cadastro_login"));
    GtkEntry *entry_senha = GTK_ENTRY(gtk_builder_get_object(builder, "cadastro_senha"));
    GtkEntry *entry_conferir_senha = GTK_ENTRY(gtk_builder_get_object(builder, "cadastro_conferir_senha"));

    // Limpa os campos de entrada
    if (entry_login) {
        gtk_entry_set_text(entry_login, "");
    }
    if (entry_senha) {
        gtk_entry_set_text(entry_senha, "");
    }
    if (entry_conferir_senha) {
        gtk_entry_set_text(entry_conferir_senha, "");
    }

    // Volta para o menu principal
    GtkStack *main_stack = GTK_STACK(gtk_builder_get_object(builder, "main_stack"));
    if (main_stack) {
        gtk_stack_set_visible_child_name(main_stack, "view_menu_principal");
    }
}


//------------------------------------BOTAO VOLTAR GERAL----------------------------------------------
void on_geral_voltar_clicked(GtkButton *button, gpointer user_data) {
    GtkBuilder *builder = GTK_BUILDER(user_data);
    GtkStack *main_stack = GTK_STACK(gtk_builder_get_object(builder, "main_stack"));
    gtk_stack_set_visible_child_name(main_stack, "view_menu_principal");

}


//------------------------------------BOTAO FECHAR O PROGRAMA-----------------
void on_botao_sair_clicked(GtkWidget *widget, gpointer data) {
    gtk_main_quit();

}
void on_menu_sair_clicked(GtkButton *button, gpointer user_data) {
    GtkBuilder *builder = GTK_BUILDER(user_data);
    GtkStack *main_stack = GTK_STACK(gtk_builder_get_object(builder, "main_stack"));
    gtk_stack_set_visible_child_name(main_stack, "view_login");

}
//------------------------------------BOTAO VOLTAR DA TELA DE CADASTRO---------------------
void on_cadastro_industria_voltar_clicked(GtkButton *button, gpointer user_data) {
    GtkBuilder *builder = GTK_BUILDER(user_data);

    // Limpa a label de status
    GtkLabel *status_label = GTK_LABEL(gtk_builder_get_object(builder, "status_cadastro_industria"));
    if (status_label) {
        gtk_label_set_text(status_label, "");
    }

    GtkEntry *entry_nome = GTK_ENTRY(gtk_builder_get_object(builder, "cadastro_nome_industria"));
    GtkEntry *entry_responsavel = GTK_ENTRY(gtk_builder_get_object(builder, "nome_responsavel"));
    GtkEntry *entry_cpf = GTK_ENTRY(gtk_builder_get_object(builder, "cpf_responsavel"));
    GtkEntry *entry_razao_social = GTK_ENTRY(gtk_builder_get_object(builder, "cadastro_razao_social"));
    GtkEntry *entry_nome_fantasia = GTK_ENTRY(gtk_builder_get_object(builder, "cadastro_nome_fantasia"));
    GtkEntry *entry_cnpj = GTK_ENTRY(gtk_builder_get_object(builder, "cadastro_cnpj"));
    GtkEntry *entry_data = GTK_ENTRY(gtk_builder_get_object(builder, "cadastro_data_abertura"));
    GtkEntry *entry_telefone = GTK_ENTRY(gtk_builder_get_object(builder, "cadastro_telefone"));
    GtkEntry *entry_email = GTK_ENTRY(gtk_builder_get_object(builder, "cadastro_email"));
    GtkEntry *entry_endereco = GTK_ENTRY(gtk_builder_get_object(builder, "cadastro_endereco"));
    GtkEntry *entry_cidade = GTK_ENTRY(gtk_builder_get_object(builder, "cadastro_cidade"));
    GtkEntry *entry_estado = GTK_ENTRY(gtk_builder_get_object(builder, "cadastro_estado"));
    GtkEntry *entry_cep = GTK_ENTRY(gtk_builder_get_object(builder, "cadastro_cep"));

    gtk_entry_set_text(entry_nome, "");
    gtk_entry_set_text(entry_responsavel, "");
    gtk_entry_set_text(entry_cpf, "");
    gtk_entry_set_text(entry_razao_social, "");
    gtk_entry_set_text(entry_nome_fantasia, "");
    gtk_entry_set_text(entry_cnpj, "");
    gtk_entry_set_text(entry_data, "");
    gtk_entry_set_text(entry_telefone, "");
    gtk_entry_set_text(entry_email, "");
    gtk_entry_set_text(entry_endereco, "");
    gtk_entry_set_text(entry_cidade, "");
    gtk_entry_set_text(entry_estado, "");
    gtk_entry_set_text(entry_cep, "");

    // Volta para o menu principal
    GtkStack *main_stack = GTK_STACK(gtk_builder_get_object(builder, "main_stack"));
    if (main_stack) {
        gtk_stack_set_visible_child_name(main_stack, "view_menu_principal");
    }
}


//------------------------------------FUNÇÃO PARA CADASTRO DE INDUSTRIAS--------
void on_cadastro_de_industria_clicked(GtkButton *button, gpointer user_data) {
    GtkBuilder *builder = GTK_BUILDER(user_data);
    GtkStack *main_stack = GTK_STACK(gtk_builder_get_object(builder, "main_stack"));
    gtk_stack_set_visible_child_name(main_stack, "view_cadastro_industria");
}

int validar_numeros(const char *str) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (!isdigit(str[i]) && str[i] != '.' && str[i] != '-') {
            return 0; // Contém caracteres inválidos
        }
    }
    return 1; // Válido
}

int validar_data(const char *data) {
    if (strlen(data) != 10 || data[2] != '/' || data[5] != '/') {
        return 0;
    }

    int dia = atoi(&data[0]);
    int mes = atoi(&data[3]);
    int ano = atoi(&data[6]);

    if (dia < 1 || dia > 31 || mes < 1 || mes > 12 || ano < 1900 || ano > 2100) {
        return 0;
    }
    return 1;
}


//------------------------------------FUNÇÃO PARA CADASTRO DE INDUSTRIAS--------
void on_confirmar_cadastro_industria_clicked(GtkWidget *widget, gpointer data) {
    GtkBuilder *builder = (GtkBuilder *)data;
    GtkEntry *entry_nome = GTK_ENTRY(gtk_builder_get_object(builder, "cadastro_nome_industria"));
    GtkEntry *entry_responsavel = GTK_ENTRY(gtk_builder_get_object(builder, "nome_responsavel"));
    GtkEntry *entry_cpf = GTK_ENTRY(gtk_builder_get_object(builder, "cpf_responsavel"));
    GtkEntry *entry_razao_social = GTK_ENTRY(gtk_builder_get_object(builder, "cadastro_razao_social"));
    GtkEntry *entry_nome_fantasia = GTK_ENTRY(gtk_builder_get_object(builder, "cadastro_nome_fantasia"));
    GtkEntry *entry_cnpj = GTK_ENTRY(gtk_builder_get_object(builder, "cadastro_cnpj"));
    GtkEntry *entry_data = GTK_ENTRY(gtk_builder_get_object(builder, "cadastro_data_abertura"));
    GtkEntry *entry_telefone = GTK_ENTRY(gtk_builder_get_object(builder, "cadastro_telefone"));
    GtkEntry *entry_email = GTK_ENTRY(gtk_builder_get_object(builder, "cadastro_email"));
    GtkEntry *entry_endereco = GTK_ENTRY(gtk_builder_get_object(builder, "cadastro_endereco"));
    GtkEntry *entry_cidade = GTK_ENTRY(gtk_builder_get_object(builder, "cadastro_cidade"));
    GtkEntry *entry_estado = GTK_ENTRY(gtk_builder_get_object(builder, "cadastro_estado"));
    GtkEntry *entry_cep = GTK_ENTRY(gtk_builder_get_object(builder, "cadastro_cep"));
    GtkLabel *label_status = GTK_LABEL(gtk_builder_get_object(builder, "status_cadastro_industria"));

    // Obtém os valores dos campos
    const char *nome = gtk_entry_get_text(entry_nome);
    const char *responsavel = gtk_entry_get_text(entry_responsavel);
    const char *cpf = gtk_entry_get_text(entry_cpf);
    const char *razao_social = gtk_entry_get_text(entry_razao_social);
    const char *nome_fantasia = gtk_entry_get_text(entry_nome_fantasia);
    const char *cnpj = gtk_entry_get_text(entry_cnpj);
    const char *data_abertura = gtk_entry_get_text(entry_data);
    const char *telefone = gtk_entry_get_text(entry_telefone);
    const char *email = gtk_entry_get_text(entry_email);
    const char *endereco = gtk_entry_get_text(entry_endereco);
    const char *cidade = gtk_entry_get_text(entry_cidade);
    const char *estado = gtk_entry_get_text(entry_estado);
    const char *cep = gtk_entry_get_text(entry_cep);

    // Validações de campos obrigatórios
    if (strlen(nome) == 0 || strlen(responsavel) == 0 || strlen(cpf) == 0 || strlen(razao_social) == 0 ||
        strlen(nome_fantasia) == 0 || strlen(cnpj) == 0 || strlen(data_abertura) == 0 || strlen(telefone) == 0 ||
        strlen(email) == 0 || strlen(endereco) == 0 || strlen(cidade) == 0 || strlen(estado) == 0 || strlen(cep) == 0) {
        gtk_label_set_text(label_status, "Por favor, preencha todos os campos.");
        return;
    }

    // Validações específicas
    if (!validar_numeros(cpf) || strlen(cpf) != 11) {
        gtk_label_set_text(label_status, "CPF inválido. Deve conter 11 números.");
        return;
    }

    if (!validar_numeros(cnpj) || strlen(cnpj) != 14) {
        gtk_label_set_text(label_status, "CNPJ inválido. Deve conter 14 números.");
        return;
    }

    if (!validar_numeros(telefone) || strlen(telefone) < 10) {
        gtk_label_set_text(label_status, "Telefone inválido. Verifique o número.");
        return;
    }

    if (!validar_numeros(cep) || strlen(cep) != 8) {
        gtk_label_set_text(label_status, "CEP inválido. Deve conter 8 números.");
        return;
    }

    if (!validar_data(data_abertura)) {
        gtk_label_set_text(label_status, "Data inválida. Use o formato DD/MM/AAAA.");
        return;
    }

    // Inicializa custos e resíduos
    double custos = 0.00;
    double residuos = 0.00;

    // Salva os dados no arquivo CSV
    FILE *file = fopen("industrias.csv", "a");
    if (file == NULL) {
        gtk_label_set_text(label_status, "Erro ao salvar os dados.");
        return;
    }

    fprintf(file, "%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%.2f,%.2f\n",
            nome, responsavel, cpf, razao_social, nome_fantasia, cnpj, data_abertura,
            telefone, email, endereco, cidade, estado, cep, custos, residuos);

    fclose(file);

    gtk_label_set_text(label_status, "Indústria cadastrada com sucesso!");

    // Limpa os campos após o cadastro
    gtk_entry_set_text(entry_nome, "");
    gtk_entry_set_text(entry_responsavel, "");
    gtk_entry_set_text(entry_cpf, "");
    gtk_entry_set_text(entry_razao_social, "");
    gtk_entry_set_text(entry_nome_fantasia, "");
    gtk_entry_set_text(entry_cnpj, "");
    gtk_entry_set_text(entry_data, "");
    gtk_entry_set_text(entry_telefone, "");
    gtk_entry_set_text(entry_email, "");
    gtk_entry_set_text(entry_endereco, "");
    gtk_entry_set_text(entry_cidade, "");
    gtk_entry_set_text(entry_estado, "");
    gtk_entry_set_text(entry_cep, "");
}


//------------------------------------FUNÇÃO PRINCIPAL----------------------
int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    criar_diretorio_relatorios();

    GtkBuilder *builder = gtk_builder_new_from_file("user_interface.glade");
    GtkWidget *main_window = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));
    setlocale(LC_NUMERIC, "C");
    //GtkListStore *liststore = GTK_LIST_STORE(gtk_builder_get_object(builder, "liststore2"));
    //atualizar_treeview(liststore, builder);
    GtkListStore *liststore = GTK_LIST_STORE(gtk_builder_get_object(builder, "liststore2"));
    GtkTreeView *treeview = GTK_TREE_VIEW(gtk_builder_get_object(builder, "atualizacao_treeview"));
    atualizar_treeview(liststore);

    //----OS MARAVILHOSOS CALLBACKS
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
        "on_botao_relatorios_clicked", G_CALLBACK(on_botao_relatorios_clicked),
        "on_salvar_relatorio_clicked", G_CALLBACK(on_salvar_relatorio_clicked),
        "on_relatorio_voltar_clicked", G_CALLBACK(on_relatorio_voltar_clicked),
        "on_cadastro_usuario_voltar_clicked", G_CALLBACK(on_cadastro_usuario_voltar_clicked),
        "on_cadastro_industria_voltar_clicked", G_CALLBACK(on_cadastro_industria_voltar_clicked),
        "on_menu_atualizar_dados_clicked", G_CALLBACK(on_menu_atualizar_dados_clicked),
        "on_menu_sair_clicked", G_CALLBACK(on_menu_sair_clicked),
        "on_confirma_atualizar_dados_clicked", G_CALLBACK(on_confirma_atualizar_dados_clicked),
        NULL
    );


    GtkTreeSelection *selection = gtk_tree_view_get_selection(treeview);
    g_signal_connect(selection, "changed", G_CALLBACK(on_treeview_selection_changed), builder);


    gtk_builder_connect_signals(builder, builder);


    gtk_widget_show_all(main_window);
    gtk_main();
    g_object_unref(builder);
    return 0;
}
