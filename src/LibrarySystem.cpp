#include "LibrarySystem.h"
#include "LibraryException.h"
#include <algorithm>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>

using namespace std;

static vector<string> splitLine(const string& text, char delimiter) {
    vector<string> parts;
    string part;
    stringstream ss(text);
    while (getline(ss, part, delimiter)) {
        parts.push_back(part);
    }
    return parts;
}

static bool isValidEmail(const string& email) {
    size_t atPosition = email.find('@');
    size_t dotPosition = email.find('.', atPosition == string::npos ? 0 : atPosition);
    return atPosition != string::npos && dotPosition != string::npos &&
           atPosition > 0 && dotPosition > atPosition + 1 &&
           dotPosition < email.length() - 1;
}

LibrarySystem::LibrarySystem()
    : nextItemId(101), nextRecordId(1), catalogFile("data/catalog.txt"),
      usersFile("data/users.txt"), recordsFile("data/borrow_records.txt") {
    loadData();
}

LibrarySystem::~LibrarySystem() {
    saveData();
}

void LibrarySystem::run() {
    try {
        mainMenu();
    } catch (const LibraryException& e) {
        cout << "\nSystem error: " << e.what() << endl;
    }
}

void LibrarySystem::loadData() {
    loadCatalog();
    loadUsers();
    loadRecords();

    if (admins.empty()) {
        admins.push_back(Admin("admin", "admin@library.com", "admin123"));
        cout << "Default admin created: username=admin password=admin123\n";
    }
}

void LibrarySystem::saveData() const {
    saveCatalog();
    saveUsers();
    saveRecords();
}

void LibrarySystem::loadCatalog() {
    ifstream file(catalogFile.c_str());
    string line;
    while (getline(file, line)) {
        if (line.empty()) {
            continue;
        }
        unique_ptr<Item> item = createItemFromFileLine(line);
        nextItemId = max(nextItemId, item->getId() + 1);
        items.push_back(move(item));
    }
}

void LibrarySystem::loadUsers() {
    ifstream file(usersFile.c_str());
    string line;
    while (getline(file, line)) {
        if (line.empty()) {
            continue;
        }
        vector<string> p = splitLine(line, '|');
        if (p.size() < 4) {
            throw LibraryException("Invalid user data found.");
        }
        if (p[0] == "Admin") {
            admins.push_back(Admin(p[1], p[2], p[3]));
        } else if (p[0] == "Member") {
            members.push_back(Member(p[1], p[2], p[3]));
        }
    }
}

void LibrarySystem::loadRecords() {
    ifstream file(recordsFile.c_str());
    string line;
    while (getline(file, line)) {
        if (line.empty()) {
            continue;
        }
        BorrowRecord record = BorrowRecord::fromFileLine(line);
        nextRecordId = max(nextRecordId, record.getRecordId() + 1);
        records.push_back(record);
    }
}

void LibrarySystem::saveCatalog() const {
    ofstream file(catalogFile.c_str());
    for (size_t i = 0; i < items.size(); ++i) {
        file << items[i]->toFileString() << endl;
    }
}

void LibrarySystem::saveUsers() const {
    ofstream file(usersFile.c_str());
    for (size_t i = 0; i < admins.size(); ++i) {
        file << admins[i].toFileString() << endl;
    }
    for (size_t i = 0; i < members.size(); ++i) {
        file << members[i].toFileString() << endl;
    }
}

void LibrarySystem::saveRecords() const {
    ofstream file(recordsFile.c_str());
    for (size_t i = 0; i < records.size(); ++i) {
        file << records[i].toFileString() << endl;
    }
}

void LibrarySystem::mainMenu() {
    while (true) {
        cout << "\n============================================\n";
        cout << "        LIBRARY BORROWING SYSTEM\n";
        cout << "============================================\n";
        cout << "1. Enter as Admin\n";
        cout << "2. Enter as Member\n";
        cout << "0. Exit\n";
        int choice = readInt("Choose option: ");

        if (choice == 1) {
            adminEntrance();
        } else if (choice == 2) {
            memberEntrance();
        } else if (choice == 0) {
            saveData();
            cout << "Data saved. Goodbye!\n";
            break;
        } else {
            cout << "Invalid option. Please try again.\n";
        }
    }
}

