#ifndef USER_H
#define USER_H

#include <string>

class User {
protected:
    std::string username;
    std::string email;
    std::string password;

public:
    User();
    User(const std::string& username, const std::string& email,
         const std::string& password);
    virtual ~User();

    std::string getUsername() const;
    std::string getEmail() const;
    std::string getPassword() const;
    void setEmail(const std::string& email);
    void setPassword(const std::string& password);

    virtual std::string getRole() const = 0;
    virtual std::string toFileString() const;
};

class Admin : public User {
public:
    Admin();
    Admin(const std::string& username, const std::string& email,
          const std::string& password);
    std::string getRole() const override;
};

class Member : public User {
public:
    Member();
    Member(const std::string& username, const std::string& email,
           const std::string& password);
    std::string getRole() const override;
};

#endif