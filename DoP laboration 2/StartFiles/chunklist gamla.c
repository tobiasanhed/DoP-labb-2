/*
* File: pqlist.c
* ------------------
*
* Niclas �strand, Data- och programstrukturer HT-2003
* Senast modifierad: 23/7 - 2003
*/

#include "pqueue.h"
#include "genlib.h"

#define MAX_ELEMS_PER_BLOCK 20

/* Types */

typedef struct chunkT {
	int values[MAX_ELEMS_PER_BLOCK];
	int elementsInValues;
	int minValue;
	int maxValue;
	struct chunkT *next;
} chunkT;

struct pqueueCDT {
	chunkT *head;
};

/* Exported entries */

static void insertElement(chunkT *chunk, int newValue){

	for (int i = chunk->elementsInValues; i >= 0; i--){
		if (newValue > chunk->values[i]){
			chunk->values[i + 1] = chunk->values[i];
		}
		else{
			chunk->values[i + 1] = newValue;
			break;
		}
	}
	if (chunk->elementsInValues == 0)
		chunk->values[0] = newValue;
	chunk->elementsInValues++;
	chunk->maxValue = chunk->values[0];
	chunk->minValue = chunk->values[chunk->elementsInValues - 1];
	

}

static void splitChunk(chunkT *current){
	chunkT *newChunk;

	newChunk = malloc(sizeof(chunkT));

	for (int i = MAX_ELEMS_PER_BLOCK / 2; i = MAX_ELEMS_PER_BLOCK; i++){
		newChunk->values[i - MAX_ELEMS_PER_BLOCK / 2] = current->values[i];
		current->elementsInValues--;
		newChunk->elementsInValues++;
	}
	
	newChunk->maxValue = newChunk->values[0];
	newChunk->minValue = newChunk->values[newChunk->elementsInValues - 1];
	current->minValue = current->values[current->elementsInValues - 1];
	newChunk->next = current->next;
	current->next = newChunk;
}

pqueueADT NewPQueue(void)
{
	pqueueADT pqueue;
	chunkT *chunk;

	chunk = malloc(sizeof(chunkT));
	pqueue = New(pqueueADT);
	pqueue->head = chunk;

	chunk->elementsInValues = 0;
	chunk->maxValue = 0;
	chunk->minValue = 0;
	chunk->next = NULL;
	return (pqueue);
}

void FreePQueue(pqueueADT pqueue)
{
	chunkT *next;

	while (pqueue->head != NULL) {
		next = pqueue->head->next;
		FreeBlock(pqueue->head);
		pqueue->head = next;
	}
	FreeBlock(pqueue);
}

bool IsEmpty(pqueueADT pqueue)
{
	return (pqueue->head == NULL);
}

bool IsFull(pqueueADT pqueue){
	return (FALSE);
}

/* Implementation notes: Enqueue
* -----------------------------
* F�r att finna r�tt position f�r ins�ttning m�ste en s�kning utf�ras.
* S�kningar i enkell�nkade listor kan vara 'kn�ligt'. I den h�r
* implementeringen anv�nds tv� 'paralella pekare, en f�ljer ett steg
* bakom den andra, till dess att vi finner korrekt position f�r ins�ttning
* av den nya noden. Notera specialfallet vid ins�ttning vid listans
* huvud.
*/

void Enqueue(pqueueADT pqueue, int newValue)
{
	chunkT *cur;

	cur = pqueue->head;

	while (newValue < cur->minValue){
		cur = cur->next;
	}

	if (cur->elementsInValues >= MAX_ELEMS_PER_BLOCK)
		splitChunk(cur);

	if (newValue > cur->minValue)
		insertElement(cur, newValue);
	else
		insertElement(cur->next, newValue);



	
}

/* Implementation notes: DequeueMax
* --------------------------------
* Det st�rsta v�rdet sparas f�rst i listan s� att det �r
* enkelt att ta bort. Notera att minne f�r noder frig�rs
* vid borttagning ur k�n.
*/

int DequeueMax(pqueueADT pqueue)
{
	int value;
	chunkT *toBeDeleted;

	if (IsEmpty(pqueue))
		Error("Tried to dequeue max from an empty pqueue!");

	value = pqueue->head->values[0];

	for (int i = 0; i < pqueue->head->elementsInValues; i++){
		pqueue->head->values[i] = pqueue->head->values[i + 1];
	}

	pqueue->head->elementsInValues--;
	if (pqueue->head->elementsInValues == 0)	{
		toBeDeleted = pqueue->head;
		if (pqueue->head->next != NULL){
			pqueue->head = pqueue->head->next;
			FreeBlock(toBeDeleted);
		}
	}
	return (value);
}

/* Implementation notes: BytesUsed
* -------------------------------
* Minnes f�rbrukningen utg�rs av minnet f�r en struct pqueueCDT +
* storleken p� summan av samtliga noder i den l�nkade listan.
*/

int BytesUsed(pqueueADT pqueue)
{
	int total;
	chunkT *cur;

	total = sizeof(*pqueue);
	for (cur = pqueue->head; cur != NULL; cur = cur->next)
		total += sizeof(*cur);

	return (total);
}
