#define _CRT_SECURE_NO_WARNINGS
#include<windows.h>
#include<tchar.h>
#include<commdlg.h>
#include<commctrl.h>
#include<fstream>
#include "Resource.h"
TBBUTTON tbb[3] =
{ {STD_FILENEW, ID_FILE_NEW, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0, 0, 0},
{STD_FILEOPEN, ID_FILE_OPEN, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0, 0, 0},
{STD_FILESAVE, ID_FILE_SAVE, TBSTATE_ENABLED, TBSTYLE_BUTTON, 0, 0, 0, 0}
};
HINSTANCE hInst;
VOID StatusOut(HWND hStatus, int count, TCHAR* str)
{
	TCHAR text[256];
	_stprintf(text, _T("Строк: %d"), count);
	SendMessage(hStatus, SB_SETTEXT, (WPARAM)0, (LPARAM)text);
	SendMessage(hStatus, SB_SETTEXT, (WPARAM)1, (LPARAM)str);
}
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
TCHAR WinName[] = _T("MainFrame");
int APIENTRY _tWinMain(HINSTANCE This, //Дескриптор текущего приложения
	HINSTANCE Prev, //В современных системах всегда 0
	LPTSTR cmd, //Командная строка
	int mode) //Режим отображения окна

{
	HWND hWnd; //Дескриптор главного окна программы
	MSG msg; //Структура для хранения сообщения
	WNDCLASS wc;//Класс окна
	//Определение класса окна
	hInst = This;
	wc.hInstance = This;
	wc.lpszClassName = WinName; //Имя класса окна
	wc.lpfnWndProc = WndProc; //Имя класса окна
	wc.style = CS_HREDRAW | CS_VREDRAW; //Стиль окна
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION); //Стандартная иконка
	wc.hCursor = LoadCursor(NULL, IDC_ARROW); //Стандартный курсор
	wc.lpszMenuName = NULL; //Нет меню
	wc.cbClsExtra = 0; //Нет дополнительных данных класса
	wc.cbWndExtra = 0; //Нет дополнительных данных окна
	//Заполнение окна белым цветом
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	if (!RegisterClass(&wc)) return 0;//Регистрация класса окна
	//Создание окна
	hWnd = CreateWindow(WinName,//Имя класса окна
		_T("Каркас Windows-приложения"),//Заголовок окна
		WS_OVERLAPPEDWINDOW, //Стиль окна
		CW_USEDEFAULT,//x
		CW_USEDEFAULT,//y Размеры окна
		CW_USEDEFAULT,//Width
		CW_USEDEFAULT,//Height
		HWND_DESKTOP,//Дескриптор родительской информации нет
		NULL,//Нет меню
		This,//Дескриптор приложения
		NULL);//Дополнительной информации нет
	ShowWindow(hWnd, mode);//Показать окно
	//Цикл обработки сообщений
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);//Функция трансляции кодов нажатой клавиши
		DispatchMessage(&msg);//Посылает сообщение функции WndProc()
	}
	return 0;

}
//Оконная функция вызывается операционной системой
//и получает сообщения из очереди для данного приложения
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static OPENFILENAME file;
	static int n, sx, sy;
	static HWND hEdit, hWndToolBar, hWndStatusBar;
	RECT rt;
	int m, k, aWidths[2];
	static TCHAR name[256] = _T(" ");
	char szText[0x7fff];
	std::ifstream in;
	std::ofstream out;
	TCHAR str0[] = _T("Файл");
	TCHAR str1[] = _T("Новый");
	TCHAR str2[] = _T("Открыть");
	TCHAR str3[] = _T("Сохранить");
	TCHAR str4[] = _T("Выход");
	HMENU hMenu;
	HMENU hPopupMenu;
	//Обработчик сообщений
	switch (message)
	{
	case WM_CREATE:
		hMenu = CreateMenu();
		hPopupMenu = CreatePopupMenu();
		AppendMenu(hPopupMenu, MF_STRING, ID_FILE_NEW, str1);
		AppendMenu(hPopupMenu, MF_STRING, ID_FILE_OPEN, str2);
		AppendMenu(hPopupMenu, MF_STRING, ID_FILE_SAVE, str3);
		AppendMenu(hPopupMenu, MF_STRING, ID_FILE_EXIT, str4);
		AppendMenu(hMenu, MF_POPUP, (UINT)hPopupMenu, str0);
		SetMenu(hWnd, hMenu);
		hWndToolBar = CreateToolbarEx(hWnd, WS_CHILD | WS_VISIBLE | CCS_TOP,
			2, 0, HINST_COMMCTRL, IDB_STD_SMALL_COLOR, tbb, 3, 0, 0, 0, 0,
			sizeof(TBBUTTON));
		hEdit = CreateWindow(WC_EDIT, NULL, WS_CHILD | WS_VISIBLE | WS_HSCROLL |
			WS_VSCROLL | ES_LEFT | ES_MULTILINE | ES_AUTOHSCROLL | ES_AUTOVSCROLL,
			0, 0, 0, 0, hWnd, NULL, hInst, NULL);
		file.lStructSize = sizeof(OPENFILENAME);
		file.hInstance = hInst;
		file.lpstrFilter = _T("Text\0*.txt\0Все файлы\0*.*");
		file.lpstrFile = name;
		file.nMaxFile = 256;
		file.lpstrInitialDir = _T(".\\");
		file.lpstrDefExt = _T("txt");
		hWndStatusBar =
			CreateWindow(STATUSCLASSNAME, NULL, WS_CHILD |
				WS_VISIBLE, 0, 0, 0, 0, hWnd, NULL, hInst, NULL);
		break;
	case WM_SIZE:
		sx = LOWORD(lParam);
		sy = HIWORD(lParam);
		aWidths[0] = 100;
		aWidths[1] = sx;
		GetWindowRect(hWndToolBar, &rt);
		m = rt.bottom - rt.top;
		SendMessage(hWndToolBar, TB_AUTOSIZE, 0, 0);
		GetWindowRect(hWndStatusBar, &rt);
		k = rt.bottom - rt.top;
		MoveWindow(hWndStatusBar, 0, sy - k, sx, sy, TRUE);
		SendMessage(hWndStatusBar, SB_SETPARTS, (WPARAM)2, (LPARAM)aWidths);
		StatusOut(hWndStatusBar, n, name);
		MoveWindow(hEdit, 0, m, sx, sy - m - k, TRUE);
		UpdateWindow(hEdit);
		SetFocus(hEdit);
		return 0;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case ID_FILE_NEW:
			szText[0] = '\0';
			SetWindowTextA(hEdit, szText);
			StatusOut(hWndStatusBar, 0, name);
			break;
		case ID_FILE_OPEN:
			file.lpstrTitle = _T("Открыть файл для чтения");
			file.Flags = OFN_HIDEREADONLY;
			if (!GetOpenFileName(&file)) return 1;
			in.open(name, std::ios::binary);
			in.read(szText, 0x7fff);
			if ((m = in.gcount()) == 0x7fff) {
				MessageBox(hWnd, _T("Слишком большой файл"),
					_T("Edit"), MB_YESNO | MB_ICONHAND);
				in.close();
				return 0;
			}
			szText[m] = '\0';
			in.close();
			SetWindowTextA(hEdit, szText);
			n = SendMessage(hEdit, EM_GETLINECOUNT, 0, 0);
			StatusOut(hWndStatusBar, n, name);
			break;
		case ID_FILE_SAVE:
			file.lpstrTitle = _T("Открыть файл для чтения");
			file.Flags = OFN_NOTESTFILECREATE | OFN_HIDEREADONLY;
			if (!GetOpenFileName(&file)) return 1;
			out.open(name, std::ios::binary);
			m = GetWindowTextA(hEdit, szText, 0x7fff);
			out.write(szText, 0x7fff);
			out.close();
			n = SendMessage(hEdit, EM_GETLINECOUNT, 0, 0);
			StatusOut(hWndStatusBar, n, name);
			break;
		case ID_FILE_EXIT: DestroyWindow(hWnd); break;
		default: return DefWindowProc(hWnd, message, wParam, lParam);
		}
		return 0;
	case WM_DESTROY:PostQuitMessage(0);
		break;//Завершение программы
		//Обработка сообщений по умолчанию
	default: return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}