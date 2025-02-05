#include "streaming_service.h"
#include <stdio.h>
#include <stdlib.h>

void printCategories(struct new_movie* head){
    struct new_movie* current = head;
    while (current != NULL) {
        printf("%d,",current->info.mid);
        current = current->next;
    }
}

void printR(int uid){
    struct user *ptr=NULL;
    ptr=User->next;
    printf("R %d\n  Users= %d",uid,ptr->uid);
    ptr=ptr->next;
    while (ptr!=NULL){
        printf(",%d",ptr->uid);
        ptr=ptr->next;
    }
    printf("\nDONE\n");
}

void printAddNewMovie(unsigned mid, movieCategory_t category, unsigned year){
    struct new_movie * ptr=NULL;
    ptr=New_movie;
    printf("A %u %d %d\n  New movies=",mid,category,year);   
    while (ptr!=NULL){
        printf("%u %d %u,",ptr->info.mid,ptr->category,ptr->info.year);
        ptr=ptr->next;
    }
    printf("\nDONE\n");
};

int register_user(int uid){
    struct user * current=NULL;
    struct user * newUser=(struct user*)malloc(sizeof(struct user));
    
    /*create new user*/
    newUser->next=NULL;
    newUser->uid=uid;
    newUser->suggestedHead=NULL;
    newUser->suggestedTail=NULL;
    newUser->watchHistory=NULL;

    current=User;

    /*if list is empty*/
    if(current==NULL){
        User->next=newUser;
        User=newUser;
        return 0;
    }

    /*search for duplicate*/
    while(current->next!=NULL){
        if(User->uid==uid){
            free(newUser);
            printf("R\n   Duplicate found with uid=%d\nDONE\n",uid);
            return -1;
        }else if(current->next->uid==uid){
            free(newUser);
            printf("R\n  Duplicate found with uid=%d\nDONE\n",uid);
            return -1;
        }
           current=current->next;
    }

    current->next=newUser;
    printR(uid);
    return 0;
};

void unregister_user(int uid){
    struct user * current;
    struct user *tmp;

    /*start from the first user*/
    current=User->next;

    /*if user is found unregister him from the list*/
    if(current->uid==uid){
        tmp=current;
        current=current->next;
        User=User->next;
        free(tmp);
    }

    printf("U  %d\n   Users=",uid);
    while (current->next!=NULL){
        if (current->next->uid==uid){
            tmp=current->next;
            current->next=current->next->next;
            free(tmp);
        }
        current=current->next;
    }

    /*print the new list*/
    struct user* printU=User->next;
    while(printU!=NULL){
        printf("%d,",printU->uid);
        printU=printU->next;
    }
    printf("\nDONE\n");
};

int add_new_movie(unsigned mid, movieCategory_t category, unsigned year){
    struct new_movie *newnode=(struct new_movie*)malloc(sizeof(struct new_movie));
    struct new_movie *current=NULL;
    struct new_movie *prev=NULL;
    current=New_movie;
    prev=New_movie;

    /*create a newnode*/
    newnode->category=category;
    newnode->info.mid=mid;
    newnode->info.year=year;
    newnode->next=NULL;

    /*insert at the head*/
    if (New_movie==NULL){
        newnode->next=New_movie;
        New_movie=newnode;
        printAddNewMovie(mid,category,year);
        return 0;
    }


    /*check if mid is unique*/
    while (current->next!=NULL){
        if(New_movie->info.mid==mid){
            free(newnode);
            printf("Duplicate found with uid=%u\n",mid);
            return -1;
        }else if(current->next->info.mid==mid){
            free(newnode);
            printf("Duplicate found with uid=%u\n",mid);
            return -1;
        }
        current=current->next;
    }

    /*add movie in the start */
    if (New_movie->info.mid>mid){
        newnode->next=New_movie;
        New_movie=newnode;
        printAddNewMovie(mid,category,year);
        return 0;
    }

    /*add movie inside the list */
    prev=NULL;
    current=New_movie;
    int k=0;
    while (k==0){
        if(current->info.mid>mid){
            prev->next=newnode;
            newnode->next=current;
            printAddNewMovie(mid,category,year);
            return 0;
        }
        if(current->next!=NULL){
            prev=current;
            current=current->next;
        }else{
            k=1;
        }
    }



    /* add movie at the end */
    current->next=newnode;
    newnode->next=NULL;
    printAddNewMovie(mid, category, year);
    return 0;
};

