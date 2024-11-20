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
#include <stdlib.h>
#include <string.h>
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
    char tipo[20];          // Tipo da entidade (ex: "INDUSTRIA")
    int id;                // ID da indústria
    char nome_empresa[100]; // Nome da empresa
    char razao_social[100]; // Razão social
    char nome_fantasia[100]; // Nome fantasia
    char cnpj[20];         // CNPJ
    char endereco[200];    // Endereço
    char cidade[100];      // Cidade
    char estado[3];        // Estado (sigla)
    char cep[10];          // CEP
    char data_abertura[11]; // Data de abertura (formato DD/MM/AAAA)
    float custos;          // Custos
    float residuos;        // Resíduos
} Industria;

Industria industrias[MAX_INDUSTRIAS];
int total_industrias = 0;

//------------------------------------CRIPTOGRAFIA MAIS AVANÇADA UTILIZADA PELA NASA---------------

void inverter_senha(const char *senha, char *senha_invertida) {
    int len = strlen(senha);
    for (int i = 0; i < len; i++) {
        senha_invertida[i] = senha[len - i - 1];
    }
    senha_invertida[len] = '\0'; // Adiciona o terminador da string
}

//------------------------------------FUNÇÃO PARA LOCALIZAR O DIRETORIO EM QUALQUER SISTEMA--------


void abrir_pasta_relatorios() {
    char path[256];

#ifdef _WIN32
    //--------------------------LOCALIZA O CAMINHO DO EXECUTAVEL NO WINDOWS
    if (GetModuleFileName(NULL, path, sizeof(path))) {
        // Obtém o diretório (remove o nome do executável)
        char *last_slash = strrchr(path, '\\');
        if (last_slash) {
            *last_slash = '\0';
        }
        // Adiciona a pasta "relatorios"
        strcat(path, "\\relatorios");

        // Verifica se a pasta "relatorios" existe
        DWORD ftyp = GetFileAttributes(path);
        if (ftyp == INVALID_FILE_ATTRIBUTES || !(ftyp & FILE_ATTRIBUTE_DIRECTORY)) {
            MessageBox(NULL, "A pasta 'relatorios' não foi encontrada.", "Erro", MB_ICONERROR);
            return;
        }

        // Abre a pasta "relatorios"
        HINSTANCE result = ShellExecute(NULL, "explore", path, NULL, NULL, SW_SHOWNORMAL);
        if ((intptr_t)result <= 32) {
            MessageBox(NULL, "Falha ao abrir a pasta.", "Erro", MB_ICONERROR);
        }
    }

#else
    //--------------------------LOCALIZA O CAMINHO DO EXECUTAVEL NO LINUX
    char *exec_path = realpath(argv[0], path);  // `argv[0]` é o caminho do executável
    if (exec_path) {
        // Remove o nome do executável e obtém o diretório
        char *last_slash = strrchr(path, '/');
        if (last_slash) {
            *last_slash = '\0';
        }

        // Adiciona a pasta "relatorios"
        strcat(path, "/relatorios");

        // Verifica se a pasta "relatorios" existe
        if (access(path, F_OK) == -1) {
            fprintf(stderr, "A pasta 'relatorios' não foi encontrada.\n");
            return;
        }

        // Abre a pasta usando o comando xdg-open
        char command[512];
        snprintf(command, sizeof(command), "xdg-open %s", path);

        // Executa o comando
        int result = system(command);
        if (result != 0) {
            fprintf(stderr, "Falha ao abrir a pasta.\n");
        }
    }
#endif
}


//------------------------------------FUNÇÃO PARA O BOTAO ABRIR O DIRETORIO------------------------

void on_abrir_local_relatorios_clicked(GtkButton *button, gpointer user_data) {
    char caminho_executavel[PATH_MAX];
    abrir_pasta_relatorios(caminho_executavel, sizeof(caminho_executavel));

    char caminho_relatorios[PATH_MAX];
    snprintf(caminho_relatorios, sizeof(caminho_relatorios), "%s/relatorios", caminho_executavel);

#if defined(_WIN32)
    char comando[PATH_MAX];
    snprintf(comando, sizeof(comando), "explorer \"%s\"", caminho_relatorios);
    system(comando);
#elif defined(__APPLE__)
    char comando[PATH_MAX];
    snprintf(comando, sizeof(comando), "open \"%s\"", caminho_relatorios);
    system(comando);
#else
    char comando[PATH_MAX];
    snprintf(comando, sizeof(comando), "xdg-open \"%s\"", caminho_relatorios);
    g_spawn_command_line_async(comando, NULL);
#endif
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

    // Verifica campos obrigatórios
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

    // Dados válidos, salvar no CSV
    Industria nova_industria;
    strcpy(nova_industria.tipo, "INDUSTRIA");
    nova_industria.id = total_industrias + 1;
    strcpy(nova_industria.nome_empresa, nome);
    strcpy(nova_industria.razao_social, razao_social);
    strcpy(nova_industria.nome_fantasia, nome_fantasia);
    strcpy(nova_industria.cnpj, cnpj);
    strcpy(nova_industria.endereco, endereco);
    strcpy(nova_industria.cidade, cidade);
    strcpy(nova_industria.estado, estado);
    strcpy(nova_industria.cep, cep);
    strcpy(nova_industria.data_abertura, data_abertura);
    nova_industria.custos = 0; // Custos iniciais
    nova_industria.residuos = 0; // Resíduos iniciais

    FILE *file = fopen("industrias.csv", "a");
    if (file == NULL) {
        gtk_label_set_text(label_status, "Erro ao salvar os dados.");
        return;
    }

    fprintf(file, "INDUSTRIA,%d,%s,%s,%s,%s,%s,%s,%s,%s,%s,%.2f,%.2f\n",
            nova_industria.id, nova_industria.nome_empresa, nova_industria.razao_social, nova_industria.nome_fantasia,
            nova_industria.cnpj, nova_industria.endereco, nova_industria.cidade, nova_industria.estado,
            nova_industria.cep, nova_industria.data_abertura, nova_industria.custos, nova_industria.residuos);
    fclose(file);

    gtk_label_set_text(label_status, "Indústria cadastrada com sucesso!");
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


//------------------------------------FUNÇÃO PARA GERAR RELATORIOS---------------


void on_relatorios_clicked(GtkButton *button, gpointer user_data) {
    GtkBuilder *builder = GTK_BUILDER(user_data);
    GtkStack *main_stack = GTK_STACK(gtk_builder_get_object(builder, "main_stack"));

    // Alterna para a view_relatorio
    gtk_stack_set_visible_child_name(main_stack, "view_relatorio");
}

//------------------------------------FUNÇÃO PRINCIPAL----------------------
int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    // Carrega a interface a partir do arquivo Glade
    GtkBuilder *builder = gtk_builder_new_from_file("user_interface.glade");
    GtkWidget *main_window = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));

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

    gtk_builder_connect_signals(builder, builder);

    // Exibe a janela principal
    gtk_widget_show_all(main_window);
    gtk_main();

    // Libera o builder
    g_object_unref(builder);
    return 0;
}
