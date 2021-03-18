#include <iostream>
#include <cstdio>
#include <AuthenticationData.h>

void AuthenticationData::LoadData() {
    std::ifstream authDataIStream;
    authDataIStream.open(_authFilePath, std::ios::in);
    while (authDataIStream.peek() != EOF) {
        ClientAuthData temp;
        authDataIStream >> temp;
        clientsAuthData[temp.GetName()] = temp;
    }
    authDataIStream.close();
}

void AuthenticationData::WriteData() {
    std::ofstream authDataOStream;
    SetPasswordFileMutable();
    authDataOStream.open(_authFilePath, std::ios::out);
    for (const auto& it: clientsAuthData) {
        authDataOStream << it.second;
    }
    authDataOStream.close();
    SetPasswordFileImmutable();
}

int AuthenticationData::SetPasswordFileMutable() {
    std::string command = "sudo -p \"Enter your sudo password to continue: \" chattr -i " + _authFilePath;
    std::cout << _authFilePath << std::endl;
    std::cout << command << std::endl;
    return system(command.c_str());
}

int AuthenticationData::SetPasswordFileImmutable() {
    std::string command = "sudo -p \"Enter your sudo password to continue: \" chattr +i " + _authFilePath;
    return system(command.c_str());
}

bool AuthenticationData::AuthenticateClient(const std::string& name, const std::string& password) {
    return clientsAuthData.at(name).Authenticate(name, password);
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
