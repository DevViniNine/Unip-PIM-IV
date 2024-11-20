#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


// Função para criptografar uma senha simples
void criptografar_senha(const char *senha, char *senha_criptografada) {
    for (int i = 0; senha[i] != '\0'; i++) {
        senha_criptografada[i] = senha[i] + 3; // Exemplo: desloca os caracteres em 3
    }
    senha_criptografada[strlen(senha)] = '\0';
}

// Função para descriptografar uma senha
void descriptografar_senha(const char *senha_criptografada, char *senha) {
    for (int i = 0; senha_criptografada[i] != '\0'; i++) {
        senha[i] = senha_criptografada[i] - 3;
    }
    senha[strlen(senha_criptografada)] = '\0';
}
//-----------------CADASTRO DE INDUSTRIA----------------------------------------------------------------------------------------------------
int validar_cnpj(const char *cnpj) {
    int i, len = strlen(cnpj);
    if (len != 14) return 0; // CNPJ deve ter 14 caracteres
    for (i = 0; i < len; i++) {
        if (cnpj[i] < '0' || cnpj[i] > '9') return 0; // Verifica se são apenas números
    }
    return 1; // CNPJ válido
}

int validar_telefone(const char *telefone) {
    int i, len = strlen(telefone);
    if (len != 10 && len != 11) return 0; // Telefone deve ter 10 ou 11 dígitos
    for (i = 0; i < len; i++) {
        if (telefone[i] < '0' || telefone[i] > '9') return 0; // Verifica se são apenas números
    }
    return 1; // Telefone válido
}

int validar_email(const char *email) {
    if (strchr(email, '@') && strchr(email, '.')) return 1;
    return 0; // Email inválido
}

int validar_data(const char *data) {
    // Verifica se a data tem o formato DD/MM/YYYY
    if (strlen(data) != 10) return 0; // A data deve ter 10 caracteres (DD/MM/YYYY)

    // Verifica se as barras estão nas posições corretas
    if (data[2] != '/' || data[5] != '/') return 0;

    // Verifica se são apenas números antes e depois das barras
    for (int i = 0; i < 10; i++) {
        if (i == 2 || i == 5) continue; // Ignora as barras
        if (data[i] < '0' || data[i] > '9') return 0; // Verifica se são números
    }

    return 1; // Data válida no formato DD/MM/YYYY
}

int validar_cep(const char *cep) {
    int i, len = strlen(cep);
    if (len != 8) return 0; // CEP deve ter 8 caracteres
    for (i = 0; i < len; i++) {
        if (cep[i] < '0' || cep[i] > '9') return 0; // Verifica se são apenas números
    }
    return 1; // CEP válido
}

void salvar_industria(const char *nome_empresa, const char *cnpj, const char *razao_social,
                      const char *nome_fantasia, const char *telefone, const char *email,
                      const char *data_abertura, const char *endereco, const char *cidade,
                      const char *estado, const char *cep) {
    FILE *arquivo = fopen("dados.txt", "a");
    if (arquivo) {
        fprintf(arquivo, "%s %s %s %s %s %s %s %s %s %s\n", nome_empresa, cnpj, razao_social, nome_fantasia,
                telefone, email, data_abertura, endereco, cidade, estado, cep);
        fclose(arquivo);
    } else {
        g_print("Erro ao abrir arquivo para salvar a indústria!\n");
    }
}


//----------------------------------------------------------------------------------------------------------------------------

void salvar_ultimo_usuario(const char *usuario) {
    FILE *arquivo = fopen("ultimo_usuario.txt", "w");
    if (arquivo) {
        fprintf(arquivo, "%s\n", usuario);
        fclose(arquivo);
    } else {
        g_print("Erro ao salvar o último usuário!\n");
    }
}

