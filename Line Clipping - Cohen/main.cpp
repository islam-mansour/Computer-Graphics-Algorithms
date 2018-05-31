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

struct Point{
    double x,y;

    Point(){}

    Point(double x, double y)
    {
        this->x = x;
        this->y = y;
    }
};

struct Code
{
    bool left, right, top, bottom; // 0 -> inside Clipping region, 1 otherwise
    Code(){
        left = right = top = bottom = 0;
    }

    void setCode(Point p, int xLeft, int xRight, int yTop, int yBottom)
    {

        if(p.x < xLeft)
            this->left = 1;
        else
            this->left = 0;

        if(p.x > xRight)
            this->right = 1;
        else
            this->right = 0;

        if(p.y < yTop)
            this->top = 1;
        else
            this->top = 0;

        if(p.y > yBottom)
            this->bottom = 1;
        else
            this->bottom = 0;

    }

    bool isOut()
    {
        return (left | right | top | bottom);
    }
};

HDC hdc;
PAINTSTRUCT ps;

void draw(Point p1, Point p2)
{
    MoveToEx(hdc, p1.x, p1.y, NULL);
    LineTo(hdc, p2.x, p2.y);
}

Point vIntersect(Point s, Point e, int x)
{
    Point p;
    p.x = x;
    p.y = s.y +(x - s.x) * (e.y - s.y) / (e.x - s.x);
    return p;

}
Point hIntersect(Point s, Point e, int y)
{
    Point p;
    p.y = y;
    p.x = s.x + (y - s.y) * (e.x - s.x) / (e.y - s.y);
    return p;

}

Point p1, p2;
bool f1, f2;

LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)                  /* handle the messages */
    {
        case WM_DESTROY:
            PostQuitMessage (0);       /* send a WM_QUIT to the message queue */
            break;

        case WM_LBUTTONDOWN:

            if(!f1){
                p1.x = LOWORD(lParam);
                p1.y = HIWORD(lParam);
                f1 = true;
            } else if (!f2){
                p2.x = LOWORD(lParam);
                p2.y = HIWORD(lParam);
                f2 = true;
            }

            break;

        case WM_PAINT:
        {

            int xLeft = 50, xRight = 300;
            int yTop = 50, yBottom = 300;


            Point topLeft(xLeft, yTop);
            Point bottomLeft(xLeft, yBottom);
            Point topRight(xRight, yTop);
            Point bottomRight(xRight, yBottom);


            hdc = BeginPaint(hwnd, &ps);

            draw(topLeft, bottomLeft);
            draw(bottomLeft, bottomRight);
            draw(bottomRight, topRight);
            draw(topRight, topLeft);

            InvalidateRect(hwnd, NULL, false);

            if (f1 && f2)
            {

                Code code1;
                code1.setCode(p1, xLeft, xRight, yTop ,yBottom);
                Code code2;
                code2.setCode(p2, xLeft, xRight, yTop ,yBottom);

                if (code1.isOut() && code1.isOut()) // both out
                {

                    f1 = f2 = false;
                    EndPaint(hwnd, &ps);
                    return 0;
                }

                while(true) // one of them is in
                {

                    if (!code1.isOut() && !code2.isOut()) // both in
                    {
                        draw(p1, p2);
                        break;
                    }

                    if(code1.isOut())
                    {

                        Point p;

                        if(code1.left)
                            p = vIntersect(p1, p2, xLeft);
                        else if(code1.top)
                            p = hIntersect(p1, p2, yTop);
                        else if(code1.right)
                            p = vIntersect(p1, p2, xRight);
                        else
                            p = hIntersect(p1, p2, yBottom);

                        p1 = p;

                        code1.setCode(p1, xLeft, xRight, yTop, yBottom);


                    }
                    else if (code2.isOut())
                    {

                        Point p;

                        if(code2.left)
                            p = vIntersect(p1, p2, xLeft);
                        else if(code2.top)
                            p = hIntersect(p1, p2, yTop);
                        else if(code2.right)
                            p = vIntersect(p1, p2, xRight);
                        else
                            p = hIntersect(p1, p2, yBottom);

                        p2 = p;

                        code2.setCode(p2, xLeft, xRight, yTop, yBottom);

                    }


                }

                f1 = f2 = false;
                EndPaint(hwnd, &ps);

            }

            break;
        }

        default:            /* for messages that we don't deal with */
            return DefWindowProc (hwnd, message, wParam, lParam);
    }

    return 0;
}
