#if defined(UNICODE) && !defined(_UNICODE)
    #define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
    #define UNICODE
#endif

#include <tchar.h>
#include <windows.h>
#include <iostream>
#include <math.h>
using namespace std;

/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);

/*  Make the class name into a global variable  */
TCHAR szClassName[ ] = _T("CodeBlocksWindowsApp");

int WINAPI WinMain (HINSTANCE hThisInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR lpszArgument,
                     int nCmdShow)
{
    HWND hwnd;               /* This is the handle for our window */
    MSG messages;            /* Here messages to the application are saved */
    WNDCLASSEX wincl;        /* Data structure for the windowclass */

    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
    wincl.style = CS_DBLCLKS;                 /* Catch double-clicks */
    wincl.cbSize = sizeof (WNDCLASSEX);

    /* Use default icon and mouse-pointer */
    wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;                 /* No menu */
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    /* Use Windows's default colour as the background of the window */
    wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND;

    /* Register the window class, and if it fails quit the program */
    if (!RegisterClassEx (&wincl))
        return 0;

    /* The class is registered, let's create the program*/
    hwnd = CreateWindowEx (
           0,                   /* Extended possibilites for variation */
           szClassName,         /* Classname */
           _T("Code::Blocks Template Windows App"),       /* Title Text */
           WS_OVERLAPPEDWINDOW, /* default window */
           CW_USEDEFAULT,       /* Windows decides the position */
           CW_USEDEFAULT,       /* where the window ends up on the screen */
           544,                 /* The programs width */
           375,                 /* and height in pixels */
           HWND_DESKTOP,        /* The window is a child-window to desktop */
           NULL,                /* No menu */
           hThisInstance,       /* Program Instance handler */
           NULL                 /* No Window Creation data */
           );

    /* Make the window visible on the screen */
    ShowWindow (hwnd, nCmdShow);

    /* Run the message loop. It will run until GetMessage() returns 0 */
    while (GetMessage (&messages, NULL, 0, 0))
    {
        /* Translate virtual-key messages into character messages */
        TranslateMessage(&messages);
        /* Send message to WindowProcedure */
        DispatchMessage(&messages);
    }

    /* The program return-value is 0 - The value that PostQuitMessage() gave */
    return messages.wParam;
}


/*  This function is called by the Windows function DispatchMessage()  */

int xs, ys, xe, ye;
bool flag1=false, flag2=false;
HDC hdc;
PAINTSTRUCT ps;

LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)                  /* handle the messages */
    {
        case WM_DESTROY:
            PostQuitMessage (0);       /* send a WM_QUIT to the message queue */
            break;

        case WM_LBUTTONDOWN:

            if (!flag1){
                xs = LOWORD(lParam);
                ys = HIWORD(lParam);
                flag1 = true;
            } else if (!flag2){
                xe = LOWORD(lParam);
                ye = HIWORD(lParam);
                flag2 = true;
            }

            break;

        case WM_PAINT:

            if (flag1 && flag2){

                hdc = BeginPaint(hwnd, &ps);

                if(xs > xe){
                    swap(xs, xe);
                    swap(ys, ye);
                }

                int dx = xe-xs, dy = ye-ys;
                double m = (double)dx/dy;

                double xMid, yMid, xInc, yInc;

                if(0<m && m<1){
                    xMid = 1, yMid = 0.5;
                    xInc = 1, yInc = 1;
                } else if (m>1){
                    xMid = 0.5, yMid = 1;
                    xInc = 1, yInc = 1;
                } else if (m<-1){
                    xMid = 0.5, yMid = -1;
                    xInc = 1, yInc = -1;
                } else if (0>m && m>-1){
                    xMid = 1, yMid = -0.5;
                    xInc = 1, yInc = -1;
                }

                if(abs(dx) > abs(dy)){
                    for(int x=xs, y=ys; x<xe; x+=xInc){

                        SetPixel(hdc, x, y, RGB(0,255,0));

                        double d = (y+yMid-ys)*dx - (x+xMid-xs)*dy;

                        if(d > 0)
                            y+=yInc;

                        InvalidateRect(hwnd, NULL,false);

                    }
                } else {
                    for(int x=xs, y=ys; y<ye; y+=yInc){

                        SetPixel(hdc, x, y, RGB(0,255,0));

                        double d = (y+yMid-ys)*dx - (x+xMid-xs)*dy;

                        if(d < 0)
                            x+=xInc;

                        InvalidateRect(hwnd, NULL,false);

                    }
                }

                flag1 = flag2 = false;

            }

            EndPaint(hwnd, &ps);

            break;

        default:                      /* for messages that we don't deal with */
            return DefWindowProc (hwnd, message, wParam, lParam);
    }

    return 0;
}