void distribute_new_movies(void){
    struct new_movie * current=New_movie;
    int i;

    /*initialize category lists
    lastUsed[] is used to save the last used node of
    each category in order to achieve the time complexity of O(n)*/
    for (i = 0; i < 6; i++){
		Categories[i]=NULL;
		lastUsed[i]=NULL;
	}

    /*insert each node accordingly*/
    while (current!= NULL) {
        int category = current->category;

        if (Categories[category] == NULL) {
            Categories[category] = current;
            lastUsed[category] = current;
            current=current->next;
            Categories[category]->next = NULL;
            lastUsed[category]->next = NULL;     
        } else {
            lastUsed[category]->next = current;
            current=current->next;
            lastUsed[category]->next->next=NULL;
            lastUsed[category]=lastUsed[category]->next;
        }

    }

    /*Print lists for each category (0 to 5)*/
    printf("D\nCategorized Movies:\n"); 
    for (i = 0; i < 6; i++) {
        if(i==0){printf("    Horror:");}
        if(i==1){printf("    Sci-fi:");}
        if(i==2){printf("    Drama:");}
        if(i==3){printf("    Romance:");}
        if(i==4){printf("    Documentary:");}
        if(i==5){printf("    Comedy:");}

        if (Categories[i]!=NULL){
                printCategories(Categories[i]);
        }else{
            printf("Empty");
        }
        printf("\n");
    }
    printf("DONE\n");

};

int watch_movie(int uid, unsigned mid) {

    /*create a new node*/
    struct movie * movieC=(struct movie*)malloc(sizeof(struct movie));
    movieC->info.mid = mid;
    movieC->next = NULL;

    /*find the right user*/
    struct user *currentUser = User;
    while (currentUser != NULL && currentUser->uid != uid) {
        currentUser = currentUser->next;
    }

    /*case user is not found*/
    if (currentUser == NULL) {
        printf("User %d not found\n",uid);
        free(movieC);           
        return -1;
    }

    /*update watchHitory of each user accordingly*/
    if (currentUser->watchHistory == NULL) {
        currentUser->watchHistory = movieC;
    } else {
        movieC->next = currentUser->watchHistory;
        currentUser->watchHistory = movieC;
    }

    
    /*print watchHistory*/
    struct movie* currentMovie = currentUser->watchHistory;
    printf("W %d %d\n", currentUser->uid, movieC->info.mid);
    printf("    User %d Watch History=", currentUser->uid);
    while (currentMovie != NULL) {
        printf("%d,", currentMovie->info.mid);
        currentMovie = currentMovie->next;
    }
    printf("\nDONE\n");

    return 0;
}

