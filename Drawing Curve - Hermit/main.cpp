#if defined(UNICODE) && !defined(_UNICODE)
    #define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
    #define UNICODE
#endif

#include <tchar.h>
#include <windows.h>
#include <math.h>
#include <iostream>

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

class Point
{
    public:

        double x, y;

        Point(double x=0, double y=0)
        {
            this->x = x;
            this->y = y;
        }
};

class Vector4
{

    public:

        double V[4];

        Vector4(double x=0, double y=0, double z=0, double w=0)
        {
            V[0] = x;
            V[1] = y;
            V[2] = z;
            V[3] = w;
        }
        double& operator[](int index)
        {
            return V[index];
        }
        double operator*(Vector4& V1)
        {
            double ret = 0;
            for(int i=0; i<4; ++i)
            {
                ret += this->V[i] * V1[i];
            }
            return ret;
        }
};

class Matrix4
{
    public:

        Vector4 M[4];

        Matrix4(double H[])
        {
            memcpy(M , H, 16*sizeof(double));
        }
        Vector4& operator[](int index)
        {
            return M[index];
        }
        Vector4 operator*(Vector4 &V) // Multiplication of matrix and vector
        {
            Vector4 ret;

            for(int i=0; i<4; ++i)
            {
                for(int j=0; j<4; ++j)
                {
                    ret[i] += this->M[i][j] * V[j];
                }
            }
            return ret;
        }

};

Vector4 getCoeff(double x0, double s0, double x1, double s1)
{
    static double H[16] = {2,1,-2,1,-3,-2,3,-1,0,1,0,0,1,0,0,0};
    static Matrix4 basis(H);
    Vector4 v(x0, s0, x1, s1);

    return (basis * v);
}

Point p1, p2, t1, t2;
bool  f1, f2, f3, f4;

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
            if(!f1){
                p1.x = LOWORD(lParam);
                p1.y = HIWORD(lParam);
                f1 = true;
            } else if (!f2){
                t1.x = LOWORD(lParam);
                t1.y = HIWORD(lParam);
                f2 = true;
            } else if (!f3){
                p2.x = LOWORD(lParam);
                p2.y = HIWORD(lParam);
                f3 = true;
            } else if (!f4){
                t2.x = LOWORD(lParam);
                t2.y = HIWORD(lParam);
                f4 = true;
            }
            break;

        case WM_PAINT:


            if(f1 && f2 && f3 && f4){


                hdc = BeginPaint(hwnd, &ps);

                MoveToEx(hdc, p1.x, p1.y, NULL);
                LineTo(hdc, t1.x, t1.y);
                MoveToEx(hdc, p2.x, p2.y, NULL);
                LineTo(hdc, t2.x, t2.y);

                Vector4 xCoeff = getCoeff(p1.x, t1.x, p2.x, t2.x);
                Vector4 yCoeff = getCoeff(p1.y, t1.y, p2.y, t2.y);

                int numPoints = 5;
                double dt = 1.0/numPoints;

                MoveToEx(hdc, p1.x, p1.y, NULL);

                for(double t=0; t<=1; t+=dt)
                {

                    Vector4 v;
                    v[3] = 1;

                    for(int i=2; i>=0; --i)
                    {
                        v[i] = v[i+1] * t;
                    }

                    int x = round(xCoeff * v);
                    int y = round(yCoeff * v);
                    LineTo(hdc, x, y);

                }

                f1 = f2 = f3 = f4 = false;
                InvalidateRect(hwnd, NULL, false);
                EndPaint(hwnd, &ps);
            }
            break;

        default:                      /* for messages that we don't deal with */
            return DefWindowProc (hwnd, message, wParam, lParam);
    }

    return 0;
}
