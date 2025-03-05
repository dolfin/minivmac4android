package name.osher.gil.minivmac;

import android.content.SharedPreferences;
import android.graphics.Color;
import android.os.Bundle;

import androidx.annotation.NonNull;
import androidx.appcompat.app.ActionBar;
import androidx.core.app.ActivityCompat;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.view.WindowCompat;
import androidx.core.view.WindowInsetsCompat;
import androidx.core.view.WindowInsetsControllerCompat;
import androidx.fragment.app.Fragment;
import androidx.preference.PreferenceManager;

import android.view.GestureDetector;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.View;
import android.view.Window;

public class MiniVMac extends AppCompatActivity
		implements ActivityCompat.OnRequestPermissionsResultCallback {
	private static final String TAG = "minivmac.MiniVMac";

	private Fragment _currentFragment;

	private boolean mUIVisible = true;
	private GestureDetector mGestureDetector;
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		mGestureDetector = new GestureDetector(this, new MiniVMac.SingleTapGestureListener());

		// Check that the the file-system is readable
		if (!FileManager.getInstance().init(this)) {
			Utils.showAlert(this, String.format(getString(R.string.errNoDataDir), FileManager.getInstance().getRomDir().getPath(),
					getString(R.string.defaultRomFileName)), true);
		}

		// Check if ROM file was already provided and copied to the ROM directory
		SharedPreferences sharedPref = PreferenceManager.getDefaultSharedPreferences(this);
		if (sharedPref.getString(SettingsFragment.KEY_PREF_ROM, null) == null) {
			// If not, show the welcome screen
			showWelcome();
		} else {
			// Else, start the emulator
			showEmulator();
		}
	}

	private void showWelcome() {
		_currentFragment = new WelcomeFragment();
		getSupportFragmentManager()
				.beginTransaction()
				.replace(android.R.id.content, _currentFragment)
				.commitAllowingStateLoss();
	}

	public void showEmulator() {
		_currentFragment = new EmulatorFragment();
		getSupportFragmentManager()
				.beginTransaction()
				.replace(android.R.id.content, _currentFragment)
				.commitAllowingStateLoss();
	}

	/*
	 * Pass-through events to the emulator fragment
	 */
	@Override
	public boolean onKeyDown (int keyCode, @NonNull KeyEvent event) {
		if (_currentFragment instanceof IOnIOEventListener) {
			if (((IOnIOEventListener)_currentFragment).onKeyDown(keyCode, event)) {
				return true;
			}
		}
		return super.onKeyDown(keyCode, event);
	}
	
	@Override
	public boolean onKeyUp (int keyCode, @NonNull KeyEvent event) {
		if (_currentFragment instanceof IOnIOEventListener) {
			if (((IOnIOEventListener)_currentFragment).onKeyUp(keyCode, event)) {
				return true;
			}
		}
		return super.onKeyUp(keyCode, event);
	}

	@Override
	public boolean onTrackballEvent (MotionEvent event) {
		if (_currentFragment instanceof IOnIOEventListener) {
			if (((IOnIOEventListener)_currentFragment).onTrackballEvent(event)) {
				return true;
			}
		}
		return super.onTrackballEvent(event);
	}

	/*
	 * Show / Hide System UI logic
	 */
	@Override
	public boolean onTouchEvent (@NonNull MotionEvent event) {
		if (this.mGestureDetector.onTouchEvent(event)) {
			return true;
		} else {
			return super.onTouchEvent(event);
		}
	}

	@Override
	public void onWindowFocusChanged(boolean hasFocus) {
		super.onWindowFocusChanged(hasFocus);

		if (hasFocus) {
			if (mUIVisible) {
				showSystemUI();
			} else {
				hideSystemUI();
			}
		}
	}

	public boolean isUIVisible() {
		return mUIVisible;
	}

	public void toggleSystemUI() {
		if (mUIVisible) {
			hideSystemUI();
		} else {
			showSystemUI();
		}
	}

	private void hideSystemUI() {
		// Enables regular immersive mode.
		Window window = getWindow();
		View decorView = window.getDecorView();
		decorView.setBackgroundColor(Color.BLACK);

		ActionBar ab = getSupportActionBar();
		if (ab != null) {
			ab.hide();
		}

		WindowCompat.setDecorFitsSystemWindows(window, true);
		WindowInsetsControllerCompat controller = WindowCompat.getInsetsController(window, decorView);
        controller.hide(WindowInsetsCompat.Type.statusBars()
                | WindowInsetsCompat.Type.navigationBars());
        controller.setSystemBarsBehavior(WindowInsetsControllerCompat.BEHAVIOR_SHOW_TRANSIENT_BARS_BY_SWIPE);

        mUIVisible = false;
	}

	// Shows the system bars by removing all the flags
	// except for the ones that make the content appear under the system bars.
	private void showSystemUI() {
		Window window = getWindow();
		View decorView = window.getDecorView();
		decorView.setBackgroundColor(Color.BLACK);

		ActionBar ab = getSupportActionBar();
		if (ab != null) {
			ab.show();
		}

		WindowCompat.setDecorFitsSystemWindows(window, true);
		WindowInsetsControllerCompat controller = WindowCompat.getInsetsController(window, decorView);
        controller.show(WindowInsetsCompat.Type.statusBars()
                | WindowInsetsCompat.Type.navigationBars());

        mUIVisible = true;
	}

	class SingleTapGestureListener extends GestureDetector.SimpleOnGestureListener {
		@Override
		public boolean onDown(@NonNull MotionEvent event) {
			return true;
		}

		@Override
		public boolean onSingleTapUp(@NonNull MotionEvent e) {
			toggleSystemUI();
			return true;
		}
	}
}
