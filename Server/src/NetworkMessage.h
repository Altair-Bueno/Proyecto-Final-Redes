#ifndef NETWORKMESSAGE_H
#define NETWORKMESSAGE_H

#include "Serializable.h"
#include <stdint.h>
#include <cstring>

enum MessageType : uint8_t
{
    ERROR_MESSAGE,
    LOGIN,
    LOGOUT,
    LEVEL_END,
    BALL_HIT,
    PLAYER_DISCONNECTED
};

inline MessageType getType(char *data)
{
    if (data == nullptr || strlen(data) < sizeof(uint8_t))
        return ERROR_MESSAGE;

    uint8_t type;
    memcpy(&type, data, sizeof(uint8_t));

    return (MessageType)type;
}

struct NetworkMessage : public Serializable
{
public:
    NetworkMessage(MessageType type) : type(type) {}
    NetworkMessage() : type(ERROR_MESSAGE) {}
    uint8_t type;

    inline void to_bin()
    {
        alloc_data(sizeof(type));
        memset(_data, 0, sizeof(type));
        memcpy(_data, &type, sizeof(type));
    };

    inline int from_bin(char *data)
    {
        alloc_data(sizeof(type));
        memcpy(static_cast<void*>(_data), data, _size);

        char* tmp = _data;
        memcpy(&type, tmp, sizeof(type));

        return 0;
    };
};

struct LoginMessage : public NetworkMessage
{
    #define loginCodeMaxSize 5 * sizeof(char)
    std::string loginCode;

    LoginMessage(std::string loginCode) : NetworkMessage(LOGIN), loginCode(loginCode) {}
    LoginMessage() : NetworkMessage(LOGIN) {}

    inline void to_bin()
    {
        uint16_t dataSize = sizeof(type) + loginCodeMaxSize;
        alloc_data(dataSize);
        memset(_data, 0, dataSize);

        char *tmp = _data;
        memcpy(tmp, &type, sizeof(type));

        tmp += sizeof(type);
        memcpy(tmp, loginCode.c_str(), loginCodeMaxSize);
    };


    inline int from_bin(char *data)
    {
        alloc_data(sizeof(type) + sizeof(loginCode));
        memcpy(static_cast<void*>(_data), data, _size);

        char* tmp = _data;
        memcpy(&type, tmp, sizeof(type));

        tmp += sizeof(type);
        char tempLoginCode[loginCodeMaxSize];
        memcpy(&tempLoginCode, tmp, loginCodeMaxSize);
        loginCode = tempLoginCode;

        return 0;
    };
};

#endif