void LibrarySystem::adminEntrance() {
    while (true) {
        cout << "\n---------- Admin Entrance ----------\n";
        cout << "1. Sign up as Admin\n";
        cout << "2. Log in as Admin\n";
        cout << "0. Back\n";
        int choice = readInt("Choose option: ");

        if (choice == 1) {
            signupAdmin();
        } else if (choice == 2) {
            Admin* admin = loginAdmin();
            if (admin != nullptr) {
                adminMenu(*admin);
            }
        } else if (choice == 0) {
            return;
        } else {
            cout << "Invalid option.\n";
        }
    }
}

void LibrarySystem::memberEntrance() {
    while (true) {
        cout << "\n---------- Member Entrance ----------\n";
        cout << "1. Sign up as New Member\n";
        cout << "2. Log in as Member\n";
        cout << "0. Back\n";
        int choice = readInt("Choose option: ");

        if (choice == 1) {
            signupMember();
        } else if (choice == 2) {
            Member* member = loginMember();
            if (member != nullptr) {
                memberMenu(*member);
            }
        } else if (choice == 0) {
            return;
        } else {
            cout << "Invalid option.\n";
        }
    }
}

void LibrarySystem::adminMenu(Admin& admin) {
    while (true) {
        cout << "\n========== Admin Menu (" << admin.getUsername() << ") ==========\n";
        cout << "1. Manage Library Items\n";
        cout << "2. Manage Members\n";
        cout << "3. View Borrowing Records\n";
        cout << "4. Save Data Now\n";
        cout << "0. Back / Logout\n";
        int choice = readInt("Choose option: ");

        if (choice == 1) {
            adminCatalogMenu();
        } else if (choice == 2) {
            adminMemberMenu();
        } else if (choice == 3) {
            adminRecordMenu();
            pause();
        } else if (choice == 4) {
            saveData();
            cout << "All data saved successfully.\n";
        } else if (choice == 0) {
            return;
        } else {
            cout << "Invalid option.\n";
        }
    }
}

void LibrarySystem::memberMenu(Member& member) {
    while (true) {
        cout << "\n========== Member Menu (" << member.getUsername() << ") ==========\n";
        cout << "1. View All Library Items\n";
        cout << "2. Search Items\n";
        cout << "3. Borrow Item\n";
        cout << "4. Return Item\n";
        cout << "5. View My Borrowing Records\n";
        cout << "0. Back / Logout\n";
        int choice = readInt("Choose option: ");

        if (choice == 1) {
            viewAllItems();
            pause();
        } else if (choice == 2) {
            searchItems();
            pause();
        } else if (choice == 3) {
            borrowItem(member);
        } else if (choice == 4) {
            returnItem(member);
        } else if (choice == 5) {
            viewMyRecords(member);
            pause();
        } else if (choice == 0) {
            return;
        } else {
            cout << "Invalid option.\n";
        }
    }
}

void LibrarySystem::adminCatalogMenu() {
    while (true) {
        cout << "\n---------- Manage Library Items ----------\n";
        cout << "1. Add Item\n";
        cout << "2. Edit Item\n";
        cout << "3. Delete Item\n";
        cout << "4. View All Items\n";
        cout << "5. Search Items\n";
        cout << "0. Back\n";
        int choice = readInt("Choose option: ");

        if (choice == 1) {
            addItem();
        } else if (choice == 2) {
            editItem();
        } else if (choice == 3) {
            deleteItem();
        } else if (choice == 4) {
            viewAllItems();
            pause();
        } else if (choice == 5) {
            searchItems();
            pause();
        } else if (choice == 0) {
            return;
        } else {
            cout << "Invalid option.\n";
        }
    }
}

