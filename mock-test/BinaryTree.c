#include <stdlib.h>
#include <stdio.h>

#define MAX_Q_SIZE (500)

typedef struct node_s
{
    int data;
    struct node_s * left;
    struct node_s * right;
}node;

static int height(node * root)
{
    if(!root)
        return 0;
    else
    {
        printf("root : %d\n", root->data);
        int lheight = height(root->left);
        int rheight = height(root->right);

        if(lheight > rheight)
            return (lheight + 1);
        else return (rheight + 1);
    }
}

static void printGivenLevel(node * root, int level)
{
    if(!root)
        return;

    if(level == 1)
    {
        printf("%d ", root->data);
    }
    else if(level > 1)
    {
        printGivenLevel(root->left, (level-1));
        printGivenLevel(root->right, (level-1));
    }
};

static void printLevelOrder(node * root)
{
    int h = height(root);
    int i = 0;

    for(i = 1; i <= h; i++)
    {
        printGivenLevel(root, i);
        printf("\n");
    }
};

static node * newNode(int data)
{
    node * new = (node *)malloc(sizeof(*new));
    new->data = data;
    new->left = NULL;
    new->right = NULL;

    return new;
}

static void cleanTree(node * root)
{
    if(!root)
        return;

    cleanTree(root->left);
    cleanTree(root->right);

    free(root);
}

/* UTILITY FUNCTIONS */
node ** createQueue(int * front, int * rear)
{
    node ** queue= (node **)malloc(sizeof(*queue) * MAX_Q_SIZE);
    *front = *rear = 0;
    return queue;
}

void enQueue(node ** queue, int * rear, node * new_node)
{
    queue[*rear] = new_node;
    (*rear)++;
}

node * deQueue(node ** queue, int * front)
{
    (*front)++;
    return queue[*front - 1];
}

//Left -> Root -> Right
static void printInOrder(node * root)
{
    if(!root)
        return;
    else
    {
        printInOrder(root->left);
        printf("%d ", root->data);
        printInOrder(root->right);
    }
}

// Root -> Left -> Right
static void printPreOrder(node * root)
{
    if(!root)
        return;
    else
    {
        printf("%d ", root->data);
        printPreOrder(root->left);
        printPreOrder(root->right);
    }
}

// Left -> Right -> Root
static void printPostOrder(node * root)
{
    if(!root)
        return;
    else
    {
        printPostOrder(root->left);
        printPostOrder(root->right);
        printf("%d ", root->data);
    }
}

int main(int argc, char * argv[])
{
    (void)argc;
    (void)argv;

    node * root         = newNode(1);
    root->left          = newNode(2);
    root->right         = newNode(3);
    root->left->left    = newNode(4);
    root->left->right   = newNode(5);

    printLevelOrder(root); printf("\n");
    printInOrder(root); printf("\n");
    printPreOrder(root); printf("\n");
    printPostOrder(root); printf("\n");

    cleanTree(root);

    return 0;
}
