#include <stdio.h>
#include <string.h>
#include <time.h>

//INITIALIZATION

typedef struct
{
    char book_name[50];
    char author[50];
    char genre1[20];
    char genre2[20];
    char genre3[20];
    int quantity;
}book;

// Global variables for profile
typedef struct {
    char name[50];
    char password[50];
    int age;
    char college[100];
    char branch[100];
    int books_read;
    int books_issued;
    int payment_per_month;
    int months_since_membership;
    int deposit;
    char issued_slots[3][50];
    char issue_dates[3][20];
    char due_dates[3][20];
    char read_book_names[50][50];
    char read_issue_dates[50][20];
    char read_return_dates[50][20];
} UserProfile;

typedef struct {
    char book_name[50];
    char author[50];
    char genre1[20];
    char genre2[20];
    char genre3[20];
} ReservedBook;

UserProfile user;
book books[100];
int total_records = 0;
ReservedBook reserved_books[50];
int total_reserved = 0;

void Quizonnextread();
void ProfilePage();
void DonateBook();
void IssueBook();
void ReturnBook();
void InitializeUserProfile(char username[]);
void suggestGenres();
void suggestBestsellers();
void suggestUpcoming();
int genreMatch(book b, char genre[]);
void SearchBook();
void MembershipDetails();
void CreateUser();
void DeleteUser();
int readInt();

//ACTUAL FUNCTIONS

void Quizonnextread() {
    int choice;
    printf("\n=======================================================\n");
    printf("       HI!! EXPLORE OPTIONS FOR YOUR NEXT READ!\n");
    printf("=======================================================\n");
    printf("1. Based on genres\n");
    printf("2. All-time favourites (Bestsellers of our library!)\n");
    printf("3. Unreleased books suggestions\n");
    printf("Enter choice: ");
    scanf("%d", &choice);

    switch(choice)
    {
        case 1: suggestGenres(); break;
        case 2: suggestBestsellers(); break;
        case 3: suggestUpcoming(); break;
        default: printf("Invalid option!\n");
    }
}


void InitializeUserProfile(char username[]) {
    // Initialize slots as empty
    for(int i = 0; i < 3; i++) {
        strcpy(user.issued_slots[i], "");
        strcpy(user.issue_dates[i], "");
        strcpy(user.due_dates[i], "");
    }
    for(int i = 0; i < 50; i++) {
        strcpy(user.read_book_names[i], "");
        strcpy(user.read_issue_dates[i], "");
        strcpy(user.read_return_dates[i], "");
    }
    user.books_read = 0;
    user.books_issued = 0;

    // Load user data from user_data.txt
    FILE *ufile = fopen("data\\user_data.txt", "r");
    if(ufile != NULL) {
        char uname[50], upass[50], ucollege[100], ubranch[100];
        int uage, upay, umonths, udeposit;
        while(fscanf(ufile, "%49[^,],%49[^,],%d,%99[^,],%99[^,],%d,%d,%d\n",
                     uname, upass, &uage, ucollege, ubranch, &upay, &umonths, &udeposit) == 8) {
            if(strcmp(uname, username) == 0) {
                strcpy(user.name, uname);
                strcpy(user.password, upass);
                user.age = uage;
                strcpy(user.college, ucollege);
                strcpy(user.branch, ubranch);
                user.payment_per_month = upay;
                user.months_since_membership = umonths;
                user.deposit = udeposit;
                break;
            }
        }
        fclose(ufile);
    }

    // Load this user's issued books from issued_books.txt
    FILE *issued_file = fopen("data\\issued_books.txt", "r");
    if(issued_file != NULL) {
        char uname[50], book_name[50], author[50], issue_date[20], due_date[20], status[20];
        char line[300];
        int slot = 0;
        while(fgets(line, 300, issued_file) && slot < 3) {
            int fields = sscanf(line, "%49[^,],%49[^,],%49[^,],%19[^,],%19[^,],%19[^,\n]",
                                uname, book_name, author, issue_date, due_date, status);
            if(fields == 6) {
                status[strcspn(status, "\r")] = 0;
                if(strcmp(uname, username) == 0 && strcmp(status, "issued") == 0) {
                    strcpy(user.issued_slots[slot], book_name);
                    strcpy(user.issue_dates[slot], issue_date);
                    strcpy(user.due_dates[slot], due_date);
                    user.books_issued++;
                    slot++;
                }
            }
        }
        fclose(issued_file);
    }

    // Derive books_read by counting this user's "returned" lines
    FILE *rfile = fopen("data\\issued_books.txt", "r");
    if(rfile != NULL) {
        char uname[50], book_name[50], author[50], issue_date[20], due_date[20], status[10], return_date[20];
        char line[300];
        while(fgets(line, 300, rfile)) {
            int fields = sscanf(line, "%49[^,],%49[^,],%49[^,],%19[^,],%19[^,],%9[^,\n],%19[^\n]",
                                uname, book_name, author, issue_date, due_date, status, return_date);
            if(fields >= 6 && strcmp(uname, username) == 0) {
                status[strcspn(status, "\r")] = 0;
                if(strcmp(status, "returned") == 0) {
                    if(user.books_read < 50) {
                        strcpy(user.read_book_names[user.books_read], book_name);
                        strcpy(user.read_issue_dates[user.books_read], issue_date);
                        if(fields == 7)
                            strcpy(user.read_return_dates[user.books_read], return_date);
                        else
                            strcpy(user.read_return_dates[user.books_read], "N/A");
                    }
                    user.books_read++;
                }
            }
        }
        fclose(rfile);
    }
}

