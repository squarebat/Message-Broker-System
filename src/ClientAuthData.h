//
// Created by mando on 19/02/21.
//

#ifndef MESSAGE_BROKER_SYSTEM_CLIENTAUTHDATA_H
#define MESSAGE_BROKER_SYSTEM_CLIENTAUTHDATA_H


#include <string>
#include <utility>

class ClientAuthData {
private:
    std::string _password;
    std::string _name;
public:
    ClientAuthData() = default;
    explicit ClientAuthData(std::string name) : _name(std::move(name)) {}
    void SetPassword(const std::string& password);
    bool Authenticate(const std::string& name, const std::string& password);
    std::string GetName();
    friend std::ostream& operator<<(std::ostream& os, const ClientAuthData& clientAuthData);
    friend std::istream& operator>>(std::istream& is, ClientAuthData& clientAuthData);

    static char GetCharFrom6BitNum(unsigned int i);
};


#endif //MESSAGE_BROKER_SYSTEM_CLIENTAUTHDATA_H