int suggest_movies(int uid){


    struct user *currentUser = User->next;
    struct user* sugUser=User->next;
    struct user * headUser=(struct user*)malloc(sizeof(struct user));
    int found=0;
    int mergeStatus=0;
    int addWhere=0;

    /*case F event did not happen before S*/
    if(mergedListHead==NULL){
        printf("merged list empty\n");
        mergeStatus=0;
    }
    /*case F event did happen but has only 1 node*/
    else if(mergedListHead==mergedListTail && addFilterListUser==uid){
        printf("merged list has 1 element\n");
        mergeStatus=1;
    }
    /*case F event did happen but has more than one node*/
    else if(mergedListHead!=mergedListTail && addFilterListUser==uid){
        printf("merged list more than 1 node\n");
        mergeStatus=2;
    }


    /*find user to suggest*/
    while(found==0 && sugUser!=NULL){
        if(sugUser->uid==uid){
            addFilterListUser=sugUser->uid;
            found=1;
            break;
        }else{
            sugUser=sugUser->next;
        }
    }
    if (found==0){
        printf("User not found in order to suggest");
        return -1;
    }
    /*case we dont have any event F before S*/
    if(mergedListHead==NULL || mergeStatus==0){
        while (currentUser != NULL) {
            if (currentUser->uid != uid) {
                printf("user: %d ", currentUser->uid);
                if (currentUser->watchHistory!= NULL) {
                    printf("last movie watched mid:%d\n", currentUser->watchHistory->info.mid);

                    /*create a new suggestion movie node*/
                    sugMovie=(struct suggested_movie*)malloc(sizeof(struct suggested_movie));
                    sugMovie->info.mid=currentUser->watchHistory->info.mid;
                    sugMovie->info.year=currentUser->watchHistory->info.year;  

                    /*insert node at the start*/
                    if(sugUser->suggestedHead==NULL && addWhere==0){
                        sugUser->suggestedHead=sugMovie;
                        sugMovie->next=NULL;
                        sugMovie->prev=NULL;
                        sugUser->suggestedTail=NULL;
                        headUser->suggestedHead=sugMovie;
                        headUser->suggestedHead->prev=NULL;
                        addWhere=1;
                    }
                    /*insert at the end*/
                    else if(addWhere==1 && sugUser->suggestedTail==NULL){
                        sugUser->suggestedTail=sugMovie;
                        sugMovie->next=NULL;
                        sugMovie->prev=NULL;
                        addWhere=0;
                    }
                    /*insert after the head accordingly*/
                    else if(addWhere==0){
                        sugUser->suggestedHead->next=sugMovie;
                        sugMovie->next=NULL;
                        sugMovie->prev=sugUser->suggestedHead;
                        sugUser->suggestedHead=sugUser->suggestedHead->next;
                        addWhere=1;
                    }
                    /*insert before the tail accordinly*/
                    else if(addWhere==1){
                        sugUser->suggestedTail->prev=sugMovie;
                        sugMovie->next=sugUser->suggestedTail;
                        sugMovie->prev=NULL;
                        sugUser->suggestedTail=sugUser->suggestedTail->prev;
                        addWhere=0;
                    }
                    
                    /*connect tail with head*/
                    if(currentUser->next==NULL && addWhere==0){
                        sugUser->suggestedTail->prev=sugUser->suggestedHead;
                        sugUser->suggestedHead->next=sugUser->suggestedTail;
                    }else if(currentUser->next==NULL && addWhere==1){
                        sugUser->suggestedTail->prev=sugUser->suggestedHead;
                        sugUser->suggestedHead->next=sugUser->suggestedTail;   
                    }

                } else {
                    printf("no movies watched\n");
                }
                currentUser = currentUser->next;
            } else {
                currentUser = currentUser->next;
            }
        }
    }
    /*case F event did happen but has only 1 node*/
    else if(mergeStatus==1){
        while (currentUser != NULL) {
            if (currentUser->uid != uid) {
                printf("user: %d ", currentUser->uid);
                if (currentUser->watchHistory!= NULL) {
                    printf("last movie watched mid:%d\n", currentUser->watchHistory->info.mid);

                    /*create a new suggestion movie node*/
                    sugMovie=(struct suggested_movie*)malloc(sizeof(struct suggested_movie));
                    sugMovie->info.mid=currentUser->watchHistory->info.mid;
                    sugMovie->info.year=currentUser->watchHistory->info.year;  

                    /*insert node at the start*/
                    if(sugUser->suggestedHead==NULL && addWhere==0){
                        sugUser->suggestedHead=sugMovie;
                        sugMovie->next=NULL;
                        sugMovie->prev=NULL;
                        sugUser->suggestedTail=NULL;
                        headUser->suggestedHead=sugMovie;
                        headUser->suggestedHead->prev=NULL;
                        addWhere=1;
                    }
                    /*insert at the end*/
                    else if(addWhere==1 && sugUser->suggestedTail==NULL){
                        sugUser->suggestedTail=sugMovie;
                        sugMovie->next=NULL;
                        sugMovie->prev=NULL;
                        addWhere=0;
                    }
                    /*insert after the head accordingly*/
                    else if(addWhere==0){
                        sugUser->suggestedHead->next=sugMovie;
                        sugMovie->next=NULL;
                        sugMovie->prev=sugUser->suggestedHead;
                        sugUser->suggestedHead=sugUser->suggestedHead->next;
                        addWhere=1;
                    }
                    /*insert before the tail accordinly*/
                    else if(addWhere==1){
                        sugUser->suggestedTail->prev=sugMovie;
                        sugMovie->next=sugUser->suggestedTail;
                        sugMovie->prev=NULL;
                        sugUser->suggestedTail=sugUser->suggestedTail->prev;
                        addWhere=0;
                    }
                    
                    /*connect tail with head*/
                    if(currentUser->next==NULL && addWhere==0){
                        sugUser->suggestedTail->prev=sugUser->suggestedHead;
                        sugUser->suggestedHead->next=sugUser->suggestedTail;
                    }else if(currentUser->next==NULL && addWhere==1){
                        sugUser->suggestedTail->prev=sugUser->suggestedHead;
                        sugUser->suggestedHead->next=sugUser->suggestedTail;   
                    }

                } else {
                    printf("no movies watched\n");
                }
                currentUser = currentUser->next;
            } else {
                currentUser = currentUser->next;
            }
        }

    }
    /*case F event did happen but has more than one node*/
    else if(mergeStatus==2){
        while (currentUser != NULL) {
            if (currentUser->uid != uid) {
                printf("user: %d ", currentUser->uid);
                if (currentUser->watchHistory!= NULL) {
                    printf("last movie watched mid:%d\n", currentUser->watchHistory->info.mid);

                    /*create a new suggestion movie node*/
                    sugMovie=(struct suggested_movie*)malloc(sizeof(struct suggested_movie));
                    sugMovie->info.mid=currentUser->watchHistory->info.mid;
                    sugMovie->info.year=currentUser->watchHistory->info.year;  

                    /*insert node at the start*/
                    if(sugUser->suggestedHead==NULL && addWhere==0){
                        sugUser->suggestedHead=sugMovie;
                        sugMovie->next=NULL;
                        sugMovie->prev=NULL;
                        sugUser->suggestedTail=NULL;
                        headUser->suggestedHead=sugMovie;
                        headUser->suggestedHead->prev=NULL;
                        addWhere=1;
                    /*insert at the end*/
                    }else if(addWhere==1 && sugUser->suggestedTail==NULL){
                        sugUser->suggestedTail=sugMovie;
                        sugMovie->next=NULL;
                        sugMovie->prev=NULL;
                        addWhere=0;
                    }
                    /*insert after the head accordingly*/
                    else if(addWhere==0){
                        sugUser->suggestedHead->next=sugMovie;
                        sugMovie->next=NULL;
                        sugMovie->prev=sugUser->suggestedHead;
                        sugUser->suggestedHead=sugUser->suggestedHead->next;
                        addWhere=1;
                    }
                    /*insert before the tail accordinly*/
                    else if(addWhere==1){
                        sugUser->suggestedTail->prev=sugMovie;
                        sugMovie->next=sugUser->suggestedTail;
                        sugMovie->prev=NULL;
                        sugUser->suggestedTail=sugUser->suggestedTail->prev;
                        addWhere=0;
                    }
                    
                    /*connect tail with head*/
                    if(currentUser->next==NULL && addWhere==0){
                        sugUser->suggestedTail->prev=sugUser->suggestedHead;
                        sugUser->suggestedHead->next=sugUser->suggestedTail;
                    }else if(currentUser->next==NULL && addWhere==1){
                        sugUser->suggestedTail->prev=sugUser->suggestedHead;
                        sugUser->suggestedHead->next=sugUser->suggestedTail;   
                    }

                } else {
                    printf("no movies watched\n");
                }
                currentUser = currentUser->next;
            } else {
                currentUser = currentUser->next;
            }
        }        
    }

    /*print suggested movies accordinlgy*/
     
    if(mergedListHead==NULL || mergeStatus==0){
        printf("S %d\n  User %d Suggested Movies=",uid,uid);
        /*check if suggested movies list is empty*/
        if(sugUser->suggestedHead!=NULL){
            while(headUser->suggestedHead!=NULL){
                printf("%d,",headUser->suggestedHead->info.mid);
                headUser->suggestedHead=headUser->suggestedHead->next;
            }
            headUser->suggestedHead=sugUser->suggestedTail;
            while(headUser->suggestedHead!=NULL){
                printf("%d,",headUser->suggestedHead->info.mid);
                headUser->suggestedHead=headUser->suggestedHead->next;
            }
        }else{
            printf("No movies to suggest!");
        }
        printf("\nDONE\n");
        
    }else if(mergeStatus==2){
        printf("S %d\n  User %d Suggested Movies=",uid,uid);
        printf("%d,",mergedListHead->info.mid);

        mergedListHead=mergedListHead->next;

        while(headUser->suggestedHead!=NULL){
            printf("%d,",headUser->suggestedHead->info.mid);
            headUser->suggestedHead=headUser->suggestedHead->next;
        }
        headUser->suggestedHead=sugUser->suggestedTail;

        while(mergedListHead->next!=mergedListTail){
            printf("%d,",mergedListHead->info.mid);
            mergedListHead=mergedListHead->next;
            if(mergedListHead->next==mergedListTail){
                printf("%d,",mergedListHead->info.mid);
            }
        }

        while(headUser->suggestedHead!=NULL){
                printf("%d,",headUser->suggestedHead->info.mid);
                headUser->suggestedHead=headUser->suggestedHead->next;
        }


        printf("%d,",mergedListTail->info.mid);

        printf("\nDONE\n");
        
    }else if(mergeStatus==1){
        printf("S %d\n  User %d Suggested Movies=",uid,uid);

        while(headUser->suggestedHead!=NULL){
                printf("%d,",headUser->suggestedHead->info.mid);
                headUser->suggestedHead=headUser->suggestedHead->next;
        }
        headUser->suggestedHead=sugUser->suggestedTail;

        printf("%d,",mergedListHead->info.mid);

        while(headUser->suggestedHead!=NULL){
            printf("%d,",headUser->suggestedHead->info.mid);
            headUser->suggestedHead=headUser->suggestedHead->next;
        }

        printf("\nDONE\n");
        
    }


return 0;
}

