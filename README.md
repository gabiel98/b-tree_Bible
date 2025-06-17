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
