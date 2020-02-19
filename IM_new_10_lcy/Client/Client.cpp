// Client.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "resource.h"
#include "FriList.h"
#include <winsock2.h>
#include "ThreadFunc.h"


#pragma comment(lib, "ws2_32.lib")

BOOL CALLBACK MainProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK RegisProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK LoginProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK ChatProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK AddFriProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

//DWORD WINAPI ChatThreadProc(LPVOID lParam);

SOCKET g_sCLient;
FRIEND g_MyInfo;
char g_FriID[11];
NEWS NewSend;
HWND hWndChat;
HBITMAP g_BitBoy;
HBITMAP g_bitGirl;
HBITMAP g_BitListBoy;
HBITMAP g_bitListGirl;
HBITMAP g_BitListBoy1;
HBITMAP g_bitListGirl1;

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
 	// TODO: Place code here.

	WSADATA wd;

	WSAStartup(0x0202, &wd);

	g_sCLient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	sockaddr_in addrClient;

	addrClient.sin_family = AF_INET;
	addrClient.sin_addr.s_addr = inet_addr("192.168.2.136");
	addrClient.sin_port = htons(6060);

	if(SOCKET_ERROR == connect(g_sCLient, (sockaddr *)&addrClient, sizeof(addrClient)))
	{
		closesocket(g_sCLient);

		return -1;
	}

	DialogBox(hInstance, MAKEINTRESOURCE(IDD_LOGIN), NULL, LoginProc);

	WSACleanup();

	return 0;
}

