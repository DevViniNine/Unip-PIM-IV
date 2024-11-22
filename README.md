Green Flow App
Programa Finalizado. O aplicativo deve ser capaz de gravar arquivos, ler, fazer modificações e gerar um relatório dos dados armazenados.
O aplicativo fica dentro da pasta bin, junto com as suas DLLs necessárias para funcionar.

O arquivo DADOS.csv foi estruturado de forma simples, para que possa ser modificado abrindo com o Bloco de Notas.

O usuário inicial é 'admin' com senha 'admin'. Para alterar a senha do usuário, com o programa aberto, delete o arquivo DADOS.csv e depois clique na opção de cadastro de usuário para fazer um novo cadastro.
Ao cadastrar um novo usuário, a senha será criptografada com um método bem simples que apenas inverte os valores.

Caso apresente algum erro no programa ao logar com um usuário ou criar um novo, abra o arquivo DADOS.csv com o Bloco de Notas ou qualquer outro editor, apague tudo do arquivo e deixe somente uma linha com esses dados: 'USUARIO,1,admin,nimda'. Isso fará com que o usuário Admin inicial volte a funcionar.

Na tela para atualização dos dados, podem ocorrer bugs, especialmente se o usuário tentar inserir caracteres inválidos. Se isso acontecer, o arquivo industrias.csv será corrompido com informações fora do padrão estabelecido, o que fará com que a tela de geração de relatórios não funcione como esperado.
Para resolver isso, basta abrir o arquivo industrias.csv em um editor qualquer e fazer a reorganização dos dados. Dentro do diretório Relatorios Greenflow há um arquivo de backup das indústrias cadastradas, basta organizar os arquivos conforme o cabeçalho.

Outros problemas relacionados à geração de arquivos, dados errados ou problemas ao gerar os relatórios: apague o arquivo de dados industrias.csv e faça novos cadastros de indústrias.
