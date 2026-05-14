#pragma once

#ifndef __PACKET_H__
#define __PACKET_H__

#include <iostream>
#include <WinSock2.h>

enum class PacketType
{
    Move = 0,   // 클라이언트 -> 서버: 이동 방향
    Position,
	C2S_File,// 클라이언트 -> 서버: 파일 전송
    S2C_File,// 서버 -> 클라이언트: 플레이어 위치
    Max
};

#pragma pack(push, 1)

// 공통 헤더: Size(데이터 크기) + Code(패킷 종류)
struct PacketHeader
{
    unsigned long Size;
    unsigned short Code;
};

// 이동 방향 데이터 (클라이언트 -> 서버)
struct MoveData
{
    char Dir;   // 'W', 'A', 'S', 'D'
};

// 파일 데이터 (서버 -> 클라이언트)
struct FileData
{
	FILE* SendFile;
};

// 위치 데이터 (서버 -> 클라이언트)
struct PositionData
{
    int X;
    int Y;
};

#pragma pack(pop)

#endif // __PACKET_H__#pragma once
