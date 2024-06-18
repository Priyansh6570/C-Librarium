#include <bits/stdc++.h>
// #include <iostream>
// #include <fstream>
// #include <vector>
// #include <algorithm>
// #include <ctime>
// #include <iomanip>
// #include <sstream>

using namespace std;

class Book
{
private:
    string title;
    string author;
    bool available;

public:
    Book(string t, string a, bool av) : title(t), author(a), available(av) {}

    string getTitle() const
    {
        return title;
    }

    string getAuthor() const
    {
        return author;
    }

    bool isAvailable() const
    {
        return available;
    }

    void setTitle(const string &t)
    {
        title = t;
    }

    void setAuthor(const string &a)
    {
        author = a;
    }

    void setAvailability(bool availability)
    {
        available = availability;
    }
};

class Student
{
private:
    string name;

public:
    Student(string n) : name(n) {}

    string getName() const
    {
        return name;
    }
};

int calculateFine(time_t issueDate)
{
    time_t currentTime = time(nullptr);
    double diffSeconds = difftime(currentTime, issueDate);
    int diffDays = static_cast<int>(diffSeconds / (24 * 60 * 60));
    int finePerDay = 10;
    int fine = finePerDay * diffDays;
    return fine;
}

time_t getIssueDate(const string &title, const string &studentName)
{
    ifstream issueFile("issued_books.txt");
    if (issueFile.is_open())
    {
        string line;
        while (getline(issueFile, line))
        {
            istringstream iss(line);
            string bookTitle, bookStudentName;
            time_t issueDate;
            if (getline(iss, bookTitle, ',') &&
                getline(iss, bookStudentName, ',') &&
                bookTitle == title && bookStudentName == studentName &&
                iss >> issueDate)
            {
                issueFile.close();
                return issueDate;
            }
        }
        issueFile.close();
    }
    return -1;
}

class Library
{
private:
    vector<Book> books;
    vector<Student> students;
    time_t currentDate;

public:
    Library()
    {
        currentDate = time(0);
        loadDataFromFile();
    }

    void addBook(const Book &book)
    {
        books.push_back(book);
        saveDataToFile();
    }

    void removeBook(const string &title)
    {
        auto it = find_if(books.begin(), books.end(), [&](const Book &b)
                          { return b.getTitle() == title; });
        if (it != books.end())
        {
            if (it->isAvailable())
            {
                books.erase(it);
                cout << "Book \"" << title << "\" removed successfully." << endl;
                saveDataToFile();
            }
            else
            {
                cout << "Cannot remove book \"" << title << "\". It is currently issued." << endl;
            }
        }
        else
        {
            cout << "Book not found." << endl;
        }
    }

    void getAllIssuedBooks() const
    {
        cout << "Issued Books:" << endl;
        for (const auto &book : books)
        {
            if (!book.isAvailable())
            {
                cout << "Title: " << book.getTitle() << ", Author: " << book.getAuthor() << endl;
            }
        }
    }

    void getOverdueBooks() const
    {
        cout << "Overdue Books:" << endl;
        for (const auto &book : books)
        {
            // if (!book.isAvailable()) {
            time_t dueDate = currentDate + 7 * 24 * 60 * 60;
            int fine = calculateFine(dueDate);
            if (fine > 0)
            {
                cout << "Title: " << book.getTitle() << ", Author: " << book.getAuthor() << ", Fine: $" << fine << endl;
            }
            // }
        }
    }

    void loadDataIfEmpty()
    {
        if (books.empty())
        {
            loadDataFromFile();
        }
    }

    void getAllBooks() const
    {
        cout << "All Books:" << endl;

        for (const auto &book : books)
        {
            cout << "Title: " << book.getTitle() << ", Author: " << book.getAuthor() << ", Availability: " << (book.isAvailable() ? "Available" : "Not Available") << endl;
        }
    }

