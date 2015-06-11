#include <stdio.h>
#include <time.h>

typedef struct Node {
    unsigned long hash;
    unsigned int time;
    struct Node *next;
} Node;

int count = 0;
Node *head = NULL;

void list_add(unsigned long hash) {
    Node *tmp = (Node*) malloc(sizeof(Node));
    count++;
    tmp->time = time(NULL);
    tmp->next = head;
    head = tmp;
    printf("nodes: %d\n", count);
}

int list_find(unsigned long hash) {
	Node *old, *parent, *tmp;

	// clear old nodes
	tmp = head, parent = NULL;
	while(tmp != NULL) {
		if (time(NULL) - tmp->time > 1) { 
			// delete node
			old = tmp, tmp = tmp->next;
			if (parent != NULL) parent->next = tmp; else head = tmp;
			free(old);
			count--;
		} else {
			// goto next
			parent = tmp;
			tmp = tmp->next;
		}
	}	

	// find by hash
	tmp = head;
	while(tmp != NULL) {
		if (tmp->hash == hash) return 1;
		tmp = tmp->next;
	}
	return 0;
}
