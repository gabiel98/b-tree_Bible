// Nome: Gabriel Peixoto menezes da Costa

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <sys/time.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <time.h>
#endif

// Definições da B-Tree
#define ORDER 3
#define MAX_KEYS (2*ORDER - 1)
#define MIN_KEYS (ORDER - 1)

// Estrutura do nó da B-Tree
typedef struct BTreeNode {
    int is_leaf;
    int num_keys;
    char **keys;
    struct BTreeNode **children;
} BTreeNode;

// Estrutura principal da B-Tree
typedef struct {
    BTreeNode *root;
    int node_count;
} BTree;

// Cria novo nó
BTreeNode* create_node(int is_leaf) {
    BTreeNode *node = malloc(sizeof(BTreeNode));
    node->is_leaf = is_leaf;
    node->num_keys = 0;
    
    node->keys = malloc(MAX_KEYS * sizeof(char*));
    for (int i = 0; i < MAX_KEYS; i++) {
        node->keys[i] = NULL;
    }
    
    node->children = malloc((MAX_KEYS+1) * sizeof(BTreeNode*));
    for (int i = 0; i <= MAX_KEYS; i++) {
        node->children[i] = NULL;
    }
    
    return node;
}

// Inicializa B-Tree
BTree* create_btree() {
    BTree *tree = malloc(sizeof(BTree));
    tree->root = create_node(1);
    tree->node_count = 1;
    return tree;
}

// Libera memória da árvore
void free_node(BTreeNode *node) {
    if (!node) return;
    
    for (int i = 0; i < node->num_keys; i++) {
        free(node->keys[i]);
    }
    free(node->keys);
    
    for (int i = 0; i <= node->num_keys; i++) {
        free_node(node->children[i]);
    }
    free(node->children);
    
    free(node);
}

void free_btree(BTree *tree) {
    free_node(tree->root);
    free(tree);
}

// Operações da B-Tree
void split_child(BTree *tree, BTreeNode *parent, int index) {
    BTreeNode *child = parent->children[index];
    BTreeNode *new_child = create_node(child->is_leaf);
    tree->node_count++;
    
    // Copia chaves e filhos
    new_child->num_keys = ORDER - 1;
    for (int i = 0; i < ORDER - 1; i++) {
        new_child->keys[i] = child->keys[i + ORDER];
        child->keys[i + ORDER] = NULL;
    }
    
    if (!child->is_leaf) {
        for (int i = 0; i < ORDER; i++) {
            new_child->children[i] = child->children[i + ORDER];
            child->children[i + ORDER] = NULL;
        }
    }
    
    child->num_keys = ORDER - 1;
    
    // Ajusta pai
    for (int i = parent->num_keys; i > index; i--) {
        parent->children[i + 1] = parent->children[i];
        parent->keys[i] = parent->keys[i - 1];
    }
    
    parent->children[index + 1] = new_child;
    parent->keys[index] = child->keys[ORDER - 1];
    child->keys[ORDER - 1] = NULL;
    parent->num_keys++;
}

// Inserção
void insert_non_full(BTree *tree, BTreeNode *node, const char *key) {
    int i = node->num_keys - 1;
    
    if (node->is_leaf) {
        while (i >= 0 && strcmp(key, node->keys[i]) < 0) {
            node->keys[i + 1] = node->keys[i];
            i--;
        }
        node->keys[i + 1] = strdup(key);
        node->num_keys++;
    } else {
        while (i >= 0 && strcmp(key, node->keys[i]) < 0) i--;
        i++;
        
        if (node->children[i]->num_keys == MAX_KEYS) {
            split_child(tree, node, i);
            if (strcmp(key, node->keys[i]) > 0) i++;
        }
        
        insert_non_full(tree, node->children[i], key);
    }
}

void insert_key(BTree *tree, const char *key) {
    BTreeNode *root = tree->root;
    
    if (root->num_keys == MAX_KEYS) {
        BTreeNode *new_root = create_node(0);
        new_root->children[0] = root;
        tree->root = new_root;
        tree->node_count++;
        split_child(tree, new_root, 0);
        insert_non_full(tree, new_root, key);
    } else {
        insert_non_full(tree, root, key);
    }
}

// Funções auxiliares para remoção
char* get_predecessor(BTreeNode *node) {
    while (!node->is_leaf) node = node->children[node->num_keys];
    return node->keys[node->num_keys - 1];
}

char* get_successor(BTreeNode *node) {
    while (!node->is_leaf) node = node->children[0];
    return node->keys[0];
}