void carregar_ultimo_usuario(GtkEntry *entry_usuario) {
    FILE *arquivo = fopen("ultimo_usuario.txt", "r");
    if (arquivo) {
        char usuario[100];
        if (fgets(usuario, sizeof(usuario), arquivo)) {
            // Remove a quebra de linha, se houver
            usuario[strcspn(usuario, "\n")] = '\0';
            gtk_entry_set_text(entry_usuario, usuario);
        }
        fclose(arquivo);
    }
}

//----------------------------------------------------------------------------------------------------------------------------
// Verificar login
int verificar_login(const char *usuario, const char *senha) {
    FILE *arquivo = fopen("dados.txt", "r");
    if (!arquivo) return 0;

    char linha[256], usuario_lido[100], senha_lida[100], senha_descriptografada[100];
    while (fgets(linha, sizeof(linha), arquivo)) {
        sscanf(linha, "%s %s", usuario_lido, senha_lida);
        descriptografar_senha(senha_lida, senha_descriptografada);
        if (strcmp(usuario, usuario_lido) == 0 && strcmp(senha, senha_descriptografada) == 0) {
            fclose(arquivo);
            return 1; // Login bem-sucedido
        }
    }
    fclose(arquivo);
    return 0; // Falha no login
}

// Callback para o botão de login
void on_botao_login_clicked(GtkButton *button, gpointer user_data) {
    GtkBuilder *builder = GTK_BUILDER(user_data);
    GtkWidget *entry_usuario = GTK_WIDGET(gtk_builder_get_object(builder, "login_usuario"));
    GtkWidget *entry_senha = GTK_WIDGET(gtk_builder_get_object(builder, "login_senha"));
    GtkWidget *label_mensagem = GTK_WIDGET(gtk_builder_get_object(builder, "label_mensagem"));
    GtkWidget *check_lembrar = GTK_WIDGET(gtk_builder_get_object(builder, "lembrar_de_mim"));
    GtkStack *stack = GTK_STACK(gtk_builder_get_object(builder, "stack"));

    const char *usuario = gtk_entry_get_text(GTK_ENTRY(entry_usuario));
    const char *senha = gtk_entry_get_text(GTK_ENTRY(entry_senha));

    if (verificar_login(usuario, senha)) {
        if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check_lembrar))) {
            salvar_ultimo_usuario(usuario);
        gtk_entry_set_text(GTK_ENTRY(entry_senha), "");
        }
        gtk_stack_set_visible_child_name(stack, "view_menu_principal");
    } else {
        gtk_label_set_text(GTK_LABEL(label_mensagem), "Login ou senha inválidos!");
    }
}

// Callback para o botão de sair no menu
void on_menu_sair_clicked(GtkButton *button, gpointer user_data) {
    GtkBuilder *builder = GTK_BUILDER(user_data);
    GtkStack *stack = GTK_STACK(gtk_builder_get_object(builder, "stack"));
    gtk_stack_set_visible_child_name(stack, "view_login");
}

void on_botao_voltar_atualizacao_clicked(GtkButton *button, gpointer user_data) {
    GtkBuilder *builder = GTK_BUILDER(user_data);
    GtkStack *stack = GTK_STACK(gtk_builder_get_object(builder, "stack"));
    gtk_stack_set_visible_child_name(stack, "view_menu_principal");
}

void on_atualizacao_de_residuos_clicked(GtkButton *button, gpointer user_data) {
    GtkBuilder *builder = GTK_BUILDER(user_data);
    GtkStack *stack = GTK_STACK(gtk_builder_get_object(builder, "stack"));
    gtk_stack_set_visible_child_name(stack, "view_atualizar_residuos");
}

// Callback para o botão de cadastro de usuário
void on_cadastro_usuario_clicked(GtkButton *button, gpointer user_data) {
    GtkBuilder *builder = GTK_BUILDER(user_data);
    GtkStack *stack = GTK_STACK(gtk_builder_get_object(builder, "stack"));
    gtk_stack_set_visible_child_name(stack, "view_cadastro_usuario");
}

