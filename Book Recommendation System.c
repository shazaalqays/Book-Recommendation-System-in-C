#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#define Slength 50 // sentence size
#define structSize 100 // struct size

// reader struct
struct reader{
	char **name; // name of the reader
	char **bookName; // name of the book
	struct rates *rates; // rates
};

// this struct is for rates
struct rates{
	int *rate; // rate
};

// users struct
struct users{
	int *id;
	float *rate;
};

// books struct
struct books{
	int *id;
	float *rate;
};

// functions to be used in the program
struct reader *readers; // global variable for reader struct
struct users *user; // global variable for users struct
struct books *book; // global variable for books struct
void initStruct(); // function to initialize the struct
int ** createSturct(struct reader *readers, char filename[Slength]); // function to create the struct and it returns a rate matrix
char * upperCase(char str[Slength]); // function to convert the user input to upper case to match the names in CSV file
int searchForReader(struct reader *readers, char *reader); // function to search for a reader in the readers struct and return its ID
char * nameOfReader(struct reader *readers, int i); // function returns the name of reader 
char * nameOfBook(struct reader *readers, int i); // function returns the name of book
float calcMean(int *row); // function to calculate the mean, takes a row and return its mean
float coeff(int *arow,int *brow); // function to calculate the similarity, takes two row and return the similarity between them
void calculatePearson(int **rates, int nu_id, int k); // function to calculate the pearson coefficient (similarity), takes the rate matrix with the new user ID and steps number
float predCalc(float ra, float sumCB, float sumCoeff); // function to calculate the prediction, takes the operation factors and return the result
float suggestion(int **rates, int newUser,int index, int k); // function to suggest a book for reader, takes the rates matrix, new user ID, index of book, and k steps and return the prediction
void getBook(int **rates,int newUserId,int k); // function to get suggested books, takes rates matrix, new user ID, and k steps 
// end of functions


// initializes struct
void initStruct(){
	int i;
	// initialize struct by using malloc and calloc 
	readers = (struct reader*) malloc(sizeof(struct reader)); 
	readers->name = (char**)calloc(structSize,sizeof(readers->name));
	readers->bookName = (char**)calloc(structSize,sizeof(readers->bookName));
	readers->rates = (struct rates*)calloc(structSize,sizeof(struct rates));
	for(i =0; i< structSize; i++){
		readers->name[i] = (char*)calloc(Slength,sizeof(readers->name));
		readers->bookName[i] = (char*)calloc(Slength,sizeof(readers->bookName));
		readers->rates[i].rate = (int*)calloc(Slength,sizeof(readers->rates));
	}
}

// function of creating the struct 
int ** createSturct(struct reader *readers, char filename[Slength]){
	int i = 1, k=0, j=0, p, x=0, w=0; // variables to use
	int **rates; // rate matrix
	char *tok; // for token 
	char line[1024]; // for line to be read

	// initialize the rate matrix with 0's
	rates = (int **) calloc(26 , sizeof(int*));
	for (w = 0; w< 26; w++) {
    	rates[w] = (int *) calloc(8 , sizeof(int));
    }
	j=0;w=0;
	FILE *file = fopen(filename,"r"); // open the file
	if(file == NULL){
		printf( "file failed to open.\n" ); // if fail to open print message
		return 0;
	}
	else{
		while (fgets(line, 1024, file) != NULL){ // read from file line by line
	        char* tmp = strdup(line); // duplicate of the string pointed to
	        if(i == 1){ // if we are in the first line then we have to read the book names
	        	for (tok = strtok(line, ";"); tok && *tok; tok = strtok(NULL, ";\n")){
					strcpy(readers->bookName[j],tok); // store the book names in the struct
					j++;
				}
				i++;
			}
			else{ // now start to store the other information
				tok = strtok(line, ";");
				strcpy(readers->name[k],tok); // store the reader name to the struct
				p = 0;
				w = 0;				
				while((tok = strtok(NULL, ";")) != NULL){ // while reading token is not null
					if(tok==NULL){ // if the token is null
						readers->rates[p].rate[0] = 0; // then the rate is 0
						rates[x][w] = 0; // add the rate to rate matrix
					}
					else{ // else the rate is not 0
						readers->rates[p].rate[0] = atoi(tok); // add the rate to struct
						rates[x][w] = atoi(tok); // add the rate to rate matrix
					}
					w++;
					p++;
				}
				x++;
				k++;
				free(tmp); // free the temprorary token
			}
		}
	}
	return rates; // return rate matrix
}


