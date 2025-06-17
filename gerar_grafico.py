import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import time

# Configurar backend para modo não interativo (mais rápido)
plt.switch_backend('Agg')

# Configurar estilo global
plt.rcParams['font.family'] = 'DejaVu Sans'
plt.rcParams['grid.alpha'] = 0.3
plt.rcParams['grid.linestyle'] = '--'
plt.rcParams['axes.titlepad'] = 20
plt.rcParams['axes.labelpad'] = 15

# Paleta de cores
AZULESCURO = "#073d64"
AMARELONEON = "#A6AC03"

# Função para carregar dados de forma otimizada
def carregar_dados(arquivo):
    return pd.read_csv(
        arquivo,
        usecols=['node_count', 'time_us'],  
        dtype={'node_count': 'int32', 'time_us': 'int32'}  
    )

# Função para gerar gráfico único
def gerar_grafico(df, titulo, nome_arquivo, cor, label):
    fig, ax = plt.subplots(figsize=(14, 8))
    
    # Plot otimizado com downsampling para grandes datasets
    if len(df) > 10000:
        step = len(df) // 5000 
        ax.plot(df['node_count'][::step], df['time_us'][::step], 
                color=cor, linewidth=1.0, label=label)
    else:
        ax.plot(df['node_count'], df['time_us'], 
                color=cor, linewidth=1.0, label=label)
    
    # Configurações do gráfico
    ax.set_title(titulo, fontsize=14)
    ax.set_xlabel("Número de Nós na Árvore", fontsize=12)
    ax.set_ylabel("Tempo por Operação (μs)", fontsize=12)
    ax.grid(True)
    ax.legend()
    
    fig.savefig(nome_arquivo, dpi=150, bbox_inches='tight')
    plt.close(fig)
    return f"Gráfico {nome_arquivo} gerado"

# Função para gerar gráfico combinado
def gerar_grafico_combinado(insert_df, delete_df):
    fig, ax = plt.subplots(figsize=(14, 8))
    
    # Downsampling se necessário
    if len(insert_df) > 10000:
        step = len(insert_df) // 5000
        ax.plot(insert_df['node_count'][::step], insert_df['time_us'][::step], 
                color=AZULESCURO, linewidth=1.0, label="Inserção")
        ax.plot(delete_df['node_count'][::step], delete_df['time_us'][::step], 
                color=AMARELONEON, linewidth=1.0, label="Remoção")
    else:
        ax.plot(insert_df['node_count'], insert_df['time_us'], 
                color=AZULESCURO, linewidth=1.0, label="Inserção")
        ax.plot(delete_df['node_count'], delete_df['time_us'], 
                color=AMARELONEON, linewidth=1.0, label="Remoção")
    
    # Configurações do gráfico
    ax.set_title("Comparação do Desempenho das Operações na B-Tree", fontsize=14)
    ax.set_xlabel("Número de Nós na Árvore", fontsize=12)
    ax.set_ylabel("Tempo por Operação (μs)", fontsize=12)
    ax.grid(True)
    ax.legend()
    
    fig.savefig("comparacao_desempenho.png", dpi=150, bbox_inches='tight')
    plt.close(fig)
    return "Gráfico comparacao_desempenho.png gerado"

# Função para gerar gráfico de tempo acumulado
def gerar_tempo_acumulado(insert_df, delete_df):
    # Calcular tempo acumulado
    insert_df["tempo_acumulado"] = insert_df["time_us"].cumsum()
    delete_df["tempo_acumulado"] = delete_df["time_us"].cumsum()
    
    fig, ax = plt.subplots(figsize=(14, 8))
    
    # Downsampling se necessário
    if len(insert_df) > 10000:
        step = len(insert_df) // 5000
        ax.plot(insert_df['node_count'][::step], insert_df['tempo_acumulado'][::step], 
                color=AZULESCURO, linewidth=1.0, label="Inserção")
        ax.plot(delete_df['node_count'][::step], delete_df['tempo_acumulado'][::step], 
                color=AMARELONEON, linewidth=1.0, label="Remoção")
    else:
        ax.plot(insert_df['node_count'], insert_df['tempo_acumulado'], 
                color=AZULESCURO, linewidth=1.0, label="Inserção")
        ax.plot(delete_df['node_count'], delete_df['tempo_acumulado'], 
                color=AMARELONEON, linewidth=1.0, label="Remoção")
    
    # Configurações do gráfico
    ax.set_title("Tempo Acumulado das Operações", fontsize=14)
    ax.set_xlabel("Número de Nós na Árvore", fontsize=12)
    ax.set_ylabel("Tempo Total Acumulado (μs)", fontsize=12)
    ax.grid(True)
    ax.legend()
    
    fig.savefig("tempo_acumulado.png", dpi=150, bbox_inches='tight')
    plt.close(fig)
    return "Gráfico tempo_acumulado.png gerado"

# Ponto de entrada principal
def main():
    print("Iniciando geração de gráficos...")
    inicio_total = time.time()
    
    try:
        # Carregar dados de forma otimizada
        print("Carregando dados...")
        insert_df = carregar_dados("insert_times.csv")
        delete_df = carregar_dados("delete_times.csv")
        print(f"Dados carregados - Insert: {len(insert_df)} linhas, Delete: {len(delete_df)} linhas")
        
        # Gerar gráficos individuais
        gerar_grafico(insert_df, 
                     "Desempenho da Inserção na B-Tree",
                     "desempenho_insercao.png",
                     AZULESCURO, "Inserção")
        
        gerar_grafico(delete_df, 
                     "Desempenho da Remoção na B-Tree",
                     "desempenho_remocao.png",
                     AMARELONEON, "Remoção")
        
        # Gerar gráfico combinado
        gerar_grafico_combinado(insert_df, delete_df)
        
        # Gerar gráfico de tempo acumulado
        gerar_tempo_acumulado(insert_df, delete_df)
        
        # Tempo total
        tempo_total = time.time() - inicio_total
        print(f"\nProcesso completo em {tempo_total:.2f} segundos")
        print("Gráficos gerados com sucesso:")
        print("- desempenho_insercao.png (Tempo de inserção por número de nós)")
        print("- desempenho_remocao.png (Tempo de remoção por número de nós)")
        print("- comparacao_desempenho.png (Comparação entre inserção e remoção)")
        print("- tempo_acumulado.png (Tempo acumulado)")
        
    except Exception as e:
        print(f"\nErro durante a execução: {e}")

if __name__ == "__main__":
    main()