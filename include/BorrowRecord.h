#ifndef BORROW_RECORD_H
#define BORROW_RECORD_H

#include <string>

class BorrowRecord {
private:
    int recordId;
    std::string memberUsername;
    int itemId;
    std::string itemTitle;
    std::string borrowDate;
    std::string dueDate;
    std::string returnDate;
    std::string status;

public:
    BorrowRecord();
    BorrowRecord(int recordId, const std::string& memberUsername, int itemId,
                 const std::string& itemTitle, const std::string& borrowDate,
                 const std::string& dueDate, const std::string& returnDate,
                 const std::string& status);

    int getRecordId() const;
    std::string getMemberUsername() const;
    int getItemId() const;
    std::string getItemTitle() const;
    std::string getBorrowDate() const;
    std::string getDueDate() const;
    std::string getReturnDate() const;
    std::string getStatus() const;

    void markReturned(const std::string& returnDate);
    void display() const;
    std::string toFileString() const;
    static BorrowRecord fromFileLine(const std::string& line);
};

#endif
