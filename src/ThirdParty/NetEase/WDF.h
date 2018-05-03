#ifndef WDF_H
#define WDF_H 
// #include <string>
// #include <vector>
#include <map>
// #include <iostream>
#include <fstream>
#include "WAS.h"
#include "Sprite2.h"

// #include "Sprite.h"
namespace NetEase {
	

	class WDF
	{
		struct Header
		{
			uint32 flag; // �����ı�ǩ
			uint32 number; // �����е��ļ�����
			uint32 offset; // �����е��ļ��б�ƫ��λ��
			Header():flag(0),number(0),offset(0) {};
		};

		struct Index
		{
			uint32 hash; // �ļ�������ɢ��
			uint32 offset; // �ļ���ƫ��
			uint32 size; // �ļ��Ĵ�С
			uint32 spaces; // �ļ��Ŀռ�
			Index( ):hash(0),offset(0),size(0),spaces(0) { }
		};

	public:
		WDF();
		WDF(std::string path) :mFilePath(path) { Init(); }
		void DataHandler(char *pData, uint32* pBmpStart, int pixelOffset, int pixelLen);
		// Sprite LoadSprite(int id);
		WAS GetWAS(uint32 id);
		std::shared_ptr<Sprite2> LoadSprite(uint32 id);
		std::vector<uint32> GetAllWASIDs()
		{
			std::vector<uint32> ids;
			for(int i=0;i<mHeader.number;i++)
			{
				ids.push_back(mIndencies[i].hash);
			}
			return ids;
		}
		std::vector<std::shared_ptr<Sprite2>> LoadAllSprite();
		~WDF();

	public:
		WDF::Header mHeader;
		std::vector<Index> mIndencies;
		map<uint32, uint32> mIdToPos;
		
		uint16 m_Palette16[256];
		String mFilePath;
		uint32 m_Palette32[256];
		String mFileName;
		
		uint32 m_WASNumber; = mHeader.number;
		uint32 m_WASOffset; = mHeader.offset;

	};
}
#endif
