/*****************************************************
 * @file   Movie.c                                   *
 * @author Ntenis Sampani <csd4739@csd.uoc.gr>     *
 *                                                   *
 * @brief Implementation for Movie.h 				 *
 * Project: Winter 2023						         *
 *****************************************************/
#include "Movie.h"
#include <time.h>

/**
 * @brief Iniliazes some must needed variables.
 * 
 */

void initializeGlobals() {

	/*initialize a and b prime numbers randomly*/
	srand(time(NULL));
    a = primes_g[rand()%170]; 
    b = primes_g[rand()%170]; 

	/*initialize hash_table_size according to max_users while being a prime number*/
	int found=0;
	int min=max_users;
	int i=0;
	int array_size=170;
	while (i<array_size){
		if(primes_g[i]>=min){
			hashtable_size=primes_g[i];
			found=1;
			break;
		}else{
			i=i+1;
		}
	}

	/*case we didn't find a greater or equal number,use the last prime number*/
	if (found == 0) {
    	hashtable_size = primes_g[array_size - 1];
	}

	user_hashtable_p=(user_t**)malloc(hashtable_size*sizeof(user_t*));
	
	for(i=0;i<hashtable_size;i++){
		user_hashtable_p[i]=NULL;
	}
	
	
}

/**
 * @brief Creates a new user.
 * Creates a new user with userID as its identification.
 *
 * @param userID The new user's identification
 *
 * @return 1 on success
 *         0 on failure
 */

 int register_user(int userID){

	/*hash function*/
	int hash=(((hashtable_size-1)*a)+(b*userID))%hashtable_size;

	/*create the user*/
	user_t* new_user=(user_t*)malloc(sizeof(user_t));
	new_user->userID=userID;
	new_user->history=NULL;
	new_user->next=NULL;

	/*insert the user ,either at the start if list is empty or at the end*/
	int found=0;
	if (user_hashtable_p[hash]==NULL){
		user_hashtable_p[hash]=new_user;
	}else{
		user_t*tmp=user_hashtable_p[hash];
		while(found==0){
			if(tmp->next==NULL){
				tmp->next=new_user;
				found=1;
			}else{
				tmp=tmp->next;
			}
		}
	}

	 return 1;
 }

/**
 * @brief Method to delete the history tree before deleting the user
 * We actually use post Order Traversal in order to free all the nodes
 * from the history tree correctly
 */
void deleteHistoryTree(userMovie_t* root){
	if(root==NULL){
		return;
	}

	deleteHistoryTree(root->lc);
	deleteHistoryTree(root->rc);

	free(root);
};
 
/**
 * @brief Deletes a user.
 * Deletes a user with userID from the system, along with users' history tree.
 *
 * @param userID The new user's identification
 *
 * @return 1 on success
 *         0 on failure
 */

 int unregister_user(int userID){
	int found=0;
	int hash=(((hashtable_size-1)*a)+(b*userID))%hashtable_size;
	user_t*tmp=user_hashtable_p[hash];
	user_t*prev;
	prev=NULL;

	/*find the user that has to be deleted*/
	while(tmp!=NULL && found==0){
		if(tmp->userID==userID){
			found=1;
		}else{
			prev=tmp;
			tmp=tmp->next;
		}
	}


	if (found==1){
		if(prev==NULL){
			user_hashtable_p[hash]=tmp->next;
		}else{
			prev->next=tmp->next;
		}
		/*free history first*/
		deleteHistoryTree(tmp->history);
		/*free user*/
		free(tmp);
	}else{
		printf("User with ID %d not found",userID);
		return 1;
	}

	/*print the users according to the index(hash)*/
	printf("Chain %d of Users:\n",hash);
	user_t*users=user_hashtable_p[hash];
	while(users!=NULL){
		printf("   User %d\n",users->userID);
		users=users->next;
	}

	 return 1;
 }

