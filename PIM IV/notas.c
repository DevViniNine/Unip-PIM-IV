gboolean buscar_industria(const char *termo, char *nome_industria, char *custos, char *residuos) {
    FILE *arquivo = fopen("industrias.csv", "r");
    if (!arquivo) return FALSE;

    char linha[256];
    while (fgets(linha, sizeof(linha), arquivo)) {
        char cnpj[20], razao_social[100], email[100], custos_atual[20], residuos_atual[20];

        // Aqui estamos tratando as colunas corretamente, com base nas aspas
        sscanf(linha, "\"%[^\"]\",\"%[^\"]\",\"%[^\"]\",\"%[^\"]\",\"%[^\"]\"", razao_social, cnpj, email, custos_atual, residuos_atual);

        // Agora vamos verificar se o termo corresponde ao CNPJ ou Razão Social
        if (strstr(cnpj, termo) || strstr(razao_social, termo)) {
            strcpy(nome_industria, razao_social);  // Armazena a razão social
            strcpy(custos, custos_atual);
            strcpy(residuos, residuos_atual);
            fclose(arquivo);
            return TRUE;
        }
    }

    fclose(arquivo);
    return FALSE;
}





void on_buscar_atualizar_clicked(GtkButton *button, gpointer user_data) {
    GtkBuilder *builder = GTK_BUILDER(user_data);
    GtkStack *stack = GTK_STACK(gtk_builder_get_object(builder, "stack"));
    GtkSearchEntry *search_entry = GTK_SEARCH_ENTRY(gtk_builder_get_object(builder, "pesquisa_industria"));
    GtkLabel *label_info = GTK_LABEL(gtk_builder_get_object(builder, "informacoes_view_atualizar"));

    const char *termo = gtk_entry_get_text(GTK_ENTRY(search_entry));

    // Verifica se o campo de pesquisa está vazio
    if (strlen(termo) == 0) {
        gtk_label_set_text(label_info, "Por favor, insira um termo para a busca.");
        return;
    }

    char nome_industria[100] = {0};
    char custos[20] = {0};
    char residuos[20] = {0};

    if (buscar_industria(termo, nome_industria, custos, residuos)) {
        // Atualiza a próxima view com os dados encontrados
        GtkEntry *entry_industria = GTK_ENTRY(gtk_builder_get_object(builder, "industria_selecionada"));
        GtkLabel *label_custos = GTK_LABEL(gtk_builder_get_object(builder, "custo_atual_atualizar"));
        GtkLabel *label_residuos = GTK_LABEL(gtk_builder_get_object(builder, "residuos_atual_atualizar"));

        gtk_entry_set_text(entry_industria, nome_industria);
        gtk_label_set_text(label_custos, strlen(custos) == 0 ? "Não informado" : custos);
        gtk_label_set_text(label_residuos, strlen(residuos) == 0 ? "Não informado" : residuos);

        gtk_stack_set_visible_child_name(stack, "view_atualizar_selecionado");
    } else {
        gtk_label_set_text(label_info, "Indústria não encontrada! Digite o CNPJ ou a razão social exata.");
    }
}





void on_atualizar_residuos_tratados_clicked(GtkButton *button, gpointer user_data) {
    GtkBuilder *builder = GTK_BUILDER(user_data);

    GtkEntry *entry_industria = GTK_ENTRY(gtk_builder_get_object(builder, "industria_selecionada"));
    GtkEntry *entry_custos = GTK_ENTRY(gtk_builder_get_object(builder, "atualizar_custos"));
    GtkEntry *entry_residuos = GTK_ENTRY(gtk_builder_get_object(builder, "atualizar_residuos_ratados"));
    GtkLabel *label_status = GTK_LABEL(gtk_builder_get_object(builder, "status_atualizacao"));

    const char *industria = gtk_entry_get_text(entry_industria);
    const char *novos_custos = gtk_entry_get_text(entry_custos);
    const char *novos_residuos = gtk_entry_get_text(entry_residuos);

    // Verifica se todos os campos estão preenchidos
    if (strlen(novos_custos) == 0 || strlen(novos_residuos) == 0 || strlen(industria) == 0) {
        gtk_label_set_text(label_status, "Por favor, preencha todos os campos.");
        return;
    }

    FILE *arquivo = fopen("industrias.csv", "r+");
    if (!arquivo) {
        gtk_label_set_text(label_status, "Erro ao abrir o arquivo.");
        return;
    }

    char linha[256], novo_conteudo[1024] = {0};
    gboolean atualizado = FALSE;

    while (fgets(linha, sizeof(linha), arquivo)) {
        char cnpj[20], razao_social[100], email[100], custos_atual[20], residuos_atual[20];
        sscanf(linha, "\"%[^\"]\",\"%[^\"]\",\"%[^\"]\",\"%[^\"]\",\"%[^\"]\"", cnpj, razao_social, email, custos_atual, residuos_atual);

        if (strcmp(razao_social, industria) == 0) {
            snprintf(linha, sizeof(linha), "\"%s\",\"%s\",\"%s\",\"%s\",\"%s\"\n", cnpj, razao_social, email, novos_custos, novos_residuos);
            atualizado = TRUE;
        }
        strcat(novo_conteudo, linha);
    }

    fclose(arquivo);

    // Se a indústria foi encontrada e atualizada, reabre o arquivo para escrever
    if (atualizado) {
        arquivo = fopen("industrias.csv", "w");
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


