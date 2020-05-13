package com.mediaroom.utils;

import android.content.Context;
import android.preference.PreferenceManager;

import com.thunder.livesdk.ThunderRtcConstant;

import tv.athena.core.axis.Axis;
import tv.athena.crash.api.ICrashService;

public class Constant {

    public static final String TAG = "mediaroom";
    public static final String APPID = "1470236061";
    public static final String FEED_BACK_APP_ID = "JLYMeet-android";

    public static String mUid = null;
    public static String mChannelId = null;

    //清晰度
    public static Integer PUBLISH_MODE = null;
    public static final String SP_PUBLISHMODE = "sppublishmode";

    /**
     * Save sharpness settings
     *
     * ZH:
     * 保存清晰度设置
     *
     */
    public static void setPublishMode(Context context, int publishMode) {
        PUBLISH_MODE = publishMode;
        PreferenceManager.getDefaultSharedPreferences(context).edit()
                .putInt(Constant.SP_PUBLISHMODE, publishMode)
                .apply();
    }

    /**
     * Get sharpness settings
     *
     * ZH:
     * 获取清晰度
     *
     */
    public static int getPublishMode(Context context) {
        if (PUBLISH_MODE == null) {
            PUBLISH_MODE =
                    PreferenceManager.getDefaultSharedPreferences(context).getInt(SP_PUBLISHMODE,
                            ThunderRtcConstant.ThunderPublishVideoMode.THUNDERPUBLISH_VIDEO_MODE_NORMAL);
        }
        return PUBLISH_MODE;
    }

    /**
     * Set local UID
     *
     * ZH:
     * 设置本地UID
     *
     * @param uid
     */
    public static void setUID(String uid) {
        Constant.mUid = uid;

        //崩溃捕捉上报
        Axis.Companion.getService(ICrashService.class).config()
                .setGUid(Constant.mUid)
                .setAppId(Constant.FEED_BACK_APP_ID);
    }

    /**
     * Set local room number
     *
     * ZH:
     * 设置本地房间号
     *
     * @param roomId
     */
    public static void setRoomId(String roomId) {
        Constant.mChannelId = roomId;
    }
}