// Callback para o botão de confirmar cadastro
void on_confirma_cadastro_usuario_clicked(GtkButton *button, gpointer user_data) {
    GtkBuilder *builder = GTK_BUILDER(user_data);
    GtkWidget *entry_usuario = GTK_WIDGET(gtk_builder_get_object(builder, "cadastro_usuario_entry"));
    GtkWidget *entry_senha = GTK_WIDGET(gtk_builder_get_object(builder, "senha_cadastro_entry"));
    GtkWidget *label_cadastro_usuario = GTK_WIDGET(gtk_builder_get_object(builder, "label_cadastro_usuario"));
    GtkStack *stack = GTK_STACK(gtk_builder_get_object(builder, "stack"));

    const char *usuario = gtk_entry_get_text(GTK_ENTRY(entry_usuario));
    const char *senha = gtk_entry_get_text(GTK_ENTRY(entry_senha));

    // Verificar se os campos de usuário ou senha estão vazios
    if (strlen(usuario) == 0 || strlen(senha) == 0) {
        gtk_label_set_text(GTK_LABEL(label_cadastro_usuario), "Usuário ou senha não podem ser vazios!");
        return;
    }

    char senha_criptografada[100];
    criptografar_senha(senha, senha_criptografada);

    // Tentar abrir o arquivo para salvar os dados
    FILE *arquivo = fopen("dados.txt", "a");
    if (arquivo) {
        fprintf(arquivo, "%s %s\n", usuario, senha_criptografada);
        fclose(arquivo);

        // Atualizar a label para informar sucesso
        gtk_label_set_text(GTK_LABEL(label_cadastro_usuario), "Cadastro realizado com sucesso!");

        // Limpar os campos de usuário e senha
        gtk_entry_set_text(GTK_ENTRY(entry_usuario), "");
        gtk_entry_set_text(GTK_ENTRY(entry_senha), "");

        // Não mudar para o menu principal ainda, até que o cadastro seja bem-sucedido
    } else {
        // Caso o arquivo não possa ser aberto, informar o erro na label
        gtk_label_set_text(GTK_LABEL(label_cadastro_usuario), "Erro ao salvar os dados.");
    }
}



// Callback para o botão de cancelar cadastro
void on_cancelar_cadastro_usuario_clicked(GtkButton *button, gpointer user_data) {
    GtkBuilder *builder = GTK_BUILDER(user_data);
    GtkStack *stack = GTK_STACK(gtk_builder_get_object(builder, "stack"));
    gtk_stack_set_visible_child_name(stack, "view_menu_principal");
}
void on_botao_sair_clicked(GtkButton *button, gpointer user_data) {
    gtk_main_quit();

}






//---------------------------------ATUALIZAÇAÕ DE RESIDUOS-----------------------------//

        ////////////////////////////FUNCAO PARA BUSCA DA EMPRESA NO ARQUIVO//////////////////////
        // Função para procurar uma indústria no banco de dados
gboolean buscar_industria(const char *termo, char *nome_industria, char *custos, char *residuos) {
    FILE *arquivo = fopen("dados.txt", "r");
    if (!arquivo) return FALSE;

    char linha[256];
    while (fgets(linha, sizeof(linha), arquivo)) {
        char cnpj[20], razao_social[100], email[100], custo_atual[20], residuos_atual[20];
        sscanf(linha, "%s %s %s %s %s", cnpj, razao_social, email, custo_atual, residuos_atual);

        if (strstr(cnpj, termo) || strstr(razao_social, termo)) {
            strcpy(nome_industria, razao_social);
            strcpy(custos, custo_atual);
            strcpy(residuos, residuos_atual);
            fclose(arquivo);
            return TRUE;
        }
    }

    fclose(arquivo);
    return FALSE;
}


            //----------------CALLBACK PARA BOTAO BUSCAR------------------//