int filtered_movie_search(int uid, movieCategory_t category1,movieCategory_t category2, unsigned year){

    struct suggested_movie* mergedList = NULL;
    /*create temporary variables to traverse through each category*/ 
    tempCategoty1=Categories[category1];
    tempCategoty2=Categories[category2];
    int f1=0;
    int f2=0;

    /*case both categories have something in them*/
    while (tempCategoty1 != NULL && tempCategoty2 != NULL) {
        /*case cat1>cat2*/
        if (tempCategoty1->info.mid <= tempCategoty2->info.mid) {
            if (tempCategoty1->info.year > 1995) {
                struct suggested_movie* newNode = (struct suggested_movie*)malloc(sizeof(struct suggested_movie));
                newNode->info = tempCategoty1->info;
                newNode->prev = NULL;
                newNode->next = NULL;
                

                if (mergedList == NULL) {
                    mergedList = newNode;
                } else {
                    /* Append to the end of the merged list */
                    struct suggested_movie* current = mergedList;
                    while (current->next != NULL) {
                        current = current->next;
                    }
                    current->next = newNode;
                    newNode->prev = current;
                }
            }
            tempCategoty1 = tempCategoty1->next;
        }
        /*case cat2>cat1*/
        else {
            if (tempCategoty2->info.year > 1995) {
                struct suggested_movie* newNode = (struct suggested_movie*)malloc(sizeof(struct suggested_movie));
                newNode->info = tempCategoty2->info;
                newNode->prev = NULL;
                newNode->next = NULL;

                if (mergedList == NULL) {
                    mergedList = newNode;
                } else {
                    /* Append to the end of the merged list */
                    struct suggested_movie* current = mergedList;
                    while (current->next != NULL) {
                        current = current->next;
                    }
                    current->next = newNode;
                    newNode->prev = current;
                }
            }
            tempCategoty2 = tempCategoty2->next;
        }
    }

    /*case the whole category1 is empty*/
    if (tempCategoty1!=NULL && tempCategoty1->next==NULL){
        if (tempCategoty1->info.year > 1995){
            struct suggested_movie* newNode = (struct suggested_movie*)malloc(sizeof(struct suggested_movie));
            newNode->info = tempCategoty1->info;
            newNode->prev = NULL;
            newNode->next = NULL;
            mergedList=newNode;
            mergedListHead=newNode;
            f1=1;
        }
    }

    /*case the whole category2 is empty*/
    if (tempCategoty2!=NULL && tempCategoty2->next==NULL){
        if (tempCategoty2->info.year > 1995){
            struct suggested_movie* newNode = (struct suggested_movie*)malloc(sizeof(struct suggested_movie));
            newNode->info = tempCategoty2->info;
            newNode->prev = NULL;
            newNode->next = NULL;
            mergedList=newNode;
            mergedListHead=newNode;
            f2=1;
        }
    }

    /*case many nodes in each category list*/
    while (tempCategoty1 != NULL && f1==0) {
        if (tempCategoty1->info.year > 1995) {
            struct suggested_movie* newNode = (struct suggested_movie*)malloc(sizeof(struct suggested_movie));
            newNode->info = tempCategoty1->info;
            newNode->prev = NULL;
            newNode->next = NULL;

            struct suggested_movie* current = mergedList;
            if (current == NULL) {
                /* The list is empty, set the new node as the head */
                mergedList = newNode;
            } else {
                /* Traverse the list to find the last node */
                while (current->next != NULL) {
                    current = current->next;
                }
                
                /* Append the new node */
                current->next = newNode;
                newNode->prev = current;
            }
        }
        tempCategoty1= tempCategoty1->next;
    }



    /*case many nodes in its category list*/
    while (tempCategoty2 != NULL && f2==0) {
        if (tempCategoty2->info.year > 1995) {
            struct suggested_movie* newNode = (struct suggested_movie*)malloc(sizeof(struct suggested_movie));
            newNode->info = tempCategoty2->info;
            newNode->prev = NULL;
            newNode->next = NULL;

            /* Append before the tail of the merged list */
            struct suggested_movie* current = mergedList;
            while (current->next != NULL) {
                current = current->next;
            }
            newNode->prev = current->prev;
            newNode->next = current;
            current->prev->next = newNode;
            current->prev = newNode;    
        }
        tempCategoty2= tempCategoty2->next;
    }


    /*find he right user*/
    struct user*SugUser=User;
    while(SugUser!=NULL){
        if (SugUser->uid!=uid){
            SugUser=SugUser->next;
        }else if(SugUser->uid==uid){
            addFilterListUser=SugUser->uid;
            break;
        }
    }

    /*get the right head*/
    while(SugUser->suggestedHead!=NULL){
        if(SugUser->suggestedHead->prev==NULL){
            break;
        }else{
            SugUser->suggestedHead=SugUser->suggestedHead->prev;
        }
    }
    
    struct suggested_movie* currentHead = SugUser->suggestedHead;
    struct suggested_movie* currentTail = SugUser->suggestedTail;

    
    
    printf("F %d %d %d %u\n",uid,category1,category2,year);
    printf("    User %d Suggested Movies=",uid);

    
    /*print suggested list*/
    while (currentHead != NULL) {
        printf("%d,", currentHead->info.mid);
        currentHead = currentHead->next;
    }

    while (currentTail != NULL) {
        printf("%d,", currentTail->info.mid);
        currentTail = currentTail->next;
    }



    struct suggested_movie* head=mergedList;
    struct suggested_movie* startFS;
   
    mergedListHead=head;
    mergedListTail=head;
    
    /*add filtered list at the end*/
    while (head != NULL) {
        printf("%d,", head->info.mid);
        if(head->next==NULL){
            break;
        }
        head = head->next;
    }
    
    mergedListTail=head;
    printf("\nDONE\n");
    

    printf("Merge list Head: %d \n",mergedListHead->info.mid);
    printf("Merge list Tail: %d \n",mergedListTail->info.mid);

    return 0;
};

