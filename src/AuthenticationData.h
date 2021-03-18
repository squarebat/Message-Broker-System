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
    int SetPasswordFileMutable();
    int SetPasswordFileImmutable();
    bool AuthenticateClient(const std::string& name, const std::string& password);
    void AddClientAuthData(const std::string& name, const std::string& password);
    void ModifyClientAuthData(const std::string& name, const std::string& password);
    void DeleteClientAuthData(const std::string& name);
    bool AuthDataExists(const std::string& name);
};

#endif //MESSAGE_BROKER_SYSTEM_AUTHENTICATIONDATA_H
