#define _CRT_SECURE_NO_WARNINGS

#include<stdio.h>
#include<string>
#include<memory.h>
#include<string>
#include<stdlib.h>
#include<vector>

using namespace std;

vector<pair<unsigned int, bool> > V;
int EntryPoint = 0x111c2;
int ImageAddress = 0;
int RVA = 0x11000;
int PointerToRawData = 0x400;
int NextOffset = 0;
bool flag = 0;

int CheckMain(int offset, char* Buf)
{
	for(int i = 0; i < 4; i++)
	{
		if(Buf[offset + 2 * i] != '\x33')
			return false;
	}
	return true;
}

void GetAddress(int offset, char* Buf)
{
	int qwe = 0;
	for(int i = 0; i < V.size(); i++)
	{
		if(V[i].first == offset)
			qwe++;
		if(qwe >= 2)
		{
			V.pop_back();
			return;
		}
	}

	int cnt = offset;
	unsigned int temp = 0;
	int CCnt = 0;

	if(Buf[cnt] == '\xe9')
	{
		for(int i = cnt + 4; i >= cnt + 1; i--)
		{
			temp = temp << 8;
			temp = temp | ((char)Buf[i] & 0xff);
		}
		unsigned int Temp = offset + CCnt + temp + 5;
		V.push_back({ Temp,flag });

		GetAddress(Temp, Buf);

	}
	else
	{
		while((Buf[cnt - 1] != '\x5d') && (Buf[cnt] != '\xc3'))
		{
			if(Buf[cnt] == '\xe8' || Buf[cnt] == '\xe9')
			{
				char a = Buf[cnt];
				if(Buf[cnt - 2] == '\x89' && Buf[cnt - 1] == '\x65')
				{
					cnt++;
					CCnt++;
					continue;
				}
				for(int i = cnt + 4; i >= cnt + 1; i--)
				{
					temp = temp << 8;
					temp = temp | ((char)Buf[i] & 0xff);
				}

				int Size = V.size();
				unsigned int Temp = offset + CCnt + temp + 5;

				if(Temp < 0x400 || Temp>0x400 + 0x5400)
				{
					cnt++;
					CCnt++;
					continue;
				}
				if(a == '\xe9')
					V.push_back({ Temp,flag });
				else
					V.push_back({ Temp,flag });

				GetAddress(Temp, Buf);

				temp = 0;
				cnt += 4;
				CCnt += 4;
			}
			else if(Buf[cnt] == '\xff' && Buf[cnt + 1] == '\x25')
				return;

			else
			{
				bool bMain = CheckMain(cnt, Buf);

				if(bMain)
				{
					if(flag == 0)
					{
						flag = 1;
						//int Size = V.size();
						//V[Size - 1].second = true;
					}
					else
						flag = 0;
				}
			}
			cnt++;
			CCnt++;
		}
	}
}


void GetOffset(char* Buf)
{
	int FileBaseAddress = EntryPoint - RVA + PointerToRawData;
	V.push_back({ FileBaseAddress,flag });
	string strTemp;
	unsigned int temp = 0;
	for(int i = FileBaseAddress + 4; i >= FileBaseAddress + 1; i--)
	{
		temp = temp << 8;
		temp = temp | ((char)Buf[i] & 0xff);
	}

	V.push_back({ temp + 5 + FileBaseAddress,flag });
	int Size = V.size();
	GetAddress(V[Size - 2].first + temp + 5, Buf);//1

}

int main()
{
	FILE* fp = fopen("TestMainPatched.dll", "rb");

	if(fp)
	{
		fseek(fp, 0, SEEK_END);
		size_t stSize = ftell(fp);

		char* buf = new char[stSize];
		char* Temp = new char[stSize];
		fseek(fp, 0, SEEK_SET);
		fread(buf, stSize, 1, fp);

		GetOffset(buf);

		fclose(fp);
		fp = fopen(R"(TestMainPatchedFixed.dll)", "wb");
		fseek(fp, 0, SEEK_SET);
		/*
		for(int i = 0x400; i < 0x7e00 + 0x400; i++)
		{
			buf[i] = '\x00';
		}*/
		int ChangeCnt = 0;
		for(int i = 0; i < V.size(); i++)
		{
			int address = V[i].first;
			bool bCheck = V[i].second;

			if(bCheck)
			{

				//if(buf[address] == '\xe9')
				//	continue;
				//memcpy(buf + address, Temp + address, 5);
				int bCheckCnt = 5;
				int cc = 0;
				while(bCheckCnt--)
				{
					buf[address + cc] ^= 8;
					cc++;
				}
				ChangeCnt++;
			}

		}
		fwrite(buf, stSize, 1, fp);
		fwrite((void*)&ChangeCnt, 4, 1, fp);
		for(int i = 0; i < V.size(); i++)
		{
			int address = V[i].first;
			bool bCheck = V[i].second;
			if(bCheck)
			{
				//if(buf[address] == '\xe9')
				//	continue;
				//memcpy(buf + address, Temp + address, 5);
				fwrite((void*)&address, 4, 1, fp);
			}

		}

		fclose(fp);
	}
}