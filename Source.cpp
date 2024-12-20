#include <iostream>
#include <string>
#include <limits>
#include <fstream>
#include <chrono>
using namespace std;

struct Book {
    string name;
    string author;
    double price;
    Book* next;
};



const string FILENAME = "books_data.txt"; // file to save and load book data
const int MAX_BOOKS = 1000;
Book* booksArray[MAX_BOOKS];
int bookCount = 0;
Book* head = nullptr;



int getValidInteger();

void addBook();

void displayBooks();

void saveToFile();

void loadFromFile();

void splitList(Book* source, Book** frontRef, Book** backRef);

Book* mergeSortedByName(Book* a, Book* b);

Book* mergeSortedByAuthor(Book* a, Book* b);

Book* mergeSortedByPrice(Book* a, Book* b);

void mergeSort(Book** headRef, int sortBy);

void toArray();

Book* binarySearch(Book* books[], const string& key, int searchBy);

void removeBook();

void freeMemory();



int main() {
    loadFromFile();

    int choice;
    do {
        cout << "\nBookstore Management System" << endl;
        cout << "1. Add a Book" << endl;
        cout << "2. Display Books" << endl;
        cout << "3. Remove a Book" << endl;
        cout << "4. Sort Books" << endl;
        cout << "5. Search Book" << endl;
        cout << "6. Exit" << endl;

        cout << "Enter your choice: ";
        choice = getValidInteger();

        switch (choice) {
        case 1:
            addBook();
            break;
        case 2:
            displayBooks();
            break;
        case 3:
            removeBook();
            break;
        case 4: {
            cout << "Sort by:\n1. Name\n2. Author\n3. Price\nEnter your choice: ";
            int sortBy = getValidInteger();

            if (sortBy == 1 || sortBy == 2 || sortBy == 3) {
                mergeSort(&head, sortBy);
                cout << "Books sorted successfully!" << endl;
            }
            else {
                cout << "Invalid sorting choice." << endl;
            }
            break;
        }
        case 5: {
            if (head == nullptr) {
                cout << "No books available to search." << endl;
                break;
            }

            cout << "Search by:\n1. Name\n2. Author\nEnter your choice: ";
            int searchBy = getValidInteger();

            if (searchBy == 1 || searchBy == 2) {
                toArray();
                mergeSort(&head, searchBy);
                string key;

                if (searchBy == 1) {
                    cout << "Enter book name to search: ";
                }
                else {
                    cout << "Enter author name to search: ";
                }

                getline(cin, key);


                // start timing
                auto start = chrono::high_resolution_clock::now();
                Book* found = binarySearch(booksArray, key, searchBy);
                // end timing
                auto end = chrono::high_resolution_clock::now();

                // calculate duration in milliseconds
                chrono::duration<double, std::milli> duration = end - start;
                if (found != nullptr) {
                    cout << "Book found: Name: " << found->name << ", Author: " << found->author << ", Price: " << found->price << "\n";
                }
                else {
                    cout << "Book not found." << endl;
                }
                // display search duration
                cout << "Search took " << duration.count() << " milliseconds." << endl;
            }
            else {
                cout << "Invalid search choice." << endl;
            }
            break;
        }
        case 6:
            // save data to file before exiting
            saveToFile();

            // free memory before exiting
            freeMemory();
            cout << "Exiting And Save program" << endl;
            break;
        default:
            cout << "Invalid choice. Please try again." << endl;
        }
    } while (choice != 6);

    return 0;
}

// function to get a valid integer from the user
int getValidInteger() {
    int num;
    while (true) {
        cin >> num;
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Please enter a valid integer: ";
        }
        else {
            // consume any extra characters after the integer
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return num;
        }
    }
}

// function to add a book to the linked list
void addBook() {
    string name, author;
    double price;

    cout << "Enter book name: ";
    getline(cin, name);

    cout << "Enter author name: ";
    getline(cin, author);

    cout << "Enter price: ";
    string priceInput; // temporary string to read price input
    while (true) {
        getline(cin, priceInput);
        try {
            price = stod(priceInput);
            if (price < 0) throw invalid_argument("Negative price");
            break;
        }
        catch (...) {
            cout << "Invalid input. Please enter a valid positive number for price: ";
        }
    }


    Book* newBook = new Book{ name, author, price, nullptr };

    if (head == nullptr) {
        head = newBook;
    }
    else {
        Book* temp = head;
        while (temp->next != nullptr) {
            temp = temp->next;
        }
        temp->next = newBook;
    }

    cout << "Book added successfully!" << endl;
}

// function to display the list of books
void displayBooks() {
    if (head == nullptr) {
        cout << "No books available." << endl;
        return;
    }

    Book* temp = head;
    while (temp != nullptr) {
        cout << "Name: " << temp->name << ", Author: " << temp->author << ", Price: " << temp->price << endl;
        temp = temp->next;
    }
}

// function to save the linked list to a file
void saveToFile() {
    ofstream outFile(FILENAME);

    if (!outFile) {
        cout << "Error opening file for saving data." << endl;
        return;
    }

    Book* temp = head;
    while (temp != nullptr) {
        outFile << temp->name << "|" << temp->author << "|" << temp->price << endl;
        temp = temp->next;
    }

    outFile.close();
    cout << "Data saved successfully to " << FILENAME << endl;
}

