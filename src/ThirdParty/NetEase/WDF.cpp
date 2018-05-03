#include "WDF.h"
// #include "WAS.h"

#include <iostream>
using std::cout ;
using std::endl ;
//#include <fstream>
namespace NetEase {
	WDF::WDF(String path)
	:m_WASNumber(0),
	m_WASOffset(0),
	m_Path(path),
	m_Indencies(0),
	mIdToPos(0)
	{
		std::fstream file(m_Path, ios::in | ios::binary);
		if(!file)
		{
			std::cout << "open file error!" << endl;
			file.close();
			return;
		}

		std::cout << "InitWDF:" << m_Path << std::endl;

		mFileName = m_Path.substr(m_Path.find_last_of("/")+1);

		
		WDF::Header header;
		file.read((char*)&header, sizeof(WDF::Header));
		if(head.flag!=0x57444650)
		{	
			std::cout << "wdf head flag error!" << endl;
			file.close();
			return;
		}

		m_WASNumber = header.number;
		m_WASOffset = header.offset;

		cout << "number:" << m_WASNumber << " offset:" << m_WASOffset << endl;

		m_Indencies.resize(m_WASNumber);
		file.seekg(m_WASOffset,std::beg);
		
		file.read((char*)&m_Indencies[0], sizeof(Index)*m_WASNumber);
		cout << "read indecies ok!" << endl;
		file.close();

		for (int i = 0; i<m_WASNumber; i++)
		{
			mIdToPos[m_Indencies[i].hash] = i;
		}

	}

	WDF::~WDF()
	{
	}

	/**
	????Sprite2
	*/
	WAS WDF::GetWAS(uint32 id)
	{
		Index index = m_Indencies[mIdToPos[id]];
		return WAS(m_Path, index.offset, index.size);
	}