void MembershipDetails() {
    printf("\n==============================\n");
    printf("    MEMBERSHIP DETAILS\n");
    printf("==============================\n\n");
    printf("Name: %s\n", user.name);
    printf("Age: %d\n", user.age);
    printf("College: %s\n", user.college);
    printf("Branch: %s\n\n", user.branch);
    printf("Payment per Month: Rs. %d\n", user.payment_per_month);
    printf("Membership Duration: %d months\n", user.months_since_membership);
    printf("Deposit Amount: Rs. %d\n", user.deposit);
}

void ProfilePage() 
{
    int choice;
    
    printf("\n==============================\n");
    printf("        PROFILE PAGE\n");
    printf("==============================\n\n");
    
    printf("Name: %s\n", user.name);
    printf("Age: %d\n", user.age);
    printf("College: %s\n", user.college);
    printf("Branch: %s\n\n", user.branch);
    
    printf("--- Select Option ---\n");
    printf("1. Books Read\n");
    printf("2. Books Issued\n");
    printf("3. Membership details\n");
    printf("4. Due date\n");
    printf("Enter choice: ");
    scanf("%d", &choice);
    
    switch(choice) {
        case 1: 
            printf("\n--- Books Read ---\n");
            if(user.books_read == 0) {
                printf("You haven't read any books yet.\n");
            } else {
                for(int i = 0; i < user.books_read && i < 50; i++) {
                    printf("\n%d. %s\n", i + 1, user.read_book_names[i]);
                    printf("   Issued On  : %s\n", user.read_issue_dates[i]);
                    printf("   Returned On: %s\n", user.read_return_dates[i]);
                }
                printf("\n------------------------------\n");
                printf("Total Books Read: %d\n", user.books_read);
            }
            break;
        case 2:
            printf("\nBooks Currently Issued: %d/3\n", user.books_issued);
            printf("\n--- Your Issued Books ---\n");
            if(user.books_issued == 0) {
                printf("No books currently issued.\n");
            } else {
                for(int i = 0; i < 3; i++) {
                    if(strlen(user.issued_slots[i]) > 0) {
                        printf("Slot %d: %s\n", i + 1, user.issued_slots[i]);
                    }
                }
            }
            break;
        case 3:
            MembershipDetails();
            break;
        case 4:
            if(user.books_issued > 0) {
                printf("\n--- Due Dates for Issued Books ---\n");
                for(int i = 0; i < 3; i++) {
                    if(strlen(user.issued_slots[i]) > 0) {
                        printf("\nBook: %s\n", user.issued_slots[i]);
                        printf("Issue Date: %s\n", user.issue_dates[i]);
                        printf("Due Date: %s\n", user.due_dates[i]);
                        printf("----------------------------\n");
                    }
                }
            } else {
                printf("\nNo books currently issued.\n");
            }
            break;
        default:
            printf("\nInvalid choice!\n");
    }
}
    