// Operações de empréstimo e fusão
void borrow_from_prev(BTreeNode *node, int idx) {
    BTreeNode *child = node->children[idx];
    BTreeNode *sibling = node->children[idx - 1];
    
    for (int i = child->num_keys - 1; i >= 0; i--) {
        child->keys[i + 1] = child->keys[i];
    }
    
    if (!child->is_leaf) {
        for (int i = child->num_keys; i >= 0; i--) {
            child->children[i + 1] = child->children[i];
        }
    }
    
    child->keys[0] = node->keys[idx - 1];
    
    if (!child->is_leaf) {
        child->children[0] = sibling->children[sibling->num_keys];
    }
    
    node->keys[idx - 1] = sibling->keys[sibling->num_keys - 1];
    sibling->keys[sibling->num_keys - 1] = NULL;
    
    child->num_keys++;
    sibling->num_keys--;
}

void borrow_from_next(BTreeNode *node, int idx) {
    BTreeNode *child = node->children[idx];
    BTreeNode *sibling = node->children[idx + 1];
    
    child->keys[child->num_keys] = node->keys[idx];
    node->keys[idx] = sibling->keys[0];
    
    for (int i = 1; i < sibling->num_keys; i++) {
        sibling->keys[i - 1] = sibling->keys[i];
    }
    sibling->keys[sibling->num_keys - 1] = NULL;
    
    if (!child->is_leaf) {
        child->children[child->num_keys + 1] = sibling->children[0];
        for (int i = 1; i <= sibling->num_keys; i++) {
            sibling->children[i - 1] = sibling->children[i];
        }
    }
    
    child->num_keys++;
    sibling->num_keys--;
}

void merge_children(BTree *tree, BTreeNode *node, int idx) {
    BTreeNode *child = node->children[idx];
    BTreeNode *sibling = node->children[idx + 1];
    
    child->keys[child->num_keys] = node->keys[idx];
    child->num_keys++;
    
    for (int i = 0; i < sibling->num_keys; i++) {
        child->keys[child->num_keys + i] = sibling->keys[i];
    }
    
    if (!child->is_leaf) {
        for (int i = 0; i <= sibling->num_keys; i++) {
            child->children[child->num_keys + i] = sibling->children[i];
        }
    }
    
    child->num_keys += sibling->num_keys;
    
    for (int i = idx; i < node->num_keys - 1; i++) {
        node->keys[i] = node->keys[i + 1];
    }
    for (int i = idx + 1; i < node->num_keys; i++) {
        node->children[i] = node->children[i + 1];
    }
    node->num_keys--;
    
    free(sibling->keys);
    free(sibling->children);
    free(sibling);
    tree->node_count--;
}

void fill_child(BTree *tree, BTreeNode *node, int idx) {
    if (idx > 0 && node->children[idx - 1]->num_keys > MIN_KEYS) {
        borrow_from_prev(node, idx);
    } else if (idx < node->num_keys && node->children[idx + 1]->num_keys > MIN_KEYS) {
        borrow_from_next(node, idx);
    } else {
        if (idx < node->num_keys) {
            merge_children(tree, node, idx);
        } else {
            merge_children(tree, node, idx - 1);
        }
    }
}

// Remoção
void delete_from_leaf(BTreeNode *node, int idx) {
    free(node->keys[idx]);
    for (int i = idx + 1; i < node->num_keys; i++) {
        node->keys[i - 1] = node->keys[i];
    }
    node->num_keys--;
}

void delete_from_non_leaf(BTree *tree, BTreeNode *node, int idx) {
    char *key = node->keys[idx];
    
    if (node->children[idx]->num_keys >= ORDER) {
        char *pred = get_predecessor(node->children[idx]);
        free(node->keys[idx]);
        node->keys[idx] = strdup(pred);
        delete_key(tree, node->children[idx], pred);
    } else if (node->children[idx + 1]->num_keys >= ORDER) {
        char *succ = get_successor(node->children[idx + 1]);
        free(node->keys[idx]);
        node->keys[idx] = strdup(succ);
        delete_key(tree, node->children[idx + 1], succ);
    } else {
        merge_children(tree, node, idx);
        delete_key(tree, node->children[idx], key);
    }
}

void delete_key(BTree *tree, BTreeNode *node, const char *key) {
    int idx = 0;
    while (idx < node->num_keys && strcmp(key, node->keys[idx]) > 0) idx++;
    
    if (idx < node->num_keys && strcmp(key, node->keys[idx]) == 0) {
        if (node->is_leaf) delete_from_leaf(node, idx);
        else delete_from_non_leaf(tree, node, idx);
    } else {
        if (node->is_leaf) return;
        
        int is_last_child = (idx == node->num_keys);
        if (node->children[idx]->num_keys < ORDER) fill_child(tree, node, idx);
        
        if (is_last_child && idx > node->num_keys) {
            delete_key(tree, node->children[idx - 1], key);
        } else {
            delete_key(tree, node->children[idx], key);
        }
    }
}