// function to load the linked list from a file
void loadFromFile() {
    ifstream inFile(FILENAME);

    if (!inFile) {
        cout << "No saved data found. Starting with an empty list." << endl;
        return;
    }

    string name, author, priceStr;
    double price;

    while (getline(inFile, name, '|') && getline(inFile, author, '|') && getline(inFile, priceStr)) {
        price = stod(priceStr);
        Book* newBook = new Book{ name, author, price, nullptr };

        if (head == nullptr) {
            head = newBook;
        }
        else {
            Book* temp = head;
            while (temp->next != nullptr) {
                temp = temp->next;
            }
            temp->next = newBook;
        }
    }

    inFile.close();
    cout << "Data loaded successfully from " << FILENAME << endl;
}

// function to split the list into two halves
void splitList(Book* source, Book** frontRef, Book** backRef) {
    Book* fast;
    Book* slow;
    slow = source;
    fast = source->next;

    while (fast != nullptr) {
        fast = fast->next;
        if (fast != nullptr) {
            slow = slow->next;
            fast = fast->next;
        }
    }
    *frontRef = source;
    *backRef = slow->next;
    slow->next = nullptr;
}

// merge two sorted lists by name
Book* mergeSortedByName(Book* a, Book* b) {
    if (a == nullptr) return b;
    if (b == nullptr) return a;

    Book* result = nullptr;

    if (a->name <= b->name) {
        result = a;
        result->next = mergeSortedByName(a->next, b);
    }
    else {
        result = b;
        result->next = mergeSortedByName(a, b->next);
    }

    return result;
}

// merge two sorted lists by author
Book* mergeSortedByAuthor(Book* a, Book* b) {
    if (a == nullptr) return b;
    if (b == nullptr) return a;

    Book* result = nullptr;

    if (a->author <= b->author) {
        result = a;
        result->next = mergeSortedByAuthor(a->next, b);
    }
    else {
        result = b;
        result->next = mergeSortedByAuthor(a, b->next);
    }

    return result;
}

// merge two sorted lists by price
Book* mergeSortedByPrice(Book* a, Book* b) {
    if (a == nullptr) return b;
    if (b == nullptr) return a;

    Book* result = nullptr;

    if (a->price <= b->price) {
        result = a;
        result->next = mergeSortedByPrice(a->next, b);
    }
    else {
        result = b;
        result->next = mergeSortedByPrice(a, b->next);
    }

    return result;
}

// merge sort function
void mergeSort(Book** headRef, int sortBy) {
    Book* h = *headRef;
    if ((h == nullptr) || (h->next == nullptr)) {
        return;
    }

    Book* a;
    Book* b;

    splitList(h, &a, &b);

    mergeSort(&a, sortBy);
    mergeSort(&b, sortBy);

    if (sortBy == 1) {
        *headRef = mergeSortedByName(a, b);
    }
    else if (sortBy == 2) {
        *headRef = mergeSortedByAuthor(a, b);
    }
    else if (sortBy == 3) {
        *headRef = mergeSortedByPrice(a, b);
    }
}

// convert linked list to array
void toArray() {
    Book* temp = head;
    bookCount = 0;
    while (temp != nullptr && bookCount < MAX_BOOKS) {
        booksArray[bookCount] = temp;
        temp = temp->next;
        bookCount++;
    }
}

Book* binarySearch(Book* books[], const string& key, int searchBy) {
    int low = 0, high = bookCount - 1;
    int comparisons = 0;

    while (low <= high) {
        comparisons++;
        int mid = low + (high - low) / 2;

        // search by name
        if (searchBy == 1) {
            if (books[mid]->name == key) {
                cout << "Number of comparisons: " << comparisons << endl;
                return books[mid];
            }
            else if (books[mid]->name < key) {
                low = mid + 1;
            }
            else {
                high = mid - 1;
            }
        }
        // search by author
        else if (searchBy == 2) {
            if (books[mid]->author == key) {
                cout << "Number of comparisons: " << comparisons << endl;
                return books[mid];
            }
            else if (books[mid]->author < key) {
                low = mid + 1;
            }
            else {
                high = mid - 1;
            }
        }
    }

    cout << "Number of comparisons: " << comparisons << endl;
    return nullptr;
}

// function to remove a book from the list
void removeBook() {
    if (head == nullptr) {
        cout << "No books available to delete.\n";
        return;
    }

    cout << "Enter the book name to delete: ";
    string key;
    getline(cin, key);

    Book* temp = head;
    Book* prev = nullptr;
    bool found = false;

    while (temp != nullptr) {
        if (temp->name == key) {
            found = true;
            if (prev == nullptr) {
                head = temp->next;
            }
            else {
                prev->next = temp->next;
            }
            delete temp;
            cout << "Book deleted successfully!\n";
            return;
        }
        prev = temp;
        temp = temp->next;
    }

    if (!found) {
        cout << "Book not found.\n";
    }
}

// function to free memory
void freeMemory() {
    Book* temp;
    while (head != nullptr) {
        temp = head;
        head = head->next;
        // deallocate memory for the book
        delete temp;
    }
    cout << "Memory freed successfully." << endl;
}