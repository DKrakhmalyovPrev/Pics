/*
Суть алгоритма: по разнице двух изображений строим "маску" (белое изображение того же формата с чёрными битами на месте
различий. Анализируем маску: ищем все связные блоки битов(практически единозначно определяются по угловому
биту. Определяем мощность блоков. Если хотя бы один блок превосходит по размерам допустимое значение "похожести" --
картинки можно считать разными. Параллельно по координатам каждого блока, превосходящего допустимое значение,
строится "опознаватель" на исходном изображении(в данном случае это чёрная полоска 10*6 битов. 
Также в качестве начального предположения считаем количество битов, различных у обоих изображений.
Если это число не превосходит определённого заданного значения(например, 100 бит), картинки можно считать подобными.
Если это число превосходит заданное значение, картинки считаются разными независимо от того, существует ли блок 
различных битов больший, чем допустимое значение.
Например, картинка может не содержать "опознавателей", но быть отличной от второй исходной благодаря незначительным,
но многочисленным изменениям.
*/
#define _CRT_SECURE_NO_DEPRECATE
# include <iostream>
# include <string>
# include <cmath>
# include <algorithm>
# include <vector>
# include "EasyBMP.cpp"

using namespace std;

bool isequal(BMP& Image1, BMP& Image2, int i, int j) // Равны ли биты, стоящие на определённой позиции в двух картинках
{
	if ((Image1(i, j)->Red != Image2(i, j)->Red) ||
		(Image1(i, j)->Blue != Image2(i, j)->Blue) ||
		(Image1(i, j)->Green != Image2(i, j)->Green) )
		return(false);
	else
		return(true);
}

bool isequalbit(BMP& Image, int i, int j, int r, int g, int b) // Равно ли значение опр.бита заданным параметрам
{
	if ((Image(i, j)->Red != r) ||
		(Image(i, j)->Green != g) ||
		(Image(i, j)->Blue != b))
		return(false);
	else
		return(true);
}

int howmanynear(BMP& Image, int i, int j, int q)
//Сколько битов, отличных от стандартного значения, принадлежат связной группе.
//Количество уровней, до которых идёт рекурсия вправо и вверх, варьируется в зависимости от допустимого сдвига и мощности
{

	int res = 1;
	if (q > 20)
		return(res);
	if (!isequalbit(Image,i + 1, j,255,255,255))
		res += howmanynear(Image, i + 1, j,q+1);
	if (!isequalbit(Image, i , j-1, 255, 255, 255))
		res += howmanynear(Image, i , j-1,q+1);
	return(res);
}

class ImageMask
{
public:
	ImageMask(BMP& Image)
	{
		ImageM.SetSize(Image.TellWidth(), Image.TellHeight());
		ImageM.SetBitDepth(8);
		for (int i = 0; i < Image.TellWidth();i++)
			for (int j = 0; j < Image.TellHeight();j++)
			{
				ImageM(i, j)->Red = 255;
				ImageM(i, j)->Green = 255;
				ImageM(i, j)->Blue = 255;
				ImageM(i, j)->Alpha = 1;
			}
	}
	int GetMask(BMP& Image1, BMP& Image2)
	{
		int co = 0;
		for (int i = 1; i < Image1.TellWidth();i++)
			for (int j = 1; j < Image1.TellHeight();j++)
			{
				if (!isequal(Image1, Image2, i, j))
				{
					co++;
					ImageM(i, j)->Red = 0;
					ImageM(i, j)->Green = 0;
					ImageM(i, j)->Blue = 0;
					ImageM(i, j)->Alpha = 1;
				}
			}
		return co;
	}
	int GetCurdif(int i, int j)
	{
		int curdif = 0;
		if ((!isequalbit(ImageM, i, j, 255, 255, 255)) &&
			(isequalbit(ImageM, i - 1, j, 255, 255, 255)) &&
			(isequalbit(ImageM, i, j + 1, 255, 255, 255)))
			curdif = howmanynear(ImageM, i, j, 0);
		return curdif;
	}

private:
	BMP ImageM;
};

void DoMark(BMP& Image1, int i, int j)
{
	for (int q = i - 10; q < i + 10;q++)
		for (int k = j + 3; k < j + 9; k++)
		{
			Image1(q, k)->Red = 0;
			Image1(q, k)->Alpha = 1;
			Image1(q, k)->Blue = 0;
			Image1(q, k)->Green = 0;
		}
};


int main() {
	BMP Image1;
	BMP Image2;
	
	int maxdifx, maxdify;
	cin >> maxdifx >> maxdify;
	// Читаем допустимый размер разницы
	Image1.ReadFromFile("image4.bmp"); 
	Image2.ReadFromFile("image5.bmp");
	// Читаем два файла из памяти
	if ((Image1.TellWidth() != Image2.TellWidth()) || (Image1.TellHeight() != Image2.TellHeight()))
	{
		cout << "Pictures have different sizes ";
		return 0;
	}
	//Если размеры изображений разные -- они сразу признаются разными
	ImageMask ImageRes(Image1);
	//Создаём белую картинку того же размера
	int co;
	co = ImageRes.GetMask(Image1, Image2);
	// Побитово сравниваем картинки. Разницу между ними наносим на маску в виде чёрных битов
	cout << co;
	if (co < 100)
	{
		cout << "Pictures seem equal";
		return 0;
	}
	//Делаем допущение, что картинки, отличающиеся незначительно, равны
	else
	{
		int curdif = 0;
		for (int i = 10; i < Image1.TellWidth()-10;i++)
			for (int j = 10;j < Image1.TellHeight()-10;j++)
			{
				curdif = ImageRes.GetCurdif(i, j);
				//Найдя чёрный бит в маске, слева и снизу от которого белые, предполагаем,
				//что он может стать основой блоку битов различия.
				//Если блок битов превышает размер допустимой разницы -- он считается различием
				if (curdif > maxdifx*maxdify)
				{
					DoMark(Image1, i, j);
					//Наносим опознавательный знак (чёрную полоску) на первое изображение
				}
				curdif = 0;
			}
		cout << "Pictures seem different";
		Image1.WriteToFile("imageRes.bmp");
		//выводим в консоль итог и сохраняем картинку
		return(0);
	}
}