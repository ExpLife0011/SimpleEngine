#ifndef SPRITE2_H
#define SPRITE2_H 
#include "../../defines.h"
#include "../../defineTypes.h"
#include <vector>
/*
һ������������
*/

class XYQImage
{
public:
	XYQImage(std::ifstream& file,uint32 size);

	uint8* data(){return m_pData;}
	uint32* size(){return m_Size;}
	~XYQImage();
private:	
	uint32 m_Size ;
	uint8* m_pData;
}

class XYQImageManager
{
public:
	XYQImageManager()
	{
		m_ImageSet.clear();
	}
	~XYQImageManager()
	{	
		for(auto it : m_ImageSet)
		{
			if(it.second != nullptr)
			{
				delete it.second;
			}
		}
		m_ImageSet.clear();
	}

	bool erase(String path)
	{
		if(m_ImageSet.count(path))
		{
			auto* tmp = m_ImageSet[path];
			m_ImageSet.erase(path);
			delete tmp;
			return true;
		}
		return false;
	}

	void put(String path,std::ifstream& file,uint32 size)
	{
		m_ImageSet[path] = new XYQImage(file,size);
	}

	XYQImage* get(String path)
	{
		if(m_ImageSet.count(path))
		{
			return m_ImageSet[path];
		}
		return nullptr;
	}
	
	static XYQImageManager* GetInstance()
	{
		static auto* p = new XYQImageManager();
		return p;
	}
private:
	std::map<String,XYQImage*> m_ImageSet;
}
#define XYQ_IMAGE_MANAGER_INSTANCE XYQImageManager::GetInstance()


class Sprite2
{

public:
	Sprite2();
	~Sprite2();

	struct Sequence
	{
		int key_x;
		int key_y;
		int width;
		int height;
		uint32 format;
	//	uint32* src;
		String path;
		Sequence()
		:key_x(0),
		key_y(0),
		width(0),
		height(0),
		format(0),
		path("")
		{

		}
	};

	int mGroupSize;		//������
	int mFrameSize;		//֡��
	int mWidth;			//���
	int mHeight;		//�߶�
	int mKeyX;			//�ؼ�֡X
	int mKeyY;			//�ؼ�֡Y
	String mID;
	String mPath;
	std::vector<Sequence> mFrames;
	// Sequence** mFrames;
	bool Error;
	void SaveImage(int index);
};
#endif
