package name.osher.gil.minivmac;

import android.view.KeyEvent;
import android.view.MotionEvent;

import androidx.annotation.NonNull;

public interface IOnIOEventListener {
    boolean onKeyDown (int keyCode, @NonNull KeyEvent event);
    boolean onKeyUp (int keyCode, @NonNull KeyEvent event);
    boolean onTrackballEvent (MotionEvent event);
}
