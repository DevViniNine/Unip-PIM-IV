# Green Flow App

Projeto Finalizado: O Green Flow App é um aplicativo capaz de gravar, ler, fazer modificações e gerar relatórios dos dados armazenados. O aplicativo é colocado dentro da pasta bin, junto com as DLLs necessárias para seu funcionamento.

Para fazer o download, procure a aba 'Releases', que geralmente fica no lado esquerdo da página, e clique em 'GreenFlow v1.1.3'.

# Estrutura do Arquivo

O arquivo DADOS.csv foi estruturado de forma simples, permitindo que seja modificado facilmente, utilizando o Bloco de Notas ou qualquer outro editor de texto.

E os dados do relatório foram estruturados para ficar mais fácil organizar em um editor como Excel ou LibreOffice.

# Usuário Inicial

O usuário inicial é 'admin' com senha 'admin'. Para alterar a senha do usuário, com o programa aberto, delete o arquivo DADOS.csv e clique na opção de cadastro de usuário para criar um novo cadastro.

Ao cadastrar um novo usuário, a senha será criptografada com um método simples, que apenas inverte os caracteres.

# Recuperação de Dados
Caso ocorra algum erro ao logar com um usuário ou ao criar um novo, abra o arquivo DADOS.csv com o Bloco de Notas ou qualquer editor de sua preferência, apague o conteúdo do arquivo e deixe apenas a seguinte linha:

USUARIO,1,admin,nimda

Isso fará com que o usuário Admin inicial volte a funcionar corretamente.

# Atualização de Dados

Na tela de atualização dos dados, podem ocorrer bugs, especialmente se o usuário tentar inserir caracteres inválidos. Se isso acontecer, o arquivo industrias.csv será corrompido com informações fora do padrão estabelecido, o que pode comprometer a geração dos relatórios.

Para resolver, abra o arquivo industrias.csv em um editor de texto e reorganize os dados. Dentro do diretório Relatorios Greenflow, há um arquivo de backup com as indústrias cadastradas. Basta organizar os dados conforme o cabeçalho.

# Problemas Comuns

Caso haja problemas relacionados à geração de arquivos, dados errados ou dificuldades ao gerar os relatórios, apague o arquivo de dados industrias.csv e faça novos cadastros de indústrias.
