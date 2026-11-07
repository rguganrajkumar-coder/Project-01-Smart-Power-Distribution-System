/*
 * TNEB Smart Power Distribution System
 * ------------------------------------
 * A console-based smart grid manager written in C.
 *
 * Data structures used:
 *   - Singly linked list : stores registered consumers
 *   - Binary search tree : fast consumer lookup by ID
 *   - Array-based queue  : fault service queue (FIFO)
 *
 * Build : gcc smart_power_distribution.c -o smart_grid
 * Run   : ./smart_grid
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_FAULTS   10      /* size of the fault queue          */
#define RATE_PER_UNIT 5      /* billing rate in rupees per unit  */
#define LINE "----------------------------------------------------\n"
#define DOUBLE_LINE "====================================================\n"

/* ================= DATA STRUCTURES ================= */

/* Linked list node: one registered consumer */
struct Consumer {
    int   id;
    char  name[50];
    char  location[50];
    float units;
    struct Consumer *next;
};

/* Binary search tree node: consumer IDs only */
struct BST {
    int id;
    struct BST *left;
    struct BST *right;
};

/* ================= GLOBAL STATE ================= */

struct Consumer *head = NULL;   /* start of the consumer list */
struct BST      *root = NULL;   /* root of the ID search tree */

int faultQueue[MAX_FAULTS];     /* fault IDs waiting for service */
int front = -1;
int rear  = -1;

/* ================= BST FUNCTIONS ================= */

/* Insert an ID into the tree and return the (possibly new) root. */
struct BST *insertBST(struct BST *node, int id)
{
    if (node == NULL) {
        struct BST *newNode = malloc(sizeof(struct BST));
        if (newNode == NULL) {
            printf("\n❌ Memory allocation failed!\n");
            return NULL;
        }
        newNode->id    = id;
        newNode->left  = NULL;
        newNode->right = NULL;
        return newNode;
    }

    if (id < node->id)
        node->left = insertBST(node->left, id);
    else
        node->right = insertBST(node->right, id);

    return node;
}

/* Search the tree for an ID and report the result. */
void searchBST(struct BST *node, int id)
{
    if (node == NULL) {
        printf("\n❌ Consumer Not Found!\n");
        return;
    }

    if (id == node->id) {
        printf("\n✔ Consumer Found in Network\n");
        printf("Status: ACTIVE | Supply: NORMAL\n");
        return;
    }

    if (id < node->id)
        searchBST(node->left, id);
    else
        searchBST(node->right, id);
}

/* Free every node in the tree (post-order). */
void freeBST(struct BST *node)
{
    if (node == NULL)
        return;
    freeBST(node->left);
    freeBST(node->right);
    free(node);
}

/* ================= CONSUMER MANAGEMENT ================= */

/* Register a new consumer at the head of the linked list. */
void addConsumer(void)
{
    struct Consumer *newNode = malloc(sizeof(struct Consumer));
    if (newNode == NULL) {
        printf("\n❌ Memory allocation failed!\n");
        return;
    }

    printf("\n➤ Registering New Consumer...\n\n");

    printf("Consumer ID      : ");
    scanf("%d", &newNode->id);

    printf("Name             : ");
    scanf("%49s", newNode->name);          /* %49s prevents buffer overflow */

    printf("Location         : ");
    scanf("%49s", newNode->location);

    printf("Energy Usage     : ");
    scanf("%f", &newNode->units);

    /* Insert at head of the list and record the ID in the BST */
    newNode->next = head;
    head = newNode;
    root = insertBST(root, newNode->id);

    printf("\n✔ Consumer Successfully Integrated into Grid\n");
}

/* Print every registered consumer. */
void displayConsumers(void)
{
    struct Consumer *temp = head;

    printf("\n[ACTIVE CONSUMERS]\n");
    printf(LINE);

    while (temp != NULL) {
        printf("ID: %d | Name: %-6s | Area: %-10s | Load: %.0f Units\n",
               temp->id, temp->name, temp->location, temp->units);
        temp = temp->next;
    }

    printf(LINE);
}

/* Free every node in the consumer list. */
void freeConsumers(void)
{
    struct Consumer *temp;

    while (head != NULL) {
        temp = head;
        head = head->next;
        free(temp);
    }
}

/* ================= LOAD BALANCING ================= */

/* Show total and average load across all consumers. */
void loadBalancing(void)
{
    struct Consumer *temp = head;
    float total = 0;
    int   count = 0;

    while (temp != NULL) {
        total += temp->units;
        count++;
        temp = temp->next;
    }

    printf("\n[LOAD DISTRIBUTION MONITOR]\n");
    printf(LINE);

    if (count == 0) {
        printf("No Data Available\n");
        return;
    }

    printf("Total Consumers      : %d\n", count);
    printf("Total Load Demand    : %.0f Units\n", total);
    printf("Average Load         : %.0f Units\n", total / count);

    printf("\n✔ Load Balanced Across Substations\n");
    printf("✔ No Overload Detected\n");
    printf(LINE);
}