void on_buscar_atualizar_clicked(GtkButton *button, gpointer user_data) {
    GtkBuilder *builder = GTK_BUILDER(user_data);
    GtkStack *stack = GTK_STACK(gtk_builder_get_object(builder, "stack"));
    GtkSearchEntry *search_entry = GTK_SEARCH_ENTRY(gtk_builder_get_object(builder, "pesquisa_industria"));
    GtkLabel *label_info = GTK_LABEL(gtk_builder_get_object(builder, "informacoes_view_atualizar"));

    const char *termo = gtk_entry_get_text(GTK_ENTRY(search_entry));

    char nome_industria[100] = {0};
    char custos[20] = {0};
    char residuos[20] = {0};

    if (buscar_industria(termo, nome_industria, custos, residuos)) {
        // Atualiza a próxima view com os dados encontrados
        GtkLabel *label_industria = GTK_LABEL(gtk_builder_get_object(builder, "industria_selecionada"));
        GtkLabel *label_custos = GTK_LABEL(gtk_builder_get_object(builder, "custo_atual_atualizar"));
        GtkLabel *label_residuos = GTK_LABEL(gtk_builder_get_object(builder, "residuos_atual_atualizar"));

        gtk_label_set_text(label_industria, nome_industria);
        gtk_label_set_text(label_custos, custos);
        gtk_label_set_text(label_residuos, residuos);

        gtk_stack_set_visible_child_name(stack, "view_atualizar_selecionado");
    } else {
        gtk_label_set_text(label_info, "Indústria não encontrada!");
    }
}

//--------------------CALLBAKC PARA BOTAO ATUALIZAR OS DADOS--------------------//


void on_atualizar_residuos_tratados_clicked(GtkButton *button, gpointer user_data) {
    GtkBuilder *builder = GTK_BUILDER(user_data);

    GtkLabel *label_industria = GTK_LABEL(gtk_builder_get_object(builder, "industria_selecionada"));
    GtkEntry *entry_custos = GTK_ENTRY(gtk_builder_get_object(builder, "atualizar_custos"));
    GtkEntry *entry_residuos = GTK_ENTRY(gtk_builder_get_object(builder, "atualizar_residuos_ratados"));
    GtkLabel *label_status = GTK_LABEL(gtk_builder_get_object(builder, "status_atualizacao"));

    const char *industria = gtk_label_get_text(label_industria);
    const char *novos_custos = gtk_entry_get_text(entry_custos);
    const char *novos_residuos = gtk_entry_get_text(entry_residuos);

    if (strlen(novos_custos) == 0 || strlen(novos_residuos) == 0) {
        gtk_label_set_text(label_status, "Por favor, preencha todos os campos.");
        return;
    }

    FILE *arquivo = fopen("dados.txt", "r+");
    if (!arquivo) {
        gtk_label_set_text(label_status, "Erro ao abrir o arquivo.");
        return;
    }

    char linha[256], novo_conteudo[1024] = {0};
    gboolean atualizado = FALSE;

    while (fgets(linha, sizeof(linha), arquivo)) {
        char cnpj[20], razao_social[100], email[100], custos[20], residuos[20];
        sscanf(linha, "%s %s %s %s %s", cnpj, razao_social, email, custos, residuos);

        if (strcmp(razao_social, industria) == 0) {
            snprintf(linha, sizeof(linha), "%s %s %s %s %s\n", cnpj, razao_social, email, novos_custos, novos_residuos);
            atualizado = TRUE;
        }
        strcat(novo_conteudo, linha);
    }

    fclose(arquivo);

    if (atualizado) {
        arquivo = fopen("dados.txt", "w");
        if (arquivo) {
            fputs(novo_conteudo, arquivo);
            fclose(arquivo);
            gtk_label_set_text(label_status, "Atualização realizada com sucesso!");
        } else {
            gtk_label_set_text(label_status, "Erro ao salvar os dados.");
        }
    } else {
        gtk_label_set_text(label_status, "Indústria não encontrada no arquivo.");
    }
}


