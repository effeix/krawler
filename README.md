# krawler

## Objetivo
Neste projeto o objetivo é construir um *web crawler* para obter informações de produtos em sites de comércio eletrônico, tais como Mercado Livre, Lojas Americanas, Ponto Frio, Magazine Luiza, Amazon, entre outros. O programa possui duas versões: uma sequencial e uma paralela, a fim de comparar o desempenho das duas implementações.

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

Atualmente, o *crawler* suporta somente URLs do site [Magazine Luiza](https://www.magazineluiza.com.br/). O link correto para o funcionamento do programa deve ser obtido ao acessar o site, clicar em **Todos os departamentos** na barra de navegação superior, clicar em qualquer uma das opções e, na página da opção, clicar em uma categoria na coluna esquerda abaixo de **categorias**. A URL da página carregada após o último clique é a correta. 

Exemplo: **Todos os departamentos > Celulares > Smartphone**, gerando o link
[https://www.magazineluiza.com.br/smartphone/celulares-e-smartphones/s/te/tcsp/]() .

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

## Testes
### Hardware
- Intel i7-4510U
- 8GB RAM
- Ubuntu 18.04
- Dell Inspiron 5447

### Tempo de execução