package com.mediaroom.adapter;

import android.content.Context;
import android.text.TextUtils;

import com.mediaroom.R;
import com.mediaroom.base.BaseAdapter;
import com.mediaroom.bean.UserInfo;
import com.mediaroom.facade.RoomManager;
import com.mediaroom.ui.MyThunderPlayerView;
import com.mediaroom.ui.MyThunderPreviewView;
import com.thunder.livesdk.ThunderRtcConstant;

import java.util.HashMap;
import java.util.List;
import java.util.Map;

/**
 *
 * Homepage View Adapter
 *
 * ZH：
 * 首页视图适配器
 *
 * @author Aslan chenhengfei@yy.com
 * @since 2019年12月18日
 */
public class RoomAdapter extends BaseAdapter<UserInfo> {

    private static final int TYPE_PREVIEW = 1;
    private static final int TYPE_PLAYVIEW = 2;

    private Context context;

    private Map<String, UserInfo> maps = new HashMap<>();

    public RoomAdapter(List<UserInfo> datas, Context context) {
        super(datas);
        this.context = context;
    }

    @Override
    public int getLayoutId(int viewType) {
        int layoutId = 0;
        switch (viewType) {
            case TYPE_PREVIEW:
                layoutId = R.layout.item_preview;
                break;
            case TYPE_PLAYVIEW:
                layoutId = R.layout.item_playview;
                break;
        }
        return layoutId;
    }

    @Override
    public int getItemViewType(int position) {
        if (position == 0) {
            return TYPE_PREVIEW;
        }
        return TYPE_PLAYVIEW;
    }

    @Override
    public void onBindViewHolder(VH holder, UserInfo data, int position) {
        if (TextUtils.equals(data.getUid(), RoomManager.getInstance(context).getUid())) {
            MyThunderPreviewView view = holder.getView(R.id.myview);
            view.joinRoom(data.getUid());
            view.changeLowNetStatus(
                    data.getTxQuality() == ThunderRtcConstant.NetworkQuality.THUNDER_QUALITY_VBAD);
            if (data.isAudioStreamStopped() || data.isMuteAudio()) {
                view.setAudioMute();
            } else {
                view.setAudioVolume(data.getMicVolume());
            }
        } else {
            MyThunderPlayerView view = holder.getView(R.id.myview);
            view.joinRoom(data.getUid());
            view.changeLowNetStatus(
                    data.getTxQuality() == ThunderRtcConstant.NetworkQuality.THUNDER_QUALITY_VBAD);
            if (data.isAudioStreamStopped() || data.isMuteAudio()) {
                view.setAudioMute();
            } else {
                view.setAudioVolume(data.getMicVolume());
            }
        }
    }

    public UserInfo getUserInfo(String targetUID) {
        return maps.get(targetUID);
    }

    @Override
    public void addItem(int postion, UserInfo data) {
        super.addItem(postion, data);
        maps.put(data.getUid(), data);
    }

    @Override
    public void addItem(UserInfo data) {
        super.addItem(data);
        maps.put(data.getUid(), data);
    }

    @Override
    public void deleteItem(UserInfo data) {
        super.deleteItem(data);
        maps.remove(data.getUid());
    }

    @Override
    public void clear() {
        super.clear();
        maps.clear();
    }
}