void LibrarySystem::adminMemberMenu() {
    while (true) {
        cout << "\n---------- Manage Members ----------\n";
        cout << "1. Add Member\n";
        cout << "2. Edit Member\n";
        cout << "3. Delete Member\n";
        cout << "4. View Members\n";
        cout << "0. Back\n";
        int choice = readInt("Choose option: ");

        if (choice == 1) {
            addMemberByAdmin();
        } else if (choice == 2) {
            editMemberByAdmin();
        } else if (choice == 3) {
            deleteMemberByAdmin();
        } else if (choice == 4) {
            viewMembers();
            pause();
        } else if (choice == 0) {
            return;
        } else {
            cout << "Invalid option.\n";
        }
    }
}

void LibrarySystem::adminRecordMenu() const {
    cout << "\n---------- Borrowing Records ----------\n";
    cout << left << setw(6) << "ID" << setw(16) << "Member" << setw(8) << "Item"
         << setw(28) << "Title" << setw(13) << "Borrowed" << setw(13)
         << "Due" << setw(13) << "Returned" << "Status" << endl;
    cout << string(110, '-') << endl;
    for (size_t i = 0; i < records.size(); ++i) {
        records[i].display();
    }
    if (records.empty()) {
        cout << "No borrowing records found.\n";
    }
}

void LibrarySystem::addItem() {
    cout << "\nItem Type\n";
    cout << "1. Book\n";
    cout << "2. Magazine\n";
    cout << "3. Journal\n";
    cout << "0. Back\n";
    int typeChoice = readInt("Choose type: ");
    if (typeChoice == 0) {
        return;
    }

    if (typeChoice == 1) {
        string title = readLine("Title: ");
        string author = readLine("Author: ");
        int year = readInt("Year: ");
        items.push_back(unique_ptr<Item>(new Book(nextItemId++, title, author, year)));
    } else if (typeChoice == 2) {
        string title = readLine("Title: ");
        string publisher = readLine("Publisher: ");
        string month = readLine("Month: ");
        items.push_back(unique_ptr<Item>(new Magazine(nextItemId++, title, publisher, month)));
    } else if (typeChoice == 3) {
        string title = readLine("Title: ");
        string editor = readLine("Editor: ");
        int year = readInt("Year: ");
        items.push_back(unique_ptr<Item>(new Journal(nextItemId++, title, editor, year)));
    } else {
        cout << "Invalid item type.\n";
        return;
    }

    saveCatalog();
    cout << "Item added successfully.\n";
}

void LibrarySystem::editItem() {
    viewAllItems();
    int id = readInt("Enter item ID to edit (0 to back): ");
    if (id == 0) {
        return;
    }

    Item* item = findItemById(id);
    if (item == nullptr) {
        cout << "Item not found.\n";
        return;
    }

    while (true) {
        cout << "\nEditing: " << item->getTitle() << "\n";
        cout << "1. Title\n";
        if (item->getType() == "Book") {
            cout << "2. Author\n";
            cout << "3. Year\n";
        } else if (item->getType() == "Journal") {
            cout << "2. Editor\n";
            cout << "3. Year\n";
        } else if (item->getType() == "Magazine") {
            cout << "2. Publisher\n";
            cout << "3. Month\n";
        }
        cout << "4. Availability\n";
        cout << "0. Back\n";
        int choice = readInt("Choose field: ");

        if (choice == 1) {
            item->setTitle(readLine("New title: "));
        } else if (choice == 2) {
            if (item->getType() == "Book") {
                item->setCreator(readLine("New author: "));
            } else if (item->getType() == "Journal") {
                item->setCreator(readLine("New editor: "));
            } else if (item->getType() == "Magazine") {
                item->setCreator(readLine("New publisher: "));
            }
        } else if (choice == 3) {
            if (item->getType() == "Magazine") {
                item->setExtraValue(readLine("New month: "));
            } else {
                item->setReleaseYear(readInt("New year: "));
            }
        } else if (choice == 4) {
            int status = readInt("1 = Available, 0 = Borrowed: ");
            item->setAvailable(status == 1);
        } else if (choice == 0) {
            saveCatalog();
            return;
        } else {
            cout << "Invalid option.\n";
        }
        cout << "Item updated.\n";
    }
}

