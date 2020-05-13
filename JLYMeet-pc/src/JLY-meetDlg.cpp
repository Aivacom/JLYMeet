#include "stdafx.h"
#include <initguid.h>
#include <Mmdeviceapi.h>
#include <mmsystem.h>
#include "JLY-meet.h"
#include "JLY-meetDlg.h"
#include "afxdialogex.h"
#include "./json/json.h"
#include "base\LoggerExt.h"
#include <string>
#include <Functiondiscoverykeys_devpkey.h>

using namespace base;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define DEF_HTTP_RQ_TYPE_TOKEN   0x01

static const char* g_APPID = "1470236061";

extern CString g_demoVer;

static const char* TAG = "CJLYMeetDlg";

const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);

CJLYMeetDlg::CJLYMeetDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_JLYVIDEODEMO_DIALOG, pParent)
	, m_isLoopback(false)
	, m_canvasInited(false)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CJLYMeetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_ROOM_ID, m_roomIdEdit);
	DDX_Control(pDX, IDC_COMBO1, m_videoDevicesCombo);
	DDX_Control(pDX, IDC_COMBO_AUDIO_INPUT, m_audioInputDevicesCombo);
	DDX_Control(pDX, IDC_COMBO_AUDIO_OUTPUT, m_audioOutputDevicesCombo);
	DDX_Control(pDX, IDC_BUTTON1, m_openAccompanyBtn);
	DDX_Control(pDX, IDC_BUTTON2, m_joinRoomBtn);
	DDX_Control(pDX, IDC_EDIT_UID, m_uidEdit);
}

BEGIN_MESSAGE_MAP(CJLYMeetDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON1, &CJLYMeetDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CJLYMeetDlg::OnBnClickedButton2)
	ON_MESSAGE(WM_JOIN_ROOM_SUCCESS, &CJLYMeetDlg::OnJoinRoomSuccess)
	ON_MESSAGE(WM_LEAVE_ROOM, &CJLYMeetDlg::OnLeaveRoom)
	ON_MESSAGE(WM_USER_STOP_VIDEO, &CJLYMeetDlg::OnUserStopVideo)
	ON_MESSAGE(WM_USER_STOP_AUDIO, &CJLYMeetDlg::OnUserStopAudio)
	ON_STN_DBLCLK(IDC_STATIC_VIDEO1, &CJLYMeetDlg::OnStnClickedVideo1)
	ON_STN_DBLCLK(IDC_STATIC_VIDEO2, &CJLYMeetDlg::OnStnClickedVideo2)
	ON_STN_DBLCLK(IDC_STATIC_VIDEO3, &CJLYMeetDlg::OnStnClickedVideo3)
	ON_STN_DBLCLK(IDC_STATIC_VIDEO4, &CJLYMeetDlg::OnStnClickedVideo4)
	ON_STN_CLICKED(IDC_BUTTON_STOP_VIDEO1, &CJLYMeetDlg::OnStnClickedStopVideo1)
	ON_STN_CLICKED(IDC_BUTTON_STOP_VIDEO2, &CJLYMeetDlg::OnStnClickedStopVideo2)
	ON_STN_CLICKED(IDC_BUTTON_STOP_VIDEO3, &CJLYMeetDlg::OnStnClickedStopVideo3)
	ON_STN_CLICKED(IDC_BUTTON_STOP_VIDEO4, &CJLYMeetDlg::OnStnClickedStopVideo4)
	ON_STN_CLICKED(IDC_BUTTON_STOP_AUDIO1, &CJLYMeetDlg::OnStnClickedStopAudio1)
	ON_STN_CLICKED(IDC_BUTTON_STOP_AUDIO2, &CJLYMeetDlg::OnStnClickedStopAudio2)
	ON_STN_CLICKED(IDC_BUTTON_STOP_AUDIO3, &CJLYMeetDlg::OnStnClickedStopAudio3)
	ON_STN_CLICKED(IDC_BUTTON_STOP_AUDIO4, &CJLYMeetDlg::OnStnClickedStopAudio4)
	ON_EN_CHANGE(IDC_EDIT_ROOM_ID, &CJLYMeetDlg::OnEnChangeEditLocalRoomId)
	ON_EN_CHANGE(IDC_EDIT_UID, &CJLYMeetDlg::OnEnChangeEditLocalUid)
	ON_WM_MOVE()
	ON_WM_CTLCOLOR()
	ON_MESSAGE(WM_HTTP_CMD, OnHttpCmd)
	ON_MESSAGE(WM_TOKEN_WILL_EXPIRE, OnTokenWillExpire)
	ON_MESSAGE(WM_VIDEO_CAPTURE_STATUS, OnVideoCaptureStatus)
	ON_MESSAGE(WM_DEVICE_DETECTED, OnDeviceDetected)
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	ON_CBN_SELCHANGE(IDC_COMBO1, &CJLYMeetDlg::OnCbnSelchangeCapture)
	ON_CBN_SELCHANGE(IDC_COMBO_AUDIO_INPUT, &CJLYMeetDlg::OnCbnSelchangeAudioInput)
	ON_CBN_SELCHANGE(IDC_COMBO_AUDIO_OUTPUT, &CJLYMeetDlg::OnCbnSelchangeAudioOutput)
END_MESSAGE_MAP()

BOOL CJLYMeetDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon of this dialog. When the main application window is not a dialog box, the framework will automatically
	SetIcon(m_hIcon, TRUE);    // set icon
	SetIcon(m_hIcon, FALSE);

	SetWindowText(L"JLY-meet  V" + g_demoVer);

	createEditControl();

	createVideoCanvas();

	MediaManager::create();
	initLogFile();

	int ret = 0;
	ret = MediaManager::instance()->init(g_APPID, 0);
	ret = MediaManager::instance()->getThunderManager()->setArea(AREA_DEFAULT);
	MediaManager::instance()->setMessageDlg(this->GetSafeHwnd());
	
	initVideoDevices();
	initAudioInputDevices();
	initAudioOutputDevices();

	m_MinSize.cx = 438;
	m_MinSize.cy = 554;

	return TRUE;
}

