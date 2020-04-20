#ifndef NATIVEEVENTFILTER_H
#define NATIVEEVENTFILTER_H
#include <QAbstractNativeEventFilter>
#include <windows.h>
#include <windowsx.h>
#include <objidl.h> // Fixes error C2504: 'IUnknown' : base class undefined
//#include <gdiplus.h>
//#include <GdiPlusColor.h>

#include <QQuickWindow>

#include <dwmapi.h>
#include <winuser.h>

class NativeEventFilter: public QAbstractNativeEventFilter
{
public:



    HWND winId;
    int desktop_width;
    int desktop_height;
    double scale_rate = 0.0;
    QQuickWindow *main_window;
    quint64 count = 0;


    const MARGINS shadow = { 1, 1, 1, 1 };
    bool m_aeroEnabled;

    virtual bool nativeEventFilter(const QByteArray &eventType, void *message, long *result) Q_DECL_OVERRIDE
    {
        MSG* msg = (MSG *)message;
        switch (msg->message)
        {
        case WM_NCCALCSIZE:
        {
            //this kills the window frame and title bar we added with WS_THICKFRAME and WS_CAPTION
            *result = 0;
            return true;
        }
        case WM_NCPAINT:
        {
            if (m_aeroEnabled)
            {
                int v = 2;
                DwmSetWindowAttribute(msg->hwnd, 2,&v, 4);
                DwmExtendFrameIntoClientArea(msg->hwnd,&shadow);
            }

            return false;

        }
        case WM_NCHITTEST:
        {

            QObject *title=main_window->findChild<QObject*>("title");


            if(title->property("opened").toBool())
            {
                *result = HTCLIENT;
                return true;
            }


            RECT winrect;
            GetWindowRect(msg->hwnd, &winrect);

            long x = GET_X_LPARAM(msg->lParam);
            long y = GET_Y_LPARAM(msg->lParam);

            int width = winrect.right - winrect.left;
            int height = winrect.bottom - winrect.top;



            int padding=6;


            //left border
            if (x >= winrect.left && x < winrect.left + padding)
            {
                *result = HTLEFT;
            }
            //right border
            if (x < winrect.right && x >= winrect.right - padding)
            {
                *result = HTRIGHT;
            }

            //bottom border
            if (y < winrect.bottom && y >= winrect.bottom - padding)
            {
                *result = HTBOTTOM;
            }
            //top border
            if (y >= winrect.top && y < winrect.top + padding)
            {
                *result = HTTOP;
            }


            // if(msg->hwnd != winId) return false; //Dialogs don't need a HTCAPTION


            //bottom left corner
            if (x >= winrect.left && x < winrect.left + padding &&
                    y < winrect.bottom && y >= winrect.bottom - padding)
            {
                *result = HTBOTTOMLEFT;
            }
            //bottom right corner
            if (x < winrect.right && x >= winrect.right - padding &&
                    y < winrect.bottom && y >= winrect.bottom - padding)
            {
                *result = HTBOTTOMRIGHT;
            }
            //top left corner
            if (x >= winrect.left && x < winrect.left + padding &&
                    y >= winrect.top && y < winrect.top + padding)
            {
                *result = HTTOPLEFT;
            }
            //top right corner
            if (x < winrect.right && x >= winrect.right - padding&&
                    y >= winrect.top && y < winrect.top + padding)
            {
                *result = HTTOPRIGHT;
            }


            // Here is for coin_type_btn
//            if((x - winrect.left >= (width - 100*scale_rate - padding)
//                && x - winrect.left <= (width - padding - 28*scale_rate)
//                && y >= (winrect.top + padding + 39*scale_rate)
//                && y <= (winrect.top + 64*scale_rate + padding) ) && msg->hwnd == winId)
//            {
//                *result = HTCLIENT;
//                return true;
//            }

            // Here is for NaviPanel
//            if((x - winrect.left > (width/2 - 333*scale_rate)
//                && x - winrect.left < (width/2 + 333*scale_rate)
//                && y >= (winrect.top + padding)
//                && y < (winrect.top + 74*scale_rate + padding)) && msg->hwnd == winId)
//            {
//                *result = HTCLIENT;
//                return true;
//            }

            // Here is for Menu
            if((x  > (winrect.left + 224*scale_rate + padding)
                && x <  (winrect.left + 537*scale_rate + padding)
                && y >= (winrect.top + padding + 20*scale_rate)
                && y < (winrect.top+ padding + 50*scale_rate)) && msg->hwnd == winId)
            {
                *result = HTCLIENT;
                return true;
            }

            // Here is for close min_mum buttons
            if((x > ( winrect.right - padding - 115*scale_rate)
                && x < (winrect.right - padding - 5*scale_rate)
                &&  y >= ( winrect.top + padding + 17*scale_rate)
                && y < ( winrect.top + 52*scale_rate + padding )) && msg->hwnd == winId)
            {
                *result = HTCLIENT;
                return true;
            }

            if ((y >= (  winrect.top + padding)
                 && y <(  winrect.top + 70*scale_rate + padding)
                 && x >(  winrect.left + padding)
                 && x <(  winrect.right - padding)) && msg->hwnd == winId
                    //&& !(x > winrect.right - padding-105 && x < winrect.right - padding &&  y >= winrect.top + padding && y < winrect.top + 28 + padding )
                    )
            {
                *result = HTCAPTION;
            }


            if (0!=*result) return true;



            *result = HTCLIENT;
            return true;

        }

        default:
            return false;
        }
    }
};

#endif // NATIVEEVENTFILTER_H