void LibrarySystem::deleteItem() {
    viewAllItems();
    int id = readInt("Enter item ID to delete (0 to back): ");
    if (id == 0) {
        return;
    }

    for (size_t i = 0; i < items.size(); ++i) {
        if (items[i]->getId() == id) {
            if (!items[i]->isAvailable()) {
                cout << "This item is currently borrowed and cannot be deleted.\n";
                return;
            }
            items.erase(items.begin() + i);
            saveCatalog();
            cout << "Item deleted successfully.\n";
            return;
        }
    }
    cout << "Item not found.\n";
}

void LibrarySystem::viewAllItems() const {
    cout << "\n---------- Books ----------\n";
    cout << left << setw(8) << "ID" << setw(35) << "Title"
         << setw(25) << "Author" << setw(8) << "Year" << "Status" << endl;
    cout << string(88, '-') << endl;
    bool hasBook = false;
    for (size_t i = 0; i < items.size(); ++i) {
        if (items[i]->getType() == "Book") {
            items[i]->display();
            hasBook = true;
        }
    }
    if (!hasBook) {
        cout << "No books found.\n";
    }

    cout << "\n---------- Journals ----------\n";
    cout << left << setw(8) << "ID" << setw(35) << "Title"
         << setw(25) << "Editor" << setw(8) << "Year" << "Status" << endl;
    cout << string(88, '-') << endl;
    bool hasJournal = false;
    for (size_t i = 0; i < items.size(); ++i) {
        if (items[i]->getType() == "Journal") {
            items[i]->display();
            hasJournal = true;
        }
    }
    if (!hasJournal) {
        cout << "No journals found.\n";
    }

    cout << "\n---------- Magazines ----------\n";
    cout << left << setw(8) << "ID" << setw(35) << "Title"
         << setw(25) << "Publisher" << setw(15) << "Month" << "Status" << endl;
    cout << string(95, '-') << endl;
    bool hasMagazine = false;
    for (size_t i = 0; i < items.size(); ++i) {
        if (items[i]->getType() == "Magazine") {
            items[i]->display();
            hasMagazine = true;
        }
    }
    if (!hasMagazine) {
        cout << "No magazines found.\n";
    }
}

void LibrarySystem::searchItems() const {
    string key = readLine("Enter title/type/author keyword: ");
    transform(key.begin(), key.end(), key.begin(), ::tolower);

    cout << "\n---------- Search Results ----------\n";
    bool found = false;
    for (size_t i = 0; i < items.size(); ++i) {
        string text = items[i]->getTitle() + " " + items[i]->getType() + " " + items[i]->getCreator();
        transform(text.begin(), text.end(), text.begin(), ::tolower);
        if (text.find(key) != string::npos) {
            cout << "ID: " << items[i]->getId()
                 << " | Type: " << items[i]->getType()
                 << " | Title: " << items[i]->getTitle();
            if (items[i]->getType() == "Book") {
                cout << " | Author: " << items[i]->getCreator()
                     << " | Year: " << items[i]->getReleaseYear();
            } else if (items[i]->getType() == "Journal") {
                cout << " | Editor: " << items[i]->getCreator()
                     << " | Year: " << items[i]->getReleaseYear();
            } else if (items[i]->getType() == "Magazine") {
                cout << " | Publisher: " << items[i]->getCreator()
                     << " | Month: " << items[i]->getExtraValue();
            }
            cout << " | Status: " << (items[i]->isAvailable() ? "Available" : "Borrowed") << endl;
            found = true;
        }
    }
    if (!found) {
        cout << "No matching items found.\n";
    }
}

void LibrarySystem::addMemberByAdmin() {
    signupMember();
}