void remove_key(BTree *tree, const char *key) {
    delete_key(tree, tree->root, key);
    
    if (tree->root->num_keys == 0) {
        BTreeNode *old_root = tree->root;
        if (tree->root->is_leaf) {
            tree->root = create_node(1);
            tree->node_count++;
        } else {
            tree->root = tree->root->children[0];
        }
        free(old_root->keys);
        free(old_root->children);
        free(old_root);
        tree->node_count--;
    }
}

// Processamento de arquivo
void tokenize_file(const char *filename, char ***tokens, int *token_count) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening file");
        exit(1);
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char *buffer = malloc(file_size + 1);
    fread(buffer, 1, file_size, file);
    buffer[file_size] = '\0';
    fclose(file);

    *token_count = 0;
    *tokens = NULL;

    char *token = strtok(buffer, " \t\n\r.,;:!?()[]{}'\"");
    while (token) {
        for (char *p = token; *p; p++) *p = tolower(*p);
        
        *tokens = realloc(*tokens, (*token_count + 1) * sizeof(char*));
        (*tokens)[*token_count] = strdup(token);
        (*token_count)++;
        
        token = strtok(NULL, " \t\n\r.,;:!?()[]{}'\"");
    }

    free(buffer);
}

// Função principal com benchmark
int main() {
    char **tokens;
    int token_count;
    tokenize_file("blivre.txt", &tokens, &token_count);
    
    BTree *tree = create_btree();
    FILE *insert_log = fopen("insert_times.csv", "w");
    FILE *delete_log = fopen("delete_times.csv", "w");
    fprintf(insert_log, "operation,node_count,time_us\n");
    fprintf(delete_log, "operation,node_count,time_us\n");
    
    #ifdef _WIN32
    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);
    #endif
    
    long long cumulative_time = 0;
    printf("Total de tokens: %d\n", token_count);

    // Benchmark de inserção
    for (int i = 0; i < token_count; i++) {
        #ifdef _WIN32
        LARGE_INTEGER start, end;
        QueryPerformanceCounter(&start);
        #else
        struct timespec start, end;
        clock_gettime(CLOCK_MONOTONIC, &start);
        #endif
        
        insert_key(tree, tokens[i]);
        
        #ifdef _WIN32
        QueryPerformanceCounter(&end);
        long long time_ns = (end.QuadPart - start.QuadPart) * 1000000000LL / frequency.QuadPart;
        long long time_us = time_ns / 1000;
        #else
        clock_gettime(CLOCK_MONOTONIC, &end);
        long long time_ns = (end.tv_sec - start.tv_sec) * 1000000000LL + (end.tv_nsec - start.tv_nsec);
        long long time_us = time_ns / 1000;
        #endif
        
        // Ajuste para tempos muito curtos
        if (time_us == 0) time_us = ++cumulative_time;
        else cumulative_time = time_us;
        
        fprintf(insert_log, "insert,%d,%lld\n", tree->node_count, time_us);
    }
    
    // Preparação para remoção
    int *delete_order = malloc(token_count * sizeof(int));
    for (int i = 0; i < token_count; i++) delete_order[i] = i;
    
    srand(time(NULL));
    for (int i = token_count - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temp = delete_order[i];
        delete_order[i] = delete_order[j];
        delete_order[j] = temp;
    }
    
    cumulative_time = 0;
    
    // Benchmark de remoção
    for (int i = 0; i < token_count; i++) {
        #ifdef _WIN32
        LARGE_INTEGER start, end;
        QueryPerformanceCounter(&start);
        #else
        struct timespec start, end;
        clock_gettime(CLOCK_MONOTONIC, &start);
        #endif
        
        remove_key(tree, tokens[delete_order[i]]);
        
        #ifdef _WIN32
        QueryPerformanceCounter(&end);
        long long time_ns = (end.QuadPart - start.QuadPart) * 1000000000LL / frequency.QuadPart;
        long long time_us = time_ns / 1000;
        #else
        clock_gettime(CLOCK_MONOTONIC, &end);
        long long time_ns = (end.tv_sec - start.tv_sec) * 1000000000LL + (end.tv_nsec - start.tv_nsec);
        long long time_us = time_ns / 1000;
        #endif
        
        if (time_us == 0) time_us = ++cumulative_time;
        else cumulative_time = time_us;
        
        fprintf(delete_log, "delete,%d,%lld\n", tree->node_count, time_us);
    }
    
    // Limpeza final
    fclose(insert_log);
    fclose(delete_log);
    for (int i = 0; i < token_count; i++) free(tokens[i]);
    free(tokens);
    free(delete_order);
    free_btree(tree);
    
    return 0;
}