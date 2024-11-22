# Green Flow App

Programa Finalizado. o aplicativo deve ser capaz de gravar arquivos, ler, fazer modificações e gerar um relatório dos dados armazenados.
O aplicativo fica dentro da pasta bin, junto com suas dlls necessárias para funcionar.
o arquivo de DADOS.csv foi estruturado de forma simples, para que possa ser modificado abrindo com o bloco de notas.

o Usuário inicial é 'admin1' com senha 'admin'. para alterar a senha do usuário. Com o programa aberto delete o arquivo DADOS.csv e depois clique na opção de cadastro de usuário para fazer um novo cadastro. 
Ao cadastrar um usuário novo a senha será criptografada com um método bem simples que apenas inverte os valores.

Caso apresente algum erro no programa ao logar com um usuário ou cirar um novo, abra o arquivo DADOS.csv com bloco de notas ou qualquer outro editor, apague tudo do arquivo e deixe somente uma linha com esses  dados 'USUARIO,1,admin1,nimda', isso fara com que o usuário admin1 inicial volte a funcionar.

Outros problemas com relação a geração de arquivos, dados errados ou problemas ao gerar os relatórios, apague o arquivo de dado com o nome 'industrias.csv' e faça novos cadastros de industrias.

Na tela para atualização dos dados, podem conter bugs, especialmente se o usuário tentar inserir caracteres inválidos, e se isso acontecer o arquivo 'industrias.csv' será corrompido com informações fora do padrão estabelecido e isso fara com que a tela de gerar relatórios não funcione de forma desejada.