/**
 * @brief Prints a binary tree in InOrder Traversal
 * 
 * @param root 
 */

 void printInOrder(struct new_movie*root){
	
	if (root==NULL){return;}

	printInOrder(root->lc);
	printf("%d ,",root->movieID);
	printInOrder(root->rc);
	
 }

/**
 * @brief Add new movie to new release binary tree.
 * Create a node movie and insert it in 'new release' binary tree.
 *
 * @param movieID The new movie identifier
 * @param category The category of the movie
 * @param year The year movie released
 *
 * @return 1 on success
 *         0 on failure
 */

 int add_new_movie(int movieID, int category, int year){
	int found=0;

	/*allocate memory for the movie*/
	new_movie_t*newNode;
	newNode=(new_movie_t*)malloc(sizeof(new_movie_t));

	if(newNode!=NULL){
		newNode->category=category;
		newNode->lc=NULL;
		newNode->rc=NULL;
		newNode->movieID=movieID;
		newNode->year=year;
		newNode->sumScore=0;
		newNode->watchedCounter=0;
	}else{
		fprintf(stderr, "Memory allocation failed\n");
	}
	
	if(new_releases==NULL){
		new_releases=newNode;
		found=1;
	}

	/*insert the movie inside the binary new_releases tree*/
	struct new_movie *tmp=new_releases;
	while(found==0){
		if(movieID<tmp->movieID && tmp->lc==NULL){
			tmp->lc=newNode;
			found=1;
			break;
		}else if(movieID<tmp->movieID && tmp->lc!=NULL){
			tmp=tmp->lc;
		}else if(movieID>tmp->movieID && tmp->rc==NULL){
			tmp->rc=newNode;
			found=1;
			break;
		}else if(movieID>tmp->movieID && tmp->rc!=NULL){
			tmp=tmp->rc;
		}
	}

	printf("	New realeases Tree:");
	printInOrder(new_releases);
	printf("\nDONE\n");
	return 1;
 }
 
/**
 * @brief method to save the movieIDs(in sorted value) and years in order to
 * create separate balanced trees 
 * 
 * @param root 
 * @param array 
 * @param cols 
 * @param maxIndex 
 * @param years 
 */

 void inOrderTraversal(new_movie_t* root,int **array,int *cols,int *maxIndex,int ** years){

	if(root==NULL){
		return;
	}

	inOrderTraversal(root->lc,array,cols,maxIndex,years);

	int category=root->category;
	int movieID=root->movieID;
	int year=root->year;

	/*allocate memory for each movie and year*/
	array[category]=(int*)realloc(array[category],(cols[category]+2)*sizeof(int));
	years[category]=(int*)realloc(years[category],(cols[category]+2)*sizeof(int));
	if (array[category]==NULL || years[category]==NULL){
		fprintf(stderr,"Memory allocation failed.\n");
	}

	/*save the right values*/
	array[category][cols[category]]=movieID;
	years[category][cols[category]]=year;
	years[category][cols[category]+1]=array[category][cols[category]+1]=-1;

	maxIndex[category]=maxIndex[category]+1;
	cols[category]=cols[category]+1;

	inOrderTraversal(root->rc,array,cols,maxIndex,years);

}

/**
 * @brief here we are using PostOrder Traversal in order
 * to have the ability every time we add in the array a node from the tree
 * to delete it as well,so using postorder we dont delete any node that has any childen
 * 
 * @param root 
 */

 void postOrderTraversalDeletion(new_movie_t* root,int **array){

	if(root==NULL){
		return;
	}

	postOrderTraversalDeletion(root->lc,array);
	postOrderTraversalDeletion(root->rc,array);

	if (root->lc == NULL && root->rc == NULL) {
        free(root);
    }

 }