void IssueBook() 
{
    char book_name[50], author[50];
    int found = 0;
    int book_index = -1;
    
    printf("\n==============================\n");
    printf("      ISSUE A BOOK\n");
    printf("==============================\n\n");
    
    // Check if user has reached the 3 book limit
    if(user.books_issued >= 3) {
        printf("You have already issued 3 books (maximum limit).\n");
        printf("Please return a book before issuing a new one.\n");
        return;
    }
    
    printf("Enter Book Name: ");
    getchar();
    fgets(book_name, 50, stdin);
    book_name[strcspn(book_name, "\n")] = 0;
    
    printf("Enter Author: ");
    fgets(author, 50, stdin);
    author[strcspn(author, "\n")] = 0;
    
    // Check if book exists in library
    for(int i = 0; i < total_records; i++) {
        if(strcmp(books[i].book_name, book_name) == 0 && 
           strcmp(books[i].author, author) == 0) {
            found = 1;
            book_index = i;
            break;
        }
    }
    
    // If book doesn't exist
    if(!found) {
        printf("\nWe're sorry, the book you want is not a part of our library!\n");
        return;
    }
    
    // Check if book is available
    if(books[book_index].quantity <= 0) {
        printf("\nWe're sorry, the book you want isnt available in our library!\n");
        return;
    }
    
    // Find an empty slot
    int slot_index = -1;
    for(int i = 0; i < 3; i++) {
        if(strlen(user.issued_slots[i]) == 0) {
            slot_index = i;
            break;
        }
    }
    
    // GET CURRENT TIME
    time_t current_time = time(NULL);
    struct tm *time_info = localtime(&current_time);
    
    // Format issue date
    char issue_date[20];
    strftime(issue_date, 20, "%d-%m-%Y", time_info);
    
    // Calculate due date (7 days later)
    time_t due_time = current_time + (7 * 24 * 60 * 60);
    struct tm *due_info = localtime(&due_time);
    
    char due_date[20];
    strftime(due_date, 20, "%d-%m-%Y", due_info);
    
    // Issue the book
    strcpy(user.issued_slots[slot_index], book_name);
    strcpy(user.issue_dates[slot_index], issue_date);
    strcpy(user.due_dates[slot_index], due_date);
    books[book_index].quantity--;
    user.books_issued++;
    
    // Add to issued_books.txt file
    FILE *issued_file = fopen("data\\issued_books.txt", "a");
    if(issued_file != NULL) {
        fprintf(issued_file, "%s,%s,%s,%s,%s,issued\n", user.name, book_name, author, issue_date, due_date);
        fclose(issued_file);
    }
    
    // Update dataset file to persist quantity change
    FILE *dataset_file = fopen("data\\FOP Assignment Sem 2 DATASET.txt", "w");
    if(dataset_file != NULL) {
        for(int i = 0; i < total_records; i++) {
            fprintf(dataset_file, "%s,%s,%s,%s,%s,%d\n",
                    books[i].book_name, books[i].author,
                    books[i].genre1, books[i].genre2,
                    books[i].genre3, books[i].quantity);
        }
        fclose(dataset_file);
    }
    
    printf("\nBook issued successfully!\n");
    printf("Book: %s by %s\n", book_name, author);
    printf("Slot: %d/3\n", slot_index + 1);
    printf("Issue Date: %s\n", issue_date);
    printf("Due Date: %s (7 days from today)\n", due_date);
    printf("Remaining Stock: %d\n", books[book_index].quantity);
}
    
