#include <iostream>
#include <stdio.h>
#include <algorithm>
#include <vector>
#include <math.h>
#include <time.h>

#define HSIZE	256
#define VSIZE	256
#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable: 4996)

bool compare(int a, int b)		//user define function
{
	return a < b;
}

struct datas {			// input vector
	int value;
	int x;
	int y;

	bool operator<(const datas& a)const		//compare function
	{
		return value < a.value;
	}
};

int main()
{
	clock_t begin, end;
	begin = clock();		//start compile time
	FILE* fp;
	unsigned char ch[VSIZE][HSIZE];
	static unsigned char ch1[VSIZE][HSIZE];
	int x, y, c;
	std::vector<datas> info;
	datas data;
	int count = 0;
	int k = 0;
	int check = 0;
	int checkNum = 1;
	unsigned int savearr[9];
	double hedge, vedge;

	if ((fp = fopen("lena.RAW", "rb")) == NULL) exit(1);
	fread(ch, 1, HSIZE * VSIZE, fp);
	fclose(fp);

	for (y = 1; y < VSIZE - 1; y++) {
		for (x = 1; x < HSIZE - 1; x++) {
			// 수직 마스크에 의한 공간 컨벌루션 처리 
			vedge = ch[y - 1][x + 1] - ch[y - 1][x - 1] + 2 * (ch[y][x + 1] - ch[y][x - 1]) + ch[y + 1][x + 1] - ch[y + 1][x - 1];

			// 수평 마스크에 의한 공간 컨벌루션 처리 	
			hedge = ch[y - 1][x - 1] - ch[y + 1][x - 1] + 2 * (ch[y - 1][x] - ch[y + 1][x]) + ch[y - 1][x + 1] - ch[y + 1][x + 1];

			//  hedge,vedge의 음수에 대한 처리와 두 수의 합   
			c = (int)sqrt(hedge * hedge + vedge * vedge);

			if (c > 255) c = 255;
			ch1[y][x] = c;
		}
	}

	if ((fp = fopen("sobelss.raw", "wb")) == NULL) exit(2);
	fwrite(ch1, 1, HSIZE * VSIZE, fp);
	fclose(fp);
	for (int i = 1; i < 255; i++)		// catch min point
	{
		for (int j = 1; j < 255; j++)
		{
			//printf("%d ", ch1[i][j]);		 // before checking labeling picture
			savearr[0] = ch1[i - 1][j - 1];  //3 by 3 layers get
			savearr[1] = ch1[i - 1][j];
			savearr[2] = ch1[i - 1][j + 1];
			savearr[3] = ch1[i][j - 1];
			savearr[4] = ch1[i][j];
			savearr[5] = ch1[i][j + 1];
			savearr[6] = ch1[i + 1][j - 1];
			savearr[7] = ch1[i + 1][j];
			savearr[8] = ch1[i + 1][j + 1];
			std::sort(savearr, savearr + 9, compare);
			if (savearr[0] == ch1[i][j])
			{
				data.value = ch1[i][j];
				data.x = i;
				data.y = j;
				info.push_back(data);
			}
		}
	}
	std::sort(info.begin(), info.end());

	unsigned char ch2[HSIZE][VSIZE] = { {0,}, };
	int xNum = -1 , yNum = -1;
	int init1 = 3 , init2 = 3;
	int initx = xNum , inity = yNum;
	for(int a = 1; a<=9; a++)			//start labeling , time complexity reduce  while replace for
	{
		for (auto i : info)
		{//check ==0 i.value == 0 checkNum == 1 start
			if (check < i.value)
			{
				check = i.value;
				checkNum++;
			}
				for (int m = 1; m <= init1; m++)
				{
					for (int n = 1; n <= init2; n++)
					{
						if (i.x + xNum >= 0 && i.x + xNum <=255 && i.y + yNum >= 0 && i.y + yNum <=255 &&ch2[i.x + xNum][i.y + yNum] == 0) ch2[i.x + xNum][i.y + yNum] = checkNum;
						yNum++;
					}
					yNum = inity;
					xNum++;
				}
				xNum = initx;
				yNum = inity;
		}
		checkNum = 1;	//reset checkNum
		check = 0;		//reset check
		init1 += 2;
		init2 += 2;
		inity = inity - 1;
		initx = initx - 1;
		xNum = xNum - 1;
		yNum = yNum - 1;
	}
	                             // checking labeling 256 x 256
	for (int i = 0; i < 256; i++)
	{
		for (int j = 0; j < 256; j++)
		{
			printf("%d ", ch2[i][j]);
		}
		printf("\n");
	}
	if ((fp = fopen("label.raw", "wb")) == NULL) exit(2);
	fwrite(ch2, 1, HSIZE* VSIZE, fp);
	fclose(fp);
	std::cout << std::endl;
	std::cout << count<<std::endl;
	end = clock();          // end compile time
	std::cout << "수행시간 : " << ((end - begin) / CLOCKS_PER_SEC) << std::endl;	
}