    void updateBook(const string &title)
    {
        auto it = find_if(books.begin(), books.end(), [&](const Book &b)
                          { return b.getTitle() == title; });
        if (it != books.end())
        {
            string newTitle, newAuthor;
            cout << "Enter new title of the book: ";
            getline(cin, newTitle);
            cout << "Enter new author of the book: ";
            getline(cin, newAuthor);
            it->setTitle(newTitle);
            it->setAuthor(newAuthor);
            cout << "Book \"" << title << "\" updated successfully." << endl;
        }
        else
        {
            cout << "Book not found." << endl;
        }
    }
    void issueBook(const string &title, const string &studentName)
    {
        auto bookIt = find_if(books.begin(), books.end(), [&](const Book &b)
                              { return b.getTitle() == title && b.isAvailable(); });
        if (bookIt != books.end())
        {
            bookIt->setAvailability(false);
            cout << "Book \"" << title << "\" issued to " << studentName << endl;
            time_t currentTime = time(nullptr);

            ofstream issueFile("issued_books.txt", ios::app);
            if (issueFile.is_open())
            {
                issueFile << title << "," << studentName << "," << currentTime << endl;
                issueFile.close();
            }
            else
            {
                cout << "Creating issued_books.txt..." << endl;
                ofstream newFile("issued_books.txt");
                if (newFile.is_open())
                {
                    newFile << title << "," << studentName << "," << currentTime << endl;
                    newFile.close();
                }
                else
                {
                    cerr << "Error: Unable to create or open issued_books.txt for writing." << endl;
                }
            }
            saveDataToFile();
        }
        else
        {
            cout << "Book not available." << endl;
        }
    }

    void checkFines(const string &studentName) const
    {
        cout << "Fines for " << studentName << ":" << endl;
        for (const auto &book : books)
        {
            // if (!book.isAvailable()) {
            time_t issueDate = getIssueDate(book.getTitle(), studentName);
            if (issueDate != -1)
            {
                int fine = calculateFine(issueDate);
                if (fine > 0)
                {
                    cout << "Book: " << book.getTitle() << ", Fine Rupees " << fine << endl;
                }
            }
            // }
        }
    }

    void searchBooks(const string &query) const
    {
        cout << "Search Result " << query << ":" << endl;
        bool found = false;
        ifstream inFile("library_data.txt");
        if (inFile.is_open())
        {
            string line;
            while (getline(inFile, line))
            {
                int pos = line.find(',');
                if (pos != string::npos)
                {
                    string bTitle = line.substr(0, pos);
                    string auth = line.substr(pos + 1);
                    if (bTitle.find(query) != string::npos)
                    {
                        cout << "Title: " << bTitle << ", Author: " << auth << endl;
                        found = true;
                    }
                }
            }
            inFile.close();
        }
        if (!found)
        {
            cout << "No books found:-" << endl;
        }
    }

    void returnBook(const string &title)
    {
        ifstream inFile("library_data.txt");
        ofstream outFile("temp.txt");
        ofstream issuedBooksFile("issued_books_temp.txt");

        if (!inFile.is_open() || !outFile.is_open() || !issuedBooksFile.is_open())
        {
            cout << "Error: Unable to open files." << endl;
            return;
        }
        string line;
        while (getline(inFile, line))
        {
            stringstream ss(line);
            string bookTitle, author;
            bool available;
            getline(ss, bookTitle, ',');
            getline(ss, author, ',');
            ss >> boolalpha >> available;

            if (bookTitle == title)
            {
                available = true;
            }

            outFile << bookTitle << "," << author << "," << boolalpha << available << endl;

            for (auto &book : books)
            {
                if (book.getTitle() == title)
                {
                    book.setAvailability(true);
                    break;
                }
            }
        }
        string studentName;
        cout << "Enter student name: ";
        getline(cin, studentName);
        checkFines(studentName);
        inFile.close();
        outFile.close();
        issuedBooksFile.close();
        remove("library_data.txt");
        rename("temp.txt", "library_data.txt");
        remove("issued_books.txt");
        rename("issued_books_temp.txt", "issued_books.txt");

        cout << "Book \"" << title << "\" returned successfully." << endl;
    }

    void viewAvailableBooks() const
    {
        cout << "Available Books:" << endl;
        for (const auto &book : books)
        {
            if (book.isAvailable())
            {
                cout << "Title: " << book.getTitle() << ", Author: " << book.getAuthor() << endl;
            }
        }
    }

    void loadDataFromFile()
    {
        ifstream file("library_data.txt");
        if (file.is_open())
        {
            string line;
            while (getline(file, line))
            {
                string title, author;
                bool available;
                stringstream ss(line);
                getline(ss, title, ',');
                getline(ss, author, ',');
                ss >> boolalpha >> available;
                books.emplace_back(title, author, available);
            }
            file.close();
        }
        else
        {
            cout << "Creating new file" << endl;
            ofstream newFile("library_data.txt");
            newFile.close();
        }
    }