BOOL CALLBACK MainProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
		case WM_INITDIALOG:
			{
				g_BitBoy = (HBITMAP)LoadImage(GetModuleHandle(NULL), TEXT("boy.bmp"), IMAGE_BITMAP, 80, 80, LR_DEFAULTCOLOR | LR_CREATEDIBSECTION | LR_LOADFROMFILE);
				g_bitGirl = (HBITMAP)LoadImage(GetModuleHandle(NULL), TEXT("girl.bmp"), IMAGE_BITMAP, 80, 80, LR_DEFAULTCOLOR | LR_CREATEDIBSECTION | LR_LOADFROMFILE);
				g_BitListBoy = (HBITMAP)LoadImage(GetModuleHandle(NULL), TEXT("Listboy.bmp"), IMAGE_BITMAP, 24, 24, LR_DEFAULTCOLOR | LR_CREATEDIBSECTION | LR_LOADFROMFILE);
				g_bitListGirl = (HBITMAP)LoadImage(GetModuleHandle(NULL), TEXT("Listgirl.bmp"), IMAGE_BITMAP, 24, 24, LR_DEFAULTCOLOR | LR_CREATEDIBSECTION | LR_LOADFROMFILE);
				g_BitListBoy1 = (HBITMAP)LoadImage(GetModuleHandle(NULL), TEXT("Listboy1.bmp"), IMAGE_BITMAP, 24, 24, LR_DEFAULTCOLOR | LR_CREATEDIBSECTION | LR_LOADFROMFILE);
				g_bitListGirl1 = (HBITMAP)LoadImage(GetModuleHandle(NULL), TEXT("Listgirl1.bmp"), IMAGE_BITMAP, 24, 24, LR_DEFAULTCOLOR | LR_CREATEDIBSECTION | LR_LOADFROMFILE);

				CloseHandle(CreateThread(NULL, 0, RecvThread, (LPVOID)hDlg, NULL, 0));
			}
			break;
		case WM_COMMAND:
			{
				if(HIWORD(wParam) == BN_CLICKED)
				{
					if(LOWORD(wParam) == IDB_INFO)
					{
						STATE SendState;
						
						SendState.m_ProtoID = USER_INFO;
						strcpy(SendState.m_szID, g_MyInfo.m_szID);
						strcpy(SendState.m_szState, g_MyInfo.m_szState);

						send(g_sCLient, (char *)&SendState, sizeof(STATE), 0);
					}

					if(LOWORD(wParam) == IDB_ADD)
					{
						DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_ADD), hDlg, AddFriProc);
					}

					if(LOWORD(wParam) == IDB_DELETE)
					{
						DELETES SendDelete;
						int nIndex = SendDlgItemMessage(hDlg, IDL_FRIEND, LB_GETCURSEL, 0, 0);
						
						SendDlgItemMessage(hDlg, IDL_FRIEND, LB_GETTEXT, (WPARAM)nIndex, (LPARAM)(SendDelete.m_szToID));
						SendDlgItemMessage(hDlg, IDL_FRIEND, LB_DELETESTRING, (WPARAM)nIndex, 0);
						strcpy(SendDelete.m_szFromID, g_MyInfo.m_szID);

						for(int i = 0; SendDelete.m_szToID[i] != '\0'; ++i)
						{
							if(' ' == SendDelete.m_szToID[i])
							{
								SendDelete.m_szToID[i] = '\0';
								break;
							}
						}
						
						SendDelete.m_ProtoID = DELETE_APPLY;
						send(g_sCLient, (char *)&SendDelete, sizeof(DELETES), 0);
					}
					if (IDR_ONLINE == LOWORD(wParam))
					{
						STATE Sendstate;
						strcpy(Sendstate.m_szState, "在线");
						
						if (0 != strcmp(Sendstate.m_szState, g_pFriHead->m_szState))
						{
							strcpy(Sendstate.m_szID, g_MyInfo.m_szID);
							
							Sendstate.m_ProtoID = REFRESH_APPLY;
							
							send(g_sCLient, (char *)&Sendstate, sizeof(STATE), 0);
						}
					}
					if (IDR_OUTLINE == LOWORD(wParam))
					{
						STATE state;
						strcpy(state.m_szState, "离线");
						
						if (0 != strcmp(state.m_szState, g_pFriHead->m_szState))
						{
							strcpy(state.m_szID, g_MyInfo.m_szID);
			
							state.m_ProtoID = REFRESH_APPLY;
							send(g_sCLient, (char *)&state, sizeof(STATE), 0);
						}
					}
					if (IDB_CHAT == LOWORD(wParam))
					{
						int nIndex = SendDlgItemMessage(hDlg, IDL_FRIEND, LB_GETCURSEL, 0, 0);
						SendDlgItemMessage(hDlg, IDL_FRIEND, LB_GETTEXT, nIndex, (LPARAM)g_FriID);

						for(int i = 0; g_FriID[i] != '\0'; ++i)
						{
							if(' ' == g_FriID[i])
							{
								g_FriID[i] = '\0';
								break;
							}
						}

						NewSend.m_Frist = 0;
						DialogBox((HINSTANCE)GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_CHAR), hDlg, ChatProc);
					}
					if (IDB_FRI_INFO == LOWORD(wParam))
					{
						STATE SendState;

						int nIndex = SendDlgItemMessage(hDlg, IDL_FRIEND, LB_GETCURSEL, 0, 0);
						SendDlgItemMessage(hDlg, IDL_FRIEND, LB_GETTEXT, nIndex, (LPARAM)g_FriID);
						
						for(int i = 0; g_FriID[i] != '\0'; ++i)
						{
							if(' ' == g_FriID[i])
							{
								g_FriID[i] = '\0';
								break;
							}
						}
						
						SendState.m_ProtoID = USER_INFO;
						strcpy(SendState.m_szID, g_FriID);
						
						send(g_sCLient, (char *)&SendState, sizeof(STATE), 0);
					}

				}

			}
			break;
		case WM_MEASUREITEM:
			{
				LPMEASUREITEMSTRUCT lpmis = (LPMEASUREITEMSTRUCT) lParam;

				lpmis->itemHeight = 30;
			}
		case WM_DRAWITEM:
			{
				LPDRAWITEMSTRUCT lpdis = (LPDRAWITEMSTRUCT) lParam;
				
				if (lpdis->CtlType == ODT_BUTTON)
				{
					HDC hMem = CreateCompatibleDC(lpdis->hDC);

					if(0 == strcmp("男", g_MyInfo.m_szSex))
					{
						SelectObject(hMem, g_BitBoy);
					}

					if(0 == strcmp("女", g_MyInfo.m_szSex))
					{
						SelectObject(hMem, g_bitGirl);
					}
					StretchBlt(lpdis->hDC, lpdis->rcItem.left, lpdis->rcItem.top ,80 , 80, hMem, 0, 0, 80, 80, SRCCOPY);
				
					DeleteDC(hMem);
				}

				if (lpdis->CtlType == ODT_LISTBOX)
				{
					HDC hBmp = CreateCompatibleDC(lpdis->hDC);
					TCHAR szName[50] = "";
					
					SendDlgItemMessage(hDlg, IDL_FRIEND, LB_GETTEXT, (WPARAM)(lpdis->itemID), (LPARAM)szName);
					
					for(int i = 0; szName[i] != '\0'; ++i)
					{
						if(' ' == szName[i])
						{
							szName[i] = '\0';
							break;
						}
					}

					FRIEND *pPos = FindNode(g_pFriHead, szName);

					if(NULL == pPos)
					{
						return FALSE;
					}

					sprintf(szName, "%s  %s  %s", pPos->m_szID, pPos->m_szName, pPos->m_szState);
					
					if (0 == strcmp(pPos->m_szSex, "男"))
					{
						if(0 == strcmp("离线", pPos->m_szState))
						{
							SelectObject(hBmp, g_BitListBoy1);
						}
						else
						{
							SelectObject(hBmp, g_BitListBoy);
						}
					}	
					else
					{
						if(0 == strcmp("离线", pPos->m_szState))
						{
							SelectObject(hBmp, g_bitListGirl1);
						}
						else
						{
							SelectObject(hBmp, g_bitListGirl);
						}
					}
					
					DrawText(lpdis->hDC, szName, strlen(szName), &(lpdis->rcItem), DT_CENTER | DT_VCENTER | DT_SINGLELINE);
				
					BitBlt(lpdis->hDC, lpdis->rcItem.left+2, lpdis->rcItem.top+2 ,24 , 24, hBmp, 0, 0, SRCCOPY);


					if (lpdis->itemAction & ODA_SELECT)
					{
						DrawFocusRect(lpdis->hDC, &(lpdis->rcItem));
					}
/*
					HBRUSH hBrush = CreateSolidBrush(RGB(255, 0, 0));
					HBRUSH hOldBrush = (HBRUSH)SelectObject(lpdis->hDC, hBrush);

					if (1 == lpdis->itemData)
					{
						Ellipse(lpdis->hDC, lpdis->rcItem.right-30, lpdis->rcItem.top+5, lpdis->rcItem.right -5, lpdis->rcItem.bottom-5);
					}

					SelectObject(lpdis->hDC, hOldBrush);

					DeleteObject(hBrush);
					DeleteObject(hOldBrush);*/
					DeleteObject(hBmp);
				}
			}
			break;
		case WM_CLOSE:
			{
				STATE state;
				strcpy(state.m_szState, "离线");
				
				if (0 != strcmp(state.m_szState, g_pFriHead->m_szState))
				{
					strcpy(state.m_szID, g_MyInfo.m_szID);
					
					state.m_ProtoID = REFRESH_APPLY;
					send(g_sCLient, (char *)&state, sizeof(STATE), 0);
				}

				EndDialog(hDlg, WM_CLOSE);
			}
			break;
		default:
			return FALSE;
	}

	return TRUE;
}

