#include <shit.hpp>
#include <Uxtheme.h>
#include <vsstyle.h>

#define LOAD_FUNC_ORD(func_name, func_ord) do { \
	win_shit.func_name = reinterpret_cast<func_name##_t>(GetProcAddress(win_shit.uxtheme_handle, MAKEINTRESOURCEA(func_ord))); \
	if (win_shit.func_name == NULL) { \
		FreeLibrary(win_shit.uxtheme_handle); \
		return true; \
	} \
} while (0)

win_shit_type win_shit;

bool fix_win32_theme(HWND hwnd) {
	win_shit.uxtheme_handle = NULL;
	win_shit.user32_handle = NULL;
	win_shit.uxtheme_handle = LoadLibraryExW(L"uxtheme.dll", NULL, LOAD_IGNORE_CODE_AUTHZ_LEVEL | LOAD_LIBRARY_SEARCH_SYSTEM32);
	win_shit.user32_handle = LoadLibraryExW(L"user32.dll", NULL, LOAD_IGNORE_CODE_AUTHZ_LEVEL | LOAD_LIBRARY_SEARCH_SYSTEM32);
	win_shit.SetWindowCompositionAttribute = (SetWindowCompositionAttribute_t)GetProcAddress(win_shit.user32_handle, "SetWindowCompositionAttribute");
	LOAD_FUNC_ORD(ShouldAppsUseDarkMode, 132);
	LOAD_FUNC_ORD(AllowDarkModeForWindow, 133);
	win_shit.AllowDarkModeForApp = NULL;
	LOAD_FUNC_ORD(SetPreferredAppMode, 135);
	LOAD_FUNC_ORD(FlushMenuThemes, 136);
	LOAD_FUNC_ORD(RefreshImmersiveColorPolicyState, 104);
	LOAD_FUNC_ORD(IsDarkModeAllowedForWindow, 137);
	LOAD_FUNC_ORD(ShouldSystemUseDarkMode, 138);
	LOAD_FUNC_ORD(IsDarkModeAllowedForApp, 139);
	// Let's begin our magic
	if (win_shit.AllowDarkModeForApp != NULL)
		win_shit.AllowDarkModeForApp(true);
	if (win_shit.SetPreferredAppMode != NULL)
		win_shit.SetPreferredAppMode(WIN_APPMODE_ALLOW_DARK);
	win_shit.RefreshImmersiveColorPolicyState();
	win_shit.AllowDarkModeForWindow(hwnd, true);
	bool enable_dark = win_shit.ShouldAppsUseDarkMode();
	BOOL win_dark = enable_dark ? TRUE : FALSE;
	WINDOWCOMPOSITIONATTRIBDATA data = { WCA_USEDARKMODECOLORS, &win_dark, sizeof(win_dark) };
	win_shit.SetWindowCompositionAttribute(hwnd, &data);
	return false;
}

void fix_cleanup() {
	FreeLibrary(win_shit.user32_handle);
	FreeLibrary(win_shit.uxtheme_handle);
}

static HTHEME g_menuTheme = nullptr;

// ugly colors for illustration purposes
static HBRUSH g_brBarBackground = CreateSolidBrush(RGB(43, 43, 43));

void UAHDrawMenuNCBottomLine(HWND hWnd)
{
    MENUBARINFO mbi = { sizeof(mbi) };
    if (!GetMenuBarInfo(hWnd, OBJID_MENU, 0, &mbi))
    {
        return;
    }

    RECT rcClient = { 0 };
    GetClientRect(hWnd, &rcClient);
    MapWindowPoints(hWnd, nullptr, (POINT*)&rcClient, 2);

    RECT rcWindow = { 0 };
    GetWindowRect(hWnd, &rcWindow);

    OffsetRect(&rcClient, -rcWindow.left, -rcWindow.top);

    // the rcBar is offset by the window rect
    RECT rcAnnoyingLine = rcClient;
    rcAnnoyingLine.bottom = rcAnnoyingLine.top;
    rcAnnoyingLine.top--;


    HDC hdc = GetWindowDC(hWnd);
    FillRect(hdc, &rcAnnoyingLine, g_brBarBackground);
    ReleaseDC(hWnd, hdc);
}

