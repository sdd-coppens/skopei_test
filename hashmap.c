#include "hashmap.h"

char* strdup(const char* s) {
    if (s == NULL) {
        return NULL;
    }
    size_t len = strlen(s) + 1;
    char* duplicate = (char*) malloc(len);
	assert(duplicate);
    strcpy(duplicate, s);
    return duplicate;
}

int hash(HashMap * hm, char * key) {
	int sum = 0;
	int len = strlen(key) + 1;

	for (int i = 0; i < len; i++) {
		sum = sum + key[i];
	}

	return sum % hm->size;
}

HashMap * create_hashmap(size_t key_size) {
	assert(key_size > 0);

	HashMap * hm = (HashMap *) malloc(sizeof(HashMap));
	assert(hm);

	// hm->buckets = calloc(key_size, sizeof(Bucket));
	hm->buckets = calloc(key_size, sizeof(Bucket *));
	assert(hm->buckets);

	hm->size  = key_size; 

	return hm;
}

void insert_data(HashMap * hm, char * key, void * data, ResolveCollisionCallback resolve_collision) {
	if (!hm || !key || !data) {
		return;
	}
	int hash_res = hash(hm, key);

	Bucket* curr_bucket = hm->buckets[hash_res];
	if (!curr_bucket) {
		Bucket* new_bucket = (Bucket*) malloc(sizeof(Bucket));
		assert(new_bucket);
		new_bucket->data = data;
		new_bucket->key = strdup(key);
		new_bucket->next = NULL;
		hm->buckets[hash_res] = new_bucket;
	} else {
		Bucket* prev_bucket = NULL;
		while (curr_bucket) {
			// Key conflict.
			if (!strcmp(key, curr_bucket->key)) {
				curr_bucket->data = resolve_collision(curr_bucket->data, data);
				return;
			}
			prev_bucket = curr_bucket;
			curr_bucket = curr_bucket->next;
		}
		Bucket* new_bucket = (Bucket*) malloc(sizeof(Bucket));
		assert(new_bucket);
		new_bucket->data = data;
		new_bucket->key = strdup(key);
		new_bucket->next = NULL;
		prev_bucket->next = new_bucket;
	}
}

void * get_data(HashMap * hm, char * key) {
	if (!hm || !key) {
		return NULL;
	}
	int hash_res = hash(hm, key);
	// Loop through buckets at hash.
	Bucket* curr_bucket = hm->buckets[hash_res];
	while (curr_bucket) {
		if (!strcmp(key, curr_bucket->key)) {
			return curr_bucket->data;
		}
		curr_bucket = curr_bucket->next;
	}
	return NULL;
}

void iterate(HashMap * hm, void (* callback)(char * key, void * data)) {
	if (!hm) {
		return;
	}
	size_t i;
	for (i = 0; i < hm->size; i++) {
		Bucket* curr_bucket = hm->buckets[i];
		while (curr_bucket) {
			callback(curr_bucket->key, curr_bucket->data);
			curr_bucket = curr_bucket->next;
		}
	}
}

void remove_data(HashMap * hm, char * key, DestroyDataCallback destroy_data) {
	if (!hm || !key) {
		return;
	}
	int hash_res = hash(hm, key);
	// Loop through buckets at hash.
	Bucket* curr_bucket = hm->buckets[hash_res];
	Bucket* prev_bucket = NULL;
	while (curr_bucket) {
		if (!strcmp(key, curr_bucket->key)) {
			if (destroy_data) {
				destroy_data(curr_bucket->data);
			}
			if (prev_bucket) {
				prev_bucket->next = curr_bucket->next;
			} else {
				hm->buckets[hash_res] = curr_bucket->next;
			}
			break;
		}
		prev_bucket = curr_bucket;
		curr_bucket = curr_bucket->next;
	}
	if (curr_bucket) {
		free(curr_bucket->key);
	}
	free(curr_bucket);
}

void delete_hashmap(HashMap * hm, DestroyDataCallback destroy_data) {
	if (!hm) {
		return;
	}
	size_t i;
	Bucket* prev_bucket;
	Bucket* curr_bucket;
	for (i = 0; i < hm->size; i++) {
		curr_bucket = hm->buckets[i];
		while (curr_bucket) {
			if (destroy_data) {
				destroy_data(curr_bucket->data);
			}
			prev_bucket = curr_bucket;
			curr_bucket = curr_bucket->next;
			if (prev_bucket) {
				free(prev_bucket->key);
			}
			free(prev_bucket);
		}
	}
	free(hm);
}