BOOL CALLBACK LoginProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
		case WM_INITDIALOG:
			{
			
			}
			break;
		case WM_COMMAND:
			{
				if (HIWORD(wParam) == BN_CLICKED)
				{
					if(LOWORD(wParam) == IDB_REG)
					{
						DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_REG), NULL, RegisProc);
					}

					if(LOWORD(wParam) == IDB_LOG)
					{
						LOGIN Login;

						GetWindowText(GetDlgItem(hDlg, IDE_LOG_ID), Login.m_szID, sizeof(Login.m_szID));
						GetWindowText(GetDlgItem(hDlg, IDE_LOG_PSW), Login.m_szPsd, sizeof(Login.m_szState));

						if(BST_CHECKED == SendDlgItemMessage(hDlg, IDC_INLINE, BM_GETSTATE, 0, 0))
						{
							strcpy(Login.m_szState, "在线");
						}

						if(BST_CHECKED == SendDlgItemMessage(hDlg, IDC_OUTLINE, BM_GETSTATE, 0, 0))
						{
							strcpy(Login.m_szState, "离线");
						}
						

						Login.m_ProtoID = LOGIN_APPLY;

						send(g_sCLient, (char *)&Login, sizeof(LOGIN), 0);

						char szBuf[255] = "";
						char *pBuf = szBuf;
						
						int nRec = recv(g_sCLient, pBuf, 255, 0);
						
						if(nRec > 0)
						{
							EVNET RecvEvent = *(EVNET *)pBuf;
							
							if(RecvEvent.m_nProtoID == LOGIN_ASW)
							{
								if (RecvEvent.m_nEvent == LOGIN_OK)
								{
									EndDialog(hDlg, WM_CLOSE);
									DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_MAIN), NULL, MainProc);
								}
								if(LOGIN_NO == RecvEvent.m_nEvent)
								{
									MessageBox(hDlg, "登录失败，请重新登录!!!", "提示", MB_OK);
								}
							}
						}
							
					}
				}
			}
			break;
		case WM_CLOSE:
			{
				EndDialog(hDlg, WM_CLOSE);
			}
			break;
		default:
			return FALSE;
	}
	
	return TRUE;
}