/**
 * @brief method to create balanced trees for each category.
 * Logic behind this method is that we have a sorted array of movied IDs
 * and the root is the mid=(start+end/2) element 
 * and we continue this recursion until the balanced tree is created
 * 
 * @param array 
 * @param start 
 * @param end 
 * @param years 
 * @param category 
 * @return movie_t* 
 */

 movie_t* categoriesFill(int *array,int start,int end,int *years,int category){
	
	if(start>end)
		return NULL;

	int mid=(start+end)/2;

	/*create the node to put in each category each
	 time this recursive function is being executed*/ 
	movie_t * root=(movie_t*)malloc(sizeof(movie_t));
	root->movieID=array[mid];
	root->sumScore=0;
	root->watchedCounter=0;
	root->year=years[mid];
	root->lc=categoryArray[category]->sentinel;
	root->rc=categoryArray[category]->sentinel;

	root->lc=categoriesFill(array,start,mid-1,years,category);
	root->rc=categoriesFill(array,mid+1,end,years,category);
	
	
	return root;
	 
	}
 
/**
 * @brief print every category tree in InOrderTraversal
 * 
 * @param root 
 */

 void printInOrder2(movie_t*root){

	if (root!=NULL){

	printInOrder2(root->lc);
	printf("%d ,",root->movieID);
	printInOrder2(root->rc);
	}
 }

/**
 * @brief Distribute the movies from new release binary tree to the array of categories.
 *
 * @return 0 on success
 *         1 on failure
 */

 int distribute_movies(void){
	int i;

	/*allocate memory for the 2D where
	* array holds the key values of movieID
	* and years holds the key values of year 
	*/
	int **array=(int**)malloc(6*sizeof(int*));
	int **years=(int**)malloc(6*sizeof(int*));
	if (array==NULL || years==NULL){
		fprintf(stderr,"Memory allocation failed.\n");
	}

	int cols[6];
	int maxIndex[6];

	for(i=0;i<6;i++){
		array[i]=NULL;
		years[i]=NULL;
		cols[i]=0;
		maxIndex[i]=0;
	}


	new_movie_t* tmp=new_releases;
	inOrderTraversal(tmp,array,cols,maxIndex,years);
	postOrderTraversalDeletion(tmp,array);
	
	/*create the balanced trees for each category*/
	for(i=0;i<6;i++){
		categoryArray[i]->movie=categoriesFill(array[i],0,maxIndex[i]-1,years[i],i);
	}

	printf("Movie Category Array:\n");
	for(i=0;i<6;i++){
		printf("   Category %d: ",i);
		printInOrder2(categoryArray[i]->movie);
		printf("\n");
	}
	printf("DONE\n");
	return 1;
  }

