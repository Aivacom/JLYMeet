package com.mediaroom.ui;

import android.Manifest;
import android.arch.lifecycle.Observer;
import android.content.Intent;
import android.support.annotation.Nullable;
import android.support.v7.widget.DefaultItemAnimator;
import android.support.v7.widget.GridLayoutManager;
import android.support.v7.widget.LinearLayoutManager;
import android.support.v7.widget.OrientationHelper;
import android.text.TextUtils;
import android.view.View;

import com.mediaroom.R;
import com.mediaroom.adapter.RoomAdapter;
import com.mediaroom.base.DataBindBaseActivity;
import com.mediaroom.bean.UserInfo;
import com.mediaroom.data.BaseError;
import com.mediaroom.data.DataRepository;
import com.mediaroom.data.HttpItemCallback;
import com.mediaroom.databinding.ActivityMainBinding;
import com.mediaroom.facade.MyThunderEventHandler;
import com.mediaroom.facade.OnRemoteListener;
import com.mediaroom.facade.RoomManager;
import com.mediaroom.ui.MenuDialog.OnItemSelectedListener;
import com.mediaroom.utils.Constant;
import com.mediaroom.utils.LogUtil;
import com.mediaroom.utils.PermissionUtils;
import com.thunder.livesdk.ThunderNotification;
import com.thunder.livesdk.ThunderRtcConstant;
import com.thunder.livesdk.ThunderVideoEncoderConfiguration;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Random;
import java.util.concurrent.CopyOnWriteArrayList;

/**
 *
 * Homepage
 *
 * ZH:
 * 首页
 *
 * @author Aslan chenhengfei@yy.com
 * @since 2019年12月18日
 */