BOOL CALLBACK RegisProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
		case WM_INITDIALOG:
			{
				SendDlgItemMessage(hDlg, IDC_SEX, CB_ADDSTRING, 0, (LPARAM)"男");
				SendDlgItemMessage(hDlg, IDC_SEX, CB_ADDSTRING, 0, (LPARAM)"女");
			}
			break;
		case WM_COMMAND:
			{

				if(HIWORD(wParam) == BN_CLICKED)
				{
					if(LOWORD(wParam) == IDB_REG_OK)
					{
						REGIS Regis;
						
						GetWindowText(GetDlgItem(hDlg, IDE_REG_ID), Regis.m_szID, sizeof(Regis.m_szID));
						GetWindowText(GetDlgItem(hDlg, IDE_REG_PSW), Regis.m_szPsd, sizeof(Regis.m_szPsd));
						GetWindowText(GetDlgItem(hDlg, IDE_NAME), Regis.m_szName, sizeof(Regis.m_szName));
						GetWindowText(GetDlgItem(hDlg, IDC_SEX), Regis.m_szSex, sizeof(Regis.m_szSex));
						GetWindowText(GetDlgItem(hDlg, IDE_AGE), Regis.m_szAge, sizeof(Regis.m_szAge));
						GetWindowText(GetDlgItem(hDlg, IDE_EMAIL), Regis.m_szEmail, sizeof(Regis.m_szEmail));
						
		
						Regis.m_ProtoID = REG_APPLY;
						send(g_sCLient, (char *)&Regis, sizeof(REGIS), 0);
						
						
						char szBuf[255] = "";
						char *pBuf = szBuf;
						
						int nRec = recv(g_sCLient, szBuf, 255, 0);
						
						if(nRec > 0)
						{
							EVNET RecvEvent = *(EVNET *)pBuf;
							
							if(RecvEvent.m_nProtoID == REG_ASW)
							{				
								if (REG_OK == RecvEvent.m_nEvent)
								{
									MessageBox(hDlg, "注册成功!", "提示", MB_OK);
									EndDialog(hDlg, WM_CLOSE);
								}
								if(REG_NO == RecvEvent.m_nEvent)
								{
									MessageBox(hDlg, "账号已存在， 注册失败，请重新注册!!!", "提示", MB_OK);
								}
							}
						}
					}

					if(LOWORD(wParam) == IDB_REG_CLOSE)
					{
						EndDialog(hDlg, WM_CLOSE);
					}
				}
				
			}
			break;
		case WM_CLOSE:
			{
				EndDialog(hDlg, WM_CLOSE);
			}
			break;
		default:
			return FALSE;
	}
	
	return TRUE;
}