void ReturnBook() 
{
    char book_name[50];
    int found = 0;
    int slot_index = -1;
    
    printf("\n==============================\n");
    printf("      RETURN A BOOK\n");
    printf("==============================\n\n");
    
    if(user.books_issued == 0) {
        printf("You have no books currently issued.\n");
        return;
    }
    
    printf("Your Issued Books:\n");
    for(int i = 0; i < 3; i++) {
        if(strlen(user.issued_slots[i]) > 0) {
            printf("%d. %s\n", i + 1, user.issued_slots[i]);
        }
    }
    
    printf("\nEnter Book Name to return: ");
    getchar();
    fgets(book_name, 50, stdin);
    book_name[strcspn(book_name, "\n")] = 0;
    
    // Check if user has this book issued
    for(int i = 0; i < 3; i++) {
        if(strcmp(user.issued_slots[i], book_name) == 0) {
            found = 1;
            slot_index = i;
            break;
        }
    }
    
    if(!found) {
        printf("\nX You haven't issued this book.\n");
        return;
    }
    
    // Find book in library and increase quantity
    for(int i = 0; i < total_records; i++) {
        if(strcmp(books[i].book_name, book_name) == 0) {
            books[i].quantity++;
            break;
        }
    }

    // Get today's date as return date
    time_t now = time(NULL);
    struct tm *now_info = localtime(&now);
    char return_date[20];
    strftime(return_date, 20, "%d-%m-%Y", now_info);

    // Add to books read list in memory
    if(user.books_read < 50) {
        strcpy(user.read_book_names[user.books_read], book_name);
        strcpy(user.read_issue_dates[user.books_read], user.issue_dates[slot_index]);
        strcpy(user.read_return_dates[user.books_read], return_date);
    }
    user.books_read++;

    // Remove from user slots
    strcpy(user.issued_slots[slot_index], "");
    strcpy(user.issue_dates[slot_index], "");
    strcpy(user.due_dates[slot_index], "");
    user.books_issued--;
    
    // Update issued_books.txt - change this user's book status from issued to returned
    FILE *issued_file = fopen("data\\issued_books.txt", "r");
    FILE *temp_file = fopen("data\\temp_issued.txt", "w");

    if(issued_file != NULL && temp_file != NULL) {
        char line[300];
        char uname[50], file_book[50], file_author[50], file_issue_date[20], file_due_date[20], file_status[20], file_return_date[20];

        while(fgets(line, 300, issued_file)) {
            int fields = sscanf(line, "%49[^,],%49[^,],%49[^,],%19[^,],%19[^,],%19[^,\n],%19[^\n]",
                                uname, file_book, file_author, file_issue_date, file_due_date, file_status, file_return_date);
            if(fields < 6) continue;
            file_status[strcspn(file_status, "\r")] = 0;

            if(strcmp(uname, user.name) == 0 && strcmp(file_book, book_name) == 0 && strcmp(file_status, "issued") == 0) {
                fprintf(temp_file, "%s,%s,%s,%s,%s,returned,%s\n", uname, file_book, file_author, file_issue_date, file_due_date, return_date);
            } else if(fields == 7) {
                file_return_date[strcspn(file_return_date, "\r")] = 0;
                fprintf(temp_file, "%s,%s,%s,%s,%s,%s,%s\n", uname, file_book, file_author, file_issue_date, file_due_date, file_status, file_return_date);
            } else {
                fprintf(temp_file, "%s,%s,%s,%s,%s,%s\n", uname, file_book, file_author, file_issue_date, file_due_date, file_status);
            }
        }

        fclose(issued_file);
        fclose(temp_file);

        remove("data\\issued_books.txt");
        rename("data\\temp_issued.txt",
               "data\\issued_books.txt");
    }
    
    // Update dataset file to persist quantity change
    FILE *dataset_file = fopen("data\\FOP Assignment Sem 2 DATASET.txt", "w");
    if(dataset_file != NULL) {
        for(int i = 0; i < total_records; i++) {
            fprintf(dataset_file, "%s,%s,%s,%s,%s,%d\n",
                    books[i].book_name, books[i].author,
                    books[i].genre1, books[i].genre2,
                    books[i].genre3, books[i].quantity);
        }
        fclose(dataset_file);
    }
    
    printf("\nCheck mark Book returned successfully!\n");
    printf("Thank you for returning: %s\n", book_name);
    printf("Books read so far: %d\n", user.books_read);
}
    