// convert the string to upper case characters
char* upperCase(char str[Slength]){
	int i=0;
	for(i = 0; i<=strlen(str); i++){
		str[i]=toupper(str[i]); // toupper() function return the small letters to capital letters
	}
	return str;
}

// function to search for a given reader name
int searchForReader(struct reader *readers, char *reader){
	int i=0;
	while(i<structSize && (readers->name[i] != 0) && (strcmp(readers->name[i],reader)!=0)){ // while the struct is not empty and the reader name is not found
		i++;
	}
	if(readers->name[i] == 0 || i>structSize){ // if the reader name is not found
		return 0; // return 0
	}
	return i; // if found return its index
}

// function return name of reader
char * nameOfReader(struct reader *readers, int i){ // takes the struct with the index number
	return readers->name[i]; // return the name
}

// function return name of book
char * nameOfBook(struct reader *readers, int i){ // takes the struct with the index number
	return readers->bookName[i]; // return the name
}


float calcMean(int *row){
	int j, counter = 0; // variables for indexing
	float mean, ra=0.0; // variables for calculations
	for(j=0;j<8;j++){
		if(row[j] != 0){ // if the rate at a spesific row is not 0
			ra = ra + row[j]; // calculate the sum of row for mean
			counter = counter+1;
		}
	}
	mean = (ra*1.0)/counter; // calculate the mean
	return mean; // return mean
}

float coeff(int *arow,int *brow){
	int j; // variable for indexing
	float ra=0,rb=0,sumab=0,suma=0,sumb=0,coeff; // variables for calculations
	ra = calcMean(arow); // calculate mean for A row
	rb = calcMean(brow); // calculate mean for B row
	for(j=0; j< 8; j++){ // loop the cols
		if(brow[j] != 0 && arow[j] != 0){ // if the rate in A row (new user) is not 0 and the rate in B row (old user) is not 0
			sumab = sumab + ((arow[j] - ra) * (brow[j] -rb)); // calculate the sumab
			suma = suma + (arow[j] - ra)*(arow[j] - ra); // calculate the suma
			sumb = sumb + (brow[j] - rb)*(brow[j] - rb); // calculate the sumb
		}
	}
    coeff = sumab / (sqrt(suma) * sqrt(sumb)); // calculate coefficient
	return coeff; // return the similarity
}


// function to calculate pearson coefficient
void calculatePearson(int **rates, int nu_id, int k){
	int i,j, temp,x=0,w; // variables for indises
	user = (struct users*)malloc(sizeof(struct users)); // intialize the users struct
	user->id = (int*)calloc(k, sizeof(user->id));
	user->rate = (float*)calloc(k, sizeof(user->rate));
	for(i=0;i<k;i++){
		user->rate[i]=-2; // intialize the rates or similarities as -2
	}
	float cof;
	for(i=0; i< 20; i++){ // loop the rows
		cof = coeff(rates[nu_id],rates[i]); // calculate the similarity
		j=k-1;
		if(cof>user->rate[j]){ // if the similarity is bigger than -2 or the similarity in the user struct
			user->rate[j]=cof; // assign the similarity to the user struct
			user->id[j]=i; // assign the ID of the user
			j--;
		}
		// in this loop I'm pushing all the similarities & IDs in the user struct down
		while(j>=0){
			if(cof>user->rate[j]){
				user->rate[j+1]=user->rate[j];
				user->rate[j]=cof;
				temp=user->id[j];
				user->id[j+1]=user->id[j];
				user->id[j]=i;
			}	
			j--;
		}	
	}
}

