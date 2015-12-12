package name.osher.gil.minivmac;

import java.io.File;
import java.io.FileFilter;
import java.io.FileInputStream;
import java.nio.ByteBuffer;
import java.util.List;

import android.app.AlertDialog;
import android.app.Dialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.DialogInterface.OnClickListener;
import android.content.SharedPreferences;
import android.content.pm.ActivityInfo;
import android.content.res.Configuration;
import android.inputmethodservice.Keyboard;
import android.inputmethodservice.KeyboardView;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.preference.PreferenceManager;
import android.support.annotation.NonNull;
import android.support.v7.app.ActionBar;
import android.support.v7.app.AppCompatActivity;
import android.text.SpannableString;
import android.text.method.LinkMovementMethod;
import android.text.util.Linkify;
import android.view.KeyEvent;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.MotionEvent;
import android.view.SubMenu;
import android.view.View;
import android.view.WindowManager;
import android.widget.TextView;

public class MiniVMac extends AppCompatActivity {
	private static MiniVMac instance;
	private final static int[] keycodeTranslationTable = {-1, -1, -1, -1, -1, -1, -1, 0x1D, 0x12, 0x13, 0x14, 0x15, 0x17, 0x16, 0x1A, 0x1C, 0x19, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 0x00, 0x0B, 0x08, 0x02, 0x0E, 0x03, 0x05, 0x04, 0x22, 0x26, 0x28, 0x25, 0x2E, 0x2D, 0x1F, 0x23, 0x0C, 0x0F, 0x01, 0x11, 0x20, 0x09, 0x0D, 0x07, 0x10, 0x06, 0x2B, 0x2F, 0x37, 0x37, 0x38, 0x38, 0x30, 0x31, 0x3A, -1, -1, 0x24, 0x33, 0x32, 0x1B, 0x18, 0x21, 0x1E, 0x2A, 0x29, 0x27, 0x2C, 0x37, 0x3A, -1, -1, 0x45, -1, -1, 0x3A, -1, -1, -1, -1, -1, -1, -1};
	private final static String[] diskExtensions = {"DSK", "dsk", "img", "IMG"};
	private final static int ACTIVITY_CREATE_DISK = 200;
	private final static int ACTIVITY_SETTINGS = 201;
	private final static int TRACKBALL_SENSITIVITY = 8;
	private static final int KEYCODE_MAC_SHIFT = 56;

	private File dataDir;
	private ScreenView screenView;
	private Boolean onActivity = false;
	private Boolean isLandscape = false;

	private KeyboardView mKeyboardView;
	private Keyboard mQwertyKeyboard;
	private Keyboard mSymbolsKeyboard;
	private Keyboard mSymbolsShiftedKeyboard;
	
	public static MiniVMac getInstance() {
		return instance;
	}
	
	public static File getDataDir() {
		return instance.dataDir;
	}
	
	public static File getDataFile(String name) {
		return new File(instance.dataDir, name);
	}
	
	public static void updateScreen(int[] update) {
		instance.screenView.updateScreen(update);
	}
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		if (instance != null) throw new RuntimeException("There should be one instance to rule them all.");
    	
        instance = this;
        
        // set screen orientation
        if (Build.VERSION.SDK_INT <= 10)
        {
        	setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
        }

        setContentView(R.layout.activity_mini_vmac);
        screenView = (ScreenView)findViewById(R.id.screen);
		isLandscape = (getResources().getConfiguration().orientation == Configuration.ORIENTATION_LANDSCAPE);
		toggleFullscreen(isLandscape);
		initKeyboard();

        onActivity = false;
        
        updateByPrefs();
        
        // find data directory
        dataDir = new File(Environment.getExternalStorageDirectory(), "minivmac");
        if (!(dataDir.isDirectory() && dataDir.canRead())) {
        	showAlert(String.format(getString(R.string.errNoDataDir), dataDir.getPath(),
                    getString(R.string.romFileName)), true);
        	return;
        }

        // load ROM
        String romFileName = getString(R.string.romFileName);
        File romFile = new File(dataDir, romFileName);
        ByteBuffer rom = ByteBuffer.allocateDirect((int)romFile.length());
        try {
        	FileInputStream romReader = new FileInputStream(romFile);
        	romReader.getChannel().read(rom);
        	romReader.close();
        } catch (Exception x) {
        	showAlert(String.format(getString(R.string.errNoROM), romFile.getPath(),
                    getString(R.string.romFileName)), true);
        	return;
        }
        
        // sound
        Core.MySound_Init();

        // initialize emulation
        if (!Core.init(rom)) {
        	showAlert(getString(R.string.errInitEmu), true);
        	return;
        }

        Core.startEmulation();
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