//------------------------------------------------------------DEPURAÇÃO-----------




//--------------------------------------------------------------------------------



//---------------------------------CADASTRO DE INDUSTRIA-------------------------------

void on_cadastro_de_industria_clicked(GtkButton *button, gpointer user_data) {
    GtkBuilder *builder = GTK_BUILDER(user_data);
    GtkWidget *stack = GTK_WIDGET(gtk_builder_get_object(builder, "stack"));  // A GtkStack do seu app
    GtkWidget *view_cadastro_industria = GTK_WIDGET(gtk_builder_get_object(builder, "view_cadastro_industria"));

    // Mostrar a view de cadastro de indústria no GtkStack
    gtk_stack_set_visible_child(GTK_STACK(stack), view_cadastro_industria);
}

void on_botao_cadastro_industria_clicked(GtkButton *button, gpointer user_data) {
    GtkBuilder *builder = GTK_BUILDER(user_data);
    GtkWidget *nome_empresa = GTK_WIDGET(gtk_builder_get_object(builder, "cadastro_nome_empresa"));
    GtkWidget *cnpj = GTK_WIDGET(gtk_builder_get_object(builder, "cadastro_cnpj"));
    GtkWidget *razao_social = GTK_WIDGET(gtk_builder_get_object(builder, "cadastro_razao"));
    GtkWidget *nome_fantasia = GTK_WIDGET(gtk_builder_get_object(builder, "cadastro_nome_fantasia"));
    GtkWidget *telefone = GTK_WIDGET(gtk_builder_get_object(builder, "cadastro_telefone"));
    GtkWidget *email = GTK_WIDGET(gtk_builder_get_object(builder, "cadastro_email"));
    GtkWidget *data_abertura = GTK_WIDGET(gtk_builder_get_object(builder, "cadastro_data"));
    GtkWidget *endereco = GTK_WIDGET(gtk_builder_get_object(builder, "cadastro_endereco"));
    GtkWidget *cidade = GTK_WIDGET(gtk_builder_get_object(builder, "cadastro_cidade"));
    GtkWidget *estado = GTK_WIDGET(gtk_builder_get_object(builder, "cadastro_estado"));
    GtkWidget *cep = GTK_WIDGET(gtk_builder_get_object(builder, "cadastro_cep"));
    GtkWidget *label_mensagem = GTK_WIDGET(gtk_builder_get_object(builder, "cadastro_industria_label"));

    const char *nome_empresa_text = gtk_entry_get_text(GTK_ENTRY(nome_empresa));
    const char *cnpj_text = gtk_entry_get_text(GTK_ENTRY(cnpj));
    const char *razao_social_text = gtk_entry_get_text(GTK_ENTRY(razao_social));
    const char *nome_fantasia_text = gtk_entry_get_text(GTK_ENTRY(nome_fantasia));
    const char *telefone_text = gtk_entry_get_text(GTK_ENTRY(telefone));
    const char *email_text = gtk_entry_get_text(GTK_ENTRY(email));
    const char *data_abertura_text = gtk_entry_get_text(GTK_ENTRY(data_abertura));
    const char *endereco_text = gtk_entry_get_text(GTK_ENTRY(endereco));
    const char *cidade_text = gtk_entry_get_text(GTK_ENTRY(cidade));
    const char *estado_text = gtk_entry_get_text(GTK_ENTRY(estado));
    const char *cep_text = gtk_entry_get_text(GTK_ENTRY(cep));

    // Validação dos campos
    if (strlen(nome_empresa_text) == 0 || strlen(cnpj_text) == 0 || strlen(razao_social_text) == 0 ||
        strlen(nome_fantasia_text) == 0 || strlen(telefone_text) == 0 || strlen(email_text) == 0 ||
        strlen(data_abertura_text) == 0 || strlen(endereco_text) == 0 || strlen(cidade_text) == 0 ||
        strlen(estado_text) == 0 || strlen(cep_text) == 0) {
        gtk_label_set_text(GTK_LABEL(label_mensagem), "Todos os campos são obrigatórios!");
        return;
    }

    if (!validar_cnpj(cnpj_text)) {
        gtk_label_set_text(GTK_LABEL(label_mensagem), "CNPJ inválido! Deve ter 14 números.");
        return;
    }

    if (!validar_telefone(telefone_text)) {
        gtk_label_set_text(GTK_LABEL(label_mensagem), "Telefone inválido! Deve ter 10 ou 11 números.");
        return;
    }

    if (!validar_email(email_text)) {
        gtk_label_set_text(GTK_LABEL(label_mensagem), "Email inválido! Verifique o formato.");
        return;
    }

    if (!validar_data(data_abertura_text)) {
        gtk_label_set_text(GTK_LABEL(label_mensagem), "Data inválida! Deve ser no formato DD/MM/YYYY.");
        return;
    }

    if (!validar_cep(cep_text)) {
        gtk_label_set_text(GTK_LABEL(label_mensagem), "CEP inválido! Deve ter 8 números.");
        return;
    }

    // Salvar dados se estiver tudo correto
    salvar_industria(nome_empresa_text, cnpj_text, razao_social_text, nome_fantasia_text,
                     telefone_text, email_text, data_abertura_text, endereco_text, cidade_text,
                     estado_text, cep_text);

    gtk_label_set_text(GTK_LABEL(label_mensagem), "Indústria cadastrada com sucesso!");
}

