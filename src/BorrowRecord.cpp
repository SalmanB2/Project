#include "BorrowRecord.h"
#include "LibraryException.h"
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;

static vector<string> splitRecordLine(const string& text, char delimiter) {
    vector<string> parts;
    string part;
    stringstream ss(text);
    while (getline(ss, part, delimiter)) {
        parts.push_back(part);
    }
    return parts;
}

BorrowRecord::BorrowRecord()
    : recordId(0), itemId(0), status("Borrowed") {
}

BorrowRecord::BorrowRecord(int recordId, const string& memberUsername, int itemId,
                           const string& itemTitle, const string& borrowDate,
                           const string& dueDate, const string& returnDate,
                           const string& status)
    : recordId(recordId), memberUsername(memberUsername), itemId(itemId),
      itemTitle(itemTitle), borrowDate(borrowDate), dueDate(dueDate),
      returnDate(returnDate), status(status) {
}

int BorrowRecord::getRecordId() const { return recordId; }
string BorrowRecord::getMemberUsername() const { return memberUsername; }
int BorrowRecord::getItemId() const { return itemId; }
string BorrowRecord::getItemTitle() const { return itemTitle; }
string BorrowRecord::getBorrowDate() const { return borrowDate; }
string BorrowRecord::getDueDate() const { return dueDate; }
string BorrowRecord::getReturnDate() const { return returnDate; }
string BorrowRecord::getStatus() const { return status; }

void BorrowRecord::markReturned(const string& date) {
    returnDate = date;
    status = "Returned";
}

void BorrowRecord::display() const {
    cout << left << setw(6) << recordId
         << setw(16) << memberUsername.substr(0, 15)
         << setw(8) << itemId
         << setw(28) << itemTitle.substr(0, 27)
         << setw(13) << borrowDate
         << setw(13) << dueDate
         << setw(13) << (returnDate.empty() ? "-" : returnDate)
         << status << endl;
}

string BorrowRecord::toFileString() const {
    stringstream ss;
    ss << recordId << "|" << memberUsername << "|" << itemId << "|"
       << itemTitle << "|" << borrowDate << "|" << dueDate << "|"
       << returnDate << "|" << status;
    return ss.str();
}

BorrowRecord BorrowRecord::fromFileLine(const string& line) {
    vector<string> p = splitRecordLine(line, '|');
    if (p.size() < 8) {
        throw LibraryException("Invalid borrow record data found.");
    }
    return BorrowRecord(stoi(p[0]), p[1], stoi(p[2]), p[3], p[4], p[5], p[6], p[7]);
}
