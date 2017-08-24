#define _CRT_SECURE_NO_DEPRECATE
# include <iostream>
# include <string>
# include <cmath>
# include <algorithm>
# include <vector>
# include "EasyBMP.cpp"

using namespace std;

bool isequal(BMP& Image1, BMP& Image2, int i, int j)
{
	if ((Image1(i, j)->Red != Image2(i, j)->Red) ||
		(Image1(i, j)->Blue != Image2(i, j)->Blue) ||
		(Image1(i, j)->Green != Image2(i, j)->Green) )
		return(false);
	else
		return(true);
}

bool isequalbit(BMP& Image, int i, int j, int r, int g, int b)
{
	if ((Image(i, j)->Red != r) ||
		(Image(i, j)->Green != g) ||
		(Image(i, j)->Blue != b))
		return(false);
	else
		return(true);
}

int howmanynear(BMP& Image, int i, int j, int q)
{

	int res = 1;
	if (q > 5)
		return(res);
	if (!isequalbit(Image,i + 1, j,0,0,0))
		res += howmanynear(Image, i + 1, j,q+1);
	if (!isequalbit(Image, i , j-1, 0, 0, 0))
		res += howmanynear(Image, i , j-1,q+1);
	return(res);
}



int main() {
	BMP Image1;
	BMP Image2;
	BMP ImageRes;
	int maxdif;
	cin >> maxdif;
	Image1.ReadFromFile("image4.bmp"); 
	Image2.ReadFromFile("image5.bmp");
	if ((Image1.TellWidth() != Image2.TellWidth()) || (Image1.TellHeight() != Image2.TellHeight()))
	{
		cout << "Pictures have different sizes ";
		return 0;
	}
	ImageRes.SetSize(Image1.TellWidth(), Image1.TellHeight());
	ImageRes.SetBitDepth(8);
	for (int i = 0; i < Image1.TellWidth();i++)
		for (int j = 0; j < Image1.TellHeight();j++)
		{
			ImageRes(i, j)->Red = 255;
			ImageRes(i, j)->Green = 255;
			ImageRes(i, j)->Blue = 255;
			ImageRes(i, j)->Alpha = 1;
		}
	int co = 0;
	for (int i = 1; i < Image1.TellWidth();i++)
		for (int j = 1; j < Image1.TellHeight();j++)
		{
			if (!isequal(Image1, Image2, i, j))
			{
				co++;
				ImageRes(i, j)->Red = 0;
				ImageRes(i, j)->Green = 0;
				ImageRes(i, j)->Blue = 0;
				ImageRes(i, j)->Alpha = 1;
			}
		}
	cout << co;
	if (co < 100)
	{
		cout << "Pictures seem equal";
		return 0;
	}
	else
	{
		int curdif = 0;
		for (int i = 10; i < Image1.TellWidth()-10;i++)
			for (int j = 10;j < Image1.TellHeight()-10;j++)
			{
				if ((!isequalbit(ImageRes, i, j, 255, 255, 255)) &&
					(isequalbit(ImageRes, i - 1, j, 255, 255, 255)) &&
					(isequalbit(ImageRes, i, j + 1, 255, 255, 255)))
					curdif = howmanynear(ImageRes, i, j, 0);
				if (curdif > maxdif)
				{
					for (int q = i - 10; q < i + 10;q++)
						for (int k = j + 3; k < j + 9; k++)
						{
							Image1(q, k)->Red = 0;
							Image1(q, k)->Alpha = 1;
							Image1(q, k)->Blue = 0;
							Image1(q, k)->Green = 0;
						}
				}
				curdif = 0;
			}
		cout << "Pictures seem different";
		Image1.WriteToFile("image11.bmp");
		return(0);
	}
}
