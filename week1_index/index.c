#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>

#include "tree.h"

int main(void)
{

    char buff[500], temp[50];
    int i,len, count=0;
    data h;
    tree cay=NULL, sw=NULL;
    FILE *fp,*fs;
    char c;
    char *p;
    fs=fopen("stopw.txt","r");
    while(1){
        fscanf(fs,"%s",buff);
        strcpy(h.word,buff);
        h.count=0;
        insertnode(&sw,h);
        if(feof(fs)) break;
    }
    fp=fopen("vanban.txt","r");
    while(1){
        fgets(buff,500,fp);
        count++;
        if(feof(fp)) break;
        len= strlen(buff);
        buff[len-1]='\0';
        p=strtok (buff," ");
        if(checkValidString(p)==1){
        formatString(p);
        strcpy(h.word,p);
        strcpy(temp,p);
        h.count=1;
        sprintf(h.line,",%d ",count);
        if(search(sw,h.word)==NULL) insertnode(&cay,h);
        }
        p=strtok(NULL," \t");
        while (p != NULL){
            if('A'<=p[0] && p[0]<='Z'){
                if(temp[strlen(temp)-1]=='.' || temp[strlen(temp)-1]=='!' || temp[strlen(temp)-1]=='?'){
                    if(checkValidString(p)==1){
                    formatString(p);
                    strcpy(h.word,p);
                    h.count=1;
                    sprintf(h.line,",%d ",count);
                    if(search(sw,h.word)==NULL) insertnode(&cay,h);
                    }
                }
            }
            else{
                if(checkValidString(p)==1){
                formatString(p);
                strcpy(h.word,p);
                h.count=1;
                sprintf(h.line,",%d ",count);
                if(search(sw,h.word)==NULL) insertnode(&cay,h);
                }
            }
            strcpy(temp,p);
            p = strtok(NULL," \t");

        }
    }
    fclose(fp);
    fclose(fs);
    printTree(cay);
    free(sw);
    free(cay);
    printf("\n");
    return 0;
}

