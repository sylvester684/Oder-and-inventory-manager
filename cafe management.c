#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Item {
    char name[50];
    char customer[50];
    int customerId;
    int quantity;
    float price;
    struct Item *left;
    struct Item *right;
} Item;

void printInventory(Item *root);
void inOrderTraversalWithTotalPrice(Item *root);
void printMenu();
Item *createItem(char *name, char *customer, int customerId, int quantity, float price);
Item *insertItem(Item *root, Item *newItem);
Item *addItem(Item *root, char *name, char *customer, int *nextCustomerId, int quantity);
Item *findMinimum(Item *root);
Item *removeItem(Item *root, char *customer);
void saveInventoryToFile(Item *root, const char *filename);
void saveInOrderTraversal(Item *root, FILE *file);
Item *loadInventoryFromFile(const char *filename);
void freeMemory(Item *root);
void cleanupAndExit(Item *root);

void printInventory(Item *root) {
    printf("     Current inventory     :\n");
    printf("%-20s %-20s %-20s %-20s %-20s %-20s\n", "Customer ID", "Customer", "Item Name", "Quantity", "Price", "Total Price");
    inOrderTraversalWithTotalPrice(root);
    printf("\n");
}

void inOrderTraversalWithTotalPrice(Item *root) {
    if (root != NULL) {
        inOrderTraversalWithTotalPrice(root->left);
        float totalPrice = root->quantity * root->price;
        printf("%-20d %-20s %-20s %-20d %-20.2f %-20.2f\n", root->customerId, root->customer, root->name, root->quantity, root->price, totalPrice);
        inOrderTraversalWithTotalPrice(root->right);
    }
}

void printMenu() {
    printf("     Menu     :\n");
    printf("%-20s %-20s\n", "Item Name", "Price");
    printf("%-20s %-20.1f\n", "Tea", 10.0);
    printf("%-20s %-20.1f\n", "Coffee", 15.0);
    printf("%-20s %-20.1f\n", "Sandwich", 25.0);
    printf("%-20s %-20.1f\n", "Burger", 30.0);
    printf("%-20s %-20.1f\n", "Pizza", 40.0);
    printf("%-20s %-20.1f\n", "Pepsi", 20.0);
    printf("\n");
}

Item *createItem(char *name, char *customer, int customerId, int quantity, float price) {
    Item *newItem = (Item *)malloc(sizeof(Item));
    strncpy(newItem->name, name, 50);
    strncpy(newItem->customer, customer, 50);
    newItem->customerId = customerId;
    newItem->quantity = quantity;
    newItem->price = price;
    newItem->left = NULL;
    newItem->right = NULL;
    return newItem;
}

Item *insertItem(Item *root, Item *newItem) {
    if (root == NULL) {
        return newItem;
    }

    if (newItem->customerId < root->customerId) {
        root->left = insertItem(root->left, newItem);
    } else {
        root->right = insertItem(root->right, newItem);
    }

    return root;
}

Item *addItem(Item *root, char *name, char *customer, int *nextCustomerId, int quantity) {
    float price = 0.0;

    if (strcmp(name, "Tea") == 0) {
        price = 10.0;
    } else if (strcmp(name, "Coffee") == 0) {
        price = 15.0;
    } else if (strcmp(name, "Sandwich") == 0) {
        price = 25.0;
    } else if (strcmp(name, "Burger") == 0) {
        price = 30.0;
    } else if (strcmp(name, "Pizza") == 0) {
        price = 40.0;
    } else if (strcmp(name, "Pepsi") == 0) {
        price = 20.0;
    }

    Item *newItem = createItem(name, customer, (*nextCustomerId)++, quantity, price);
    return insertItem(root, newItem);
}

Item *findMinimum(Item *root) {
    if (root == NULL)
        return NULL;
    else if (root->left == NULL)
        return root;
    else
        return findMinimum(root->left);
}

Item *removeItem(Item *root, char *customer) {
    if (root == NULL)
        return NULL;

    if (strcmp(customer, root->customer) < 0)
        root->left = removeItem(root->left, customer);
    else if (strcmp(customer, root->customer) > 0)
        root->right = removeItem(root->right, customer);
    else {
        if (root->left == NULL && root->right == NULL) {
            free(root);
            return NULL;
        } else if (root->left == NULL) {
            Item *temp = root->right;
            free(root);
            return temp;
        } else if (root->right == NULL) {
            Item *temp = root->left;
            free(root);
            return temp;
        } else {
            Item *minRight = findMinimum(root->right);
            strcpy(root->name, minRight->name);
            strcpy(root->customer, minRight->customer);
            root->customerId = minRight->customerId;
            root->quantity = minRight->quantity;
            root->price = minRight->price;
            root->right = removeItem(root->right, minRight->customer);
        }
    }

    return root;
}

void saveInventoryToFile(Item *root, const char *filename) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        printf("Error opening file for writing.\n");
        return;
    }
    saveInOrderTraversal(root, file);
    fclose(file);
    printf("Inventory data saved to file: %s\n", filename);
}

void saveInOrderTraversal(Item *root, FILE *file) {
    if (root != NULL) {
        saveInOrderTraversal(root->left, file);
        fprintf(file, "%d;%s;%s;%d;%.2f\n", root->customerId, root->customer, root->name, root->quantity, root->price);
        saveInOrderTraversal(root->right, file);
    }
}

Item *loadInventoryFromFile(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening file for reading.\n");
        return NULL;
    }

    Item *root = NULL;
    char line[256];

    while (fgets(line, sizeof(line), file) != NULL) {
        Item *newItem = (Item *)malloc(sizeof(Item));
        sscanf(line, "%d;%[^;];%[^;];%d;%f", &newItem->customerId, newItem->customer, newItem->name, &newItem->quantity, &newItem->price);
        newItem->left = NULL;
        newItem->right = NULL;
        root = insertItem(root, newItem);
    }

    fclose(file);
    printf("Inventory data loaded from file: %s\n", filename);
    return root;
}

void freeMemory(Item *root) {
    if (root != NULL) {
        freeMemory(root->left);
        freeMemory(root->right);
        free(root);
    }
}

void cleanupAndExit(Item *root) {
    freeMemory(root);
    exit(0);
}

int main() {
    Item *root = NULL;
    int choice;
    char name[50];
    char customer[50];
    int nextCustomerId = 1;

    do {
        printf("===========================================\n");
        printf("              Cafe Management System             \n");
        printf("===========================================\n");
        printf("1. Print current inventory\n");
        printf("2. Add item to inventory\n");
        printf("3. Remove item from inventory\n");
        printf("4. Save inventory to file\n");
        printf("5. Load inventory from file\n");
        printf("6. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                printInventory(root);
                break;

            case 2:
                printMenu();
                printf("Enter the item name to add: ");
                scanf("%s", name);
                printf("Enter the customer name: ");
                scanf("%s", customer);
                printf("Enter the quantity: ");
                int quantity;
                scanf("%d", &quantity);
                root = addItem(root, name, customer, &nextCustomerId, quantity);
                break;

            case 3:
                printf("Enter the customer name: ");
                scanf("%s", customer);
                root = removeItem(root, customer);
                break;

            case 4:
                saveInventoryToFile(root, "inventory.txt");
                break;

            case 5:
                root = loadInventoryFromFile("inventory.txt");
                break;

            case 6:
                printf("Exiting...\n");
                cleanupAndExit(root);
                break;

            default:
                printf("Invalid choice. Please try again.\n");
                break;
        }

        printf("\n");
    } while (choice != 6);

    cleanupAndExit(root);
    return 0;
}