void DonateBook() 
{
    char book_name[50], author[50], genre1[20], genre2[20], genre3[20];
    int found = 0;
    
    printf("\n==============================\n");
    printf("      DONATE A BOOK\n");
    printf("==============================\n\n");
    
    printf("Enter Book Name: ");
    getchar();  // Clear buffer
    fgets(book_name, 50, stdin);
    book_name[strcspn(book_name, "\n")] = 0;  // Remove newline
    
    printf("Enter Author Name: ");
    fgets(author, 50, stdin);
    author[strcspn(author, "\n")] = 0;
    
    printf("Enter Genre 1: ");
    fgets(genre1, 20, stdin);
    genre1[strcspn(genre1, "\n")] = 0;
    
    printf("Enter Genre 2: ");
    fgets(genre2, 20, stdin);
    genre2[strcspn(genre2, "\n")] = 0;
    
    printf("Enter Genre 3: ");
    fgets(genre3, 20, stdin);
    genre3[strcspn(genre3, "\n")] = 0;
    
    // Check if book already exists
    for(int i = 0; i < total_records; i++) {
        if(strcmp(books[i].book_name, book_name) == 0 && 
           strcmp(books[i].author, author) == 0 &&
       strcmp(books[i].genre1, genre1) == 0 &&
       strcmp(books[i].genre2, genre2) == 0 &&
       strcmp(books[i].genre3, genre3) == 0){
            books[i].quantity++;
            found = 1;
            printf("\nBook already exists! Quantity increased to %d.\n", books[i].quantity);
            break;
        }
    }
    
    // If book doesn't exist, add new entry
    if(!found) {
        strcpy(books[total_records].book_name, book_name);
        strcpy(books[total_records].author, author);
        strcpy(books[total_records].genre1, genre1);
        strcpy(books[total_records].genre2, genre2);
        strcpy(books[total_records].genre3, genre3);
        books[total_records].quantity = 1;
        total_records++;
        printf("\nNew book added to the library!Thank you for donating!\n");
    }
    
    // Update the file
    FILE *file = fopen("data\\FOP Assignment Sem 2 DATASET.txt", "w");
    if(file == NULL) {
        printf("Error updating file.\n");
        return;
    }
    
    for(int i = 0; i < total_records; i++) {
        fprintf(file, "%s,%s,%s,%s,%s,%d\n",
                books[i].book_name,
                books[i].author,
                books[i].genre1,
                books[i].genre2,
                books[i].genre3,
                books[i].quantity);
    }
    
    fclose(file);
    printf("Library database updated successfully!\n");
}

int genreMatch(book b, char genre[])
{
    if(strcmp(b.genre1, genre) == 0) return 1;
    if(strcmp(b.genre2, genre) == 0) return 1;
    if(strcmp(b.genre3, genre) == 0) return 1;
    return 0;
}

void suggestGenres()
{
    char fav[20];
    printf("Enter a genre you like: (First letter should be capital)");
    scanf("%s", fav);

    printf("\nBooks you may like:\n");
    for(int i = 0; i < total_records; i++)
    {
        if(genreMatch(books[i], fav))
        {
            printf("%s by %s\n", books[i].book_name, books[i].author);
        }
    }
}
void suggestBestsellers()
{
    printf("\nPopular Bestsellers:\n");
    for(int i = 0; i < total_records; i++)
    {
        if(books[i].quantity ==0)
        {
            printf("%s by %s\n", books[i].book_name, books[i].author);
        }
    }
}

void suggestUpcoming()
{
    printf("\nUpcoming Books:\n");
    printf("The Midnight Train by Matt Haig, based on time travel Coming Soon!!\n");
    printf("Vigil by George Saunders, An electric novel from the #1 New York Times bestselling Releasing in 2026\n");
    printf("Cool Machine by Colson Whitehead, a novel that brings to life 1980s New York in the magnificent final volume of his Harlem Trilogy\n");
}

