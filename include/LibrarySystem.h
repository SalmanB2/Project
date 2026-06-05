#ifndef LIBRARY_SYSTEM_H
#define LIBRARY_SYSTEM_H

#include "BorrowRecord.h"
#include "Item.h"
#include "User.h"
#include <memory>
#include <string>
#include <vector>

class LibrarySystem {
private:
    std::vector<std::unique_ptr<Item> > items;
    std::vector<Admin> admins;
    std::vector<Member> members;
    std::vector<BorrowRecord> records;

    int nextItemId;
    int nextRecordId;

    const std::string catalogFile;
    const std::string usersFile;
    const std::string recordsFile;

    void loadData();
    void saveData() const;
    void loadCatalog();
    void loadUsers();
    void loadRecords();
    void saveCatalog() const;
    void saveUsers() const;
    void saveRecords() const;

    void mainMenu();
    void adminEntrance();
    void memberEntrance();
    void adminMenu(Admin& admin);
    void memberMenu(Member& member);

    void adminCatalogMenu();
    void adminMemberMenu();
    void adminRecordMenu() const;

    void addItem();
    void editItem();
    void deleteItem();
    void viewAllItems() const;
    void searchItems() const;

    void addMemberByAdmin();
    void editMemberByAdmin();
    void deleteMemberByAdmin();
    void viewMembers() const;

    void borrowItem(Member& member);
    void returnItem(Member& member);
    void viewMyRecords(const Member& member) const;

    Admin* loginAdmin();
    Member* loginMember();
    void signupAdmin();
    void signupMember();

    Item* findItemById(int id) const;
    Member* findMemberByUsername(const std::string& username);
    Admin* findAdminByUsername(const std::string& username);
    bool usernameExists(const std::string& username) const;
    bool hasActiveBorrow(const std::string& username, int itemId) const;

    int readInt(const std::string& prompt) const;
    std::string readLine(const std::string& prompt) const;
    void pause() const;
    std::string todayDate() const;
    std::string dateAfterDays(int days) const;

public:
    LibrarySystem();
    ~LibrarySystem();
    void run();
};

#endif
