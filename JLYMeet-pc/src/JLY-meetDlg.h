#pragma once
#include "afxwin.h"
#include "MediaManager.h"
#include <map>
#include<memory>
#include <mmdeviceapi.h>

struct Video_Canvas 
{
	CStatic* m_uidText;
	CStatic* m_videoCavas;
	__int64 m_uid;
	bool m_bigCanvas;
	CButton* m_StopVideoBtn;
	CButton* m_StopAudioBtn;
};

struct Remote_User
{
	__int64 uid;
	int index;
	bool video;
	bool audio;
	bool audioStopped;
	bool videoStopped;
};

class CJLYMeetDlg : public CDialogEx
{
public:
	CJLYMeetDlg(CWnd* pParent = NULL);

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_JLYVIDEODEMO_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);

protected:
	HICON m_hIcon;

	// The generated message mapping function
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnStnClickedVideo1();
	afx_msg void OnStnClickedVideo2();
	afx_msg void OnStnClickedVideo3();
	afx_msg void OnStnClickedVideo4();
	afx_msg void OnStnClickedStopVideo1();
	afx_msg void OnStnClickedStopVideo2();
	afx_msg void OnStnClickedStopVideo3();
	afx_msg void OnStnClickedStopVideo4();
	afx_msg void OnStnClickedStopAudio1();
	afx_msg void OnStnClickedStopAudio2();
	afx_msg void OnStnClickedStopAudio3();
	afx_msg void OnStnClickedStopAudio4();
	afx_msg void OnEnChangeEditLocalUid();
	afx_msg void OnEnChangeEditLocalRoomId();
	afx_msg void OnCbnSelchangeCapture();
	afx_msg void OnCbnSelchangeAudioInput();
	afx_msg void OnCbnSelchangeAudioOutput();
	afx_msg LRESULT OnJoinRoomSuccess(WPARAM, LPARAM);
	afx_msg LRESULT OnLeaveRoom(WPARAM, LPARAM);
	afx_msg LRESULT OnUserStopVideo(WPARAM, LPARAM);
	afx_msg LRESULT OnUserStopAudio(WPARAM, LPARAM);
	afx_msg LRESULT OnHttpCmd(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnTokenWillExpire(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnVideoCaptureStatus(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDeviceDetected(WPARAM wParam, LPARAM lParam);
	afx_msg void OnMove(int x, int y);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);

	DECLARE_MESSAGE_MAP()

private:
	void createVideoCanvas();

	void releaseVideoCanvas();

	void refreshVideoCanvas();

	int getHeighByCWnd(CWnd& wnd);

	void initVideoDevices();

	void initAudioInputDevices();

	void initAudioOutputDevices();

	void joinRoom();

	void leaveRoom();

	int getCanvas(__int64 uid, bool local);

	void bindUserToCanvas(int index, __int64 uid);

	void bindNewUserToCanvas(int index);

	void setRemoteVideoCanvas(__int64 uid, HWND hwnd);
	
	void switchBigCanvas(int index);

	void stopVideoStream(int index);

	void stopAudioStream(int index);

	void requestToken(__int64 uid, const std::string& roomNane);

	void requestTokenSuccess(const std::string& token);

	void requestTokenFailed();

	void createEditControl();

	void checkEditUid(CEdit& edit, LPCWSTR messgaeBoxStr);

	void showControl(BOOL show);
	
	void reset();

	void switchCapture();

	void switchAudioInput();

	void switchAudioOutput();

	void initLogFile();

	void uninitLogFile();

	void audioDetected(bool curIsGuidNull, GUID& devGuid, std::vector<GUID>& deviceVector, CComboBox& comBox, bool isInput) ;

private:
	Video_Canvas m_videoCanvs[4];
	bool m_canvasInited;
	
	__int64 m_localUid;
	std::string m_roomName;

	enum JoinRoomStatus {
		Leave = 0,
		Joining,
		Joined
	};

	JoinRoomStatus m_iJoinRoomStatus = JoinRoomStatus::Leave;
	bool m_isLoopback;

	SIZE m_MinSize;
	CEdit m_roomIdEdit;
	CEdit m_uidEdit;
	CButton m_openAccompanyBtn;
	CButton m_joinRoomBtn;
	CComboBox m_videoDevicesCombo;
	CComboBox m_audioInputDevicesCombo;
	CComboBox m_audioOutputDevicesCombo;
	std::vector<GUID> m_oAudioInputDeviceVector;
	std::vector<GUID> m_oAudioOutputDeviceVector;
	std::map<__int64, std::shared_ptr<Remote_User>> m_users;
	IMMDeviceEnumerator* m_pEnumerator;
};