void SearchBook()
{
    int filter_choice;
    char search_term[50];
    int found = 0;
    
    printf("\n==============================\n");
    printf("      SEARCH YOUR BOOK\n");
    printf("==============================\n\n");
    printf("Filter based on:\n");
    printf("1. Author\n");
    printf("2. Name\n");
    printf("3. Genre\n");
    printf("Enter choice: ");
    scanf("%d", &filter_choice);
    
    getchar(); // Clear extra \n due to enter
    
    switch(filter_choice)
    {
        case 1: // Search by Author
            printf("\nEnter Author Name: ");
            fgets(search_term, 50, stdin);
            search_term[strcspn(search_term, "\n")] = 0;
            
            printf("\n--- Search Results ---\n");
            for(int i = 0; i < total_records; i++)
            {
                if(strcmp(books[i].author, search_term) == 0)
                {
                    found = 1;
                    printf("\nBook Name: %s\n", books[i].book_name);
                    printf("Author: %s\n", books[i].author);
                    printf("Genres: %s, %s, %s\n", books[i].genre1, books[i].genre2, books[i].genre3);
                    
                    if(books[i].quantity >= 1)
                        printf("Availability: Available (Stock: %d)\n", books[i].quantity);
                    else
                        printf("Availability: Not Available\n");
                    
                    printf("----------------------------\n");
                }
            }
            
            if(!found)
                printf("\nWe're sorry, %s is not a part of our library \n", search_term);
            break;
            
        case 2: // Search by Name
            printf("\nEnter Book Name: ");
            fgets(search_term, 50, stdin);
            search_term[strcspn(search_term, "\n")] = 0;

            
            printf("\n--- Search Results ---\n");
            for(int i = 0; i < total_records; i++)
            {
                if(strcmp(books[i].book_name, search_term) == 0)
                {
                    found = 1;
                    printf("\nBook Name: %s\n", books[i].book_name);
                    printf("Author: %s\n", books[i].author);
                    printf("Genres: %s, %s, %s\n", books[i].genre1, books[i].genre2, books[i].genre3);
                    
                    if(books[i].quantity >= 1)
                        printf("Availability: Available (Stock: %d)\n", books[i].quantity);
                    else
                        printf("Availability: Not Available, the book you want has already been booked!\n");
                    
                    printf("----------------------------\n");
                }
            }
            
            if(!found)
                printf("\nBook not found: %s\n", search_term);
            break;
            
        case 3: // Search by Genre
            printf("\nEnter Genre: ");
            fgets(search_term, 50, stdin);
            search_term[strcspn(search_term, "\n")] = 0;
            
            printf("\n--- Search Results ---\n");
            for(int i = 0; i < total_records; i++)
            {
                if(genreMatch(books[i], search_term))
                {
                    found = 1;
                    printf("\nBook Name: %s\n", books[i].book_name);
                    printf("Author: %s\n", books[i].author);
                    printf("Genres: %s, %s, %s\n", books[i].genre1, books[i].genre2, books[i].genre3);
                    
                    if(books[i].quantity >= 1)
                        printf("Availability: Available (Stock: %d)\n", books[i].quantity);
                    else
                        printf("Availability: Not Available, the book you want has already been booked!\n");
                    
                    printf("----------------------------\n");
                }
            }
            
            if(!found)
                printf("\nWe're sorry, %s is not a part of our library \n", search_term);
            break;
            
        default:
            printf("\nInvalid choice!\n");
    }
}


int readInt() {
    int val;
    while(scanf("%d", &val) != 1 || val < 0) {
        while(getchar() != '\n');
        printf("Invalid input (enter a positive whole number): ");
    }
    return val;
}

