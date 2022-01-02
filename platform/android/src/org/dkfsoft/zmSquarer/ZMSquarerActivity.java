package org.dkfsoft.zmSquarer;

import android.os.Bundle;
import android.util.Log;
import org.qtproject.qt5.android.bindings.QtActivity;
import org.qtproject.qt5.android.bindings.QtApplication;
import android.view.WindowManager;
import static android.view.WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON;

public class ZMSquarerActivity extends QtActivity {

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        this.getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
    }

    @Override
    public void onPause() {
        super.onPause();
    }

    @Override
    public void onResume() {
        super.onResume();
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
    }
}