				if (!key.sticky || !key.on) {
					Core.setKeyDown(primaryCode);
				}
			}
		}

		@Override public void onRelease(int primaryCode) {
			if (primaryCode >= 0) {
				Keyboard.Key key = getKey(primaryCode);

				if (!key.sticky || !key.on) {
					Core.setKeyUp(primaryCode);
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

	private void updateByPrefs() {
		SharedPreferences sharedPref = PreferenceManager.getDefaultSharedPreferences(this);
		Boolean scalePref = sharedPref.getBoolean(SettingsActivity.KEY_PREF_SCALE, true);
		Boolean scrollPref = sharedPref.getBoolean(SettingsActivity.KEY_PREF_SCROLL, false);
		screenView.setScaled(scalePref);
		screenView.setScroll(scrollPref);
	}
    
    public void onPause () {
    	Core.pauseEmulation();
    	super.onPause();
    	if (!Core.hasDisksInserted() && !onActivity) {
    		Core.uninit();
    		System.exit(0);
    	}
    }
    
    public void onResume () {
    	super.onResume();
    	
    	Core.resumeEmulation();
    }

	public static void showAlert(String msg, boolean end) {
		final SpannableString s = new SpannableString(msg);
	    Linkify.addLinks(s, Linkify.ALL);
	    
		AlertDialog.Builder alert = new AlertDialog.Builder(instance);
		alert.setMessage(s);
		alert.setCancelable(false);
		if (end) {
			alert.setNegativeButton(R.string.btn_quit, new OnClickListener() { 
				public void onClick(DialogInterface di, int i) {
					System.exit(0);
				}
			});
		} else {
			alert.setNeutralButton(R.string.btn_ok, null);
		}
		
		AlertDialog d = alert.create();
		d.show();
		
	    // Make the textview clickable. Must be called after show()
	    ((TextView)d.findViewById(android.R.id.message)).setMovementMethod(LinkMovementMethod.getInstance());
	}
	
	public static void showWarnMessage(String title, String msg, boolean end) {
		AlertDialog.Builder alert = new AlertDialog.Builder(instance);
		alert.setTitle(title);
		alert.setMessage(msg);
		alert.setNegativeButton(R.string.btn_quit, new OnClickListener() {
			public void onClick(DialogInterface di, int i) {
				System.exit(0);
			}
		});
		if (!end) {
			alert.setPositiveButton(R.string.btn_continue, new OnClickListener() {
				public void onClick(DialogInterface di, int i) {
					Core.resumeEmulation();
				}
			});
		}
		
		Core.pauseEmulation();
		alert.show();
	}
	
	@Override
	public boolean onKeyDown (int keyCode, @NonNull KeyEvent event) {
		if (screenView.isScroll()) {
			switch(keyCode) {
			case KeyEvent.KEYCODE_DPAD_UP:
			case KeyEvent.KEYCODE_DPAD_DOWN:
			case KeyEvent.KEYCODE_DPAD_LEFT:
			case KeyEvent.KEYCODE_DPAD_RIGHT:
				screenView.scrollScreen(keyCode, 8);
				return true;
			}
		}
		
		int macKey = translateKeyCode(keyCode);
		if (macKey >= 0) {
			Core.setKeyDown(macKey);
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
			Core.setKeyUp(macKey);
			return true;
		}
		return false;
	}

	public boolean onTrackballEvent (MotionEvent event) {
		if (event.getX() > 0) screenView.scrollScreen(KeyEvent.KEYCODE_DPAD_RIGHT, (int)(TRACKBALL_SENSITIVITY*event.getX()));
		else if (event.getX() < 0) screenView.scrollScreen(KeyEvent.KEYCODE_DPAD_LEFT, (int)-(TRACKBALL_SENSITIVITY*event.getX()));
		if (event.getY() > 0) screenView.scrollScreen(KeyEvent.KEYCODE_DPAD_DOWN, (int)(TRACKBALL_SENSITIVITY*event.getY()));
		else if (event.getY() < 0) screenView.scrollScreen(KeyEvent.KEYCODE_DPAD_UP, (int)-(TRACKBALL_SENSITIVITY*event.getY()));

		return true;
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

	private void toggleFullscreen(Boolean isFullscreen) {
		ActionBar actionBar = getSupportActionBar();
		if(isFullscreen) {
			getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
			actionBar.hide();
		} else {
			getWindow().clearFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);
			actionBar.show();
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
		// add disks
		File[] disks = getAvailableDisks();
		for(int i=0; disks != null && i < disks.length; i++) {
			String diskName = disks[i].getName();
			MenuItem m = dm.add(R.id.action_insert_disk, diskName.hashCode(), 0, diskName.substring(0, diskName.lastIndexOf(".")));
			m.setEnabled(!Core.isDiskInserted(disks[i]));
		}
		return true;
	}
	
	public boolean onOptionsItemSelected (MenuItem item) {
		if (item.getGroupId() == R.id.action_insert_disk) {
			File[] disks = getAvailableDisks();
            for (File disk : disks) {
                if (disk.getName().hashCode() == item.getItemId()) {
                    Core.insertDisk(disk);
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
		case R.id.action_settings:
			showSettings();
			break;
		}
		return true;
	}
	
	public void showAbout() {
		Dialog dialog = new AboutDialog(this);
        dialog.show();
	}
	
	public void showCreateDisk() {
		onActivity = true;
		Intent i = new Intent(MiniVMac.this, CreateDisk.class);
		startActivityForResult(i, ACTIVITY_CREATE_DISK);
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
	
	public void reset() {
		Core.setWantMacReset();
	}
	
	public void interrupt() {
		Core.setWantMacInterrupt();
	}
	
	public File[] getAvailableDisks () {
		return dataDir.listFiles(new FileFilter(){
			public boolean accept (File pathname) {
				if (!pathname.isFile()) return false;
				if (pathname.isDirectory()) return false;
				String ext = pathname.getName().substring(1+pathname.getName().lastIndexOf("."));
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
	    			Core.insertDisk(diskPath);
	    		}
	    	}
		}
		else if (requestCode == ACTIVITY_SETTINGS)
		{
			updateByPrefs();
		}
	}
}