	Sprite2* WDF::LoadSprite(uint32 id)
	{
		if(!mIdToPos.count(id))return nullptr;
		std::fstream file(m_Path, ios::in | ios::binary);
		if(!file){file.close(); return nullptr};
		
        
		WAS::Header header;
        memset(&header,0,sizeof(WAS::Header));
	
		

		Index index = m_Indencies[mIdToPos[id]];
		uint32 wasOffset = index.offset;
		uint32 wasSize = index.size;
		// file.seekg(wasOffset,ios::beg);
		// char* outfilec=new char[wasSize];
		// file.read(outfilec,wasSize);
		// fstream of("a.was",ios::binary|ios::out);
		// of.write(outfilec,wasSize);
		// of.close();

		file.seekg(wasOffset, ios::beg);
		file.read((char*)&header, sizeof(header));
		if(header.flag != 0x5053)
		{
			file.close();
			return nullptr;
		}

		Sprite2* sprite = new Sprite2();
		sprite->mID = std::to_string(id);
		sprite->mPath = mFileName+"/"+sprite->mID;
		sprite->mGroupSize = header.group;
		sprite->mFrameSize = header.frame;
		sprite->mWidth = header.width;
		sprite->mHeight = header.height;
		sprite->mKeyX = header.key_x;
		sprite->mKeyY = header.key_y;	
		sprite->mFrames.resize(header.group*header.frame);
	
		// ?��????????????????12
		if (header.len != 12)
		{
			// ???????????????
			int AddonHeadLen = header.len - 12;
			uint8* m_AddonHead = new uint8[AddonHeadLen]; // ???��??????????
			file.read((char*)m_AddonHead, AddonHeadLen); // ???????????
		}

		// ????????????
		memset(m_Palette16,0, 256*2);
		file.read((char*)&m_Palette16[0], 256 * 2); // Palette[0]?????
		for (int k = 0; k < 256; k++)
		{
			m_Palette32[k] = WAS::RGB565to888(m_Palette16[k], 0xff); // 16to32????????
		}

		int frameTotalSize = header.group * header.frame;
		// std::cout <<"frameTotalSize: "<< frameTotalSize<<std::endl;

		uint32* frameIndexes = new uint32[frameTotalSize];
		memset(frameIndexes,0, frameTotalSize * 4);
		file.read((char*)frameIndexes, frameTotalSize * 4);

		uint32 pixels = header.width*header.height;
		int frameHeadOffset = 2 + 2 + header.len;
		
		uint32* frameLine = new uint32[header.height]; // ???????????��????
        memset(frameLine,0,4*header.height);

		uint8* lineData = new uint8[header.width * 4]; // ?????????????
        memset(lineData,0,4*header.width);

		int x = 0; // ???????
		int z = 0; // ???????
		WAS::FrameHeader tempFreamHeader;
		for (int i = 0; i<frameTotalSize; i++)
		// for (int i = frameTotalSize/2; i< frameTotalSize/2+1; i++)
		{
            memset(&tempFreamHeader,0,sizeof(WAS::FrameHeader ));
            file.seekg(wasOffset + frameHeadOffset + frameIndexes[i], ios::beg);
			file.read((char*)&tempFreamHeader, sizeof(WAS::FrameHeader));
			if(tempFreamHeader.height >= (1<<15) || tempFreamHeader.width >= (1 <<15) ||tempFreamHeader.key_x >= (1<<15) || tempFreamHeader.key_y >= (1 <<15))
            {	
				cout<<"tempFreamHeader error!" << endl;
				file.close();
                return nullptr;
            }

			int gpos = i / (header.frame);
			int cpos = i % (header.frame);

			Sprite2::Sequence& fream = sprite->mFrames[gpos* header.frame +  cpos];

			frame.key_x = tempFreamHeader.key_x;
			frame.key_y = tempFreamHeader.key_y;
			frame.width = tempFreamHeader.width;
			frame.height = tempFreamHeader.height;
			frame.path = sprite->mPath+"/"+i;
            frame.src = new uint32[pixels];
			memset(frame.src, 0, pixels * 4);

			// ??????????????
			file.read((char*)frameLine, tempFreamHeader.height * 4);
			uint32* pBmpStart = frame.src;//=frame.src+pixels*3;
			for (int j = 0; j< tempFreamHeader.height; j++)
			{
				pBmpStart = frame.src + sprite->mWidth*(j);
				int lineDataLen = 0;
				if (j < tempFreamHeader.height - 1)
				{
                	lineDataLen = frameLine[j + 1] - frameLine[j]; // ???????????��
				}
				else
				{
					if (i<frameTotalSize - 1) {
						lineDataLen = frameIndexes[i + 1] - (frameIndexes[i] + frameLine[j]);
					}
					else {
						lineDataLen = wasSize - (frameIndexes[i] + frameLine[j]);
					}
				}
                // printf("lineLen:%d\n",lineDataLen);
				memset(lineData, 0, frame.width);
				file.seekg(wasOffset + frameIndexes[i] + frameHeadOffset + frameLine[j], ios::beg);
				file.read((char*)lineData, lineDataLen);
				// printf("before handler:  %x\n",  sprite->mFrames[gpos][cpos].src );
				int pixelOffset = (sprite->mKeyX - frame.key_x);
				int pixelLen = sprite->mWidth;
				// printf("pixelOffset: %d  pixelLen: %d\n",pixelOffset,pixelLen );
				pBmpStart += pixelOffset;
				pBmpStart += (sprite->mKeyY - frame.key_y)*sprite->mWidth;

				DataHandler((char*)lineData, pBmpStart, pixelOffset, pixelLen);

			}
			 //sprite->SaveImage(i);
		}
		file.close();
		sprite->Error= false;
		return sprite;
	}

	std::vector<std::shared_ptr<Sprite2>> WDF::LoadAllSprite()
	{
		std::vector<std::shared_ptr<Sprite2>> v;
		for (int i = 0; i<mHeader.number; i++)
		{
			
			auto p = LoadSprite(m_Indencies[i].hash);
			if(!p->Error)
			{
				v.push_back(p);
			}
		}
		return v;
	}


