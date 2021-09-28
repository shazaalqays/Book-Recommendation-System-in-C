# Book-Recommendation-System-in-C
The project consists of three phases:<br/>
1.	Read the CSV file and construct the rates matrix. Here I used, “ char *strtok(char *str, const char *delim) ” function, where str − The contents of this string are modified and broken into smaller strings (tokens). And, delim − This is the C string containing the delimiters. These may vary from one call to another.<br/>
2.	Calculate the similarity by giving the new user row of rates and the old users rows of rates for a specific number of K.<br/>
3.	Make prediction for the new user to read a book that he/she didn’t read before. And print the maximum suggested book to read.<br/>

# Algorithm
```
1.	Read CSV file and construct the rates matrix.
2.	Get the new user ID and K number from user. Here we have to check whether if the entered user ID is in between the users or not.
3.	Calculate the similarity:
    a. Loop all users and one by one take both of the new user row of rates and the old user row of rates.
    b. Calculate the mean of both of rows then loop for all books check if the new user and old user have read the book than make the calculations of the equation. 
    c. After each calculation fill the users struct with the information of user ID and the similarity. In this step we fill the struct from the maximum similarity to lowest similarity.
    d. Print the users struct to view the user name and similarity of each.
4.	Predict a book:
    a. For each book that the new user that didn’t read make a suggestion or prediction.
    b. Prediction starts with calculating the mean of new user row of rates.
    c. After that for a k number we make the calculations of the given equation. Here we use the users similarities which we store it in the users struct.
    d. After getting the prediction we print the predicted books to the user. Also these information was stored in a books struct.
    e. Lastly we check for the maximum predicted book and print it to the screen.
```