BOOL CALLBACK ChatProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
		case WM_INITDIALOG:
			{
				hWndChat = hDlg;

				if ((0 == NewSend.m_Frist) && 0 != strlen(NewSend.m_szMessage))
				{
					SendDlgItemMessage(hWndChat, IDL_CHAT, LB_ADDSTRING, 0, (LPARAM)(NewSend.m_szMessage));
				}
			}
			break;
		case WM_COMMAND:
			{
				SetWindowText(hDlg,  g_FriID);
				
				if (BN_CLICKED == HIWORD(wParam))
				{
					if (IDB_SEND == LOWORD(wParam))
					{
						char szMessage[255] = "";
						strcpy(NewSend.m_szFromID, g_pFriHead->m_szID);
						strcpy(NewSend.m_szToID, g_FriID);
						GetDlgItemText(hDlg, IDE_CHAT, NewSend.m_szMessage, 150);
						SetDlgItemText(hDlg, IDE_CHAT, "");

						strcpy(szMessage, NewSend.m_szMessage);
						strcat(szMessage, " :");
						strcat(szMessage, NewSend.m_szFromID);

						SendDlgItemMessage(hDlg, IDL_CHAT, LB_ADDSTRING, 0, (LPARAM)szMessage);
						int nIndex = SendDlgItemMessage(hDlg, IDL_CHAT, LB_GETCOUNT, 0, 0);
						SendDlgItemMessage(hDlg, IDL_CHAT, LB_SETITEMDATA, (WPARAM)(nIndex-1), (LPARAM)1);

						NewSend.m_ProtoID = MESSAGE_SEND;
						send(g_sCLient, (char *)&NewSend, sizeof(NEWS), 0);
					}
				}
			}
			break;
		case WM_MEASUREITEM:
			{
				LPMEASUREITEMSTRUCT lpmis = (LPMEASUREITEMSTRUCT) lParam;

				lpmis->itemHeight = 30;
			}
			break;
		case WM_DRAWITEM:
			{
				LPDRAWITEMSTRUCT lpdis = (LPDRAWITEMSTRUCT) lParam;
				
				if (lpdis->CtlType == ODT_LISTBOX)
				{					
					TCHAR szName[128] = "";

					SendDlgItemMessage(hDlg, IDL_CHAT, LB_GETTEXT, (WPARAM)(lpdis->itemID), (LPARAM)szName);	
					
					if (1 == lpdis->itemData)
					{
						DrawText(lpdis->hDC, szName, strlen(szName), &(lpdis->rcItem), DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
					}
					else
					{
						DrawText(lpdis->hDC, szName, strlen(szName), &(lpdis->rcItem), DT_LEFT | DT_VCENTER | DT_SINGLELINE);
					}

					if (lpdis->itemAction & ODA_SELECT)
					{
						DrawFocusRect(lpdis->hDC, &(lpdis->rcItem));
					}
				}
			}
			break;
		case WM_CLOSE:
			{
				FRIEND *pFri = FindNode(g_pFriHead, g_FriID);

				NewSend.m_Frist = 0;
			//	SendDlgItemMessage(hDlg,IDL_FRIEND, LB_SETITEMDATA, (WPARAM)(pFri->m_nCount), (LPARAM)0);
			//	InvalidateRect(hDlgMain, NULL, TRUE);
			//	strcpy(NewSend.m_szMessage, "");
				EndDialog(hDlg, WM_CLOSE);
			}
			break;
		default:
			return FALSE;
	}
	
	return TRUE;
}

BOOL CALLBACK AddFriProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
		case WM_INITDIALOG:
			{
				
			}
			break;
		case WM_COMMAND:
			{
				if(HIWORD(wParam) == BN_CLICKED)
				{
					if(LOWORD(wParam) == IDB_ADD)
					{
						ADD AddInfo;
						
						strcpy(AddInfo.m_szFromID, g_MyInfo.m_szID);
						GetWindowText(GetDlgItem(hDlg, IDE_ADD_ID), AddInfo.m_szToID, sizeof(AddInfo.m_szToID));
						
						AddInfo.m_ProtoID = ADD_APPLY;
						send(g_sCLient, (char *)&AddInfo, sizeof(ADD), 0);


						EndDialog(hDlg, WM_CLOSE);
					}
					if(LOWORD(wParam) == IDB_CANCEL)
					{
						EndDialog(hDlg, WM_CLOSE);
					}
				}
			}
			break;
		case WM_CLOSE:
			{
				EndDialog(hDlg, WM_CLOSE);
			}
			break;
		default:
			return FALSE;
	}
	
	return TRUE;
}