/**
 * @brief method to insert a node in the history tree(a leaf oriented tree)
 * 
 * @param history (pointer to each history list of each user)
 * @param Movie (pointer to the movie we want to add in)
 * @return a pointer to the root if the history tree
 */

 userMovie_t* InsertLeafOriented(userMovie_t** history,userMovie_t *Movie){
	userMovie_t* hist=*history;

	/*case we have only the root in the leaf oriented tree*/
	if (hist->movieID>Movie->movieID && hist->lc==NULL && hist->rc==NULL){
		/*insert left the new node*/
		hist->lc=(userMovie_t*)malloc(sizeof(userMovie_t));
		hist->lc->parent=hist;
		hist->lc->lc=NULL;
		hist->lc->rc=NULL;
		hist->lc->category=Movie->category;
		hist->lc->movieID=Movie->movieID;
		hist->lc->score=Movie->score;

		/*create the same node as the root right*/
		hist->rc=(userMovie_t*)malloc(sizeof(userMovie_t));
		hist->rc->parent=hist;
		hist->rc->rc=NULL;
		hist->rc->lc=NULL;
		hist->rc->category=hist->category;
		hist->rc->movieID=hist->movieID;
		hist->rc->score=hist->score;

		hist=hist->lc;
		return *history;
	}else if(hist->movieID<=Movie->movieID && hist->lc==NULL && hist->rc==NULL){
		userMovie_t* new_root=(userMovie_t*)malloc(sizeof(userMovie_t));
		new_root->category=Movie->category;
		new_root->movieID=Movie->movieID;
		new_root->score=Movie->score;
		new_root->parent=NULL;
		new_root->rc=NULL;

		/*make root be a left child*/
		new_root->lc=hist;
		new_root->lc->category=hist->category;
		new_root->lc->movieID=hist->movieID;
		new_root->lc->parent=new_root;
		new_root->lc->score=hist->score;
		
		hist=new_root;

		/*create the same node right too*/
		hist->rc=(userMovie_t*)malloc(sizeof(userMovie_t));
		hist->rc->parent=hist;
		hist->rc->rc=NULL;
		hist->rc->lc=NULL;
		hist->rc->category=hist->category;
		hist->rc->movieID=hist->movieID;
		hist->rc->score=hist->score;
		

		*history=hist;
		return *history;
	}
	
	/*move to the childen*/
	if(hist->movieID>Movie->movieID){
		hist=hist->lc;
	}else if(hist->movieID>=Movie->movieID){
		hist=hist->rc;
	}

	/*find the case to be inserted*/
	int found=0;
	while (found==0){
		/*case LL*/
		if(hist->movieID>Movie->movieID && hist->lc==NULL && hist->rc==NULL && hist->parent->lc==hist){
			found=1;
		/*case RR*/	
		}else if(hist->movieID<=Movie->movieID && hist->lc==NULL && hist->rc==NULL && hist && hist->parent->rc==hist){
			found=2;
		/*case LR*/
		}else if(hist->movieID<=Movie->movieID && hist->lc==NULL && hist->rc==NULL && hist && hist->parent->lc==hist){
			found=3;
		/*case RL*/	
		}else if(hist->movieID>Movie->movieID && hist->lc==NULL && hist->rc==NULL && hist && hist->parent->rc==hist){
			found=4;
		}else if(hist->movieID>Movie->movieID){
			hist->lc->parent=hist;
			hist=hist->lc;
		}else if(hist->movieID<=Movie->movieID){
			hist->rc->parent=hist;
			hist=hist->rc;
		}
	}

	
	/*now we know the parent and we need to insert correctly*/
	/*found=1,node needs to be inserted left,coming left from the parent*/
	/*found=2,node needs to be inserted right(also root changes),coming right from the parent*/
	/*found=3,node needs to be inserted right(also root changes),coming left from the parent*/
	/*found=4,node needs to be inserted left,coming right from the parent*/
	if(found==1){
		/*insert left the new node*/
		hist->lc=(userMovie_t*)malloc(sizeof(userMovie_t));
		hist->lc->parent=hist;
		hist->lc->lc=NULL;
		hist->lc->rc=NULL;
		hist->lc->category=Movie->category;
		hist->lc->movieID=Movie->movieID;
		hist->lc->score=Movie->score;

		/*create the same node as the root and put it as the rc*/
		hist->rc=(userMovie_t*)malloc(sizeof(userMovie_t));
		hist->rc->parent=hist;
		hist->rc->rc=NULL;
		hist->rc->lc=NULL;
		hist->rc->category=hist->category;
		hist->rc->movieID=hist->movieID;
		hist->rc->score=hist->score;

		return *history;
	}else if(found==2){
		
		/*create a new root*/
		userMovie_t* new_root=(userMovie_t*)malloc(sizeof(userMovie_t));
		new_root->category=Movie->category;
		new_root->movieID=Movie->movieID;
		new_root->score=Movie->score;
		new_root->parent=hist->parent;
		hist->parent->rc=new_root;
		hist->parent=NULL;
		new_root->rc=NULL;


		/*make root be a left child too*/
		new_root->lc=(userMovie_t*)malloc(sizeof(userMovie_t));
		new_root->lc->parent=new_root;
		new_root->lc->lc=NULL;
		new_root->lc->rc=NULL;
		new_root->lc->category=hist->category;
		new_root->lc->movieID=hist->movieID;
		new_root->lc->score=hist->score;

	
		/*create the same node right too and let it be rc*/
		new_root->rc=(userMovie_t*)malloc(sizeof(userMovie_t));
		new_root->rc->parent=new_root;
		new_root->rc->rc=NULL;
		new_root->rc->lc=NULL;
		new_root->rc->category=new_root->category;
		new_root->rc->movieID=new_root->movieID;
		new_root->rc->score=new_root->score;

		return *history;
	}else if(found==3){
		
		/*create a new node*/
		userMovie_t* new_root=(userMovie_t*)malloc(sizeof(userMovie_t));
		new_root->category=Movie->category;
		new_root->movieID=Movie->movieID;
		new_root->score=Movie->score;
		new_root->parent=hist->parent;
		hist->parent->lc=new_root;
		hist->parent=NULL;

		/*create a left child with and connent with new_root*/
		new_root->lc=(userMovie_t*)malloc(sizeof(userMovie_t));
		new_root->lc->parent=new_root;
		new_root->lc->lc=NULL;
		new_root->lc->rc=NULL;
		new_root->lc->category=hist->category;
		new_root->lc->movieID=hist->movieID;
		new_root->lc->score=hist->score;

	    /*create the same node right too and let it be rc*/
		new_root->rc=(userMovie_t*)malloc(sizeof(userMovie_t));
		new_root->rc->parent=new_root;
		new_root->rc->rc=NULL;
		new_root->rc->lc=NULL;
		new_root->rc->category=new_root->category;
		new_root->rc->movieID=new_root->movieID;
		new_root->rc->score=new_root->score;

		return *history;
	}else if(found==4){

		/*create a new node and put let it be lc*/
		userMovie_t* new_node=(userMovie_t*)malloc(sizeof(userMovie_t));
		new_node->category=Movie->category;
		new_node->movieID=Movie->movieID;
		new_node->score=Movie->score;
		new_node->lc=NULL;
		new_node->rc=NULL;
		new_node->parent=hist;
		hist->lc=new_node;

		/*create the same node as the root and let it be rc*/
		hist->rc=(userMovie_t*)malloc(sizeof(userMovie_t));
		hist->rc->category=hist->category;
		hist->rc->movieID=hist->movieID;
		hist->rc->score=hist->score;
		hist->rc->parent=hist;
		hist->rc->rc=NULL;
		hist->rc->lc=NULL;

		return *history;
	}
  
  }

