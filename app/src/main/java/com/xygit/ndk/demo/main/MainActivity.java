package com.xygit.ndk.demo.main;

import android.view.View;
import android.widget.Button;

import com.github.angads25.filepicker.controller.DialogSelectionListener;
import com.github.angads25.filepicker.model.DialogConfigs;
import com.github.angads25.filepicker.model.DialogProperties;
import com.github.angads25.filepicker.view.FilePickerDialog;
import com.xygit.ndk.demo.R;
import com.xygit.ndk.demo.base.BaseActivity;
import com.xygit.ndk.demo.view.XyPlayer;

import java.io.File;

public class MainActivity extends BaseActivity {

    private Button btnSelectFile;

    private XyPlayer xyPlayer;

    @Override
    public int layoutId() {
        return R.layout.activity_main;
    }

    @Override
    public void initView() {
        // Example of a call to a native method
//        TextView tv = (TextView) findViewById(R.id.sample_text);
//        tv.setText("C++");
        btnSelectFile = findViewById(R.id.btn_select_file);
        xyPlayer = findViewById(R.id.xyPlayer);
    }

    @Override
    public void initAction() {

    }

    @Override
    public void initData() {

    }

    @Override
    public void clearData() {

    }

    public void onClick2(View view) {
        DialogProperties properties = new DialogProperties();
        properties.selection_mode = DialogConfigs.SINGLE_MODE;
        properties.selection_type = DialogConfigs.FILE_SELECT;
        properties.root = new File(DialogConfigs.DEFAULT_DIR);
        properties.error_dir = new File(DialogConfigs.DEFAULT_DIR);
        properties.offset = new File(DialogConfigs.DEFAULT_DIR);
        properties.extensions = null;
        final FilePickerDialog dialog = new FilePickerDialog(MainActivity.this, properties);
        dialog.setTitle("选择文件");
        dialog.show();
        dialog.setDialogSelectionListener(new DialogSelectionListener() {
            @Override
            public void onSelectedFilePaths(String[] files) {
                //files is the array of the paths of files selected by the Application User.
                //Toast.makeText(MainActivity.this, Arrays.toString(files), Toast.LENGTH_SHORT).show();
                dialog.dismiss();
                xyPlayer.start(files[0]);
            }
        });
    }
}
