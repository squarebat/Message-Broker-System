//
// Created by mando on 19/02/21.
//

#include <iostream>
#include "AuthenticationData.h"

void AuthenticationData::LoadData() {
    std::ifstream authDataIStream;
    authDataIStream.open(_authFilePath, std::ios::in);
    ClientAuthData temp;
    while (authDataIStream.peek() != EOF) {
        authDataIStream >> temp;
        clientsAuthData[temp.GetName()] = temp;
    }
    authDataIStream.close();
}

void AuthenticationData::WriteData() {
    std::ofstream authDataOStream;
    authDataOStream.open(_authFilePath, std::ios::out);
    for (auto& it: clientsAuthData) {
        authDataOStream << it.second;
    }
    authDataOStream.close();
}

bool AuthenticationData::AuthenticateClient(const std::string &name, const std::string &password) {
    return clientsAuthData[name].Authenticate(name, password);
}

void AuthenticationData::AddClientAuthData(const std::string& name, const std::string& password) {
    ClientAuthData clientAuthData(name);
    clientAuthData.SetPassword(password);
    clientsAuthData.emplace(std::pair<std::string, ClientAuthData>(name, clientAuthData));
}

void AuthenticationData::ModifyClientAuthData(const std::string& name, const std::string& password) {
    ClientAuthData& clientAuthData = clientsAuthData[name];
    clientAuthData.SetPassword(password);
}

void AuthenticationData::DeleteClientAuthData(const std::string& name) {
    clientsAuthData.erase(name);
}

bool AuthenticationData::AuthDataExists(const std::string& name) {
    return clientsAuthData.find(name) != clientsAuthData.end();
}
