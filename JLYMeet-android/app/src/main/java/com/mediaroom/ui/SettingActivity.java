package com.mediaroom.ui;

import android.support.v7.widget.Toolbar;
import android.text.Editable;
import android.text.TextUtils;
import android.text.TextWatcher;
import android.view.View;
import android.widget.EditText;
import android.widget.RelativeLayout;
import android.widget.TextView;

import com.mediaroom.R;
import com.mediaroom.base.BaseActivity;
import com.mediaroom.facade.RoomManager;
import com.mediaroom.ui.MenuDialog.OnItemSelectedListener;
import com.mediaroom.utils.Constant;
import com.thunder.livesdk.ThunderRtcConstant;
import com.thunder.livesdk.ThunderVideoEncoderConfiguration;

import java.util.Arrays;

/**
 *
 * Set Page
 *
 * ZH:
 * 设置界面
 *
 * @author Aslan chenhengfei@yy.com
 * @since 2019年12月18日
 */
public class SettingActivity extends BaseActivity implements View.OnClickListener {

    private TextView mToolbarTitle;

    private TextView mTvSettingLive;
    private EditText mEdtSettingUid;
    private RelativeLayout mRlSettingLiveTitle;

    @Override
    protected int getLayoutResId() {
        return R.layout.activity_setting;
    }

    @Override
    protected void initView() {
        Toolbar toolbar = findViewById(R.id.toolbar);
        toolbar.setTitle("");
        toolbar.setNavigationOnClickListener(v -> SettingActivity.this.finish());
        mToolbarTitle = findViewById(R.id.toolbar_title);
        mToolbarTitle.setText(getString(R.string.setting));
        mEdtSettingUid = findViewById(R.id.edt_setting_uid);
        mRlSettingLiveTitle = findViewById(R.id.rl_setting_livetitle);
        mTvSettingLive = findViewById(R.id.tv_setting_live);

        mRlSettingLiveTitle.setOnClickListener(this::onClick);

        mEdtSettingUid.setText(Constant.mUid);
        mEdtSettingUid.setEnabled(RoomManager.getInstance(this).isJoinRoom.getValue() == false);
        mEdtSettingUid.addTextChangedListener(new TextWatcher() {
            @Override
            public void beforeTextChanged(CharSequence charSequence, int i, int i1, int i2) {
            }

            @Override
            public void onTextChanged(CharSequence charSequence, int i, int i1, int i2) {
                Constant.mUid = charSequence.toString();
            }

            @Override
            public void afterTextChanged(Editable editable) {
            }
        });

        toolbar.setNavigationOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (TextUtils.isEmpty(Constant.mUid)) {
                    showToast(R.string.error_ileagal_uid);
                    return;
                }

                if (TextUtils.isDigitsOnly(Constant.mUid) == false) {
                    showToast(R.string.error_ileagal_number);
                    return;
                }

                if (Integer.valueOf(Constant.mUid) < 100000 ||
                        Integer.valueOf(Constant.mUid) > 999999) {
                    showToast(R.string.error_ileagal_uid);
                    return;
                }

                finish();
            }
        });
    }

    @Override
    public void onBackPressed() {
        if (TextUtils.isEmpty(Constant.mUid)) {
            showToast(R.string.error_ileagal_uid);
            return;
        }

        if (TextUtils.isDigitsOnly(Constant.mUid) == false) {
            showToast(R.string.error_ileagal_number);
            return;
        }

        if (Integer.valueOf(Constant.mUid) < 100000 ||
                Integer.valueOf(Constant.mUid) > 999999) {
            showToast(R.string.error_ileagal_uid);
            return;
        }

        super.onBackPressed();
    }

    /**
     * choose Sharpness
     *
     * ZH:
     * 清晰度选择
     *
     */
    private void showPublishModeDialog() {
        String[] menus = getResources().getStringArray(R.array.publish_mode);
        new MenuDialog(this, Arrays.asList(menus), new OnItemSelectedListener() {

            @Override
            public void onItemSelected(int index) {
                int publishMode = 0;
                if (index == 0) {
                    publishMode =
                            ThunderRtcConstant.ThunderPublishVideoMode.THUNDERPUBLISH_VIDEO_MODE_FLUENCY;
                } else if (index == 1) {
                    publishMode =
                            ThunderRtcConstant.ThunderPublishVideoMode.THUNDERPUBLISH_VIDEO_MODE_NORMAL;
                } else if (index == 2) {
                    publishMode =
                            ThunderRtcConstant.ThunderPublishVideoMode.THUNDERPUBLISH_VIDEO_MODE_HIGHQULITY;
                } else if (index == 3) {
                    publishMode =
                            ThunderRtcConstant.ThunderPublishVideoMode.THUNDERPUBLISH_VIDEO_MODE_SUPERQULITY;
                }

                mTvSettingLive.setText(menus[index]);
                Constant.setPublishMode(SettingActivity.this, publishMode);

                ThunderVideoEncoderConfiguration configuration =
                        new ThunderVideoEncoderConfiguration();
                configuration.publishMode = publishMode;
                RoomManager.getInstance(SettingActivity.this).setVideoEncoderConfig(configuration);
            }
        }).show();
    }

    @Override
    protected void initData() {
        int publishMode = Constant.getPublishMode(this);
        String[] menus = getResources().getStringArray(R.array.publish_mode);
        if (publishMode
                == ThunderRtcConstant.ThunderPublishVideoMode.THUNDERPUBLISH_VIDEO_MODE_FLUENCY) {
            mTvSettingLive.setText(menus[0]);
        } else if (publishMode
                == ThunderRtcConstant.ThunderPublishVideoMode.THUNDERPUBLISH_VIDEO_MODE_NORMAL) {
            mTvSettingLive.setText(menus[1]);
        } else if (publishMode
                ==
                ThunderRtcConstant.ThunderPublishVideoMode.THUNDERPUBLISH_VIDEO_MODE_HIGHQULITY) {
            mTvSettingLive.setText(menus[2]);
        } else if (publishMode
                ==
                ThunderRtcConstant.ThunderPublishVideoMode.THUNDERPUBLISH_VIDEO_MODE_SUPERQULITY) {
            mTvSettingLive.setText(menus[3]);
        }
    }

    @Override
    public void onClick(View view) {
        switch (view.getId()) {
            case R.id.rl_setting_livetitle:
                showPublishModeDialog();
                break;
        }
    }

    @Override
    protected void onDestroy() {
        Constant.setUID(Constant.mUid);
        super.onDestroy();
    }
}