/* ================= FAULT MANAGEMENT ================= */

/* Add a fault ID to the back of the queue. */
void enqueue(int id)
{
    if (rear == MAX_FAULTS - 1) {
        printf("Queue Full!\n");
        return;
    }

    if (front == -1)
        front = 0;

    faultQueue[++rear] = id;
    printf("\n⚠️ Fault Logged Successfully → Sent to Service Queue\n");
}

/* Resolve the fault at the front of the queue. */
void dequeue(void)
{
    if (front == -1 || front > rear) {
        printf("No Faults!\n");
        return;
    }

    printf("\n🔧 Resolving Fault ID: %d...\n", faultQueue[front++]);
    printf("✔ Power Restored Successfully\n");
}

/* ================= BILLING ================= */

/* Print a bill for every consumer (units x rate). */
void billing(void)
{
    struct Consumer *temp = head;

    printf("\n[ENERGY BILLING REPORT]\n");
    printf(LINE);

    while (temp != NULL) {
        printf("ID: %d | %-6s | Units: %.0f | Bill: ₹%.0f\n",
               temp->id, temp->name, temp->units, temp->units * RATE_PER_UNIT);
        temp = temp->next;
    }

    printf("\n✔ Reports Generated Successfully\n");
    printf(LINE);
}

/* ================= POWER STATION ================= */

void stationDashboard(void)
{
    printf("\n[POWER STATION DASHBOARD]\n");
    printf(LINE);
    printf("Station Name     : Chennai Main Grid\n");
    printf("Substations      : 3 Active\n");
    printf("Current Load     : Stable\n");
    printf("\n✔ Power Distribution Running Normally\n");
    printf(LINE);
}

/* ================= MENUS ================= */

void faultControlCenter(void)
{
    int option, id;

    printf("\n[FAULT CONTROL CENTER]\n");
    printf("1. Report Fault\n");
    printf("2. Resolve Fault\n");
    printf("Enter Option: ");
    scanf("%d", &option);

    if (option == 1) {
        printf("\nFault ID        : ");
        scanf("%d", &id);
        printf("Fault Location  : Line\n");
        enqueue(id);
    } else if (option == 2) {
        dequeue();
    } else {
        printf("Invalid Option!\n");
    }
}

void printHeader(void)
{
    printf(DOUBLE_LINE);
    printf("   ⚡ TNEB SMART POWER DISTRIBUTION SYSTEM ⚡\n");
    printf(DOUBLE_LINE);
    printf("Status        : ONLINE\n");
    printf("Region        : Tamil Nadu Grid\n");
    printf("Control Mode  : Automatic\n");
    printf(DOUBLE_LINE);
}

void printMenu(void)
{
    printf("\nMAIN MENU\n");
    printf(LINE);
    printf("1. 👤 Consumer Control Panel\n");
    printf("2. 🏭 Power Station Dashboard\n");
    printf("3. ⚖️ Load Distribution Monitor\n");
    printf("4. 🚨 Fault Control Center\n");
    printf("5. 💰 Billing & Energy Report\n");
    printf("6. 🔍 Consumer Lookup\n");
    printf("7. ❌ Shutdown System\n");
    printf(LINE);
    printf("Enter Command: ");
}

/* ================= MAIN ================= */

int main(void)
{
    int choice, id;

    printHeader();

    while (1) {
        printMenu();

        /* Stop cleanly if input is not a number (or input ends) */
        if (scanf("%d", &choice) != 1) {
            printf("Invalid Input! Shutting down.\n");
            break;
        }

        switch (choice) {
            case 1:
                addConsumer();
                displayConsumers();
                break;

            case 2:
                stationDashboard();
                break;

            case 3:
                loadBalancing();
                break;

            case 4:
                faultControlCenter();
                break;

            case 5:
                billing();
                break;

            case 6:
                printf("\n[CONSUMER SEARCH]\n");
                printf("Enter Consumer ID: ");
                scanf("%d", &id);
                searchBST(root, id);
                break;

            case 7:
                printf("\n⚡ System Shutting Down...\n");
                printf("✔ Thank You for Using Smart Grid System\n");
                freeConsumers();
                freeBST(root);
                return 0;

            default:
                printf("Invalid Command!\n");
        }
    }

    freeConsumers();
    freeBST(root);
    return 0;
}
