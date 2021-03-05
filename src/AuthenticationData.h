//
// Created by mando on 19/02/21.
//

#ifndef MESSAGE_BROKER_SYSTEM_AUTHENTICATIONDATA_H
#define MESSAGE_BROKER_SYSTEM_AUTHENTICATIONDATA_H


#include <fstream>
#include <utility>
#include <unordered_map>
#include <ClientAuthData.h>

class AuthenticationData {
private:
    std::string _authFilePath;
    std::unordered_map<std::string, ClientAuthData> clientsAuthData;
public:
    AuthenticationData() = delete;
    explicit AuthenticationData(std::string authFilePath) : _authFilePath(std::move(authFilePath)) {}

    void LoadData();
    void WriteData();
    bool AuthenticateClient(const std::string& name, const std::string& password);
    void AddClientAuthData(const std::string& name, const std::string& password);
    void ModifyClientAuthData(const std::string& name, const std::string& password);
    void DeleteClientAuthData(const std::string& name);
    bool AuthDataExists(const std::string& name);
};

#endif //MESSAGE_BROKER_SYSTEM_AUTHENTICATIONDATA_H