void LibrarySystem::editMemberByAdmin() {
    viewMembers();
    string username = readLine("Enter member username to edit (0 to back): ");
    if (username == "0") {
        return;
    }
    Member* member = findMemberByUsername(username);
    if (member == nullptr) {
        cout << "Member not found.\n";
        return;
    }
    member->setEmail(readLine("New email: "));
    member->setPassword(readLine("New password: "));
    saveUsers();
    cout << "Member updated successfully.\n";
}

void LibrarySystem::deleteMemberByAdmin() {
    viewMembers();
    string username = readLine("Enter member username to delete (0 to back): ");
    if (username == "0") {
        return;
    }

    for (size_t i = 0; i < records.size(); ++i) {
        if (records[i].getMemberUsername() == username && records[i].getStatus() == "Borrowed") {
            cout << "This member has active borrowed items and cannot be deleted.\n";
            return;
        }
    }

    for (size_t i = 0; i < members.size(); ++i) {
        if (members[i].getUsername() == username) {
            members.erase(members.begin() + i);
            saveUsers();
            cout << "Member deleted successfully.\n";
            return;
        }
    }
    cout << "Member not found.\n";
}

void LibrarySystem::viewMembers() const {
    cout << "\n---------- Members ----------\n";
    cout << left << setw(20) << "Username" << "Email" << endl;
    cout << string(50, '-') << endl;
    for (size_t i = 0; i < members.size(); ++i) {
        cout << left << setw(20) << members[i].getUsername()
             << members[i].getEmail() << endl;
    }
    if (members.empty()) {
        cout << "No members found.\n";
    }
}

void LibrarySystem::borrowItem(Member& member) {
    try {
        viewAllItems();
        int id = readInt("Enter item ID to borrow (0 to back): ");
        if (id == 0) {
            return;
        }

        Item* item = findItemById(id);
        if (item == nullptr) {
            throw LibraryException("Item not found.");
        }
        if (!item->isAvailable()) {
            throw LibraryException("This item is already borrowed.");
        }
        if (hasActiveBorrow(member.getUsername(), id)) {
            throw LibraryException("You already borrowed this item.");
        }

        item->setAvailable(false);
        records.push_back(BorrowRecord(nextRecordId++, member.getUsername(), id,
                                       item->getTitle(), todayDate(), dateAfterDays(14),
                                       "", "Borrowed"));
        saveData();
        cout << "Borrow successful. Due date: " << dateAfterDays(14) << endl;
    } catch (const LibraryException& e) {
        cout << e.what() << endl;
    }
}

void LibrarySystem::returnItem(Member& member) {
    try {
        viewMyRecords(member);
        int id = readInt("Enter item ID to return (0 to back): ");
        if (id == 0) {
            return;
        }

        for (size_t i = 0; i < records.size(); ++i) {
            if (records[i].getMemberUsername() == member.getUsername() &&
                records[i].getItemId() == id &&
                records[i].getStatus() == "Borrowed") {
                records[i].markReturned(todayDate());
                Item* item = findItemById(id);
                if (item != nullptr) {
                    item->setAvailable(true);
                }
                saveData();
                cout << "Item returned successfully.\n";
                return;
            }
        }
        throw LibraryException("No active borrowing record found for this item.");
    } catch (const LibraryException& e) {
        cout << e.what() << endl;
    }
}

void LibrarySystem::viewMyRecords(const Member& member) const {
    cout << "\n---------- My Borrowing Records ----------\n";
    cout << left << setw(6) << "ID" << setw(16) << "Member" << setw(8) << "Item"
         << setw(28) << "Title" << setw(13) << "Borrowed" << setw(13)
         << "Due" << setw(13) << "Returned" << "Status" << endl;
    cout << string(110, '-') << endl;
    bool found = false;
    for (size_t i = 0; i < records.size(); ++i) {
        if (records[i].getMemberUsername() == member.getUsername()) {
            records[i].display();
            found = true;
        }
    }
    if (!found) {
        cout << "You have no borrowing records.\n";
    }
}

