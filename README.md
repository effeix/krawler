# krawler

## Objetivo
Um *Web Crawler* tem como objetivo recuperar informações de uma ou mais páginas web de maneira inteligente, percorrendo caminhos criados pelos links de cada página. Neste projeto, o objetivo é criar um [Web Crawler](https://en.wikipedia.org/wiki/Web_crawler) capaz de recuperar informações de produtos disponíveis em sites de e-commerce e disponibilizá-las para o usuário, semelhante ao funcionamento do site [Buscapé](https://www.buscape.com.br/). Este *crawler* deve ser implementado de maneira paralela, afim de diminuir o tempo de execução do programa, criando assim um sistema amigável para o usuário.

A biblioteca PThread será utilizada para realizar esta implementação paralela. A paralelização do programa se dá por meio da utilização de diversas threads e troca de dados entre estas threads, ações facilitadas pela biblioteca. O intuito deste tipo de implementação é permitir a execução mais rápida do programa

Ao final da execução, o programa deve imprimir no terminal os produtos buscados no seguinte formato:

```json
{
    "nome": "",
    "descricao": "",
    "foto": "",
    "preco": 0,
    "preco_parcelado": 0,
    "preco_num_parcelas": 0,
    "categoria": "",
    "url": ""
}
```

## Utilização
Primeiramente, deve-se compilar o projeto. Na raíz do repositório, crie uma pasta de nome `build/`, ficando com a seguinte árvore:
```sh
$ mkdir krawler/build/
```

```
krawler/
    krawler/
        build/
        ....
    ...
```

Em seguida, vá para a pasta `build/`
```sh
$ cd build/
```

Por fim, realize a compilação:
```sh
$ cmake ..
$ make -j4
```

Um executável será criado dentro da pasta `build/`, de nome `krawler`.

O projeto possui apenas um executável, capaz de executar a versão sequencial ou paralela de acordo com o desejo do usuário. Existem quatro variáveis de ambiente a serem definidas antes da execução do programa:

| Variável 	| Descrição                                                     	| Padrão 	|
|:--------:	|---------------------------------------------------------------	|--------	|
|    URL   	| URL de uma categoria de site de E-Commerce                    	| nulo   	|
|   MODE   	| Modo de execução. "seq" para sequencial e "par" para paralelo 	| seq    	|
|  N_PROD  	| Quantidade de threads produtoras para o modo paralelo         	| 2      	|
|  N_CONS  	| Quantidade de threads consumidoras para o modo paralelo       	| 2      	|

#### Sites suportados
- [Magazine Luiza](https://www.magazineluiza.com.br/)

O programa utiliza categorias de produtos como entrada. A categoria é uma URL de um site suportado. No caso do Magazine Luiza, a URL deve ser obtida navegando para o site, clicando em **Todos os Departamentos** na parte superior esquerda, e escolhendo um dos links destacados pela área vermelha:

<img src="https://i.imgur.com/ief76w1.png" alt="" width="50%" height="50%" />

Na próxima página clique em um dos links destacados novamente pela área vermelha:

<img src="https://i.imgur.com/FNuQU2P.png" alt="" width="50%" height="50%" />

A URL da págna que abrirá é a que deve ser utilizada:

<img src="https://i.imgur.com/NmOzgTQ.png" alt="" width="50%" height="50%" />

O teste acima foi realizado clicando em: **Todos os Departamentos > Celulares > Smartphones** obtendo a URL **https://www.magazineluiza.com.br/smartphone/celulares-e-smartphones/s/te/tcsp/**

A URL precisa ser definida, senão o programa não executará e mostrará uma mensagem de erro. No modo sequencial, as variáveis N_PROD e N_CONS são ignoradas. É possível definir N_PROD e N_CONS como 0, mas causarará comportamento inesperado.

Exemplo de utilização sequencial:
```sh
$ URL=https://www.magazineluiza.com.br/smartphone/celulares-e-smartphones/s/te/tcsp/ MODE=seq ./krawler
```

Exemplo de utilização paralela:
```sh
$ URL=https://www.magazineluiza.com.br/smartphone/celulares-e-smartphones/s/te/tcsp/ MODE=par N_PROD=4 N_CONS=4 ./krawler
```

## Dependências
- libcurl
- Boost
- PThreads

## Funcionamento
### Versão sequencial
Esta versão é bastante simples. Primeiramente é coletada a URL do usuário e feito o download do HTML da página. Por meio do uso de *regex*, é possível obter outras páginas de produtos da mesma categoria. As diversas páginas de produtos são baixadas e, novamente utilizando *regex*, as informações dos produtos montadas de acordo com a visualização mencionada na introdução.

### Versão paralela
Já a versão paralela é um pouco mais complexa. Para este projeto foi escolhida a implementação do modelo Produtor-Consumidor, em que diversas threads são criadas, algumas como produtoras e outra como consumidoras. As threads produtoras estão encarregadas de realizar os downloads das diversas páginas e guardar este conteúdo em uma fila, enquanto as threads consumidoras coletam o conteúdo do download e aplicam as regras em *regex* para obter as informações desejadas. No final, as threads consumidoras imprimem cada produto no terminal.

A sincronização entre threads é realizada com a utilização de semáforos, que bloqueiam ou liberam o acesso à fila de tarefas (URLs) a serem cumpridas.

## Testes
URL: https://www.magazineluiza.com.br/agendas-e-diarios/papelaria/s/pa/pagd/ (568 produtos)
### Hardware
- Intel i7-4510U
- 8GB RAM
- Ubuntu 18.04
- Dell Inspiron 5447
- 4 núcleos (2 virtuais)

### Métricas

Afim de explorar a performance deste programa, algumas métricas de tempo são coletadas ao longo do *crawling*. São elas:

- Tempo ocioso (TOTAL_IDLE_TIME): tempo de espera pelo download de páginas
- Tempo por produto (PROD_TIME): tempo gasto para baixar uma página específica de um produto e criar sua visualização em JSON. Cada produto baixado possui uma linha com o tempo gasto.
- Tempo médio por produto (AVG_TIME_PER_PRODUCT): tempo total de execução dividido pelo número de produtos coletados.


Todas as métricas são enviadas para a saída de erro padrão do sistema, que pode ser redirecionada para um arquivo de texto a gosto do usuário, como no exemplo a seguir (todos os valores estão em segundos):

```
1.2
0.85
0.50
...
TOTAL_IDLE_TIME: 180
AVG_TIME_PER_PRODUCT: 0.7
```

### Resultados
Foram realizados testes utilizando as versões sequenciais e paralelas, sendo a segunda testada três vezes: 1) 3 threads produtoras e 1 consumidora; 2) 2 produtoras e 2 consumidoras; 3) 1 produtora e 3 consumidoras. Como o computador utilizado para testes possui apenas 4 núcleos, uma execução com mais threads causaria troca de contexto entre threads, o que não é interessante para o estudo dos tempos de execução do programa. Os valores de tempo ocioso para a implementação paralela são ambíguos, pois apresentam uma soma do tempo total, sendo que downloads esto sendo feitos paralelamente. Assim, o tempo foi dividido por 4, para bter uma média do tempo de download por thread.

| Tempos (s)          | Sequencial | Paralela (1) | Paralela (2) | Paralela (3) |
|:--------------------:|------------|--------------|--------------|--------------|
| TOTAL_IDLE_TIME      | 638.904    | 228.385      | 238.122      | 241.005      |
| AVG_TIME_PER_PRODUCT | 1.56       | 0.74         | 0.94         | 0.97         |

Foram analisados também os consumos de memória para a versão sequencial e a versão paralela 2, utilizando a biblioteca mprof (em python) como a seguir:

```sh
mprof run URL=https://www.magazineluiza.com.br/agendas-e-diarios/papelaria/s/pa/pagd/ MODE=seq ./krawler
```
e
```sh
mprof run URL=https://www.magazineluiza.com.br/agendas-e-diarios/papelaria/s/pa/pagd/ MODE=par N_PROD=2 N_CONS=2 ./krawler
```
Os resultados podem ser vistos com:
```sh
mprof plot
```
Em seguida, limpe a análise com:
```sh
mprof clean
```
