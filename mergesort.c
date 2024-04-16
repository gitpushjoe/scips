#include "scratchmips-essentials.h"

#define MAX_ARRAY_SIZE 2048

int *parse_array(char* str, int* size) {
	int* arr = (int*)_malloc(MAX_ARRAY_SIZE * sizeof(int));
	int i = 0;
	char *last = str;
	while (1) {
		if (!(*str)) {
			if (last != str) {
				arr[i++] = atoi(last);
			}
			break;
		}
		if (*str == ' ') {
			*str = '\0';
			arr[i++] = atoi(last);
			*str = ' ';
			last = str + 1;	
		}
		str++;
	}
	*size = i;
	return arr;
}

void sort(int *arr, int size, int left, int right, int *aux);
void merge(int *arr, int size, int left, int mid, int right, int *aux);
void print_array(int *arr, int size);

void mergesort(int *arr, int size) {
	int *aux = (int *)_malloc(size * sizeof(int));
	sort(arr, size, 0, size - 1, aux);
	free(aux);
}

void sort(int *arr, int size, int left, int right, int *aux) {
	if (left < right) {
		int mid = (left + right) / 2;
		sort(arr, size, left, mid, aux);
		sort(arr, size, mid + 1, right, aux);
		merge(arr, size, left, mid, right, aux);
	}
}


void merge(int *arr, int size, int left, int mid, int right, int *aux) {
	int n1 = mid - left + 1;
	int n2 = right - mid;
	for (int i = 0; i < n1; i++) {
		aux[i] = arr[left + i];
	}
	for (int i = 0; i < n2; i++) {
		aux[n1 + i] = arr[mid + 1 + i];
	}
	int i = 0, j = 0, k = left;
	while (i < n1 && j < n2) {
		if (aux[i] <= aux[n1 + j]) {
			arr[k++] = aux[i++];
		} else {
			arr[k++] = aux[n1 + j++];
		}
	}
	while (i < n1) {
		arr[k++] = aux[i++];
	}
	while (j < n2) {
		arr[k++] = aux[n1 + j++];
	}
}

void print_array(int *arr, int size) {
	putchar('[');
	for (int i = 0; i < size; i++) {
		printf("%d%s", arr[i], i == size - 1 ? "" : ", ");
	}
	putchar(']');
}

int merge_sort__start() {
	while (1) {
		char buf[131072];
		printf("Enter an array of integers separated by spaces:");
		gets(buf, 131072);
		int size;
		int *arr = parse_array(buf, &size);
		printf("\n\nArray size: %d", size);
		printf("\nOriginal array: ");
		print_array(arr, size);
		mergesort(arr, size);
		printf("\nSorted array: ");
		print_array(arr, size);
		printf("\n\n[Space] Continue");
		printf("\n[E] Exit\n");
		char c = getchar();
		while (c != ' ' && c != 'E' && c != 'e') {
			c = getchar();
		}
		if (c == 'E' || c == 'e') {
			free(arr);
			break;
		}
		cls();
	}
	return 0;
}
