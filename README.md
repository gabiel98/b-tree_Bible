# Introdução

O objetivo é medir, para cada token lido do arquivo da Bíblia em formato txt, quanto tempo (em μs) leva para inserir e, depois, remover esse token da B-Tree, em diferentes estágios de crescimento da árvore. Os tempos são registrados em CSVs e plotados para análise de comportamento amortizado e variações pontuais (split, merge, rebalanceamento).

## Como Rodar

### 1. Compilar e Executar o Código em C

```bash
# Compilar
gcc btree.c -o btree

# Executar (gera insert_times.csv e delete_times.csv lendo 'blivre.txt')
./btree
```
### 2. Gerar Gráficos com Python

```bash
# Instale as dependências (pandas, matplotlib)
pip install pandas matplotlib

# Gerar todos os gráficos
python gerar_grafico.py
```
## Estrutura do Projeto

```bash
├── btree2.c # Implementação em C da B‑Tree e benchmark
├── blivre.txt # Arquivo de entrada (texto a ser indexado)
├── insert_times.csv # Saída: tempos de inserção (μs)
├── delete_times.csv # Saída: tempos de remoção (μs)
├── gerar_grafico.py # Script em Python para gerar gráficos
├── desempenho_insercao.png # Gráfico: inserção vs. tamanho
├── desempenho_remocao.png # Gráfico: remoção vs. tamanho
├── comparacao_desempenho.png # Gráfico comparativo
└── tempo_acumulado.png # Gráfico de tempo total acumulado
```

## Pseudocódigo da B‑Tree (Ordem 3)

```text
CONST ORDER ← 3
MAX_KEYS ← 2·ORDER − 1
MIN_KEYS ← ORDER − 1

função CREATE_NODE(is_leaf):
    node.is_leaf ← is_leaf
    node.num_keys ← 0
    alocar node.keys[0…MAX_KEYS−1]
    alocar node.children[0…MAX_KEYS]

função SPLIT_CHILD(parent, i):
    y ← parent.children[i]
    z ← CREATE_NODE(y.is_leaf)
    // mover chaves e filhos
    para j de 0 até ORDER−2:
        z.keys[j] ← y.keys[j+ORDER]
    se não y.is_leaf:
        para j de 0 até ORDER−1:
            z.children[j] ← y.children[j+ORDER]
    y.num_keys ← ORDER−1

    inserir z como child de parent em posição i+1
    deslocar parent.keys e parent.children após i
    parent.keys[i] ← y.keys[ORDER−1]
    parent.num_keys++

função INSERT_NON_FULL(x, key):
    i ← x.num_keys−1
    se x.is_leaf:
        // inserir key em posição ordenada
        enquanto i ≥ 0 e key < x.keys[i]:
            x.keys[i+1] ← x.keys[i]
            i--
        x.keys[i+1] ← key
        x.num_keys++
    senão:
        while i ≥ 0 e key < x.keys[i]: i--
        i++
        se x.children[i].num_keys == MAX_KEYS:
            SPLIT_CHILD(x, i)
            se key > x.keys[i]: i++
        INSERT_NON_FULL(x.children[i], key)

função INSERT(tree, key):
    r ← tree.root
    se r.num_keys == MAX_KEYS:
        s ← CREATE_NODE(FALSE)
        s.children[0] ← r
        tree.root ← s
        SPLIT_CHILD(s, 0)
        INSERT_NON_FULL(s, key)
    senão:
        INSERT_NON_FULL(r, key)

função DELETE(tree, key):
    DELETE_KEY(tree.root, key)  // lógica completa em btree2.c
    se tree.root.num_keys == 0:
        ajustar raiz (merge ou novo nó vazio)
```