void CreateUser() {
    char uname[50], upass[50], ucollege[100], ubranch[100];
    int uage, upay, umonths, udeposit;

    printf("\n==============================\n");
    printf("        CREATE USER\n");
    printf("==============================\n\n");

    printf("Enter Username: ");
    getchar();
    fgets(uname, 50, stdin);
    uname[strcspn(uname, "\n")] = 0;

    printf("Enter Password: ");
    fgets(upass, 50, stdin);
    upass[strcspn(upass, "\n")] = 0;

    // Check if username already exists
    FILE *check = fopen("data\\user_data.txt", "r");
    if(check != NULL) {
        char existing_name[50], line[300];
        while(fgets(line, 300, check)) {
            sscanf(line, "%49[^,]", existing_name);
            if(strcmp(existing_name, uname) == 0) {
                printf("User '%s' already exists!\n", uname);
                fclose(check);
                return;
            }
        }
        fclose(check);
    }

    printf("Enter Age: ");
    uage = readInt();
    getchar();

    printf("Enter College: ");
    fgets(ucollege, 100, stdin);
    ucollege[strcspn(ucollege, "\n")] = 0;

    printf("Enter Branch: ");
    fgets(ubranch, 100, stdin);
    ubranch[strcspn(ubranch, "\n")] = 0;

    printf("Enter Payment per Month: ");
    upay = readInt();

    printf("Enter Months since Membership: ");
    umonths = readInt();

    printf("Enter Deposit Amount: ");
    udeposit = readInt();

    FILE *ufile = fopen("data\\user_data.txt", "r");
    if(ufile != NULL) {
        fseek(ufile, -1, SEEK_END);
        int last = fgetc(ufile);
        fclose(ufile);
        if(last != '\n') {
            FILE *fix = fopen("data\\user_data.txt", "a");
            fprintf(fix, "\n");
            fclose(fix);
        }
    }
    ufile = fopen("data\\user_data.txt", "a");
    if(ufile != NULL) {
        fprintf(ufile, "%s,%s,%d,%s,%s,%d,%d,%d\n", uname, upass, uage, ucollege, ubranch, upay, umonths, udeposit);
        fclose(ufile);
        printf("\nUser '%s' created successfully!\n", uname);
    } else {
        printf("Error creating user.\n");
    }
}

void DeleteUser() {
    char uname[50], upass[50];

    printf("\n==============================\n");
    printf("        DELETE USER\n");
    printf("==============================\n\n");

    printf("Enter Username to delete: ");
    getchar();
    fgets(uname, 50, stdin);
    uname[strcspn(uname, "\n")] = 0;

    printf("Enter Password: ");
    fgets(upass, 50, stdin);
    upass[strcspn(upass, "\n")] = 0;

    int found = 0;

    // First check if username AND password match
    FILE *check = fopen("data\\user_data.txt", "r");
    if(check != NULL) {
        char line[300], existing_name[50], existing_pass[50];
        while(fgets(line, 300, check)) {
            sscanf(line, "%49[^,],%49[^,]", existing_name, existing_pass);
            if(strcmp(existing_name, uname) == 0 && strcmp(existing_pass, upass) == 0) {
                found = 1;
                break;
            }
        }
        fclose(check);
    }

    if(!found) {
        printf("Invalid username or password.\n");
        return;
    }

    // Remove from user_data.txt
    FILE *ufile = fopen("data\\user_data.txt", "r");
    FILE *temp = fopen("data\\temp_users.txt", "w");
    if(ufile != NULL && temp != NULL) {
        char line[300], existing_name[50], existing_pass[50];
        while(fgets(line, 300, ufile)) {
            sscanf(line, "%49[^,],%49[^,]", existing_name, existing_pass);
            if(strcmp(existing_name, uname) == 0 && strcmp(existing_pass, upass) == 0) {
                continue; // skip this user
            }
            fputs(line, temp);
        }
        fclose(ufile);
        fclose(temp);
        remove("data\\user_data.txt");
        rename("data\\temp_users.txt",
               "data\\user_data.txt");
    }

    // Remove their entries from issued_books.txt
    FILE *ifile = fopen("data\\issued_books.txt", "r");
    FILE *itemp = fopen("data\\temp_issued.txt", "w");
    if(ifile != NULL && itemp != NULL) {
        char line[300], existing[50];
        while(fgets(line, 300, ifile)) {
            sscanf(line, "%49[^,]", existing);
            if(strcmp(existing, uname) != 0) {
                fputs(line, itemp);
            }
        }
        fclose(ifile);
        fclose(itemp);
        remove("data\\issued_books.txt");
        rename("data\\temp_issued.txt",
               "data\\issued_books.txt");
    }

    printf("User '%s' deleted successfully.\n", uname);
}

//MAIN

//READING FILE