/**
 * @brief print the history tree (a leaf oriented tree) in inOrder Traversal
 * 
 * @param root 
 */

  void printInOrderLeafTree(userMovie_t* node) {
    if (node == NULL) {return;}
        printInOrderLeafTree(node->lc);
		if(node->lc==NULL && node->rc==NULL){
        	printf("	MovieID: %d, Score: %d\n",node->movieID, node->score);
		}
        printInOrderLeafTree(node->rc);
    
}
 
/**
 * @brief User rates the movie with identification movieID with score
 *
 * @param userID The identifier of the user
 * @param category The Category of the movie
 * @param movieID The identifier of the movie
 * @param score The score that user rates the movie with id movieID
 *
 * @return 1 on success
 *         0 on failure
 */

 int watch_movie(int userID,int category, int movieID, int score){

	movie_t*tmp=categoryArray[category]->movie;
	int found=0;
	/*find the movie in the correct category tree*/
	while(tmp!=NULL&&found==0){
		if (tmp->movieID==movieID){
			found=1;
		}else if(tmp->movieID>movieID){
			tmp=tmp->lc;
		}else{
			tmp=tmp->rc;
		}
	}
	/*increase the watchedCounter and add the score*/
	tmp->watchedCounter=tmp->watchedCounter+1;
	tmp->sumScore=tmp->sumScore+score;



	/*create the movie*/
	userMovie_t * Movie=(userMovie_t*)malloc(sizeof(userMovie_t));
	Movie->movieID=tmp->movieID;
	Movie->category=category;
	Movie->score=score;
	Movie->lc=NULL;
	Movie->rc=NULL;
	Movie->parent=NULL;

	
	/*find the user*/
	int hash=(((hashtable_size-1)*a)+(b*userID))%hashtable_size;

	printf("History tree of user %d\n",userID);

	user_t * tmp2=user_hashtable_p[hash];
	
	/*find the user history tree*/
	if(tmp2->userID==userID){
		/*if the history tree is empty initiliaze it*/
		if(tmp2->history==NULL){
			tmp2->history=(userMovie_t*)malloc(sizeof(userMovie_t));
			tmp2->history->category=Movie->category;
			tmp2->history->movieID=Movie->movieID;
			tmp2->history->score=Movie->score;
			tmp2->history->lc=NULL;
			tmp2->history->rc=NULL;
			tmp2->history->parent=NULL;
			printInOrderLeafTree(tmp2->history);
		}else{
		/*else insert the movie in the oriented tree*/	
			tmp2->history=InsertLeafOriented(&tmp2->history,Movie);
			printInOrderLeafTree(tmp2->history);
		}
	}else{
		/*else search for the right user*/
		found=0;
		while(found==0){
			tmp2=tmp2->next;
			if(tmp2->userID==userID){
				found=1;
			}
		}
		if(tmp2->history==NULL){
			tmp2->history=(userMovie_t*)malloc(sizeof(userMovie_t));
			tmp2->history->category=Movie->category;
			tmp2->history->movieID=Movie->movieID;
			tmp2->history->score=Movie->score;
			tmp2->history->lc=NULL;
			tmp2->history->rc=NULL;
			tmp2->history->parent=NULL;
			printInOrderLeafTree(tmp2->history);
		}else{
			tmp2->history=InsertLeafOriented(&tmp2->history,Movie);
			printInOrderLeafTree(tmp2->history);
		}
	}

	printf("DONE\n");
	 return 1;
 }

