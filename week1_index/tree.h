#include <stdlib.h>

typedef struct
{
    char word[20];
    int count;
    char line[100];
}data;

typedef struct nodetype
{
    data key;
    struct nodetype *right,*left;
}node;

typedef struct nodetype *tree;
void insertnode(tree *root,data x)
{
    if((*root)==NULL)
    {
        *root=(node*)malloc(sizeof(node));
        (*root)->key=x;
        (*root)->right=NULL;
        (*root)->left=NULL;
    }
    else if(strcmp(x.word,((*root)->key).word)>0) insertnode(&(*root)->right,x);
    else if(strcmp(x.word,((*root)->key).word)<0) insertnode(&(*root)->left,x);
    else{
        ((*root)->key).count++;
        if(strstr(((*root)->key).line,x.line)==NULL) strcat(((*root)->key).line,x.line);
    }
}

int checkValidString(char s[])
{
    int i=0;
    while(i<strlen(s)){
        if(isdigit(s[i])) return 0;
        else i++;
    }
    return 1;
}

void formatString(char *str)
{
    int i,k,j;
    k=strlen(str);
    for(j=0;j<strlen(str);j++)
        {
            if('A'<=str[j]&&str[j]<='Z') str[j]=str[j]+'a'-'A';
        }
    while(!isalpha(str[0]))
        {
            for(i=0;i<k-1;i++) str[i]=str[i+1];
            str[k-1]='\0';
            k=strlen(str);
        }
    while(!isalpha(str[k-1]))
        {
            str[k-1]='\0';
            k=strlen(str);
        }

}

tree search(tree root,char *x)
{
    if(root==NULL) return NULL;
    else if(strcmp(x,(root->key).word)>0) search(root->right,x);
    else if(strcmp(x,(root->key).word)<0) search(root->left,x);
    else return root;
}

void printTree(tree root)
{
    if (root!=NULL)
    {
        printTree(root->left);
        printf("\n%-20s%-2i%s",(root->key).word,(root->key).count,(root->key).line);
        printTree(root->right);
    }
}