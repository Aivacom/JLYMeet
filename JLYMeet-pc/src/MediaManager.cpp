#include "stdafx.h"
#include "MediaManager.h"
#include "base\LoggerExt.h"

using namespace base;

static const char* TAG = "MediaManager";

MediaManager* MediaManager::m_pInstance = NULL;


void MediaManager::create()
{
	if (m_pInstance == NULL)
	{
		m_pInstance = new MediaManager();
	}
}

void MediaManager::release()
{
	if (m_pInstance != NULL)
	{
		delete m_pInstance;
		m_pInstance = NULL;
	}
}

MediaManager* MediaManager::instance()
{
	return m_pInstance;
}

MediaManager::MediaManager()
{
	m_thunderManager = new ThunderManager();
}

MediaManager::~MediaManager()
{
	delete m_thunderManager;
	m_thunderManager = NULL;
}

int MediaManager::init(const char* appId, int sceneId)
{
	return m_thunderManager->init(appId, sceneId, this);
}

int MediaManager::deInit()
{
	return m_thunderManager->deInit();
}

void MediaManager::setMessageDlg(HWND dlg)
{
	m_hMessageDlg = dlg;
}

void MediaManager::onJoinRoomSuccess(const char* roomName, const char* uid, int elapsed)
{
	::PostMessage(m_hMessageDlg, WM_JOIN_ROOM_SUCCESS, 0, 0);
}

void MediaManager::onLeaveRoom()
{
	::PostMessage(m_hMessageDlg, WM_LEAVE_ROOM, 0, 0);
}

void MediaManager::onRemoteAudioStopped(const char * uid, bool stop)
{
	int isStop = stop ? 1 : 0;
	unsigned int iUid = strtoul((const char*)uid, 0, 10);
	::PostMessage(m_hMessageDlg, WM_USER_STOP_AUDIO, (WPARAM)iUid, isStop);
}

void MediaManager::onRemoteVideoStopped(const char* uid, bool stop)
{
	int isStop = stop ? 1 : 0;
	unsigned int iUid = strtoul((const char*)uid, 0, 10);
	::PostMessage(m_hMessageDlg, WM_USER_STOP_VIDEO, (WPARAM)iUid, isStop);
}

void MediaManager::onTokenWillExpire(const char * token)
{
	::PostMessage(m_hMessageDlg, WM_TOKEN_WILL_EXPIRE, 0, 0);
}

ThunderManager*	MediaManager::getThunderManager()
{
	return m_thunderManager;
}

void MediaManager::onVideoCaptureStatus(int status) {
	::PostMessage(m_hMessageDlg, WM_VIDEO_CAPTURE_STATUS, 0, status);
}

void MediaManager::OnAudioDeviceStateChange(const char* deviceId, int deviceType, int deviceState) {
	Logd(TAG, Log(__FUNCTION__).addDetail("deviceId", deviceId).addDetail("deviceState", std::to_string(deviceState))
		.addDetail("deviceType", std::to_string(deviceType)));

	DeviceDetected* device = new DeviceDetected();
	device->deviceId = new char[strlen(deviceId) + 1];
	memset(device->deviceId, 0, strlen(deviceId) + 1);
	memcpy(device->deviceId, deviceId, strlen(deviceId));
	device->deviceState = (MEDIA_DEVICE_STATE_TYPE)deviceState;
	device->deviceType = (MEDIA_DEVICE_TYPE)deviceType;
	::PostMessage(m_hMessageDlg, WM_DEVICE_DETECTED, (WPARAM)device, 0);
}