/**
 * @brief method to sum up the movies with average score>=score
 * 
 * @param root 
 * @param score 
 * @return int 
 */

 int inOrderTraversalFilter(movie_t*root,int score){
	int currentCount;
	int averageScore;

	if (root == NULL) {
        return 0;
    }

    int leftCount = inOrderTraversalFilter(root->lc, score);
	if(root->watchedCounter<=0){
		averageScore=0;
	}else{
		averageScore=(root->sumScore / root->watchedCounter);
	}
    int rightCount = inOrderTraversalFilter(root->rc, score);

	if(averageScore>=score){
		currentCount=1;
	}else{
		currentCount=0;
	}

    /*return the total count for the current subtree*/
    return leftCount + currentCount + rightCount;

 }

 /**
 * @brief method to save the movies with average score>=score in the array
 * 
 * @param root 
 * @param score 
 * @param array
 * @param currentIndex
 */

 void inOrderTraversalSave(movie_t* root, int score, movie_t*** array, int* currentIndex) {
    if (root == NULL) {
        return;
    }

    inOrderTraversalSave(root->lc, score, array, currentIndex);

    int averageScore = (root->watchedCounter <= 0) ? 0 : (root->sumScore / root->watchedCounter);

	/*if we meet the citeria ,reallocate the memory of the array and save the move_t node*/
    if (averageScore >= score) {
        
        *array = (movie_t**)realloc(*array, (*currentIndex + 1) * sizeof(movie_t*));
  
        if (*array == NULL) {
            fprintf(stderr, "Memory reallocation failed\n");
            exit(EXIT_FAILURE);
        }

        (*array)[*currentIndex] = root;
        (*currentIndex)++;
    }

    inOrderTraversalSave(root->rc, score, array, currentIndex);
}

/*Function to swap two movies in the array */
void swap(movie_t** array, int i, int j) {
    movie_t* temp = array[i];
    array[i] = array[j];
    array[j] = temp;
}