    void saveDataToFile()
    {
        ofstream file("library_data.txt");
        if (file.is_open())
        {
            for (const auto &book : books)
            {
                file << book.getTitle() << "," << book.getAuthor() << "," << (book.isAvailable() ? "true" : "false") << endl;
            }
            file.close();
        }
        else
        {
            cout << "Unable to open file" << endl;
        }
    }

    int calculateFine(time_t dueDate) const
    {
        const int sec_in_d = 86400;
        const int fine_per_d = 10;

        time_t currentTime = time(0);
        int overdueDays = (currentTime - dueDate) / sec_in_d;

        if (overdueDays > 0)
            return overdueDays * fine_per_d;
        else
            return 0;
    }
};

int main()
{
    Library library;

    int userType;
    do
    {
        cout << "Welcome to Library Management System" << endl;
        cout << "Please select user type:" << endl;
        cout << "1. Admin" << endl;
        cout << "2. Student" << endl;
        cout << "3. Quit" << endl;
        cout << "Enter your choice: ";

        if (!(cin >> userType))
        {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Please enter a number:" << endl;
            continue;
        }
        cin.ignore();

        switch (userType)
        {
        case 1:
        {
            int adminChoice;
            do
            {
                cout << "\nAdmin Features:" << endl;
                cout << "1) Add Book" << endl;
                cout << "2) Remove Book" << endl;
                cout << "3) Get All Issued Books" << endl;
                cout << "4) Get Overdue Books" << endl;
                cout << "5) Get All Books" << endl;
                cout << "6) Update Book" << endl;
                cout << "7) Search Book by Name" << endl;
                cout << "8) Return to Main Menu" << endl;
                cout << "Enter your choice: ";
                if (!(cin >> adminChoice))
                {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << "Invalid input. Please enter a number." << endl;
                    continue;
                }
                cin.ignore();

                switch (adminChoice)
                {
                case 1:
                {
                    string title, author;
                    cout << "Enter title of the book: ";
                    getline(cin, title);
                    cout << "Enter author of the book: ";
                    getline(cin, author);
                    library.addBook(Book(title, author, true));
                    break;
                }
                case 2:
                {
                    string title;
                    cout << "Enter title of the book to remove: ";
                    getline(cin, title);
                    library.removeBook(title);
                    break;
                }
                case 3:
                {
                    library.getAllIssuedBooks();
                    break;
                }
                case 4:
                {
                    library.getOverdueBooks();
                    break;
                }
                case 5:
                {
                    library.getAllBooks();
                    break;
                }
                case 6:
                {
                    string title;
                    cout << "Enter title of the book to update: ";
                    getline(cin, title);
                    library.updateBook(title);
                    break;
                }
                case 7:
                {
                    string query;
                    cout << "Enter search title: ";
                    getline(cin, query);
                    library.searchBooks(query);
                    break;
                }

                case 8:
                {
                    break;
                }
                default:
                    cout << "Invalid choice. Please try again." << endl;
                }
            } while (adminChoice != 8);
            break;
        }
        case 2:
        {
            int studentChoice;
            do
            {
                cout << "\nStudent Features:" << endl;
                cout << "1) View Available Books" << endl;
                cout << "2) Issue Book" << endl;
                cout << "3) Return Book" << endl;
                cout << "4) Check Fines" << endl;
                cout << "5) Return to Main Menu" << endl;
                cout << "Enter your choice: ";
                if (!(cin >> studentChoice))
                {
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << "Invalid input. Please enter a number." << endl;
                    continue;
                }
                cin.ignore();

                switch (studentChoice)
                {
                case 1:
                {
                    library.viewAvailableBooks();
                    break;
                }
                case 2:
                {
                    string title, studentName;
                    cout << "Enter title of the book to issue: ";
                    getline(cin, title);
                    cout << "Enter your name: ";
                    getline(cin, studentName);
                    library.issueBook(title, studentName);
                    break;
                }
                case 3:
                {
                    string title;
                    cout << "Enter title of the book to return: ";
                    getline(cin, title);
                    library.returnBook(title);
                    break;
                }
                case 4:
                {
                    string studentName;
                    cout << "Enter your name: ";
                    getline(cin, studentName);
                    library.checkFines(studentName);
                    break;
                }
                case 5:
                {
                    break;
                }
                default:
                    cout << "Invalid choice. Please try again." << endl;
                }
            } while (studentChoice != 5);
            break;
        }
        case 3:
        {
            cout << "Exiting:-" << endl;
            break;
        }
        default:
            cout << "Invalid choice. Please try again." << endl;
        }
    } while (userType != 3);

    return 0;
}