bool UAHWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT* lr)
{
    switch (message)
    {
    case WM_UAHDRAWMENU:
    {
        UAHMENU* pUDM = (UAHMENU*)lParam;
        RECT rc = { 0 };

        // get the menubar rect
        {
            MENUBARINFO mbi = { sizeof(mbi) };
            GetMenuBarInfo(hWnd, OBJID_MENU, 0, &mbi);

            RECT rcWindow;
            GetWindowRect(hWnd, &rcWindow);

            // the rcBar is offset by the window rect
            rc = mbi.rcBar;
            OffsetRect(&rc, -rcWindow.left, -rcWindow.top);
        }

        FillRect(pUDM->hdc, &rc, g_brBarBackground);

        return true;
    }
    case WM_UAHDRAWMENUITEM:
    {
        UAHDRAWMENUITEM* pUDMI = (UAHDRAWMENUITEM*)lParam;

        // ugly colors for illustration purposes
        static HBRUSH g_brItemBackground = CreateSolidBrush(RGB(43, 43, 43));
        static HBRUSH g_brItemBackgroundHot = CreateSolidBrush(RGB(53, 53, 53));
        static HBRUSH g_brItemBackgroundSelected = CreateSolidBrush(RGB(53, 53, 53));

        HBRUSH* pbrBackground = &g_brItemBackground;

        // get the menu item string
        wchar_t menuString[256] = { 0 };
        MENUITEMINFOW mii = { sizeof(mii), MIIM_STRING };
        {
            mii.dwTypeData = menuString;
            mii.cch = (sizeof(menuString) / 2) - 1;

            GetMenuItemInfoW(pUDMI->um.hmenu, pUDMI->umi.iPosition, TRUE, &mii);
        }

        // get the item state for drawing

        DWORD dwFlags = DT_CENTER | DT_SINGLELINE | DT_VCENTER;

        int iTextStateID = 0;
        int iBackgroundStateID = 0;
        {
            if ((pUDMI->dis.itemState & ODS_INACTIVE) | (pUDMI->dis.itemState & ODS_DEFAULT)) {
                // normal display
                iTextStateID = MPI_NORMAL;
                iBackgroundStateID = MPI_NORMAL;
            }
            if (pUDMI->dis.itemState & ODS_HOTLIGHT) {
                // hot tracking
                iTextStateID = MPI_HOT;
                iBackgroundStateID = MPI_HOT;

                pbrBackground = &g_brItemBackgroundHot;
            }
            if (pUDMI->dis.itemState & ODS_SELECTED) {
                // clicked -- MENU_POPUPITEM has no state for this, though MENU_BARITEM does
                iTextStateID = MPI_HOT;
                iBackgroundStateID = MPI_HOT;

                pbrBackground = &g_brItemBackgroundSelected;
            }
            if ((pUDMI->dis.itemState & ODS_GRAYED) || (pUDMI->dis.itemState & ODS_DISABLED)) {
                // disabled / grey text
                iTextStateID = MPI_DISABLED;
                iBackgroundStateID = MPI_DISABLED;
            }
            if (pUDMI->dis.itemState & ODS_NOACCEL) {
                dwFlags |= DT_HIDEPREFIX;
            }
        }

        if (!g_menuTheme) {
            g_menuTheme = OpenThemeData(hWnd, L"Menu");
        }
        
        FillRect(pUDMI->um.hdc, &pUDMI->dis.rcItem, *pbrBackground);

#ifdef _MSC_VER
        DTTOPTS opts = {
            sizeof(opts), DTT_TEXTCOLOR,
            (GetForegroundWindow() == hWnd) ? (iTextStateID != MPI_DISABLED ? RGB(255, 255, 255) : RGB(0x40, 0x40, 0x40)) : RGB(128, 128, 128)
        };
        DrawThemeTextEx(g_menuTheme, pUDMI->um.hdc, MENU_BARITEM, MBI_NORMAL, menuString, mii.cch, dwFlags, &pUDMI->dis.rcItem, &opts);
#endif

        return true;
    }
    case WM_UAHMEASUREMENUITEM:
    {
        UAHMEASUREMENUITEM* pMmi = (UAHMEASUREMENUITEM*)lParam;

        // allow the default window procedure to handle the message
        // since we don't really care about changing the width
        *lr = DefWindowProcW(hWnd, message, wParam, lParam);

        // but we can modify it here to make it 1/3rd wider for example
        // pMmi->mis.itemWidth = (pMmi->mis.itemWidth * 4) / 3;

        return true;
    }
    case WM_THEMECHANGED:
    {
        if (g_menuTheme) {
            CloseThemeData(g_menuTheme);
            g_menuTheme = nullptr;
        }
        // continue processing in main wndproc
        return false;
    }
    case WM_NCPAINT:
    case WM_NCACTIVATE:
        *lr = DefWindowProcW(hWnd, message, wParam, lParam);
        UAHDrawMenuNCBottomLine(hWnd);
        return true;
        break;
    default:
        return false;
    }
}
