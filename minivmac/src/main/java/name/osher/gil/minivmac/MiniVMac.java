package name.osher.gil.minivmac;

import java.io.File;
import java.io.FileFilter;
import java.io.FileInputStream;
import java.nio.ByteBuffer;
import java.util.List;

import android.Manifest;
import android.app.Dialog;
import android.content.Intent;
import android.content.SharedPreferences;
import android.content.pm.PackageManager;
import android.content.res.Configuration;
import android.inputmethodservice.Keyboard;
import android.inputmethodservice.KeyboardView;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.preference.PreferenceManager;
import android.support.annotation.NonNull;
import android.support.design.widget.Snackbar;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.support.v4.view.GestureDetectorCompat;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.view.GestureDetector;
import android.view.KeyEvent;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.MotionEvent;
import android.view.SubMenu;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;

import com.nononsenseapps.filepicker.FilePickerActivity;

public class MiniVMac extends AppCompatActivity
		implements ActivityCompat.OnRequestPermissionsResultCallback {

	private final static int[] keycodeTranslationTable = {-1, -1, -1, -1, -1, -1, -1, 0x1D, 0x12, 0x13, 0x14, 0x15, 0x17, 0x16, 0x1A, 0x1C, 0x19, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 0x00, 0x0B, 0x08, 0x02, 0x0E, 0x03, 0x05, 0x04, 0x22, 0x26, 0x28, 0x25, 0x2E, 0x2D, 0x1F, 0x23, 0x0C, 0x0F, 0x01, 0x11, 0x20, 0x09, 0x0D, 0x07, 0x10, 0x06, 0x2B, 0x2F, 0x37, 0x37, 0x38, 0x38, 0x30, 0x31, 0x3A, -1, -1, 0x24, 0x33, 0x32, 0x1B, 0x18, 0x21, 0x1E, 0x2A, 0x29, 0x27, 0x2C, 0x37, 0x3A, -1, -1, 0x45, -1, -1, 0x3A, -1, -1, -1, -1, -1, -1, -1};
	private final static String[] diskExtensions = {"DSK", "dsk", "img", "IMG"};
	private final static int ACTIVITY_CREATE_DISK = 200;
	private final static int ACTIVITY_SETTINGS = 201;
	private final static int ACTIVITY_SELECT_DISK = 202;
	private final static int ACTIVITY_SELECT_FOLDER = 203;
	private final static int MY_PERMISSIONS_REQUEST_WRITE_EXTERNAL_STORAGE = 1;
	private final static int TRACKBALL_SENSITIVITY = 8;
	private final static int KEYCODE_MAC_SHIFT = 56;

	private ScreenView mScreenView;
	private Boolean onActivity = false;
	private Boolean isLandscape = false;
	private Boolean hasPermission = false;
	private GestureDetectorCompat mGestureDetector;
	private boolean mUIVisible = true;

	private KeyboardView mKeyboardView;
	private Keyboard mQwertyKeyboard;
	private Keyboard mSymbolsKeyboard;
	private Keyboard mSymbolsShiftedKeyboard;

	private int mTempSize;
	private String mTempFilename;

	private View mLayout;

	private Core mCore;
	private Handler mUIHandler;
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

        setContentView(R.layout.activity_mini_vmac);
		mLayout = findViewById(R.id.main_layout);
        mScreenView = findViewById(R.id.screen);
		mGestureDetector = new GestureDetectorCompat(this, new SingleTapGestureListener());
        mUIHandler = new Handler(getMainLooper());

		isLandscape = (getResources().getConfiguration().orientation == Configuration.ORIENTATION_LANDSCAPE);
		toggleFullscreen(isLandscape);
		initKeyboard();

        onActivity = false;
        
        updateByPrefs();

		askForPermissions();
	}

	private void initEmulator() {
		if (!FileManager.getInstance().init()) {
			Utils.showAlert(this, String.format(getString(R.string.errNoDataDir), FileManager.getInstance().getDataDir().getPath(),
					getString(R.string.romFileName)), true);
		}

		// load ROM
		String romFileName = getString(R.string.romFileName);
		File romFile = FileManager.getInstance().getDataFile(romFileName);
		final ByteBuffer rom = ByteBuffer.allocateDirect((int)romFile.length());
		try {
            FileInputStream romReader = new FileInputStream(romFile);
            romReader.getChannel().read(rom);
            romReader.close();
        } catch (Exception x) {
            Utils.showAlert(this, String.format(getString(R.string.errNoROM), romFile.getPath(),
					getString(R.string.romFileName)), true);
            return;
        }

        this.invalidateOptionsMenu();

        final MiniVMac thiz = this;

		Thread emulation = new Thread(new Runnable() {
			@Override
			public void run() {
				mCore = new Core(thiz);

				mCore.setOnInitScreenListener(new Core.OnInitScreenListener() {
					@Override
					public void onInitScreen(final int screenWidth, final int screenHeight) {
						mUIHandler.post(new Runnable() {
											@Override
											public void run() {
												mScreenView.setTargetScreenSize(screenWidth, screenHeight);
											}
										}
						);
					}
				});

				mScreenView.setOnMouseEventListener(new ScreenView.OnMouseEventListener() {
					@Override
					public void onMouseMove(int x, int y) {
						mCore.setMousePosition(x, y);
					}

					@Override
					public void onMouseClick(boolean down) {
						mCore.setMouseBtn(down);
					}
				});

				mCore.setOnUpdateScreenListener(new Core.OnUpdateScreenListener() {
					@Override
					public void onUpdateScreen(final int[] update, final int top, final int left,
											   final int bottom, final int right) {
						mUIHandler.post(new Runnable() {
							@Override
							public void run() {
								mScreenView.updateScreen(update, top, left, bottom, right);
							}
						});
					}
				});

				mCore.setOnDiskEventListener(new Core.OnDiskEventListener() {

					@Override
					public void onDiskInserted(String filename) {
						invalidateOptionsMenu();
					}

					@Override
					public void onDiskEjected(String filename) {
						invalidateOptionsMenu();
					}

					@Override
					public void onCreateDisk(int size, String filename) {
						mTempSize = size;
						mTempFilename = filename;
						showSelectFolder();
					}
				});

				//mCore.resumeEmulation();
				mCore.initEmulation(mCore, rom);
				System.exit(0);
			}
		});
		emulation.setName("EmulationThread");
		emulation.start();
	}

	private void initKeyboard() {
		// Create the Keyboard
		mQwertyKeyboard = new Keyboard(this, R.xml.qwerty);
		mSymbolsKeyboard = new Keyboard(this, R.xml.symbols);
		mSymbolsShiftedKeyboard = new Keyboard(this, R.xml.symbols_shift);

		// Lookup the KeyboardView
		mKeyboardView = (KeyboardView)findViewById(R.id.keyboard);
		// Attach the keyboard to the view
		mKeyboardView.setKeyboard(mQwertyKeyboard);
		// Do not show the preview balloons
		mKeyboardView.setPreviewEnabled(false);
		// Install the key handler
		mKeyboardView.setOnKeyboardActionListener(mOnKeyboardActionListener);
	}

	private KeyboardView.OnKeyboardActionListener mOnKeyboardActionListener = new KeyboardView.OnKeyboardActionListener() {

		@Override public void onKey(int primaryCode, int[] keyCodes) {
			if (mKeyboardView != null) {
				if (primaryCode == Keyboard.KEYCODE_MODE_CHANGE) {
					Keyboard current = mKeyboardView.getKeyboard();
					if (current == mSymbolsKeyboard) {
						mKeyboardView.setKeyboard(mQwertyKeyboard);
						setShifted(false);
						mKeyboardView.setShifted(false);
					} else if (current == mSymbolsShiftedKeyboard) {
						mKeyboardView.setKeyboard(mQwertyKeyboard);
						setShifted(true);
						mKeyboardView.setShifted(true);
					} else {
						if (getKey(KEYCODE_MAC_SHIFT).on) {
							mKeyboardView.setKeyboard(mSymbolsShiftedKeyboard);
							setShifted(true);
						} else {
							mKeyboardView.setKeyboard(mSymbolsKeyboard);
							setShifted(false);
						}
					}
				} else if (primaryCode == KEYCODE_MAC_SHIFT) {
					Keyboard currentKeyboard = mKeyboardView.getKeyboard();
					if (mQwertyKeyboard == currentKeyboard) {
						mKeyboardView.setShifted(!mKeyboardView.isShifted());
					} else if (currentKeyboard == mSymbolsKeyboard) {
						setShifted(true);
						mKeyboardView.setKeyboard(mSymbolsShiftedKeyboard);
						setShifted(true);
					} else if (currentKeyboard == mSymbolsShiftedKeyboard) {
						setShifted(false);
						mKeyboardView.setKeyboard(mSymbolsKeyboard);
						setShifted(false);
					}
				}
			}
		}

		@Override public void onPress(int primaryCode) {
			if (primaryCode >= 0) {
				Keyboard.Key key = getKey(primaryCode);

				if (key != null && (!key.sticky || !key.on)) {
					mCore.keyDown(primaryCode);
				}
			}
		}

		@Override public void onRelease(int primaryCode) {
			if (primaryCode >= 0) {
				Keyboard.Key key = getKey(primaryCode);

				if (key != null && (!key.sticky || !key.on)) {
					mCore.keyUp(primaryCode);
				}
			}
		}

		@Override public void onText(CharSequence text) {
		}

		@Override public void swipeDown() {
		}

		@Override public void swipeLeft() {
		}

		@Override public void swipeRight() {
		}

		@Override public void swipeUp() {
		}

		private Keyboard.Key getKey(int primaryCode) {
			List<Keyboard.Key> keys = mKeyboardView.getKeyboard().getKeys();
			for (Keyboard.Key key : keys) {
				if (key.codes.length > 0 && key.codes[0] == primaryCode) {
					return key;
				}
			}
			return null;
		}

		public void setShifted(boolean shiftState) {
			Keyboard.Key shiftKey = getKey(KEYCODE_MAC_SHIFT);
			if (shiftKey != null) {
				shiftKey.on = shiftState;
			}
		}
	};

	private void askForPermissions() {
		if (ContextCompat.checkSelfPermission(MiniVMac.this,
				Manifest.permission.WRITE_EXTERNAL_STORAGE)
				!= PackageManager.PERMISSION_GRANTED) {

			// Should we show an explanation?
			if (ActivityCompat.shouldShowRequestPermissionRationale(MiniVMac.this,
					Manifest.permission.WRITE_EXTERNAL_STORAGE)) {
				Snackbar.make(mLayout, getString(R.string.permissionsExplain),
						Snackbar.LENGTH_INDEFINITE).setAction("OK", new View.OnClickListener() {
					@Override
					public void onClick(View view) {
						// Request the permission
						onActivity = true;
						ActivityCompat.requestPermissions(MiniVMac.this,
								new String[]{Manifest.permission.WRITE_EXTERNAL_STORAGE},
								MY_PERMISSIONS_REQUEST_WRITE_EXTERNAL_STORAGE);
					}
				}).show();
			} else {
				// No explanation needed, we can request the permission.
				onActivity = true;
				ActivityCompat.requestPermissions(MiniVMac.this,
						new String[]{Manifest.permission.WRITE_EXTERNAL_STORAGE},
						MY_PERMISSIONS_REQUEST_WRITE_EXTERNAL_STORAGE);
			}
		} else {
			hasPermission = true;
			initEmulator();
		}
	}

	@Override
	public void onRequestPermissionsResult(int requestCode,
										   @NonNull String permissions[],
										   @NonNull int[] grantResults) {
		onActivity = false;
		switch (requestCode) {
			case MY_PERMISSIONS_REQUEST_WRITE_EXTERNAL_STORAGE: {
				// If request is cancelled, the result arrays are empty.
				if (grantResults.length > 0
						&& grantResults[0] == PackageManager.PERMISSION_GRANTED) {
					hasPermission = true;
					initEmulator();
				} else {
					Utils.showAlert(this, getString(R.string.errNoPermissions), true);
				}
				return;
			}
			default:
				super.onRequestPermissionsResult(requestCode, permissions, grantResults);
		}
	}

	private void updateByPrefs() {
		SharedPreferences sharedPref = PreferenceManager.getDefaultSharedPreferences(this);
		Boolean scalePref = sharedPref.getBoolean(SettingsActivity.KEY_PREF_SCALE, true);
		Boolean scrollPref = sharedPref.getBoolean(SettingsActivity.KEY_PREF_SCROLL, false);
		mScreenView.setScaled(scalePref);
		mScreenView.setScroll(scrollPref);
	}

	@Override
    protected void onPause () {
		if (mCore != null) {
			mCore.pauseEmulation();
		}

    	super.onPause();

    	if (mCore != null && !mCore.hasDisksInserted() && !onActivity) {
    		mCore.requestMacOff();
    	}
    }

	@Override
	protected void onResume() {
		super.onResume();

		if (mCore != null) {
			mCore.resumeEmulation();
		}
	}

	@Override
	public boolean onKeyDown (int keyCode, @NonNull KeyEvent event) {
		if (mScreenView.isScroll()) {
			switch(keyCode) {
			case KeyEvent.KEYCODE_DPAD_UP:
			case KeyEvent.KEYCODE_DPAD_DOWN:
			case KeyEvent.KEYCODE_DPAD_LEFT:
			case KeyEvent.KEYCODE_DPAD_RIGHT:
				mScreenView.scrollScreen(keyCode, 8);
				return true;
			}
		}
		
		int macKey = translateKeyCode(keyCode);
		if (macKey >= 0) {
			mCore.keyDown(macKey);
			return true;
		}
		
		if(keyCode == KeyEvent.KEYCODE_BACK) {
			// letting this through will break on next launch
			// since it will create a new instance instead of resuming
			// this one. I thought singleInstance was for that.
			
			// Close the keyboard, if it is open
			toggleKeyboard();
			
			return true;
		}
		
		return super.onKeyDown(keyCode, event);
	}
	
	@Override
	public boolean onKeyUp (int keyCode, @NonNull KeyEvent event) {
		int macKey = translateKeyCode(keyCode);
		if (macKey >= 0) {
			mCore.keyUp(macKey);
			return true;
		}
		return false;
	}

	@Override
	public boolean onTrackballEvent (MotionEvent event) {
		if (event.getX() > 0) mScreenView.scrollScreen(KeyEvent.KEYCODE_DPAD_RIGHT, (int)(TRACKBALL_SENSITIVITY*event.getX()));
		else if (event.getX() < 0) mScreenView.scrollScreen(KeyEvent.KEYCODE_DPAD_LEFT, (int)-(TRACKBALL_SENSITIVITY*event.getX()));
		if (event.getY() > 0) mScreenView.scrollScreen(KeyEvent.KEYCODE_DPAD_DOWN, (int)(TRACKBALL_SENSITIVITY*event.getY()));
		else if (event.getY() < 0) mScreenView.scrollScreen(KeyEvent.KEYCODE_DPAD_UP, (int)-(TRACKBALL_SENSITIVITY*event.getY()));

		return true;
	}

	@Override
	public boolean onTouchEvent (@NonNull MotionEvent event) {
		if (this.mGestureDetector.onTouchEvent(event)) {
			return true;
		}
		return super.onTouchEvent(event);
	}


	public int translateKeyCode (int keyCode) {
		if (keyCode < 0 || keyCode >= keycodeTranslationTable.length) return -1;
		return keycodeTranslationTable[keyCode];
	}

	@Override
	public void onConfigurationChanged(Configuration newConfig) {
		super.onConfigurationChanged(newConfig);
		isLandscape = (newConfig.orientation == Configuration.ORIENTATION_LANDSCAPE);
		toggleFullscreen(isLandscape);
		initKeyboard();
	}

	@Override
	public void onWindowFocusChanged(boolean hasFocus) {
		super.onWindowFocusChanged(hasFocus);
		if (hasFocus) {
			hideSystemUI();
		}
	}

	private void hideSystemUI() {
		// Enables regular immersive mode.
		if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.KITKAT) {
			View decorView = getWindow().getDecorView();
			decorView.setSystemUiVisibility(
					View.SYSTEM_UI_FLAG_IMMERSIVE
							// Set the content to appear under the system bars so that the
							// content doesn't resize when the system bars hide and show.
							| View.SYSTEM_UI_FLAG_LAYOUT_STABLE
							| View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
							| View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
							// Hide the nav bar and status bar
							| View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
							| View.SYSTEM_UI_FLAG_FULLSCREEN);
			mUIVisible = false;
		}
	}

	// Shows the system bars by removing all the flags
	// except for the ones that make the content appear under the system bars.
	private void showSystemUI() {
		if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.KITKAT) {
			View decorView = getWindow().getDecorView();
			decorView.setSystemUiVisibility(
					View.SYSTEM_UI_FLAG_LAYOUT_STABLE
							| View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
							| View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN);
			mUIVisible = true;
		}
	}

	private void toggleFullscreen(Boolean isFullscreen) {
		if(isFullscreen) {
			getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
		} else {
			getWindow().clearFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);
		}
	}

	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu items for use in the action bar
		MenuInflater inflater = getMenuInflater();
		inflater.inflate(R.menu.minivmac_actions, menu);
		return super.onCreateOptionsMenu(menu);
	}
	
	public boolean onPrepareOptionsMenu(Menu menu) {
		SubMenu dm = menu.findItem(R.id.action_insert_disk).getSubMenu();
		dm.clear();
		dm.setHeaderIcon(R.drawable.disk_floppy_color);
		// add create disk
		dm.add(0, R.id.action_create_disk, 0, R.string.menu_create_disk);
		dm.add(0, R.id.action_select_disk, 1, R.string.menu_select_disk);
		// add disks
		if (hasPermission) {
			File[] disks = getAvailableDisks();
			for (int i = 0; disks != null && i < disks.length; i++) {
				String diskName = disks[i].getName();
				MenuItem m = dm.add(R.id.action_insert_disk, diskName.hashCode(), i+2, diskName.substring(0, diskName.lastIndexOf(".")));
				m.setEnabled(mCore == null || !mCore.isDiskInserted(disks[i]));
			}
		}
		return super.onPrepareOptionsMenu(menu);
	}
	
	public boolean onOptionsItemSelected (MenuItem item) {
		if (item.getGroupId() == R.id.action_insert_disk) {
			File[] disks = getAvailableDisks();
            for (File disk : disks) {
                if (disk.getName().hashCode() == item.getItemId()) {
                    mCore.insertDisk(disk);
                    return true;
                }
            }
			// disk not found
			return true;
		}
		switch(item.getItemId()) {
		case R.id.action_keyboard:
			toggleKeyboard();
			break;
		case R.id.action_create_disk:
			showCreateDisk();
			break;
		case R.id.action_select_disk:
			showSelectDisk();
			break;
		case R.id.action_settings:
			showSettings();
			break;
		}
		return true;
	}

	private void showAbout() {
		Dialog dialog = new AboutDialog(this);
		dialog.show();
	}
	
	public void showCreateDisk() {
		onActivity = true;
		Intent i = new Intent(MiniVMac.this, CreateDisk.class);
		startActivityForResult(i, ACTIVITY_CREATE_DISK);
	}

	public void showSelectDisk() {
		if (FileManager.getInstance().getDataDir() == null) return;

		onActivity = true;
		Intent i = new Intent(this, FilePickerActivity.class);
		i.putExtra(FilePickerActivity.EXTRA_ALLOW_MULTIPLE, false);
		i.putExtra(FilePickerActivity.EXTRA_ALLOW_CREATE_DIR, false);
		i.putExtra(FilePickerActivity.EXTRA_MODE, FilePickerActivity.MODE_FILE);
		i.putExtra(FilePickerActivity.EXTRA_START_PATH, FileManager.getInstance().getDataDir().getAbsolutePath());

		startActivityForResult(i, ACTIVITY_SELECT_DISK);
	}
	
	public void showSettings() {
		onActivity = true;
		Intent i = new Intent(MiniVMac.this, SettingsActivity.class);
		startActivityForResult(i, ACTIVITY_SETTINGS);
	}
	
	public void toggleKeyboard() {
		View kbd = findViewById(R.id.keyboard);
		if (kbd.getVisibility() == View.VISIBLE) {
			kbd.setVisibility(View.GONE);
			kbd.setEnabled(false);
		} else {
			kbd.setVisibility(View.VISIBLE);
			kbd.setEnabled(true);
		}
	}

	public void showSelectFolder() {
		if (FileManager.getInstance().getDataDir() == null) return;
		
		onActivity = true;
		Intent i = new Intent(this, FilePickerActivity.class);
		i.putExtra(FilePickerActivity.EXTRA_ALLOW_MULTIPLE, false);
		i.putExtra(FilePickerActivity.EXTRA_ALLOW_CREATE_DIR, true);
		i.putExtra(FilePickerActivity.EXTRA_MODE, FilePickerActivity.MODE_DIR);
		i.putExtra(FilePickerActivity.EXTRA_START_PATH, FileManager.getInstance().getDataDir().getAbsolutePath());

		startActivityForResult(i, ACTIVITY_SELECT_FOLDER);
	}

	public void reset() {
		mCore.wantMacReset();
	}
	
	public void interrupt() {
		mCore.wantMacInterrupt();
	}
	
	public File[] getAvailableDisks () {
		return FileManager.getInstance().getDataDir().listFiles(new FileFilter() {
			public boolean accept(File pathname) {
				if (!pathname.isFile()) return false;
				if (pathname.isDirectory()) return false;
				String ext = pathname.getName().substring(1 + pathname.getName().lastIndexOf("."));
				for (String diskExtension : diskExtensions) {
					if (diskExtension.equals(ext)) return true;
				}
				return false;
			}
		});
	}

	@Override
	protected void onActivityResult(int requestCode, int resultCode, Intent data)
	{
		super.onActivityResult(requestCode, resultCode, data);
		
		onActivity = false;
		
		if (requestCode == ACTIVITY_CREATE_DISK && resultCode == RESULT_OK)
		{
			Bundle extras = data.getExtras();
	    	
	    	if (extras != null) {
	    		if (extras.containsKey("diskPath")) {
	    			String diskPath = extras.getString("diskPath");
	    			mCore.insertDisk(diskPath);
	    		}
	    	}
		}
		else if (requestCode == ACTIVITY_SELECT_DISK && resultCode == RESULT_OK)
		{
			Uri uri = data.getData();
			File file = com.nononsenseapps.filepicker.Utils.getFileForUri(uri);
			mCore.insertDisk(file.getPath());
		}
		else if (requestCode == ACTIVITY_SELECT_FOLDER)
		{
			if (resultCode == RESULT_OK && mTempSize > 0) {
				Uri uri =  data.getData();
				File file = com.nononsenseapps.filepicker.Utils.getFileForUri(uri);
				mCore.makeNewDisk(mTempSize, file.getPath(), mTempFilename);
			}
			mTempSize = 0;
			mTempFilename = "";
			Core.notifyDiskCreated();
		}
		else if (requestCode == ACTIVITY_SETTINGS)
		{
			switch (resultCode) {
				case SettingsActivity.RESULT_RESET:
					reset();
					break;
				case SettingsActivity.RESULT_INTERRUPT:
					interrupt();
					break;
				case SettingsActivity.RESULT_ABOUT:
					showAbout();
					break;
			}
			updateByPrefs();
		}
	}

	class SingleTapGestureListener extends GestureDetector.SimpleOnGestureListener {
		@Override
		public boolean onDown(MotionEvent event) {
			return true;
		}

		@Override
		public boolean onSingleTapUp(MotionEvent e) {
			if (mUIVisible) {
				hideSystemUI();
			} else {
				showSystemUI();
			}
			return true;
		}
	}
}