int main()
{
    FILE *file;
    file = fopen("data\\FOP Assignment Sem 2 DATASET.txt", "r");
    
    if(file == NULL)
    {
        printf("Error opening the file.\n");
        return 1;
    }
    
    int read = 0;
    
    do
    {
        read = fscanf(file,
                      "%49[^,],%19[^,],%19[^,],%19[^,],%19[^,],%d\n",
                      books[total_records].book_name,
                      books[total_records].author,
                      books[total_records].genre1,
                      books[total_records].genre2,
                      books[total_records].genre3,
                      &books[total_records].quantity);

    
        if(read == 6)
        {
            total_records++;
        }
        if(read != 6 && !feof(file))
        {
            printf("File format incorrect.\n");
            return 1;
        }
        if(ferror(file))
        {
            printf("Error reading file.\n");
            return 1;
        }
    }while(!feof(file));
    
    fclose(file);
    printf("\n%d records read.\n\n", total_records);
    for(int i = 0; i < total_records; i++)
    {
        printf("%s %s %s %s %s %d\n", 
        books[i].book_name,
        books[i].author,
        books[i].genre1,
        books[i].genre2,
        books[i].genre3,
        books[i].quantity);
    }
    printf("\n");

    // Initialize user profile
    // (called after login below)

//STARTUP MENU

    int startup_choice;
    do {
        printf("\n==============================\n");
        printf(" LIBRARY MANAGEMENT SYSTEM\n");
        printf("==============================\n");
        printf("1. Login\n");
        printf("2. Create User\n");
        printf("3. Delete User\n");
        printf("4. Exit\n");
        printf("Enter choice: ");
        if(scanf("%d", &startup_choice) != 1) {
            while(getchar() != '\n');
            startup_choice = -1;
        }

        switch(startup_choice) {
            case 1: break; // proceed to login below
            case 2: CreateUser(); break;
            case 3: DeleteUser(); break;
            case 4: printf("Goodbye!\n"); return 0;
            default: printf("Invalid choice.\n");
        }
    } while(startup_choice != 1);

//LOGIN

    char username[50], password[50];
    int attempts = 3;
    int logged_in = 0;
    int choice;

    while(attempts > 0) {
        printf("\n==============================\n");
        printf("WELCOME TO LIBRARY MANAGEMENT SYSTEM!\n");
        printf("STUDENT LOGIN\n");
        printf("==============================\n");

        printf("Username: ");
        scanf("%49s", username);
        printf("Password: ");
        scanf("%49s", password);

        // Check credentials against user_data.txt
        FILE *ufile = fopen("data\\user_data.txt", "r");
        if(ufile != NULL) {
            char uname[50], upass[50], rest[300];
            while(fscanf(ufile, "%49[^,],%49[^,],%299[^\n]\n", uname, upass, rest) == 3) {
                if(strcmp(uname, username) == 0 && strcmp(upass, password) == 0) {
                    logged_in = 1;
                    break;
                }
            }
            fclose(ufile);
        } else {
            printf("Error: user_data.txt not found.\n");
            return 1;
        }

        if(logged_in) {
            printf("\nLogin Successful! Welcome, %s!\n", username);
            InitializeUserProfile(username);
            break;
        } else {
            attempts--;
            printf("\nInvalid username or password!");
            printf("\nAttempts left: %d\n", attempts);
        }
    }

    if(!logged_in) {
        printf("\nToo many failed attempts. Access denied.\n");
        return 0;
    }

    // MAIN MENU
    do {
        printf("\n----------- MAIN MENU ---------------\n");
        printf("1. Explore options for your next read!\n");
        printf("2. Profile Page\n");
        printf("3. Donate a Book\n");
        printf("4. Issue a Book\n");
        printf("5. Return your Book\n");
        printf("6. Search your Book\n");
        printf("7. Exit\n");
        printf("Enter choice: ");
        if(scanf("%d", &choice) != 1) {
            while(getchar() != '\n');
            choice = -1;
        }

        switch (choice) {
            case 1: Quizonnextread(); break;
            case 2: ProfilePage(); break;
            case 3: DonateBook(); break;
            case 4: IssueBook(); break;
            case 5: ReturnBook(); break;
            case 6: SearchBook(); break;
            case 7: printf("Thank you for being a part of our library!!\n"); break;
            default: printf("Invalid choice\n");
        }
    } while (choice != 7);

    return 0;
}