/*Heapify a subtree rooted with node i, assuming the heaps rooted at left and right are heapified*/ 
void heapify(movie_t** array, int n, int i) {
    int largest = i; /*Initialize largest as root */
    int left = 2 * i + 1; /*Left child*/
    int right = 2 * i + 2; /*Right child*/

	

    /*If left child is larger than root */
    if (left < n && (array[left]->watchedCounter > 0 && array[largest]->watchedCounter>0 )&& (array[left]->sumScore / array[left]->watchedCounter) > (array[largest]->sumScore / array[largest]->watchedCounter)) {
        largest = left;
    }


    /*If right child is larger than largest so far */
    if (right < n && (array[right]->watchedCounter > 0 && array[largest]->watchedCounter>0)&& (array[right]->sumScore / array[right]->watchedCounter) > (array[largest]->sumScore / array[largest]->watchedCounter)) {
        largest = right;
    }


    /* If largest is not root */
    if (largest != i) {
        swap(array, i, largest);

        /*Recursively heapify the affected sub-tree*/
        heapify(array, n, largest);
    }
}

/*Perform heap sort on the array*/
void heapSort(movie_t** array, int n) {
    /*Build heap (rearrange array)*/
    for (int i = n / 2 - 1; i >= 0; i--) {
        heapify(array, n, i);
    }

     /*One by one extract an element from the heap*/
    for (int i = n - 1; i > 0; i--) {
        /*Move current root to end*/
        swap(array, 0, i);

        /*Call max heapify on the reduced heap*/
        heapify(array, i, 0);
    }
}

/**
 * @brief Identify the best rating score movie and cluster all the movies of a category.
 *
 * @param userID The identifier of the user
 * @param score The minimum score of a movie
 *
 * @return 1 on success
 *         0 on failure
 */

 int filter_movies(int userID, int score) {
    int i;
    int numMovies = 0;
	int zeros=0;

    /*sum up the movies that match the criteria*/
    for (i = 0; i < 6; i++) {
        movie_t* tmp = categoryArray[i]->movie;
		if(tmp==NULL){
			continue;
		}
        numMovies += inOrderTraversalFilter(tmp, score);
    }

    
    movie_t** array = (movie_t**)malloc(sizeof(movie_t*));
	movie_t** array1 = (movie_t**)malloc(sizeof(movie_t*));
	array=NULL;
	array1=NULL;
    int currentIndex = 0;


    /*perform in order to save the movies that match the critria*/ 
    for (i = 0; i < 6; i++) {
        movie_t* tmp = categoryArray[i]->movie;
        inOrderTraversalSave(tmp, score, &array, &currentIndex);
    }

	/*remove the zero avgScores and print them ,since the dont need sorting*/
	int NonZero=0;
	for(i=0;i<numMovies;i++){
		if(array[i]->watchedCounter==0 || array[i]->sumScore==0){
			/*print the avgScore==0 elements since they dont need sorting*/
			printf("	Movie ID:%d Avg Score:%d\n",array[i]->movieID,0);
			zeros=zeros+1;
		}else{
			/*array1 has the elements that needs to be sorted by HeapSort*/
			NonZero++;
			array1=(movie_t**)realloc(array1,NonZero*sizeof(movie_t*));
			array1[NonZero-1]=array[i];
			
		}
	}

	/*call heapsort*/
	if(NonZero>1){
    	heapSort(array1,NonZero);
	}
	
	/*print the remaining elements*/
	for(i=0;i<NonZero;i++){
			printf("	Movie ID:%d Avg Score:%d\n",array1[i]->movieID,(array1[i]->sumScore/array1[i]->watchedCounter));
	}
	
	printf("DONE\n");
	
    return 1;
}

/**
 * @brief Method that finds the next leaf
 * 
 * @param tmp 
 * @return a pointer to the leaf
 */