public class MainActivity extends DataBindBaseActivity<ActivityMainBinding>
        implements View.OnClickListener {

    private RoomAdapter adapter;

    private String[] permissions = new String[]{
            Manifest.permission.CAMERA,
            Manifest.permission.RECORD_AUDIO,
            Manifest.permission.WRITE_EXTERNAL_STORAGE,
            Manifest.permission.READ_EXTERNAL_STORAGE};

    private volatile static boolean isFullScreen = false;

    private Map<String, UserInfo> localUserMap = new HashMap<>();

    public volatile static List<OnRemoteListener> mRemoteListeners = new CopyOnWriteArrayList<>();

    private MyThunderEventHandler mMyThunderEventHandler = new MyThunderEventHandler() {
        @Override
        public void onJoinRoomSuccess(String room, String uid, int elapsed) {
            super.onJoinRoomSuccess(room, uid, elapsed);
            dissMissDialogProgress();

            onJoinRoomStatus();
            startPlay();
        }

        @Override
        public void onLeaveRoom(RoomStats status) {
            super.onLeaveRoom(status);
            dissMissDialogProgress();

            onUnJoinRoomStatus();
        }

        @Override
        public void onRemoteAudioStopped(String uid, boolean stop) {
            super.onRemoteAudioStopped(uid, stop);

            UserInfo userInfo = adapter.getUserInfo(uid);
            if (stop) {
                if (userInfo != null) {
                    userInfo.setAudioStreamStopped(stop);

                    if (userInfo.isVideoStreamStopped() == true) {
                        onRemoteLeaveRoom(userInfo);
                    } else {
                        int index = adapter.indexOf(userInfo);
                        adapter.updateItem(index, userInfo);
                    }
                }
            } else {
                if (userInfo != null) {
                    userInfo.setAudioStreamStopped(stop);
                    int index = adapter.indexOf(userInfo);
                    adapter.updateItem(index, userInfo);
                } else {
                    userInfo = new UserInfo(uid);
                    userInfo.setAudioStreamStopped(stop);
                    onRemoteJoinRoom(userInfo);
                }
            }
        }

        @Override
        public void onRemoteVideoStopped(String uid, boolean stop) {
            super.onRemoteVideoStopped(uid, stop);

            UserInfo userInfo = adapter.getUserInfo(uid);
            if (stop) {
                if (userInfo != null) {
                    userInfo.setVideoStreamStopped(stop);

                    if (userInfo.isAudioStreamStopped() == true) {
                        onRemoteLeaveRoom(userInfo);
                    } else {
                        int index = adapter.indexOf(userInfo);
                        adapter.updateItem(index, userInfo);
                    }
                }
            } else {
                if (userInfo != null) {
                    userInfo.setVideoStreamStopped(stop);
                    int index = adapter.indexOf(userInfo);
                    adapter.updateItem(index, userInfo);
                } else {
                    userInfo = new UserInfo(uid);
                    userInfo.setVideoStreamStopped(stop);
                    onRemoteJoinRoom(userInfo);
                }
            }
        }

        @Override
        public void onCaptureVolumeIndication(int totalVolume, int cpt, int micVolume) {
            super.onCaptureVolumeIndication(totalVolume, cpt, micVolume);

            String uid = RoomManager.getInstance(MainActivity.this).getUid();
            UserInfo userInfo = adapter.getUserInfo(uid);
            if (userInfo != null) {
                userInfo.onCaptureVolumeIndication(totalVolume, cpt, micVolume);

                int index = adapter.indexOf(userInfo);
                adapter.updateItem(index, userInfo);
            }
        }

        @Override
        public void onPlayVolumeIndication(AudioVolumeInfo[] speakers, int totalVolume) {
            super.onPlayVolumeIndication(speakers, totalVolume);

            for (AudioVolumeInfo speaker : speakers) {
                String uid = speaker.uid;
                UserInfo userInfo = adapter.getUserInfo(uid);
                if (userInfo != null) {
                    userInfo.onPlayVolumeIndication(speaker);

                    int index = adapter.indexOf(userInfo);
                    adapter.updateItem(index, userInfo);
                }
            }
        }

        @Override
        public void onNetworkQuality(String uid, int txQuality, int rxQuality) {
            super.onNetworkQuality(uid, txQuality, rxQuality);

            if (TextUtils.equals("0", uid)) {
                uid = RoomManager.getInstance(MainActivity.this).getUid();
            }

            UserInfo userInfo = adapter.getUserInfo(uid);
            if (userInfo != null) {
                userInfo.onNetworkQuality(uid, txQuality, rxQuality);

                int index = adapter.indexOf(userInfo);
                adapter.updateItem(index, userInfo);
            }
        }

        @Override
        public void onRoomStats(ThunderNotification.RoomStats stats) {
            super.onRoomStats(stats);

            mDataBinding.roomStatus.setText("链路发送码率: " + stats.txBitrate
                    + " bps\n链路接收码率: " + stats.rxBitrate
                    + " bps\n音频包的发送码率: " + stats.txAudioBitrate
                    + " bps\n音频包的接收码率: " + stats.rxAudioBitrate
                    + " bps\n视频包的发送码率: " + stats.txVideoBitrate
                    + " bps\n视频包的接收码率: " + stats.rxVideoBitrate
                    + " bps");
        }
    };

    @Override
    protected int getLayoutResId() {
        return R.layout.activity_main;
    }

    //当前菜单点击的
    private UserInfo userInfo;

    private GridLayoutManager gridLayoutManager = new GridLayoutManager(this, 2);
    private LinearLayoutManager layoutManager =
            new LinearLayoutManager(this, OrientationHelper.VERTICAL, false) {
                @Override
                public boolean canScrollVertically() {
                    return false;
                }
            };

    @Override
    protected void initView() {
        mDataBinding.tvVideoRoom.setOnClickListener(this);
        mDataBinding.imgSwitchcamera.setOnClickListener(this);
        mDataBinding.imageviewSetting.setOnClickListener(this);
        mDataBinding.imageviewLog.setOnClickListener(this);
        mDataBinding.imageviewFeedback.setOnClickListener(this);

        adapter = new RoomAdapter(new CopyOnWriteArrayList<>(), MainActivity.this);
        mDataBinding.recyclerView.setLayoutManager(gridLayoutManager);
        mDataBinding.recyclerView.setAdapter(adapter);
        mDataBinding.recyclerView.setItemAnimator(new DefaultItemAnimator());
        adapter.setOnItemClickListener((view, position) -> {
            UserInfo userInfo = adapter.getItem(position);
            if (userInfo == null) {
                return;
            }

            MainActivity.this.userInfo = userInfo;
            showVideoMenuDialog(view, userInfo, position);
        });
    }

    @Override
    protected void initData() {
        PermissionUtils.checkAndRequestMorePermissions(MainActivity.this, permissions,
                PermissionUtils.REQUEST_CODE_PERMISSIONS,
                () -> LogUtil.v(Constant.TAG, "已授予权限"));

        String uid = String.valueOf(new Random().nextInt(899999) + 100000);
        Constant.setUID(uid);

        onUnJoinRoomStatus();

        monitorFrontCameraStatus();

        RoomManager.getInstance(this).register(mMyThunderEventHandler);
        RoomManager.getInstance(this).createEngine();
        RoomManager.getInstance(this).setAudioVolumeIndication();
        RoomManager.getInstance(this).enableCaptureVolumeIndication();

        //设置播放参数
        ThunderVideoEncoderConfiguration configuration = new ThunderVideoEncoderConfiguration();
        configuration.publishMode = Constant.getPublishMode(this);
        configuration.playType =
                ThunderRtcConstant.ThunderPublishPlayType.THUNDERPUBLISH_PLAY_INTERACT;
        RoomManager.getInstance(this).setVideoEncoderConfig(configuration);

        //设置模式
        RoomManager.getInstance(MainActivity.this)
                .setMediaMode(
                        ThunderRtcConstant.ThunderRtcProfile.THUNDER_PROFILE_DEFAULT);

        //设置房间场景模式
        RoomManager.getInstance(MainActivity.this)
                .setRoomMode(
                        ThunderRtcConstant.RoomConfig.THUNDER_ROOMCONFIG_LIVE);
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()) {
            case R.id.tv_video_room:
                if (RoomManager.getInstance(this).isJoinRoom.getValue() == true) {
                    leaveRoom();
                } else {
                    joinRoom();
                }
                break;
            case R.id.img_switchcamera:
                switchFrontCamera();
                break;
            case R.id.imageview_setting:
                startActivity(new Intent(MainActivity.this, SettingActivity.class));
                break;
            case R.id.imageview_log:
                handleLogView();
                break;
            case R.id.imageview_feedback:
                startActivity(new Intent(MainActivity.this, FeedBackActivity.class));
                break;
        }
    }

    /**
     * Click Log view
     *
     * ZH：
     * 点击LogView
     */
    private void handleLogView() {
        if (mDataBinding.scLog.getVisibility() == View.VISIBLE) {
            mDataBinding.scLog.setVisibility(View.GONE);
        } else {
            mDataBinding.scLog.setVisibility(View.VISIBLE);
        }
    }

    /**
     * Switch front and rear cameras
     *
     * ZH：
     * 切换前后置摄像头
     */
    private void switchFrontCamera() {
        int result = RoomManager.getInstance(this).switchFrontCamera();
        if (result != 0) {
            showToast(getString(R.string.error_switch_forn_camera, String.valueOf(result)));
        }
    }

    /**
     * Join the room
     *
     * ZH：
     * 加入房间
     */
    private void joinRoom() {
        String roomId = mDataBinding.videoEdittextRoomid.getText().toString().trim();
        if (TextUtils.isEmpty(roomId)) {
            showToast(R.string.error_ileagal_room_id);
            return;
        }

        if (TextUtils.isDigitsOnly(roomId) == false) {
            showToast(R.string.error_ileagal_number);
            return;
        }

        Constant.setRoomId(roomId);

        showDialogProgress();
        DataRepository.getInstance(this)
                .refreshToken(Constant.APPID, Constant.mUid, new HttpItemCallback<String>() {
                    @Override
                    public void onSuccess(String token) {
                        runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                dissMissDialogProgress();
                                if (TextUtils.isEmpty(token)) {
                                    showToast(R.string.token_error);
                                    return;
                                }

                                //加入房间
                                showDialogProgress();
                                int result =
                                        RoomManager.getInstance(MainActivity.this)
                                                .joinRoom(token, Constant.mChannelId,
                                                        Constant.mUid);
                                if (result != 0) {
                                    dissMissDialogProgress();
                                    showToast(getString(R.string.error_join_room, result));
                                }
                            }
                        });
                    }

                    @Override
                    public void onFail(BaseError error) {
                        runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                dissMissDialogProgress();
                                showToast(R.string.token_error);
                            }
                        });
                    }
                });
    }

    /**
     * Leave the room
     *
     * ZH：
     * 离开房间
     */
    private void leaveRoom() {
        //还原操作
        localUserMap.clear();
        dissMissDialogProgress();
        RoomManager.getInstance(MainActivity.this).stopAllRemoteAudioStreams(false);
        RoomManager.getInstance(MainActivity.this).stopAllRemoteVideoStreams(false);

        exitFullScreen();

        //Stop local audio and video
        //停止本地开播
        RoomManager.getInstance(this).stopAudioAndVideo();

        //Exit the room
        //退出房间
        RoomManager.getInstance(this).leaveRoom();
    }

    /**
     * The status after joining the room
     *
     * ZH:
     * 加入房间后状态
     */
    private void onJoinRoomStatus() {
        mDataBinding.tvVideoRoom.setText(R.string.leave_room);
        mDataBinding.videoEdittextRoomid.setEnabled(false);

        mDataBinding.imageviewLog.setEnabled(true);
    }

    /**
     * The status of not joining the room
     *
     * ZH:
     * 未加入房间的状态
     */
    private void onUnJoinRoomStatus() {
        mDataBinding.recyclerView.setVisibility(View.VISIBLE);
        isFullScreen = false;
        MainActivity.this.userInfo = null;

        for (UserInfo userInfo : adapter.getData()) {
            for (OnRemoteListener listener : mRemoteListeners) {
                String uid = userInfo.getUid();
                listener.onLeaveRoom(uid);
            }
        }
        mRemoteListeners.clear();
        adapter.clear();
        mDataBinding.tvVideoRoom.setText(R.string.join_room);
        mDataBinding.videoEdittextRoomid.setEnabled(true);

        mDataBinding.imageviewLog.setEnabled(false);
        mDataBinding.scLog.setVisibility(View.GONE);
    }

    /**
     * Monitor the status of front and rear cameras
     *
     * ZH:
     * 监控前后置摄像头状态
     */
    private void monitorFrontCameraStatus() {
        RoomManager.getInstance(this).mIsFrontCamera.observe(this, new Observer<Boolean>() {
            @Override
            public void onChanged(@Nullable Boolean aBoolean) {
                if (aBoolean == null) {
                    mDataBinding.imgSwitchcamera.setEnabled(false);
                } else {
                    mDataBinding.imgSwitchcamera.setEnabled(true);
                }
            }
        });
    }

    /**
     * Start pushing
     *
     * ZH：
     * 开始推流
     */
    private void startPlay() {

        int result = RoomManager.getInstance(this).startAudioAndVideo();
        if (result != 0) {
            showToast(getString(R.string.error_start_play, result));
            return;
        }

        String uid = RoomManager.getInstance(this).getUid();
        UserInfo userInfo = new UserInfo(uid);
        userInfo.setAudioStreamStopped(false);
        userInfo.setVideoStreamStopped(false);
        adapter.addItem(0, userInfo);
        for (OnRemoteListener listener : mRemoteListeners) {
            listener.onJoinRoom(userInfo.getUid());
        }
    }

    @Override
    protected void onDestroy() {
        leaveRoom();
        RoomManager.getInstance(this).unRegister(mMyThunderEventHandler);
        RoomManager.getInstance(this).destroyEngine();
        super.onDestroy();
    }

    @Override
    public void onBackPressed() {
        moveTaskToBack(true);
    }

    /**
     * Exit Full Screen
     *
     * ZH:
     * 退出全屏
     */
    private void exitFullScreen() {
        if (isFullScreen == false) {
            return;
        }

        mDataBinding.recyclerView.setLayoutManager(gridLayoutManager);
        mDataBinding.recyclerView.scrollToPosition(0);

        mDataBinding.line1.setVisibility(View.VISIBLE);
        mDataBinding.line2.setVisibility(View.VISIBLE);

        isFullScreen = false;
    }

    private MenuDialog menuDialog;

    /**
     * Menu
     *
     * ZH:
     * 菜单
     */
    private void showVideoMenuDialog(View view, UserInfo userInfo, int position) {
        String uid = RoomManager.getInstance(MainActivity.this).getUid();
        String targetUid = userInfo.getUid();
        boolean isLocal = TextUtils.equals(uid, targetUid);

        List<String> menus = new ArrayList<>();

        //amplification
        //放大
        if (isFullScreen) {
            menus.add(getString(R.string.full_screen_return));
        } else {
            menus.add(getString(R.string.full_screen));
        }

        //Video
        //视频
        if (isLocal == true) {
            //Local
            //本地
            if (userInfo.isVideoStreamStopped()) {
                menus.add(getString(R.string.open_video));
            } else {
                menus.add(getString(R.string.stop_video));
            }
        } else {
            //Remotely
            //远程
            // if (userInfo.isVideoStreamStopped() == false) {
            //The camera is not turned off remotely
            //远程未关闭摄像头
            if (userInfo.isMuteVideo()) {
                menus.add(getString(R.string.open_remote_video));
            } else {
                menus.add(getString(R.string.close_remote_video));
            }
            // }
        }

        //Audio
        //音频
        if (isLocal == true) {
            //Local
            //本地
            if (userInfo.isAudioStreamStopped()) {
                menus.add(getString(R.string.open_audio));
            } else {
                menus.add(getString(R.string.stop_audio));
            }
        } else {
            //Remotely
            //远程
            // if (userInfo.isAudioStreamStopped() == false) {
            //Does not turn off audio
            //远程未关闭音频
            if (userInfo.isMuteAudio()) {
                menus.add(getString(R.string.open_remote_audio));
            } else {
                menus.add(getString(R.string.close_remote_audio));
            }
            // }
        }

        menuDialog = new MenuDialog(this, menus, new OnItemSelectedListener() {
            @Override
            public void onItemSelected(int index) {
                if (index == 0) {
                    handleFullScreen(view, userInfo, position);
                } else if (index == 1) {
                    handleVideo(view, userInfo, position);
                } else if (index == 2) {
                    handleAuido(view, userInfo, position);
                }

                //Refresh the view
                //刷新视图
                adapter.updateItem(position, userInfo);
            }
        });
        menuDialog.show();
    }

    private void closeVideoMenuDialog() {
        if (menuDialog != null && menuDialog.isShowing()) {
            menuDialog.dismiss();
        }
    }

    /**
     * Processing zoom logic
     *
     * ZH：
     * 处理放大缩小逻辑
     */
    private void handleFullScreen(View view, UserInfo userInfo, int position) {
        String uid = RoomManager.getInstance(MainActivity.this).getUid();
        String targetUid = userInfo.getUid();
        boolean isLocal = TextUtils.equals(uid, targetUid);

        if (isFullScreen) {
            exitFullScreen();

            //处理个别手机surfaceview会重合问题，所以恢复的时候就还原设置
            List<UserInfo> lists = adapter.getData();
            for (UserInfo data : lists) {
                if (TextUtils.equals(data.getUid(), userInfo.getUid()) == false) {
                    if (data.isMuteVideo() == false) {
                        //打开其他视频流
                        RoomManager.getInstance(MainActivity.this)
                                .stopRemoteVideoStream(data.getUid(), false);
                    }
                }
            }
        } else {
            mDataBinding.recyclerView.setLayoutManager(layoutManager);
            mDataBinding.recyclerView.scrollToPosition(position);

            mDataBinding.line1.setVisibility(View.GONE);
            mDataBinding.line2.setVisibility(View.GONE);
            isFullScreen = true;

            //处理个别手机surfaceview会重合问题，所以全屏的时候就关闭其他
            List<UserInfo> lists = adapter.getData();
            for (UserInfo data : lists) {
                if (TextUtils.equals(data.getUid(), userInfo.getUid()) == false) {
                    //关闭其他视频流
                    RoomManager.getInstance(MainActivity.this)
                            .stopRemoteVideoStream(data.getUid(), true);
                }
            }
        }
    }

    /**
     * Processing audio logic
     *
     * ZH：
     * 处理音频逻辑
     */
    private void handleAuido(View view, UserInfo userInfo, int position) {
        String uid = RoomManager.getInstance(MainActivity.this).getUid();
        String targetUid = userInfo.getUid();
        boolean isLocal = TextUtils.equals(uid, targetUid);

        if (isLocal == true) {
            //Set Local
            //设置本地
            userInfo.setMuteAudio(false);
            if (userInfo.isAudioStreamStopped()) {
                int result = RoomManager.getInstance(MainActivity.this).startAudio();
                if (result != 0) {
                    return;
                }
                userInfo.setAudioStreamStopped(false);
            } else {
                int result = RoomManager.getInstance(MainActivity.this).stopAudio();
                if (result != 0) {
                    return;
                }
                userInfo.setAudioStreamStopped(true);
            }
        } else {
            //设置远程
            // if (userInfo.isAudioStreamStopped()) {
            //     //对方已经关闭了，没法设置
            //     showToast("对方关闭了音频，无法打开");
            //     return;
            // }

            if (userInfo.isMuteAudio()) {
                int result = RoomManager.getInstance(MainActivity.this)
                        .stopRemoteAudioStream(targetUid, false);
                if (result != 0) {
                    return;
                }
                userInfo.setMuteAudio(false);
            } else {
                int result = RoomManager.getInstance(MainActivity.this)
                        .stopRemoteAudioStream(targetUid, true);
                if (result != 0) {
                    return;
                }
                userInfo.setMuteAudio(true);
            }
        }
    }

    /**
     * Processing video logic
     *
     * ZH:
     * 处理视频逻辑
     */
    private void handleVideo(View view, UserInfo userInfo, int position) {
        String uid = RoomManager.getInstance(MainActivity.this).getUid();
        String targetUid = userInfo.getUid();
        boolean isLocal = TextUtils.equals(uid, targetUid);

        //Video control
        //视频控制
        if (isLocal == true) {

            //Set Local
            //设置本地
            userInfo.setMuteVideo(false);
            if (userInfo.isVideoStreamStopped()) {
                //open video
                //打开
                int result = RoomManager.getInstance(MainActivity.this).startVideo();
                if (result != 0) {
                    return;
                }

                userInfo.setVideoStreamStopped(false);
            } else {
                //close video
                //关闭
                int result = RoomManager.getInstance(MainActivity.this).stopVideo();
                if (result != 0) {
                    return;
                }

                MyThunderPreviewView thunderPreviewView = view.findViewById(R.id.myview);
                thunderPreviewView.resetView();
                userInfo.setVideoStreamStopped(true);
            }
        } else {
            //Set up remote
            //设置远程
            // if (userInfo.isVideoStreamStopped()) {
            //     //对方已经关闭了，没法设置
            //     showToast("对方关闭了视频，无法打开");
            //     return;
            // }

            if (userInfo.isMuteVideo()) {
                //open
                //打开
                int result = RoomManager.getInstance(MainActivity.this)
                        .stopRemoteVideoStream(targetUid, false);
                if (result != 0) {
                    return;
                }

                userInfo.setMuteVideo(false);
            } else {
                //close video
                //关闭
                int result = RoomManager.getInstance(MainActivity.this)
                        .stopRemoteVideoStream(targetUid, true);
                if (result != 0) {
                    return;
                }

                userInfo.setMuteVideo(true);
            }
        }
    }

    /**
     * Remote user joins the room
     *
     * ZH：
     * 远程用户加入房间
     *
     * @param userInfo
     */
    private void onRemoteJoinRoom(UserInfo userInfo) {
        String uid = userInfo.getUid();
        UserInfo local = localUserMap.get(uid);
        if (local != null) {
            //This user has been to the room before
            //此用户之前来过房间
            userInfo.setMuteAudio(local.isMuteAudio());
            userInfo.setMuteVideo(local.isMuteVideo());
        }

        adapter.addItem(userInfo);
        localUserMap.put(uid, userInfo);
        for (OnRemoteListener listener : mRemoteListeners) {
            listener.onJoinRoom(userInfo.getUid());
        }
    }

    /**
     * Remote user exits the room
     *
     * ZH:
     * 远程用户退出房间
     *
     * @param userInfo
     */
    private void onRemoteLeaveRoom(UserInfo userInfo) {
        //If you are in full screen, exit full screen mode
        //如果正在全屏，就退出全屏模式
        if (MainActivity.this.userInfo != null &&
                TextUtils.equals(MainActivity.this.userInfo.getUid(), userInfo.getUid())) {

            //close menu
            //关闭菜单
            closeVideoMenuDialog();

            if (isFullScreen) {
                exitFullScreen();
            }
            MainActivity.this.userInfo = null;
        }

        for (OnRemoteListener listener : mRemoteListeners) {
            listener.onLeaveRoom(userInfo.getUid());
        }
        adapter.deleteItem(userInfo);
    }
}