void take_off_movie(unsigned mid){
    int i;
    int found=0;
    struct new_movie* cat;
   
    struct user* curUser=User->next;
    printf("T %d\n",mid);

    /*remove from each category list*/
    for(i=0;i<6;i++){
        struct new_movie* prev=NULL;
        cat=Categories[i];
        while(cat!=NULL){
            /*find mid to delete*/
            if(cat->info.mid==mid){
                found=1;
                /*mid is the head of the list*/
                if(prev==NULL){
                    Categories[i]=cat->next;
                }else{
                    prev->next=cat->next;
                }
                free(cat);
                printf("    Movie %d is removed from category %d\n",mid,i);
                break;
            }
            prev=cat;
            cat=cat->next;
        }

        if(found==1){
            cat=Categories[i];
            printf("    Category %d list=",i);
            while(cat!=NULL){
                printf("%d,",cat->info.mid);
                cat=cat->next;
            }
            printf("\n");
            break;
        }
    }
    printf("DONE\n");

    /*this solution is not including removing from suggested lists!*/
     
};

void print_movies(void){
    printf("M\nCategorized Movies:\n"); 
    int i;
    for ( i = 0; i < 6; i++) {
        if(i==0){printf("    Horror:");}
        if(i==1){printf("    Sci-fi:");}
        if(i==2){printf("    Drama:");}
        if(i==3){printf("    Romance:");}
        if(i==4){printf("    Documentary:");}
        if(i==5){printf("    Comedy:");}

        if (Categories[i]!=NULL){
                printCategories(Categories[i]);
        }else{
            printf("Empty");
        }
        printf("\n");
    }
    printf("DONE\n");
};

void print_users(void){
    int count=0;
    if(User==NULL){
        printf("There is no user");
    }
    printf("P\nUsers:\n");
    struct user *ptr;
    ptr=User->next;
    while(ptr!=NULL){
        printf("    %d:\n",ptr->uid);
        /*suggested which are not correct*/
        printf("    Suggested:");
        while(ptr->suggestedHead!=NULL){
            printf("%d,",ptr->suggestedHead->info.mid);
            ptr->suggestedHead=ptr->suggestedHead->next;
        }
        printf("\n");
        /*watch history*/
        printf("    Watch History:");
        struct movie * history=ptr->watchHistory;
        while(history!=NULL){
            printf("%d,",history->info.mid);
            history=history->next;
        }
        printf("\n");
        ptr =ptr->next;
    }
    printf("DONE\n");

};