userMovie_t* FindNextLeaf(userMovie_t* tmp) {
    
	
	if(tmp==NULL){
		return NULL;/*case we dont have a next leaf*/
	}

	
	if(tmp->rc!=NULL){
		/*case if there is a right subtree,go to the most left node of it*/
		tmp=tmp->rc;
		while(tmp->lc !=NULL){
			tmp=tmp->lc;
		}
		return tmp;
	}else{
		/*case there is no right subtree,go up until find a left child*/
		while(tmp->parent!=NULL && tmp==tmp->parent->rc){
			tmp=tmp->parent;
		}
		return tmp->parent;
	}
	
}

/**
 * @brief Find movies from categories withn median_score >= score t
 *
 * @param userID The identifier of the user
 * @param category Array with the categories to search.
 * @param score The minimum score the movies we want to have
 *
 * @return 1 on success
 *         0 on failure
 */

 int user_stats(int userID){

	int ScoreSum=0;
	int counter=0;

	/*hash function*/
	int hash=(((hashtable_size-1)*a)+(b*userID))%hashtable_size;

	/*search in a leaf oriented tree*/
	userMovie_t*tmp=user_hashtable_p[hash]->history;

	/*find the most left leaf*/
	while(tmp->lc!=NULL){
		tmp=tmp->lc;
		if(tmp->lc==NULL){
			break;
		}
	}

	/*start from the left most leaf and find the next leaf*/
	while(tmp!=NULL){
		ScoreSum=ScoreSum+tmp->score;
		counter=counter+1;
		tmp=FindNextLeaf(tmp);
	}

	/*print the result*/
	printf("	User:%d MScore:%d\n",userID,ScoreSum/counter);
	 return 1;
 }
 
/**
 * @brief Search for a movie with identification movieID in a specific category.
 *
 * @param movieID The identifier of the movie
 * @param category The category of the movie
 *
 * @return 1 on success
 *         0 on failure
 */

 int search_movie(int movieID, int category){
	
	movie_t*tmp=categoryArray[category]->movie;
	int found=0;

	/*search for the movie inside the tree*/
	while(tmp!=NULL&&found==0){
		if (tmp->movieID==movieID){
			printf("Movie %d found in category %d\n",movieID,category);
			found=1;
			return 1;
		}else if(tmp->movieID>movieID){
			tmp=tmp->lc;
		}else{
			tmp=tmp->rc;
		}
	}

	printf("Movie %d not found in category %d\n",movieID,category);
	 return 1;
 }
 
 /**
 * @brief Prints the movies in movies categories array.
 * @return 1 on success
 *         0 on failure
 */

 int print_movies(void){
	int i;
	printf("Movie Category Array:\n");
	for(i=0;i<6;i++){
		printf("   Category %d: ", i);
    	if (categoryArray[i] != NULL) {
        	printInOrder2(categoryArray[i]->movie);
    	} else {
        	printf("NULL category\n");
    	}
    	printf("\n");
	}
	printf("DONE\n");
	return 1;
 }

/**
 * @brief Method to print the history tree of a user 
 * in InOrder Traversal(only the leafs)
 * 
 * @param root 
 */

void HistoryTreePrint(userMovie_t*root){

	if(root==NULL){
		return;
	}

	HistoryTreePrint(root->lc);
	if(root->lc==NULL && root->rc==NULL){
		printf("	 Movie:%d , Score:%d\n",root->movieID,root->score);
	}
	HistoryTreePrint(root->rc);
}

  /**
 * @brief Prints the users hashtable.
 * @return 1 on success
 *         0 on failure
 */

 int print_users(void){
	int i=0;
	int found=0;
	for(i=0;i<hashtable_size;i++){
		printf("Chain %d of Users:\n",i);
		found=0;
		user_t * tmp=user_hashtable_p[i];
		while(found==0){
			if(tmp==NULL){
				found=1;
			}else{
				printf("   User:%d\n",tmp->userID);
				/*print history tree*/
				printf("	History Tree:\n");
				userMovie_t*hist=tmp->history;
				HistoryTreePrint(hist);
				tmp=tmp->next;
			}
		}
	}
	 return 1;
 }
 