void CJLYMeetDlg::createEditControl() {
	// 1. roomid 随机 1 ~9999
	// 2. uid 随机 100000 - 999999
	// 3. roomid 和 uid 手动输入，首数字不能是0， 长度 8 位字符
	// 1. roomid rondom limit 1 ~9999
	// 2. uid rondom limit 100000 - 999999
	// 3. roomid and uid input self，first is not 0， limit length is 8

	m_uidEdit.SetLimitText(8);
	m_roomIdEdit.SetLimitText(8);

	srand(GetTickCount());
	m_localUid = int((float(rand()) / RAND_MAX * 899999) + 100000);
	m_uidEdit.SetWindowText(std::to_wstring(m_localUid).c_str());

	int roomId = int((float(rand()) / RAND_MAX * 9998) + 1);
	m_roomIdEdit.SetWindowText(std::to_wstring(roomId).c_str());
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

// If you add a minimize button to the dialog box, you need the following code
// to draw the icon. For MFC applications that use document / view models,
// This will be done automatically by the framework.
void CJLYMeetDlg::OnPaint()
{
	if (IsIconic())
	{
		// Device context for drawing
		CPaintDC dc(this); 

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center the icon in the work area rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to get the cursor when the user drags the minimized window
//display.
HCURSOR CJLYMeetDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CJLYMeetDlg::createVideoCanvas()
{
	for (int i = 0; i < _countof(m_videoCanvs); i++) {
		m_videoCanvs[i].m_uidText = new CStatic();
		m_videoCanvs[i].m_uidText->Create(L"", WS_CHILD | WS_VISIBLE | SS_NOTIFY| SS_CENTERIMAGE, CRect(0, 0, 0, 0), this);
		m_videoCanvs[i].m_uidText->SetFont(GetFont());

		m_videoCanvs[i].m_videoCavas = new CStatic();
		m_videoCanvs[i].m_videoCavas->Create(L"", WS_CHILD | WS_VISIBLE | SS_NOTIFY, CRect(0, 0, 0, 0), this, IDC_STATIC_VIDEO1+i);

		m_videoCanvs[i].m_StopAudioBtn = new CButton();
		m_videoCanvs[i].m_StopAudioBtn->Create(L"close audio", WS_CHILD | WS_VISIBLE , CRect(0, 0, 0, 0), this, IDC_BUTTON_STOP_AUDIO1 + i); 
		m_videoCanvs[i].m_StopAudioBtn->SetFont(GetFont());

		m_videoCanvs[i].m_StopVideoBtn = new CButton();
		m_videoCanvs[i].m_StopVideoBtn->Create(L"close video", WS_CHILD | WS_VISIBLE, CRect(0, 0, 0, 0), this, IDC_BUTTON_STOP_VIDEO1 + i);
		m_videoCanvs[i].m_StopVideoBtn->SetFont(GetFont());

		m_videoCanvs[i].m_uid = 0;
		m_videoCanvs[i].m_bigCanvas = false;
	}

	m_canvasInited = true;
}

void CJLYMeetDlg::releaseVideoCanvas()
{
	for (int i = 0; i < _countof(m_videoCanvs); i++) {
		delete m_videoCanvs[i].m_uidText;
		delete m_videoCanvs[i].m_videoCavas;
		delete m_videoCanvs[i].m_StopAudioBtn;
		delete m_videoCanvs[i].m_StopVideoBtn;
	}
}

int CJLYMeetDlg::getHeighByCWnd(CWnd& wnd) {
	CRect rc;
	wnd.GetClientRect(rc);
	return rc.Height();
}

void CJLYMeetDlg::refreshVideoCanvas()
{
	CRect rc;
	GetClientRect(rc);

	int uidEditHeight = getHeighByCWnd(m_uidEdit);
	int audioInputDevicesComboHeight = getHeighByCWnd(m_audioInputDevicesCombo);
	int openAccompanyBtnHeight = getHeighByCWnd(m_openAccompanyBtn);

	int top = uidEditHeight + audioInputDevicesComboHeight + openAccompanyBtnHeight + 75;

	int left = 15, width = (rc.Width() - 30) / 2, height = (rc.Height() - (top + 30)) / 2, btn_w = 90, btn_h = 20;
	bool hasBigCanvas = false;
	for (int i = 0; i < _countof(m_videoCanvs); i++) {
		if (m_videoCanvs[i].m_uid != 0 && m_videoCanvs[i].m_bigCanvas) {
			m_videoCanvs[i].m_videoCavas->MoveWindow(left, top, width*2, height*2);
			m_videoCanvs[i].m_uidText->MoveWindow(left, top - 20, 70, 20);
			m_videoCanvs[i].m_StopAudioBtn->MoveWindow(left + width * 2 - btn_w*2, top - btn_h, btn_w, btn_h);
			m_videoCanvs[i].m_StopVideoBtn->MoveWindow(left + width * 2 - btn_w, top - btn_h, btn_w, btn_h);
			m_videoCanvs[i].m_videoCavas->ShowWindow(SW_SHOW);
			m_videoCanvs[i].m_uidText->ShowWindow(SW_SHOW);
			m_videoCanvs[i].m_StopAudioBtn->ShowWindow(SW_SHOW);
			m_videoCanvs[i].m_StopVideoBtn->ShowWindow(SW_SHOW);
			hasBigCanvas = true;

			if (m_users[m_videoCanvs[i].m_uid]->audio) {
				m_videoCanvs[i].m_StopAudioBtn->EnableWindow(TRUE);
				if (m_users[m_videoCanvs[i].m_uid]->audioStopped) {
					m_videoCanvs[i].m_StopAudioBtn->SetWindowText(L"open audio");
				}
				else {
					m_videoCanvs[i].m_StopAudioBtn->SetWindowText(L"close audio");
				}
			}
			else {
				m_videoCanvs[i].m_StopAudioBtn->SetWindowText(L"close audio");
				m_videoCanvs[i].m_StopVideoBtn->EnableWindow(FALSE);
			}

			if (m_users[m_videoCanvs[i].m_uid]->video) {
				m_videoCanvs[i].m_StopVideoBtn->EnableWindow(TRUE);
				if (m_users[m_videoCanvs[i].m_uid]->videoStopped) {
					m_videoCanvs[i].m_StopVideoBtn->SetWindowText(L"open video");
				}
				else {
					m_videoCanvs[i].m_StopVideoBtn->SetWindowText(L"close video");
				}
			}
			else {
				m_videoCanvs[i].m_StopVideoBtn->SetWindowText(L"close video");
				m_videoCanvs[i].m_StopVideoBtn->EnableWindow(FALSE);
			}
		}
		else {
			m_videoCanvs[i].m_videoCavas->ShowWindow(SW_HIDE);
			m_videoCanvs[i].m_uidText->ShowWindow(SW_HIDE);
			m_videoCanvs[i].m_StopAudioBtn->ShowWindow(SW_HIDE);
			m_videoCanvs[i].m_StopVideoBtn->ShowWindow(SW_HIDE);
		}
	}

	if (hasBigCanvas) {
		return;
	}

	int row = 0, colum = 0, nRow = 2, nColum = 2;
	int top2[] = {top, top + height *2 + btn_h};

	DWORD style[] = { SS_LEFT, SS_RIGHT};

	for (int i = 0; i < _countof(m_videoCanvs); i++) {
		if (m_videoCanvs[i].m_uid != 0) {
			m_videoCanvs[i].m_videoCavas->MoveWindow(left + colum*width, top + row*height, width, height);
			Logd(TAG, Log("refreshVideoCanvas").addDetail("index", std::to_string(i)).addDetail("Canvas Uid", std::to_string(m_videoCanvs[i].m_uid))
				.setMessage("x=%d, y=%d, w=%d, h=%d", left + colum*width, top + row*height, width, height)
			);
			m_videoCanvs[i].m_uidText->MoveWindow(left + colum*width, top2[row]- 20, 70, 20);
			m_videoCanvs[i].m_StopAudioBtn->MoveWindow(left + (colum+1)*width - btn_w*2, top2[row] - btn_h, btn_w, btn_h);
			m_videoCanvs[i].m_StopVideoBtn->MoveWindow(left + (colum + 1)*width - btn_w, top2[row]- btn_h, btn_w, btn_h);
			m_videoCanvs[i].m_videoCavas->ShowWindow(SW_SHOW);
			m_videoCanvs[i].m_uidText->ShowWindow(SW_SHOW);
			m_videoCanvs[i].m_StopAudioBtn->ShowWindow(SW_SHOW);
			m_videoCanvs[i].m_StopVideoBtn->ShowWindow(SW_SHOW);

			if (m_users[m_videoCanvs[i].m_uid]->audio) {
				m_videoCanvs[i].m_StopAudioBtn->EnableWindow(TRUE);
				if (m_users[m_videoCanvs[i].m_uid]->audioStopped) {
					m_videoCanvs[i].m_StopAudioBtn->SetWindowText(L"open audio");
				}
				else {
					m_videoCanvs[i].m_StopAudioBtn->SetWindowText(L"close audio");
				}
			}
			else {
				m_videoCanvs[i].m_StopAudioBtn->SetWindowText(L"close audio");
				m_videoCanvs[i].m_StopAudioBtn->EnableWindow(FALSE);
			}

			if (m_users[m_videoCanvs[i].m_uid]->video) {
				m_videoCanvs[i].m_StopVideoBtn->EnableWindow(TRUE);
				if (m_users[m_videoCanvs[i].m_uid]->videoStopped) {
					m_videoCanvs[i].m_StopVideoBtn->SetWindowText(L"open video");
				}
				else {
					m_videoCanvs[i].m_StopVideoBtn->SetWindowText(L"close video");
				}
			}
			else {
				m_videoCanvs[i].m_StopVideoBtn->SetWindowText(L"close video");
				m_videoCanvs[i].m_StopVideoBtn->EnableWindow(FALSE);
			}

			if (colum == nColum - 1) {
				colum = 0;
				row += 1;
			}
			else {
				colum += 1;
			}
		}
		else {
			m_videoCanvs[i].m_videoCavas->ShowWindow(SW_HIDE);
			m_videoCanvs[i].m_uidText->ShowWindow(SW_HIDE);
			m_videoCanvs[i].m_StopAudioBtn->ShowWindow(SW_HIDE);
			m_videoCanvs[i].m_StopVideoBtn->ShowWindow(SW_HIDE);
		}
	}
}

void CJLYMeetDlg::initVideoDevices()
{
	VideoDeviceList videoDevList;

	if (MediaManager::instance()->getThunderManager()->getVideoDeviceMgr() != NULL)
	{
		MediaManager::instance()->getThunderManager()->getVideoDeviceMgr()->enumVideoDevices(videoDevList);
		for (int i = 0; i < videoDevList.count; ++i)
		{
			Logd(TAG, Log(__FUNCTION__).addDetail("index", std::to_string(i)).addDetail("name", videoDevList.device[i].name));
			wchar_t wname[1024] = { 0 };
			MultiByteToWideChar(CP_UTF8, 0, videoDevList.device[i].name, -1, wname, _countof(wname));
			m_videoDevicesCombo.InsertString(i, wname);
			m_videoDevicesCombo.SetCurSel(0);
		}
	}
}

static std::string GUIDToString(GUID guid) {
	char pSubKey[MAX_PATH] = { 0 };
	sprintf(pSubKey,
		"GUID:{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}",
		guid.Data1,
		guid.Data2,
		guid.Data3,
		guid.Data4[0],
		guid.Data4[1],
		guid.Data4[2],
		guid.Data4[3],
		guid.Data4[4],
		guid.Data4[5],
		guid.Data4[6],
		guid.Data4[7]);

	return std::string(pSubKey);
}

void CJLYMeetDlg::initAudioInputDevices()
{
	AudioDeviceList audioDeviceList;

	if (MediaManager::instance()->getThunderManager()->getAudioDeviceMgr() != NULL)
	{
		MediaManager::instance()->getThunderManager()->getAudioDeviceMgr()->enumInputDevices(audioDeviceList);
		for (int i = 0; i < audioDeviceList.count; ++i)
		{
			Logd(TAG, Log(__FUNCTION__).addDetail("index", std::to_string(i)).addDetail("guid", GUIDToString(audioDeviceList.device[i].id)));
			wchar_t wname[1024] = { 0 };
			MultiByteToWideChar(CP_UTF8, 0, audioDeviceList.device[i].desc, -1, wname, _countof(wname));
			m_audioInputDevicesCombo.InsertString(i, wname);
			m_audioInputDevicesCombo.SetCurSel(0);
			m_oAudioInputDeviceVector.emplace_back(audioDeviceList.device[i].id);
		}
	}
}

void CJLYMeetDlg::initAudioOutputDevices()
{
	AudioDeviceList audioDeviceList;

	if (MediaManager::instance()->getThunderManager()->getAudioDeviceMgr() != NULL)
	{
		MediaManager::instance()->getThunderManager()->getAudioDeviceMgr()->enumOutputDevices(audioDeviceList);
		for (int i = 0; i < audioDeviceList.count; ++i)
		{
			Logd(TAG, Log(__FUNCTION__).addDetail("index", std::to_string(i)).addDetail("guid", GUIDToString(audioDeviceList.device[i].id)));
			wchar_t wname[1024] = { 0 };
			MultiByteToWideChar(CP_UTF8, 0, audioDeviceList.device[i].desc, -1, wname, _countof(wname));
			m_audioOutputDevicesCombo.InsertString(i, wname);
			m_audioOutputDevicesCombo.SetCurSel(0);
			m_oAudioOutputDeviceVector.emplace_back(audioDeviceList.device[i].id);
		}
	}
}

void CJLYMeetDlg::joinRoom()
{
	CString cstrRoom, cstrUid;
	m_roomIdEdit.GetWindowText(cstrRoom);
	m_uidEdit.GetWindowText(cstrUid);

	if (cstrUid.IsEmpty())
	{
		MessageBox(L"uid can not be empty!", L"warning", MB_OK);
		return;
	}

	if (cstrRoom.IsEmpty())
	{
		MessageBox(L"roomid can not be empty!", L"warning", MB_OK);
		return;
	}

	Logd(TAG, Log("joinRoom").addDetail("Uid", std::string(CW2A(cstrUid.GetBuffer(0)))).addDetail("RoomId", std::string(CW2A(cstrRoom.GetBuffer(0)))));

	m_localUid = wcstoull(cstrUid, nullptr, 10);

	m_iJoinRoomStatus = JoinRoomStatus::Joining;
	showControl(FALSE);

	m_roomName = std::string(CW2A(cstrRoom.GetBuffer(0)));
	requestToken(m_localUid, m_roomName);
}

void CJLYMeetDlg::leaveRoom()
{
	Logd(TAG, Log("leaveRoom"));
	m_joinRoomBtn.EnableWindow(FALSE);

	// To exit the room, call the following process
	std::map<__int64, std::shared_ptr<Remote_User>>::iterator iter = m_users.begin();
	for (; iter != m_users.end(); ++iter)
	{
		if (iter->second->audioStopped) {
			if (iter->first == m_localUid) {
				MediaManager::instance()->getThunderManager()->stopLocalAudioStream(true);
			}
			else {
				MediaManager::instance()->getThunderManager()->stopRemoteAudioStream(std::to_string(iter->first).c_str(), true);
			}
		}

		if (iter->second->videoStopped) {
			if (iter->first == m_localUid) {
				MediaManager::instance()->getThunderManager()->stopLocalVideoStream(true);
			}
			else {
				MediaManager::instance()->getThunderManager()->stopRemoteVideoStream(std::to_string(iter->first).c_str(), true);
			}
		}
	}

	MediaManager::instance()->getThunderManager()->stopVideoPreview();
	MediaManager::instance()->getThunderManager()->getVideoDeviceMgr()->stopVideoDeviceCapture();
	MediaManager::instance()->getThunderManager()->leaveRoom();
}

int CJLYMeetDlg::getCanvas(__int64 uid, bool local)
{
	if (local) {
		return 0;
	}
	else {
		for (int i = 1; i < _countof(m_videoCanvs); i++) {
			if (m_videoCanvs[i].m_uid == 0) {
				return i;
			}
		}
	}
	return -1;
}

void CJLYMeetDlg::bindUserToCanvas(int index, __int64 uid)
{
	if (index >= 0 && index < _countof(m_videoCanvs)) {
		m_videoCanvs[index].m_uid = uid;
		m_videoCanvs[index].m_bigCanvas = false;
		m_videoCanvs[index].m_uidText->SetWindowText(std::to_wstring(uid).c_str());
	}
}

void CJLYMeetDlg::bindNewUserToCanvas(int index)
{
	std::map<__int64, std::shared_ptr<Remote_User>>::iterator iter = m_users.begin();
	for (; iter != m_users.end(); ++iter)
	{
		if (iter->second->index < 0) {
			iter->second->index = index;
			bindUserToCanvas(index, iter->first);

			setRemoteVideoCanvas(iter->first, m_videoCanvs[index].m_videoCavas->GetSafeHwnd());
			break;
		}
	}
}

void CJLYMeetDlg::setRemoteVideoCanvas(__int64 uid, HWND hwnd)
{
	VideoCanvas canvas;
	canvas.renderMode = VIDEO_RENDER_MODE_CLIP_TO_BOUNDS;
	sprintf_s(canvas.uid, "%I64d", uid);
	canvas.hWnd = hwnd;
	MediaManager::instance()->getThunderManager()->setRemoteVideoCanvas(canvas);
}

void CJLYMeetDlg::switchBigCanvas(int index)
{
	Logd(TAG, Log("switchBigCanvas").addDetail("index", std::to_string(index)));
	if (index >= 0 && index < _countof(m_videoCanvs)) {
		m_videoCanvs[index].m_bigCanvas = !m_videoCanvs[index].m_bigCanvas;
		refreshVideoCanvas();
	}
}

void CJLYMeetDlg::stopVideoStream(int index)
{
	if (m_users[m_videoCanvs[index].m_uid]->videoStopped) {
		if (index == 0) {
			MediaManager::instance()->getThunderManager()->stopLocalVideoStream(false);
		}
		else {
			MediaManager::instance()->getThunderManager()->stopRemoteVideoStream(std::to_string(m_videoCanvs[index].m_uid).c_str(), false);
		}
	}
	else {
		if (index == 0) {
			MediaManager::instance()->getThunderManager()->stopLocalVideoStream(true);
		}
		else {
			MediaManager::instance()->getThunderManager()->stopRemoteVideoStream(std::to_string(m_videoCanvs[index].m_uid).c_str(), true);
		}
	}
	m_users[m_videoCanvs[index].m_uid]->videoStopped = !m_users[m_videoCanvs[index].m_uid]->videoStopped;

	refreshVideoCanvas();
}

void CJLYMeetDlg::stopAudioStream(int index)
{
	if (m_users[m_videoCanvs[index].m_uid]->audioStopped) {
		if (index == 0) {
			MediaManager::instance()->getThunderManager()->stopLocalAudioStream(false);
		}
		else {
			MediaManager::instance()->getThunderManager()->stopRemoteAudioStream(std::to_string(m_videoCanvs[index].m_uid).c_str(), false);
		}
	}
	else {
		if (index == 0) {
			MediaManager::instance()->getThunderManager()->stopLocalAudioStream(true);
		}
		else {
			MediaManager::instance()->getThunderManager()->stopRemoteAudioStream(std::to_string(m_videoCanvs[index].m_uid).c_str(), true);
		}
	}

	m_users[m_videoCanvs[index].m_uid]->audioStopped = !m_users[m_videoCanvs[index].m_uid]->audioStopped;

	refreshVideoCanvas();
}

void CJLYMeetDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: add message handler code here
	MediaManager::instance()->deInit();

	MediaManager::release();

	releaseVideoCanvas();

	uninitLogFile();

	if (m_pEnumerator != NULL)
		m_pEnumerator->Release();

	m_pEnumerator = nullptr;
}


void CJLYMeetDlg::OnBnClickedButton1()
{
	if (m_isLoopback)
	{
		m_isLoopback = false;
		m_openAccompanyBtn.SetWindowText(L"Turn on the accompaniment");
	}
	else
	{
		m_isLoopback = true;
		m_openAccompanyBtn.SetWindowText(L"Turn off the accompaniment");
	}
	int ret = MediaManager::instance()->getThunderManager()->enableLoopbackRecording(m_isLoopback);
}


void CJLYMeetDlg::OnBnClickedButton2()
{
	if (m_iJoinRoomStatus == JoinRoomStatus::Joining || m_iJoinRoomStatus == JoinRoomStatus::Joined) {
		leaveRoom();
	}
	else {
		joinRoom();
	}
}

void CJLYMeetDlg::OnStnClickedVideo1()
{
	switchBigCanvas(0);
}

void CJLYMeetDlg::OnStnClickedVideo2()
{
	switchBigCanvas(1);
}

void CJLYMeetDlg::OnStnClickedVideo3()
{
	switchBigCanvas(2);
}

void CJLYMeetDlg::OnStnClickedVideo4()
{
	switchBigCanvas(3);
}

void CJLYMeetDlg::OnStnClickedStopVideo1()
{
	stopVideoStream(0);
}

void CJLYMeetDlg::OnStnClickedStopVideo2()
{
	stopVideoStream(1);
}

void CJLYMeetDlg::OnStnClickedStopVideo3()
{
	stopVideoStream(2);
}

void CJLYMeetDlg::OnStnClickedStopVideo4()
{
	stopVideoStream(3);
}

void CJLYMeetDlg::OnStnClickedStopAudio1()
{
	stopAudioStream(0);
}

void CJLYMeetDlg::OnStnClickedStopAudio2()
{
	stopAudioStream(1);
}

void CJLYMeetDlg::OnStnClickedStopAudio3()
{
	stopAudioStream(2);
}

void CJLYMeetDlg::OnStnClickedStopAudio4()
{
	stopAudioStream(3);
}

void CJLYMeetDlg::switchCapture() {
	Logd(TAG, Log(__FUNCTION__));
	if (m_iJoinRoomStatus == JoinRoomStatus::Joined) {
		Logd(TAG, Log(__FUNCTION__).setMessage("User in room").addDetail("camera index", std::to_string(m_videoDevicesCombo.GetCurSel())));
		if (MediaManager::instance()->getThunderManager()->getVideoDeviceMgr() != NULL)
		{
			MediaManager::instance()->getThunderManager()->getVideoDeviceMgr()->startVideoDeviceCapture(m_videoDevicesCombo.GetCurSel());
		}
	}
}

void CJLYMeetDlg::switchAudioInput() {
	Logd(TAG, Log(__FUNCTION__));
	if (m_iJoinRoomStatus == JoinRoomStatus::Joined) {
		Logd(TAG, Log(__FUNCTION__).setMessage("User in room"));
		if (MediaManager::instance()->getThunderManager()->getAudioDeviceMgr() != NULL)
		{
			MediaManager::instance()->getThunderManager()->getAudioDeviceMgr()->setInputtingDevice(m_oAudioInputDeviceVector[m_audioInputDevicesCombo.GetCurSel()]);
		}
	}
}

void CJLYMeetDlg::switchAudioOutput() {
	Logd(TAG, Log(__FUNCTION__));
	if (m_iJoinRoomStatus == JoinRoomStatus::Joined) {
		Logd(TAG, Log(__FUNCTION__).setMessage("User in room"));
		if (MediaManager::instance()->getThunderManager()->getAudioDeviceMgr() != NULL)
		{
			MediaManager::instance()->getThunderManager()->getAudioDeviceMgr()->setOutputtingDevice(m_oAudioOutputDeviceVector[m_audioOutputDevicesCombo.GetCurSel()]);
		}
	}
}

LRESULT CJLYMeetDlg::OnJoinRoomSuccess(WPARAM, LPARAM)
{
	Logd(TAG, Log("OnJoinRoomSuccess"));
	m_iJoinRoomStatus = JoinRoomStatus::Joined;
	m_joinRoomBtn.SetWindowText(L"leave room");
	m_joinRoomBtn.EnableWindow(TRUE);
	m_videoDevicesCombo.EnableWindow(TRUE);

	switchCapture();
	switchAudioInput();
	switchAudioOutput();

	bindUserToCanvas(getCanvas(m_localUid, true), m_localUid);

	std::shared_ptr<Remote_User> user(new Remote_User);
	user->uid = m_localUid;
	user->index = 0;
	user->video = true;
	user->audio = true;
	user->videoStopped = false;
	user->audioStopped = false;
	m_users[m_localUid] = user;

	refreshVideoCanvas();

	VideoCanvas canvas;
	canvas.renderMode = VIDEO_RENDER_MODE_CLIP_TO_BOUNDS;
	sprintf_s(canvas.uid, "%I64d", m_localUid);
	canvas.hWnd = m_videoCanvs[0].m_videoCavas->GetSafeHwnd();
	MediaManager::instance()->getThunderManager()->setLocalVideoCanvas(canvas); // Set the local view

	VideoEncoderConfiguration config;
	config.playType = VIDEO_PUBLISH_PLAYTYPE_SINGLE;
	config.publishMode = VIDEO_PUBLISH_MODE_SMOOTH_DEFINITION;
	MediaManager::instance()->getThunderManager()->setVideoEncoderConfig(config); // Set encoding configuration

	int ret = MediaManager::instance()->getThunderManager()->startVideoPreview(); 

	MediaManager::instance()->getThunderManager()->stopLocalVideoStream(false); // Can send local video
	MediaManager::instance()->getThunderManager()->stopLocalAudioStream(false); // Can send local audio

	return 0;
}

LRESULT CJLYMeetDlg::OnLeaveRoom(WPARAM, LPARAM)
{
	Logd(TAG, Log("OnLeaveRoom"));
	reset();

	for (int i = 0; i < _countof(m_videoCanvs); i++) {
		bindUserToCanvas(i, 0);
	}

	refreshVideoCanvas();

	m_users.clear();

	return 0;
}

LRESULT CJLYMeetDlg::OnUserStopVideo(WPARAM uid, LPARAM isStop)
{
	Logd(TAG, Log("OnUserStopVideo").addDetail("Uid", std::to_string(uid)).addDetail("isStop", std::to_string(isStop)));
	if (isStop == 0) {
		
		std::map<__int64, std::shared_ptr<Remote_User>>::iterator iter = m_users.find(uid);
		if (iter == m_users.end()) {
			std::shared_ptr<Remote_User> user(new Remote_User);
			user->uid = uid;
			user->video = true;
			user->audio = false;
			user->videoStopped = false;
			user->audioStopped = false;
			m_users[uid] = user;
			int index = getCanvas(uid, false);
			user->index = index;

			if (index >= 0) {
				bindUserToCanvas(index, uid);

				refreshVideoCanvas();

				setRemoteVideoCanvas(uid, m_videoCanvs[index].m_videoCavas->GetSafeHwnd());
			}
		}
		else {
			iter->second->video = true;

			refreshVideoCanvas();

			if (iter->second->index > 0) {
				setRemoteVideoCanvas(uid, m_videoCanvs[iter->second->index].m_videoCavas->GetSafeHwnd());
			}
		}
	}
	else {
		std::map<__int64, std::shared_ptr<Remote_User>>::iterator iter = m_users.find(uid);
		if (iter != m_users.end()) {
			iter->second->video = false;
			// If there is no audio, clear the local view
			if (!iter->second->audio) {
				int index = iter->second->index;

				if (iter->second->audioStopped) {
					MediaManager::instance()->getThunderManager()->stopRemoteAudioStream(std::to_string(uid).c_str(), false);
				}

				if (iter->second->videoStopped) {
					MediaManager::instance()->getThunderManager()->stopRemoteVideoStream(std::to_string(uid).c_str(), false);
				}

				m_users.erase(iter);

				// Clear the remote view
				setRemoteVideoCanvas(uid, NULL);

				if (index >= 0) {
					// Empty the binding relationship
					bindUserToCanvas(index, 0);

					// Try to bind a new user
					bindNewUserToCanvas(index);

					// Adjust the view
					refreshVideoCanvas();
				}
			}
		}
	}
	return 0;
}

LRESULT CJLYMeetDlg::OnUserStopAudio(WPARAM uid, LPARAM isStop)
{
	Logd(TAG, Log("OnUserStopAudio").addDetail("Uid", std::to_string(uid)).addDetail("isStop", std::to_string(isStop)));
	if (isStop == 0) {

		std::map<__int64, std::shared_ptr<Remote_User>>::iterator iter = m_users.find(uid);
		if (iter == m_users.end()) {
			std::shared_ptr<Remote_User> user(new Remote_User);
			user->uid = uid;
			user->video = false;
			user->audio = true;
			user->videoStopped = false;
			user->audioStopped = false;
			m_users[uid] = user;
			int index = getCanvas(uid, false);
			user->index = index;

			if (index >= 0) {
				bindUserToCanvas(index, uid);

				refreshVideoCanvas();
			}
		}
		else {
			iter->second->audio = true;

			refreshVideoCanvas();
		}
	}
	else {
		std::map<__int64, std::shared_ptr<Remote_User>>::iterator iter = m_users.find(uid);
		if (iter != m_users.end()) {
			iter->second->audio = false;
			// If there is no video, clear the local view
			if (!iter->second->video) {
				int index = iter->second->index;

				if (iter->second->audioStopped) {
					MediaManager::instance()->getThunderManager()->stopRemoteAudioStream(std::to_string(uid).c_str(), false);
				}

				if (iter->second->videoStopped) {
					MediaManager::instance()->getThunderManager()->stopRemoteVideoStream(std::to_string(uid).c_str(), false);
				}

				m_users.erase(iter);

				// Clear the remote view
				setRemoteVideoCanvas(uid, NULL);

				if (index >= 0) {
					// Empty the binding relationship
					bindUserToCanvas(index, 0);

					// Try to bind a new user
					bindNewUserToCanvas(index);

					// Adjust the view
					refreshVideoCanvas();
				}
			}
		}
	}
	return 0;
}

LRESULT CJLYMeetDlg::OnHttpCmd(WPARAM wParam, LPARAM lParam)
{
	Logd(TAG, Log("OnHttpCmd"));
	POST_CMD* pCmd = (POST_CMD*)wParam;
	if (NULL != pCmd)
	{
		switch (pCmd->nCmdType)
		{
		case DEF_HTTP_RQ_TYPE_TOKEN:
		{
			Logd(TAG, Log("OnHttpCmd").setMessage("DEF_HTTP_RQ_TYPE_TOKEN"));
			if (pCmd->bResult) {
				Json::Reader reader;
				Json::Value root;
				if (reader.parse(pCmd->strRespond, root)) {
					if (root.isMember("code") && root["code"].asInt() == 0) {
						requestTokenSuccess(root["object"].asString());
					}
				}
			}
			else {
				requestTokenFailed();
			}
		}
		break;
		default:
			break;
		}
	}

	return LRESULT();
}

LRESULT CJLYMeetDlg::OnTokenWillExpire(WPARAM wParam, LPARAM lParam)
{
	return LRESULT();
}

LRESULT CJLYMeetDlg::OnVideoCaptureStatus(WPARAM wParam, LPARAM lParam) {
	int status = lParam;
	Logd(TAG, Log(__FUNCTION__).addDetail("status", std::to_string(status)));
	if (status == THUNDER_VIDEO_CAPTURE_STATUS_RESTRICTED || status == THUNDER_VIDEO_CAPTURE_STATUS_DENIED)
			MessageBox(L"Camera is occupied", L"warning", MB_OK);

	return LRESULT();
}

static std::string FromWide(const wchar_t *wide)
{
	static char charData[512];
	std::string strData;
	size_t len = WideCharToMultiByte(CP_UTF8, 0, wide, -1, NULL, 0, NULL, NULL);
	if (len == 0)
		strData = "";
	else
	{
		WideCharToMultiByte(CP_UTF8, 0, wide, -1, charData, len, NULL, NULL);
		strData = std::string(charData, len);
	}
	return strData;
}

static std::wstring s2ws(const std::string& str)
{
	int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
	std::wstring wstrTo(size_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
	return wstrTo;
}

static std::string GetDeviceGetFriendlyName(IMMDevice *dev)
{
	IPropertyStore *props;
	PROPVARIANT v;
	HRESULT hr;

	std::string name;

	hr = dev->OpenPropertyStore(STGM_READ, &props);
	if (FAILED(hr))
		name = "";
	else
	{
		PropVariantInit(&v);
		hr = props->GetValue(PKEY_Device_FriendlyName, &v);
		if (SUCCEEDED(hr))
		{
			name = FromWide(v.pwszVal);
			PropVariantClear(&v);
		}

		props->Release();
	}

	return name;
}

static HRESULT GetGuidFromEndPoint(IMMDevice* pDevice, GUID* pDevGuid)
{
#define EXIT_ON_ERROR(hr)  \
              if (FAILED(hr)) { goto Exit; }

#define SAFE_RELEASE(punk)  \
              if ((punk) != NULL)  \
                { (punk)->Release(); (punk) = NULL; }

	IPropertyStore *pProps = NULL;
	PROPVARIANT var;
	PropVariantInit(&var);

	HRESULT hr = pDevice->OpenPropertyStore(STGM_READ, &pProps);
	EXIT_ON_ERROR(hr)

		hr = pProps->GetValue(PKEY_AudioEndpoint_GUID, &var);
	EXIT_ON_ERROR(hr)

		hr = CLSIDFromString(var.pwszVal, pDevGuid);
	EXIT_ON_ERROR(hr)

		Exit:
	PropVariantClear(&var);
	SAFE_RELEASE(pProps);
	return hr;
}

void CJLYMeetDlg::audioDetected(bool curIsGuidNull, GUID& devGuid, std::vector<GUID>& deviceVector, CComboBox& comBox, bool isInput) {
	GUID guidNull;
	memset(&guidNull, 0, sizeof(GUID));
	IAudioDeviceManager* audioDeviceManager = MediaManager::instance()->getThunderManager()->getAudioDeviceMgr();
	int cur_sel = comBox.GetCurSel();

	// 1. 如果是默认
	// 1.1 删除选择框列表中数据
	// 1. If it is the default
	// 1.1 Delete the data in the selection box list
	if (curIsGuidNull)
	{
		// 需要重新设置下
		// need to reset
		if (isInput)
			audioDeviceManager->setInputtingDevice(guidNull);
		else
			audioDeviceManager->setOutputtingDevice(guidNull);

		int delSel = -1;
		for (int i = 0; i < deviceVector.size(); i++)
		{
			if (IsEqualGUID(devGuid, deviceVector[i]))
			{
				delSel = i;
			}
		}

		if (delSel != -1)
		{
			comBox.DeleteString(delSel);
			deviceVector.erase(deviceVector.begin() + delSel);
		}
	}
	else
	{
		// 2. 如果不是默认
		// 2. If it is not the default
		if (!IsEqualGUID(devGuid, deviceVector[cur_sel]))
		{
			// 2.1 如果不是选择框的选项
			// 2.2 查找
			// 2.1 If it is not an option of the selection box
			// 2.2 Find
			int delSel = -1;
			for (int i = 0; i < deviceVector.size(); i++)
			{
				if (IsEqualGUID(devGuid, deviceVector[i]))
				{
					delSel = i;
				}
			}

			// 2.3 删除选择框数据
			// 2.3 Delete selection box data
			if (delSel != -1)
			{
				comBox.DeleteString(delSel);
				deviceVector.erase(deviceVector.begin() + delSel);
			}
		}
		else
		{
			// 3. 如果是选择框的选项
			// 3.1 删除选择框数据
			// 3.2 设置默认
			// 3. If it is an option of the selection box
			// 3.1 Delete selection box data
			// 3.2 Set default
			// change to NULL
			comBox.DeleteString(cur_sel);
			comBox.SetCurSel(0);
			deviceVector.erase(deviceVector.begin() + cur_sel);
			if (isInput)
				audioDeviceManager->setInputtingDevice(guidNull);
			else
				audioDeviceManager->setOutputtingDevice(guidNull);
		}
	}
}

/*
1. UI显示音频的输入输出需要增加/删减一条数据
这个应该就是界面刷新一下，当前选中的是什么，依然选中显示

2. 这个时候在说话，是切换到热插拔的设备上，还是不切呢？
1）如果正在使用输入输出设备A和A-，插了新设备B和B-，仅刷新列表，输出输入不变
2）如果正在使用输入输出设备A和A-，拔了设备A和A-，就跳去系统默认设备
3）如果选择了系统默认，此时系统连接了蓝牙，根据腾讯会议和微信的效果，系统默认会调用蓝牙的输出，保持电脑的输入
4）如果当前选择了输入输出设备A和A-，连接了蓝牙，输入输出都不变，与1）是一样的

1. UI display audio input and output need to add / delete a piece of data
This should be a refresh of the interface. What is currently selected is still selected and displayed

2. Are you talking at this time, switch to hot-swappable devices, or not?
1) If the input and output devices A and A- are being used, new devices B and B- are inserted, only the list is refreshed, and the output and input are unchanged
2) If you are using input and output devices A and A-, unplug devices A and A-, skip to the system default device
3) If the system default is selected, the system is connected to Bluetooth at this time. According to the effect of Tencent conference and WeChat, the system will call the output of Bluetooth by default to keep the input of the computer
4) If the input and output devices A and A- are currently selected and the Bluetooth is connected, the input and output are unchanged, the same as 1)
*/

LRESULT CJLYMeetDlg::OnDeviceDetected(WPARAM wParam, LPARAM lParam) {
	DeviceDetected* device = (DeviceDetected*)wParam;

	Logd(TAG, Log(__FUNCTION__).addDetail("deviceId", device->deviceId).addDetail("deviceState", std::to_string(device->deviceState))
		.addDetail("deviceType", std::to_string(device->deviceType)));

	IMMDevice *dev;
	std::string deviceIdStr = std::string(device->deviceId);
	wstring deviceIdWstr = s2ws(deviceIdStr);
	IMMDeviceEnumerator* pEnumerator = (IMMDeviceEnumerator*)m_pEnumerator;
	HRESULT hr = pEnumerator->GetDevice(deviceIdWstr.c_str(), &dev);
	GUID devGuid;
	GetGuidFromEndPoint(dev, &devGuid);
	std::string devDesc = GetDeviceGetFriendlyName(dev);

	IAudioDeviceManager* audioDeviceManager = MediaManager::instance()->getThunderManager()->getAudioDeviceMgr();
	
	if (device->deviceType == AUDIO_PLAYOUT_DEVICE) {
		int cur_sel = m_audioOutputDevicesCombo.GetCurSel();
		if (cur_sel == -1)
			goto _End;

		GUID guidNull;
		memset(&guidNull, 0, sizeof(GUID));
		bool curIsGuidNull = IsEqualGUID(m_oAudioOutputDeviceVector[cur_sel], GUID_NULL);
		int new_sel = 0;

		if (device->deviceState == MEDIA_DEVICE_STATE_ACTIVE) {
			// 如果是 音频的输出插入
			// 1. 如果是默认
			// 1.1 如果选择蓝牙，使用蓝牙输出
			// 2. 如果不是默认，不做任何改动
			// 3. 刷新选择框列表
			// If it is audio output insertion
			// 1. If it is the default
			// 1.1 If Bluetooth is selected, use Bluetooth output
			// 2. If it is not the default, do not make any changes
			// 3. Refresh the selection box list
			if (curIsGuidNull) {
				// need to reset
				audioDeviceManager->setOutputtingDevice(guidNull);
			}

			wstring desc = s2ws(devDesc);
			m_oAudioOutputDeviceVector.push_back(devGuid);
			m_audioOutputDevicesCombo.InsertString(m_oAudioOutputDeviceVector.size() - 1, desc.c_str());
		}
		else
		{
			audioDetected(curIsGuidNull, devGuid, m_oAudioOutputDeviceVector, m_audioOutputDevicesCombo, false);
		}
	}
	else if (device->deviceType == AUDIO_RECORDING_DEVICE) {
		int cur_sel = m_audioInputDevicesCombo.GetCurSel();
		if (cur_sel == -1)
			goto _End;

		GUID guidNull;
		memset(&guidNull, 0, sizeof(GUID));
		bool curIsGuidNull = IsEqualGUID(m_oAudioInputDeviceVector[cur_sel], GUID_NULL);
		int new_sel = 0;

		if (device->deviceState == MEDIA_DEVICE_STATE_ACTIVE) {
			// 如果是 音频的输入插入
			// 1. 刷新选择框列表
			// If it is audio input insertion
			// 1. Refresh the selection box list
			if (curIsGuidNull) {
				// 是否是蓝牙
				// 需要重新设置下
				// Whether it is Bluetooth
				// need to reset
				audioDeviceManager->setInputtingDevice(guidNull);
			}

			wstring desc = s2ws(devDesc);
			m_oAudioInputDeviceVector.push_back(devGuid);
			m_audioInputDevicesCombo.InsertString(m_oAudioInputDeviceVector.size() - 1, desc.c_str());
		}
		else
		{
			audioDetected(curIsGuidNull, devGuid, m_oAudioInputDeviceVector, m_audioInputDevicesCombo, true);
		}
	}

_End:
	delete device->deviceId;
	delete device;
	dev->Release();

	return LRESULT();
}

void CJLYMeetDlg::OnOK()
{
}

void CJLYMeetDlg::OnMove(int x, int y)
{
	CDialogEx::OnMove(x, y);

	for (int i = 0; i < _countof(m_videoCanvs); i++) {
		if (m_videoCanvs[i].m_videoCavas) {
			m_videoCanvs[i].m_videoCavas->PostMessage(WM_MOVE);
		}
	}
}

HBRUSH CJLYMeetDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	int id = pWnd->GetDlgCtrlID();
	if (id >= IDC_STATIC_VIDEO1 && id <= IDC_STATIC_VIDEO4) {
		pDC->SetBkColor(RGB(0, 0, 0));
		return (HBRUSH)::GetStockObject(BLACK_BRUSH);
	}
	// TODO: If the default is not the desired brush, return another brush
	return hbr;
}

void CJLYMeetDlg::requestToken(__int64 uid, const std::string& roomNane)
{
	char postData[1024] = { 0 };
	sprintf_s(postData, "{\"appId\":\"%s\", \"uid\" : \"%I64d\", \"channelName\":\"%s\", \"validTime\" : \"10000\"}",
		g_APPID, uid, roomNane.c_str());

	std::string url = "http://webapi.sunclouds.com/webservice/app/v2/auth/genToken?" + std::string(postData);

	Logd(TAG, Log("requestToken").addDetail("Url", url));
	theApp.getPostOffice().Request(CA2W(url.c_str()), DEF_HTTP_RQ_TYPE_TOKEN, 1, GetSafeHwnd());
}

void CJLYMeetDlg::requestTokenSuccess(const std::string& token)
{
	Logd(TAG, Log("requestTokenSuccess").addDetail("Token", token));
	std::string uid = std::to_string(m_localUid);
	if (m_iJoinRoomStatus == JoinRoomStatus::Joined) {
		// update token
		Logd(TAG, Log("requestTokenSuccess").setMessage("updateToken"));
		MediaManager::instance()->getThunderManager()->updateToken(token.c_str(), token.length());
	}
	else {
		Logd(TAG, Log("requestTokenSuccess").setMessage("joinRoom").addDetail("RoomId", m_roomName).addDetail("Uid", uid));

		// join room
		MediaManager::instance()->getThunderManager()->setMediaMode(PROFILE_NORMAL);
		if (MediaManager::instance()->getThunderManager()->joinRoom(token.c_str(), token.length(), m_roomName.c_str(), uid.c_str()) != 0)
		{
			Logw(TAG, Log("requestTokenSuccess").setMessage("joinRoom failed!!!"));
			reset();
			MessageBox(L"join room failed", L"warning", MB_OK);
		}
	}
}

void CJLYMeetDlg::requestTokenFailed()
{
	Logd(TAG, Log("requestTokenFailed"));
	reset();
	MessageBox(L"get token failed", L"warning", MB_OK);
}

void CJLYMeetDlg::reset() {
	m_iJoinRoomStatus = JoinRoomStatus::Leave;
	m_joinRoomBtn.SetWindowText(L"join room");
	showControl(TRUE);
}

void CJLYMeetDlg::showControl(BOOL show) {
	m_joinRoomBtn.EnableWindow(show);
	m_roomIdEdit.EnableWindow(show);
	m_uidEdit.EnableWindow(show);
	m_videoDevicesCombo.EnableWindow(show);
}

void CJLYMeetDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: add message handler code here
	if (nType == SIZE_MAXIMIZED || nType == SIZE_RESTORED) {
		if (m_canvasInited) {
			refreshVideoCanvas();
		}
	}
}

void CJLYMeetDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	// TODO: add message handler code and / or call default value here
	if (m_MinSize.cx > 0 && m_MinSize.cy > 0)
	{
		lpMMI->ptMinTrackSize.x = m_MinSize.cx;   // x
		lpMMI->ptMinTrackSize.y = m_MinSize.cy;   // y  
	}

	RECT rcMax;
	SystemParametersInfo(SPI_GETWORKAREA, NULL, (PVOID)&rcMax, 0);
	lpMMI->ptMaxPosition.x = rcMax.left;
	lpMMI->ptMaxPosition.y = rcMax.top;
	lpMMI->ptMaxSize.x = rcMax.right - rcMax.left;
	lpMMI->ptMaxSize.y = rcMax.bottom - rcMax.top;
	CDialogEx::OnGetMinMaxInfo(lpMMI);
}

