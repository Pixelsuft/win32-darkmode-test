#pragma once
#include <Windows.h>

typedef struct {
	ULONG dwOSVersionInfoSize;
	ULONG dwMajorVersion;
	ULONG dwMinorVersion;
	ULONG dwBuildNumber;
	ULONG dwPlatformId;
	WCHAR szCSDVersion[128];
	USHORT wServicePackMajor;
	USHORT wServicePackMinor;
	USHORT wSuiteMask;
	UCHAR wProductType;
	UCHAR wReserved;
} WIN_NTDLL_OSVERSIONINFOEXW;

typedef enum {
	WIN_APPMODE_DEFAULT,
	WIN_APPMODE_ALLOW_DARK,
	WIN_APPMODE_FORCE_DARK,
	WIN_APPMODE_FORCE_LIGHT,
	WIN_APPMODE_MAX
} WinPreferredAppMode;

enum WINDOWCOMPOSITIONATTRIB {
	WCA_UNDEFINED = 0,
	WCA_NCRENDERING_ENABLED = 1,
	WCA_NCRENDERING_POLICY = 2,
	WCA_TRANSITIONS_FORCEDISABLED = 3,
	WCA_ALLOW_NCPAINT = 4,
	WCA_CAPTION_BUTTON_BOUNDS = 5,
	WCA_NONCLIENT_RTL_LAYOUT = 6,
	WCA_FORCE_ICONIC_REPRESENTATION = 7,
	WCA_EXTENDED_FRAME_BOUNDS = 8,
	WCA_HAS_ICONIC_BITMAP = 9,
	WCA_THEME_ATTRIBUTES = 10,
	WCA_NCRENDERING_EXILED = 11,
	WCA_NCADORNMENTINFO = 12,
	WCA_EXCLUDED_FROM_LIVEPREVIEW = 13,
	WCA_VIDEO_OVERLAY_ACTIVE = 14,
	WCA_FORCE_ACTIVEWINDOW_APPEARANCE = 15,
	WCA_DISALLOW_PEEK = 16,
	WCA_CLOAK = 17,
	WCA_CLOAKED = 18,
	WCA_ACCENT_POLICY = 19,
	WCA_FREEZE_REPRESENTATION = 20,
	WCA_EVER_UNCLOAKED = 21,
	WCA_VISUAL_OWNER = 22,
	WCA_HOLOGRAPHIC = 23,
	WCA_EXCLUDED_FROM_DDA = 24,
	WCA_PASSIVEUPDATEMODE = 25,
	WCA_USEDARKMODECOLORS = 26,
	WCA_LAST = 27
};

typedef struct {
	WINDOWCOMPOSITIONATTRIB Attrib;
	PVOID pvData;
	SIZE_T cbData;
} WINDOWCOMPOSITIONATTRIBDATA;

typedef bool (WINAPI* ShouldAppsUseDarkMode_t)(void);
typedef void (WINAPI* AllowDarkModeForWindow_t)(HWND, bool);
typedef void (WINAPI* AllowDarkModeForApp_t)(bool);
typedef void (WINAPI* FlushMenuThemes_t)(void);
typedef void (WINAPI* RefreshImmersiveColorPolicyState_t)(void);
typedef bool (WINAPI* IsDarkModeAllowedForWindow_t)(HWND);
typedef bool (WINAPI* ShouldSystemUseDarkMode_t)(void);
typedef WinPreferredAppMode(WINAPI* SetPreferredAppMode_t)(WinPreferredAppMode);
typedef bool (WINAPI* IsDarkModeAllowedForApp_t)(void);

typedef BOOL(WINAPI* SetWindowCompositionAttribute_t)(HWND, const WINDOWCOMPOSITIONATTRIBDATA*);

typedef struct {
	HMODULE uxtheme_handle;
	HMODULE user32_handle;
	ShouldAppsUseDarkMode_t ShouldAppsUseDarkMode;
	AllowDarkModeForWindow_t AllowDarkModeForWindow;
	AllowDarkModeForApp_t AllowDarkModeForApp;
	FlushMenuThemes_t FlushMenuThemes;
	RefreshImmersiveColorPolicyState_t RefreshImmersiveColorPolicyState;
	IsDarkModeAllowedForWindow_t IsDarkModeAllowedForWindow;
	ShouldSystemUseDarkMode_t ShouldSystemUseDarkMode;
	SetPreferredAppMode_t SetPreferredAppMode;
	IsDarkModeAllowedForApp_t IsDarkModeAllowedForApp;
	SetWindowCompositionAttribute_t SetWindowCompositionAttribute;
} win_shit_type;

extern win_shit_type win_shit;

bool fix_win32_theme(HWND hwnd);
void fix_cleanup();

bool UAHWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam, LRESULT* lr);

// window messages related to menu bar drawing
#define WM_UAHDESTROYWINDOW    0x0090	// handled by DefWindowProc
#define WM_UAHDRAWMENU         0x0091	// lParam is UAHMENU
#define WM_UAHDRAWMENUITEM     0x0092	// lParam is UAHDRAWMENUITEM
#define WM_UAHINITMENU         0x0093	// handled by DefWindowProc
#define WM_UAHMEASUREMENUITEM  0x0094	// lParam is UAHMEASUREMENUITEM
#define WM_UAHNCPAINTMENUPOPUP 0x0095	// handled by DefWindowProc

typedef union tagUAHMENUITEMMETRICS
{
	struct {
		DWORD cx;
		DWORD cy;
	} rgsizeBar[2];
	struct {
		DWORD cx;
		DWORD cy;
	} rgsizePopup[4];
} UAHMENUITEMMETRICS;

typedef struct tagUAHMENUPOPUPMETRICS
{
	DWORD rgcx[4];
	DWORD fUpdateMaxWidths : 2;
} UAHMENUPOPUPMETRICS;

typedef struct tagUAHMENU
{
	HMENU hmenu;
	HDC hdc;
	DWORD dwFlags;
} UAHMENU;

typedef struct tagUAHMENUITEM
{
	int iPosition;
	UAHMENUITEMMETRICS umim;
	UAHMENUPOPUPMETRICS umpm;
} UAHMENUITEM;

typedef struct UAHDRAWMENUITEM
{
	DRAWITEMSTRUCT dis;
	UAHMENU um;
	UAHMENUITEM umi;
} UAHDRAWMENUITEM;

typedef struct tagUAHMEASUREMENUITEM
{
	MEASUREITEMSTRUCT mis;
	UAHMENU um;
	UAHMENUITEM umi;
} UAHMEASUREMENUITEM;