void on_botao_cancelar_industria_clicked(GtkButton *button, gpointer user_data) {
    GtkBuilder *builder = GTK_BUILDER(user_data);
    GtkStack *stack = GTK_STACK(gtk_builder_get_object(builder, "stack"));
    gtk_stack_set_visible_child_name(stack, "view_menu_principal");
}


//----------------------FUNCAO PRINCIPAL--------------------------------------------

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    GtkBuilder *builder = gtk_builder_new_from_file("user_interface.glade");
    GtkWidget *main_window = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));

    GtkWidget *entry_usuario = GTK_WIDGET(gtk_builder_get_object(builder, "login_usuario"));
    carregar_ultimo_usuario(GTK_ENTRY(entry_usuario));

    // Conectar sinais
    gtk_builder_add_callback_symbols(
        builder,
        "on_botao_login_clicked", G_CALLBACK(on_botao_login_clicked),
        "on_menu_sair_clicked", G_CALLBACK(on_menu_sair_clicked),
        "on_cadastro_usuario_clicked", G_CALLBACK(on_cadastro_usuario_clicked),
        "on_confirma_cadastro_usuario_clicked", G_CALLBACK(on_confirma_cadastro_usuario_clicked),
        "on_cancelar_cadastro_usuario_clicked", G_CALLBACK(on_cancelar_cadastro_usuario_clicked),
        "on_cadastro_de_industria_clicked", G_CALLBACK(on_cadastro_de_industria_clicked),
        "on_botao_cadastro_industria_clicked", G_CALLBACK(on_botao_cadastro_industria_clicked),
        "on_botao_cancelar_industria_clicked", G_CALLBACK(on_botao_cancelar_industria_clicked),
        "on_botao_sair_clicked", G_CALLBACK(on_botao_sair_clicked),
        "on_atualizacao_de_residuos_clicked", G_CALLBACK(on_atualizacao_de_residuos_clicked),
        "on_botao_voltar_atualizacao_clicked",G_CALLBACK(on_botao_voltar_atualizacao_clicked),
                                     "on_buscar_atualizar_clicked",G_CALLBACK(on_buscar_atualizar_clicked),
                                     //"on_selecionar_pesquisa_clicked",G_CALLBACK(on_selecionar_pesquisa_clicked),




        NULL
    );





    gtk_builder_connect_signals(builder, builder);

    gtk_widget_show_all(main_window);
    gtk_main();

    g_object_unref(builder);
    return 0;
}