void CJLYMeetDlg::OnEnChangeEditLocalUid() {
	checkEditUid(m_uidEdit, L"The first digit of uid cannot be 0!");
}

void CJLYMeetDlg::checkEditUid(CEdit& edit, LPCWSTR messgaeBoxStr) {
	if (edit.GetWindowTextLength() != 0) {
		CString uid;
		edit.GetWindowText(uid);
		int t = int(wcstoull(uid, nullptr, 10));
		if (t == 0) {
			MessageBox(messgaeBoxStr, L"warning", MB_OK);
			edit.SetWindowText(L"");
			return;
		}
	}
}

void CJLYMeetDlg::OnEnChangeEditLocalRoomId() {
	checkEditUid(m_roomIdEdit, L"The first digit of roomid cannot be 0!");
}


void CJLYMeetDlg::OnCbnSelchangeCapture() {
	switchCapture();
}

void CJLYMeetDlg::OnCbnSelchangeAudioInput() {
	switchAudioInput();
}

void CJLYMeetDlg::OnCbnSelchangeAudioOutput() {
	switchAudioOutput();
}

static void createLogDir(CString& path) {
	// 1. Determine whether the log directory has been created, if not, do not create it
	// 2. Create a directory
	std::string strStr = CW2A(path.GetString());
	if (_waccess(path, 0) == -1) {
		if (::CreateDirectoryW(path, NULL)) {
			Logd(TAG, Log("createLogDir").setMessage("Dir path[%s] create OK!!!!", strStr.c_str()));
		}
		else {
			Logd(TAG, Log("createLogDir").setMessage("Dir path[%s] create Failed!!!!", strStr.c_str()));
		}
	}
	else {
		Logd(TAG, Log("createLogDir").setMessage("Dir path[%s] is exist!!!!", strStr.c_str()));
	}
}

