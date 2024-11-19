#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/md5.h>

GtkBuilder *builder;
GtkWidget *window;
GtkWidget *entry_usuario;
GtkWidget *entry_senha;
GtkWidget *label_mensagem;

//---------------------------------------------------------------

#define CHAVE_XOR 0xAA  // Chave de criptografia simples XOR
#define ARQUIVO_DADOS "dados.txt"  // Arquivo onde os dados serão armazenados

// Função de criptografia simples XOR
void criptografar_senha(char *senha) {
    for (int i = 0; senha[i] != '\0'; i++) {
        senha[i] ^= CHAVE_XOR;  // Aplicando XOR com a chave
    }
}

// Função para salvar os dados no arquivo
void salvar_dados(const char *usuario, const char *senha) {
    FILE *file = fopen(ARQUIVO_DADOS, "a");  // Abrindo o arquivo para adicionar os dados
    if (file == NULL) {
        gtk_label_set_text(GTK_LABEL(label_mensagem), "Erro ao abrir o arquivo!");
        return;
    }

    // Criptografando a senha
    char senha_criptografada[100];
    strcpy(senha_criptografada, senha);
    criptografar_senha(senha_criptografada);  // Criptografando a senha

    // Salvando os dados no arquivo
    fprintf(file, "Usuário: %s\n", usuario);
    fprintf(file, "Senha: %s\n", senha_criptografada);
    fprintf(file, "-----------------------------\n");

    fclose(file);  // Fechando o arquivo
    gtk_label_set_text(GTK_LABEL(label_mensagem), "Usuário cadastrado com sucesso!");
}


// Função de leitura dos dados do arquivo (exemplo de como ler)
void ler_dados() {
    FILE *file = fopen(ARQUIVO_DADOS, "r");
    if (file == NULL) {
        gtk_label_set_text(GTK_LABEL(label_mensagem), "Erro ao abrir o arquivo!");
        return;
    }

    char linha[256];
    while (fgets(linha, sizeof(linha), file)) {
        printf("%s", linha);  // Exibindo as informações no terminal
    }

    fclose(file);  // Fechando o arquivo
}


// Função de verificação de login
int verificar_login(const char *usuario, const char *senha) {
    // Usuário e senha fixos para fins de demonstração
    const char *usuario_valido = "admin";
    char senha_valida[] = "admin";  // Senha não criptografada

    // Verificando o nome de usuário
    if (strcmp(usuario, usuario_valido) == 0) {
        char senha_criptografada[100];
        strcpy(senha_criptografada, senha);

        // Criptografar a senha digitada para comparação
        criptografar_senha(senha_criptografada);

        // Criptografando a senha válida
        criptografar_senha(senha_valida);

        // Comparando as senhas criptografadas
        if (strcmp(senha_criptografada, senha_valida) == 0) {
            return 1;  // Login bem-sucedido
        }
    }
    return 0;  // Login falhou
}

// Função chamada ao clicar no botão de login
void on_botao_login_clicked(GtkWidget *widget, gpointer data) {
    const char *usuario = gtk_entry_get_text(GTK_ENTRY(entry_usuario));
    const char *senha = gtk_entry_get_text(GTK_ENTRY(entry_senha));

    if (verificar_login(usuario, senha)) {
        gtk_label_set_text(GTK_LABEL(label_mensagem), "Login bem-sucedido!");
    } else {
        gtk_label_set_text(GTK_LABEL(label_mensagem), "Usuário ou senha incorretos.");
    }
}

//---------------------------------------------------------------


void on_main_window_destroy(GtkWidget *widget, gpointer data){
    gtk_main_quit();
}

void on_botao_cancelar_inicial_clicked(GtkWidget *widget, gpointer data){
    gtk_main_quit();

}
//----------------------------------------------------------------//

void on_consulta_industria_clicked(GtkWidget *widget, gpointer data){
    gtk_main_quit();
}

void on_atualizar_residuos_clicked(GtkWidget *widget, gpointer data){
    gtk_main_quit();
}

