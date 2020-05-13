package com.mediaroom.ui;

import android.support.annotation.NonNull;
import android.support.v7.widget.Toolbar;
import android.text.TextUtils;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

import com.mediaroom.BuildConfig;
import com.mediaroom.R;
import com.mediaroom.base.BaseActivity;
import com.mediaroom.utils.Constant;
import com.mediaroom.utils.FileUtil;

import org.jetbrains.annotations.NotNull;

import java.io.File;
import java.util.ArrayList;
import java.util.List;

import tv.athena.core.axis.Axis;
import tv.athena.feedback.api.FeedbackData;
import tv.athena.feedback.api.IFeedbackService;

/**
 *
 * FeedBack Activity
 *
 * ZH:
 * 反馈界面
 *
 * @author Aslan chenhengfei@yy.com
 * @since 2019年12月18日
 */
public class FeedBackActivity extends BaseActivity {

    private TextView mVersionText, mToolbarTitle;
    private EditText mFeedbackEdit;
    private Button mFeedbackBtn;

    @Override
    protected int getLayoutResId() {
        return R.layout.activity_feedback;
    }

    @Override
    protected void initView() {
        mFeedbackBtn = findViewById(R.id.imageview_feedback);
        mVersionText = findViewById(R.id.tv_app_version);
        mFeedbackEdit = findViewById(R.id.et_feedback_content);
        Toolbar toolbar = findViewById(R.id.toolbar);
        mToolbarTitle = findViewById(R.id.toolbar_title);
        toolbar.setTitle("");
        mToolbarTitle.setText(R.string.upload_log_tittle);

        toolbar.setNavigationOnClickListener(v -> FeedBackActivity.this.finish());
        mFeedbackBtn.setOnClickListener(view -> {
            String msg = mFeedbackEdit.getText().toString().trim();
            if (TextUtils.isEmpty(msg) == true) {
                showToast(R.string.input_content_tip);
                return;
            }

            feedback(msg);
        });
        mVersionText.setText(
                FeedBackActivity.this.getString(R.string.app_version, BuildConfig.VERSION_NAME,
                        String.valueOf(BuildConfig.VERSION_CODE)));
    }

    @Override
    protected void initData() {

    }

    /**
     *
     * The default feedback will only upload the logs under the main directory, not the logs under the folder, so we need to add them manually.
     *
     * ZH:
     * 默认反馈只会上传主目录下的日志，不能上传文件夹下面日志，所以需要我们手动添加。
     *
     */
    private void addFiles(@NonNull List<File> files, @NonNull File fileDirectory) {
        if (fileDirectory.exists()) {
            if (fileDirectory.isDirectory()) {
                File[] listFiles = fileDirectory.listFiles();
                if (listFiles == null) {
                    return;
                }

                for (File file : listFiles) {
                    if (file.exists()) {
                        if (file.isDirectory()) {
                            addFiles(files, file);
                        } else {
                            files.add(file);
                        }
                    }
                }
            } else {
                files.add(fileDirectory);
            }
        }
    }

    private void feedback(String msg) {
        showDialogProgress();
        File fileLogs = new File(FileUtil.getLog(this));
        List<File> files = new ArrayList<>();
        addFiles(files, fileLogs);

        FeedbackData feedbackData =
                new FeedbackData.Builder(Constant.FEED_BACK_APP_ID, Long.parseLong(Constant.mUid),
                        msg)
                        .setExternPathlist(files)
                        .setFeedbackStatusListener(new FeedbackData.FeedbackStatusListener() {
                            @Override
                            public void onFailure(@NotNull FailReason failReason) {
                                dissMissDialogProgress();
                                showToast(R.string.feedback_failed);
                            }

                            @Override
                            public void onComplete() {
                                dissMissDialogProgress();
                                showToast(R.string.feedback_success);
                                finish();
                            }

                            @Override
                            public void onProgressChange(int i) {

                            }
                        }).create();
        Axis.Companion.getService(IFeedbackService.class).sendNewLogUploadFeedback(feedbackData);
    }
}