Admin* LibrarySystem::loginAdmin() {
    string username = readLine("Username: ");
    string password = readLine("Password: ");
    for (size_t i = 0; i < admins.size(); ++i) {
        if (admins[i].getUsername() == username && admins[i].getPassword() == password) {
            cout << "Admin login successful.\n";
            return &admins[i];
        }
    }
    cout << "Invalid admin username or password.\n";
    return nullptr;
}

Member* LibrarySystem::loginMember() {
    string username = readLine("Username: ");
    string password = readLine("Password: ");
    for (size_t i = 0; i < members.size(); ++i) {
        if (members[i].getUsername() == username && members[i].getPassword() == password) {
            cout << "Member login successful.\n";
            return &members[i];
        }
    }
    cout << "Invalid member username or password.\n";
    return nullptr;
}

void LibrarySystem::signupAdmin() {
    string username = readLine("Choose username: ");
    if (usernameExists(username)) {
        cout << "Username already exists.\n";
        return;
    }
    string email = readLine("Email: ");
    if (!isValidEmail(email)) {
        cout << "Invalid email address.\n";
        return;
    }
    string password = readLine("Choose password: ");
    admins.push_back(Admin(username, email, password));
    saveUsers();
    cout << "Admin account created successfully.\n";
}

void LibrarySystem::signupMember() {
    string username = readLine("Choose username: ");
    if (usernameExists(username)) {
        cout << "Username already exists.\n";
        return;
    }
    string email = readLine("Email: ");
    if (!isValidEmail(email)) {
        cout << "Invalid email address.\n";
        return;
    }
    string password = readLine("Choose password: ");
    members.push_back(Member(username, email, password));
    saveUsers();
    cout << "Member account created successfully.\n";
}

Item* LibrarySystem::findItemById(int id) const {
    for (size_t i = 0; i < items.size(); ++i) {
        if (items[i]->getId() == id) {
            return items[i].get();
        }
    }
    return nullptr;
}

Member* LibrarySystem::findMemberByUsername(const string& username) {
    for (size_t i = 0; i < members.size(); ++i) {
        if (members[i].getUsername() == username) {
            return &members[i];
        }
    }
    return nullptr;
}

Admin* LibrarySystem::findAdminByUsername(const string& username) {
    for (size_t i = 0; i < admins.size(); ++i) {
        if (admins[i].getUsername() == username) {
            return &admins[i];
        }
    }
    return nullptr;
}

bool LibrarySystem::usernameExists(const string& username) const {
    for (size_t i = 0; i < admins.size(); ++i) {
        if (admins[i].getUsername() == username) {
            return true;
        }
    }
    for (size_t i = 0; i < members.size(); ++i) {
        if (members[i].getUsername() == username) {
            return true;
        }
    }
    return false;
}

bool LibrarySystem::hasActiveBorrow(const string& username, int itemId) const {
    for (size_t i = 0; i < records.size(); ++i) {
        if (records[i].getMemberUsername() == username &&
            records[i].getItemId() == itemId &&
            records[i].getStatus() == "Borrowed") {
            return true;
        }
    }
    return false;
}

int LibrarySystem::readInt(const string& prompt) const {
    int value;
    while (true) {
        cout << prompt;
        if (cin >> value) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return value;
        }
        if (cin.eof()) {
            return 0;
        }
        cout << "Please enter a valid number.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

string LibrarySystem::readLine(const string& prompt) const {
    string value;
    cout << prompt;
    getline(cin, value);
    return value;
}

void LibrarySystem::pause() const {
    cout << "\nPress Enter to continue...";
    string temp;
    getline(cin, temp);
}

string LibrarySystem::todayDate() const {
    time_t now = time(nullptr);
    tm* local = localtime(&now);
    char buffer[11];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d", local);
    return string(buffer);
}

string LibrarySystem::dateAfterDays(int days) const {
    time_t now = time(nullptr) + static_cast<time_t>(days) * 24 * 60 * 60;
    tm* local = localtime(&now);
    char buffer[11];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d", local);
    return string(buffer);
}