void CJLYMeetDlg::initLogFile() {
	// 1. Get the current directory address
	CString localPath = theApp.GetModuleDir();
	if (localPath.IsEmpty()) {
		Logw(TAG, Log("initLogFile").setMessage("Get localPath is empty!!!"));
		return;
	}

	// 2. Determine whether the log directory has been created, if not, do not create it
	CString logPath = localPath + L"\\log";
	CString meetPath = logPath + L"\\meet";
	CString sdkPath = logPath + L"\\sdk";
	createLogDir(logPath);
	createLogDir(meetPath);
	createLogDir(sdkPath);
	
	MediaManager::instance()->getThunderManager()->setLogFilePath(std::string(CW2A(sdkPath.GetString())).c_str());

#ifdef DEBUG
	MediaManager::instance()->getThunderManager()->setLogLevel(LOG_LEVEL_TRACE);
#else
	// 3. Create a log file
	CreateLogFile(std::string(CW2A(meetPath.GetString())), "JLY-Meet");

	// 4. set sdk log level
	MediaManager::instance()->getThunderManager()->setLogLevel(LOG_LEVEL_WARN);

	// 5. if set sdkPath += L"\\1.txt", set sdk log level is LOG_LEVEL_TRACE, means all log
	sdkPath += L"\\1.txt";
	if (_waccess(sdkPath, 0) != -1) {
		MediaManager::instance()->getThunderManager()->setLogLevel(LOG_LEVEL_TRACE);
	}
#endif // DEBUG
}

void CJLYMeetDlg::uninitLogFile() {
	DestoryLogFile();
}