	void WDF::DataHandler(char *pData, uint32* pBmpStart, int pixelOffset, int pixelLen)
	{
		uint32 temp_pixel = 0;

		uint32 Pixels = pixelOffset;
		uint32 PixelLen = pixelLen;
		uint16 AlphaPixel = 0;

		while (*pData != 0) // {00000000} ????????��??????????????????????????
		{
			uint8 style = 0;
			uint8 Level = 0; // Alpha????
			uint8 Repeat = 0; // ???????
			style = (*pData & 0xc0) >> 6; // ??????????????
			switch (style)
			{
			case 0: // {00******}
				if (*pData & 0x20) // {001*****} ???????Alpha????????????
				{
					// {001 +5bit Alpha}+{1Byte Index}, ???????Alpha?????????????
					// {001 +0~31??Alpha???}+{1~255???????????}
					Level = (*pData) & 0x1f; // 0x1f=(11111) ???Alpha??????
					pData++; // ????????
					if (Pixels < PixelLen)
					{
						AlphaPixel = WAS::Alpha565(m_Palette16[(uint8)(*pData)], 0, Level); // ???
						*pBmpStart++ = WAS::RGB565to888(AlphaPixel, Level * 8);
						Pixels++;
						pData++;
					}
				}
				else // {000*****} ??????n?��???Alpha?????????
				{
					// {000 +5bit Times}+{1Byte Alpha}+{1Byte Index}, ??????n?��???Alpha??????????
					// {000 +???1~31??}+{0~255??Alpha???}+{1~255???????????}
					// ?: ?????{00000000} ??????????��???????????????????1~31?��?
					Repeat = (*pData) & 0x1f; // ???????????
					pData++;
					Level = *pData; // ???Alpha????
					pData++;
					for (int i = 1; i <= Repeat; i++)
					{
						if (Pixels < PixelLen)
						{
							AlphaPixel = WAS::Alpha565(m_Palette16[(uint8)*pData], 0, Level); // ???
							*pBmpStart++ = WAS::RGB565to888(AlphaPixel, Level * 8);
							Pixels++;
						}
					}
					pData++;
				}
				break;
			case 1: // {01******} ???????Alpha??????????n???????????????
					// {01  +6bit Times}+{nByte Datas},???????Alpha??????????n??????????????��?
					// {01  +1~63??????}+{n??????????},{01000000}????
				Repeat = (*pData) & 0x3f; // ??????????��????
				pData++;
				for (int i = 1; i <= Repeat; i++)
				{
					if (Pixels < PixelLen)
					{
						*pBmpStart++ = m_Palette32[(uint8)*pData];
						Pixels++;
						pData++;
					}
				}
				break;
			case 2: // {10******} ??????n??????
					// {10  +6bit Times}+{1Byte Index}, ??????n???????
					// {10  +???1~63??}+{0~255???????????},{10000000}????
				Repeat = (*pData) & 0x3f; // ???????????
				pData++;
				for (int i = 1; i <= Repeat; i++)
				{
					if (Pixels <PixelLen)
					{
						*pBmpStart++ = m_Palette32[(uint8)*pData];
						Pixels++;
					}
				}
				pData++;
				break;
			case 3: // {11******} ???????n???????????????????????????
					// {11  +6bit Times}, ???????n??????????????????????????��
					// {11  +????1~63??????},{11000000}????
				Repeat = (*pData) & 0x3f; // ??????????
				for (int i = 1; i <= Repeat; i++)
				{
					if (Pixels < PixelLen)
					{
						pBmpStart++;
						Pixels++;
					}
				}
				pData++;
				break;
			default: // ??????????????
				cerr << "Error!" << endl;
				exit(EXIT_FAILURE);
				break;
			}
		}
		if (*pData == 0)
		{
			uint8 Repeat = 0;
			Repeat = PixelLen - Pixels;
			for (int i = 0; i < Repeat; i++)
			{
				if (Pixels < PixelLen)
				{
					pBmpStart++;
					Pixels++;
				}
			}
		}
	}
}


// int main(int argc,char* argv[])
// {
// 	// shared_ptr<WDF> p = make_shared<WDF>("./data/shape.wdf");
// 	// cout<<p.use_count();
// 	WDF wdf("./data/shape.wdf");
// 	// wdf.GetWAS(0x00642F2B);
// 	wdf.LoadSprite(0x49386FCE);
// }