void on_menu_relatorios_clicked(GtkWidget *widget, gpointer data){
    gtk_main_quit();
}

//----------------Funcao para cadastro de usuario
void on_cadastro_usuario_clicked(GtkWidget *widget, gpointer data) {
    const char *usuario = gtk_entry_get_text(GTK_ENTRY(entry_usuario));
    const char *senha = gtk_entry_get_text(GTK_ENTRY(entry_senha));

    // Aqui podemos armazenar o usuário e a senha criptografada
    // Para fins de exemplo, estamos apenas exibindo a senha criptografada no label
    char senha_criptografada[100];
    strcpy(senha_criptografada, senha);
    criptografar_senha(senha_criptografada);  // Criptografando a senha

    gtk_label_set_text(GTK_LABEL(label_mensagem), "Usuário cadastrado com sucesso!");
}

//------------------------------------------------------------

void on_confirma_cadastro_usuario_clicked(GtkWidget *widget, gpointer data) {
    // Obter dados do cadastro
    const char *usuario = gtk_entry_get_text(GTK_ENTRY(entry_usuario));
    const char *senha = gtk_entry_get_text(GTK_ENTRY(entry_senha));

    if (usuario[0] == '\0' || senha[0] == '\0') {
        gtk_label_set_text(GTK_LABEL(label_mensagem), "Preencha todos os campos.");
        return;
    }

    // Salvar os dados no arquivo
    salvar_dados(usuario, senha);
}

//------------------------------------------------------------


void on_menu_sair_clicked(GtkWidget *widget, gpointer data){
    gtk_main_quit();
}

void on_botao_cadastro_industria_clicked(GtkWidget *widget, gpointer data){
    gtk_main_quit();
}

void on_botao_cancelar_industria_clicked(GtkWidget *widget, gpointer data){
    gtk_main_quit();
}

/*void on_main_window_destroy(GtkWidget *widget, gpointer data){
    gtk_main_quit();
}*/





int main(int argc, char *argv[]){
    gtk_init(&argc, &argv);
    GtkBuilder *builder = gtk_builder_new_from_file("user_interface.glade");

    gtk_builder_add_callback_symbols(builder,

                                     "on_main_window_destroy", G_CALLBACK(on_main_window_destroy),
                                     "on_botao_cancelar_inicial_clicked", G_CALLBACK(on_botao_cancelar_inicial_clicked),
                                     "on_botao_login_clicked", G_CALLBACK(on_botao_login_clicked),
                                     "on_consulta_industria_clicked", G_CALLBACK(on_consulta_industria_clicked),
                                     "on_atualizar_residuos_clicked", G_CALLBACK(on_atualizar_residuos_clicked),


                                     "on_menu_relatorios_clicked", G_CALLBACK(on_menu_relatorios_clicked),
                                     "on_cadastro_usuario_clicked", G_CALLBACK(on_cadastro_usuario_clicked),
                                     "on_menu_sair_clicked", G_CALLBACK(on_menu_sair_clicked),
                                     "on_botao_cadastro_industria_clicked", G_CALLBACK(on_botao_cadastro_industria_clicked),
                                     "on_botao_cancelar_industria_clicked", G_CALLBACK(on_botao_cancelar_industria_clicked),
                                     //"on_main_window_destroy", G_CALLBACK(on_main_window_destroy),
                                    "on_confirma_cadastro_usuario_clicked", G_CALLBACK(on_confirma_cadastro_usuario_clicked),NULL);


gtk_builder_connect_signals(builder, NULL);
GtkWidget *window = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));
entry_usuario = GTK_WIDGET(gtk_builder_get_object(builder, "cadastro_usuario_entry"));
entry_senha = GTK_WIDGET(gtk_builder_get_object(builder, "senha_cadastro_entry"));
label_mensagem = GTK_WIDGET(gtk_builder_get_object(builder, "label_mensagem"));

gtk_widget_show_all(window);
gtk_main();

return 0;
}