// function to calculate the prediction to suggest a book
float predCalc(float ra, float sumCB, float sumCoeff){
	return ra+(sumCB/sumCoeff);
}

// function to make a prediction
float suggestion(int **rates, int newUser,int index, int k){
	float  rbp; // variable for calculations
	float ra; // variable for calculations
    float sumCoeff = 0.0, sumCB = 0.0; // variable for calculations
    float pred; // variable to hold prediction
    int i; // // variable for looping

    ra = calcMean(rates[newUser]); // calculate the mean of new user
    for(i=0;i<k;i++){ // loop till reaching the k steps
		rbp = calcMean(rates[user->id[i]]); // calculate the mean of the old user
		sumCB += user->rate[i]*(rates[user->id[i]][index] - rbp); // calculate the sum of the similarity multiplied by the mean
		sumCoeff = sumCoeff + user->rate[i]; // calculate the sum of similarities
	}
    pred = predCalc(ra, sumCB, sumCoeff); // calculate the prediction
    printf("%s\t%2.4f\n",nameOfBook(readers,index), pred); // print the prediction to the screen
	return pred; // return the prediction
}

// function to get the books to suggest
void getBook(int **rates,int newUserId,int k){
	int w,i=0,ind; // variables for indexing
	float max=-10; // variable to find the maximum suggested book
    book = (struct books*)malloc(sizeof(struct books)); // initialize the books struct
	book->id = (int*)calloc(k, sizeof(book->id));
	book->rate = (float*)calloc(k, sizeof(book->rate));
	for(i=0;i<k;i++){
		book->rate[i]=-2; // make the rates or predictions as -2
	}
	for(w=0;w<8;w++){ // loop the books
		if(rates[newUserId][w] == 0 ){ // if the new user didn't read the book
			book->rate[i]=suggestion(rates, newUserId, w,k); // make the suggestion and save it to books struct
			book->id[i] = w; // save the ID of the book
			if(book->rate[i]>max){ // here I'm finding the maximum suggested book
				max = book->rate[i];
				ind=w;
			}
			i++;
		}	
	}
	printf("\nThe best suggested book is:\n");
	printf("%s\n",nameOfBook(readers,ind)); // print the maximum suggested book
}


int main(int argc, char *argv[]) {
	int i, w=0, k, enter=0; // variables
	char reader[Slength], *username;
	char filename[Slength] = "RecomendationDataSet.csv"; // take the file name
	int **rates = NULL;
	initStruct(); // initialize the struct
	rates = createSturct(readers,filename); // get the rate matrix
	printf("Please enter name of reader:\n");
	while(enter == 0){ // this loop to make sure that the user entered a valid name
		scanf("%s", &reader);
		upperCase(reader);
		if( (i = searchForReader(readers,reader)) == 0){ // here we search for the entered name in the struct
			printf("You entered a wrong reader name, please try again.\n");
			getchar();
		}
		else{
			enter = 1; // exit when found
		}
	}
	printf("Please enter similarity factor (k):\n");
	while(scanf("%d",&k) != 1){ // check if the user enters numbers only
		printf("please enter only numbers\n");
		getchar();
	}
	printf("The simillar readers to %s are:\n", reader);
	calculatePearson(rates, i,k); // calculate pearson
	for(w=0;w<k;w++){ // here I print the users in struct of users
		username = nameOfReader(readers, user->id[w]); // here I took the name of user from the readers struct
		printf("%s \t%2.4f\n", username, user->rate[w]);
	}
	printf("\nThe suggested books for user %s:\n", reader);
	getBook(rates,i,k); // here I make the suggestion
}


