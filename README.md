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
