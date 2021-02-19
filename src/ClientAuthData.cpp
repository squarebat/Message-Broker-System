//
// Created by mando on 19/02/21.
//

#include <unistd.h>
#include <ctime>
#include <iostream>
#include <crypt.h>
#include <cstring>
#include "ClientAuthData.h"

void ClientAuthData::SetPassword(const std::string &password) {
    unsigned long seed[2];
    char salt[] = "$1$........";
    char *passwd;
    int i;
    seed[0] = time(nullptr);
    seed[1] = getpid() ^ (seed[0] >> 14 & 0x30000);

    for (i = 0; i < 8; i++) {
        salt[3 + i] = GetCharFrom6BitNum((seed[i / 5] >> (i % 5) * 6) & 0x3f);
    }

    passwd = crypt(password.c_str(), salt);
    _password.erase(0, _password.length());
    for (char* c = passwd; *c != '\0'; c++) {
        _password.push_back(*c);
    }
}

bool ClientAuthData::Authenticate(const std::string &name, const std::string &password) {
    return _name == name && strcmp(crypt(password.c_str(), _password.c_str()), _password.c_str()) == 0 ;
}

std::ostream& operator<<(std::ostream& os, const ClientAuthData& clientAuthData) {
    os << clientAuthData._name << " : " << clientAuthData._password << "\n";
    return os;
}

std::istream& operator>>(std::istream& is, ClientAuthData& clientAuthData) {
    char temp;
    is >> clientAuthData._name >> temp >> clientAuthData._password;
    return is;
}

char ClientAuthData::GetCharFrom6BitNum(unsigned int i) {
    static const char* alphabets = "./0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    return alphabets[i];
}

std::string ClientAuthData::GetName() {
    return _name;
}
