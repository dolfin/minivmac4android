package name.osher.gil.minivmac;

import java.io.File;
import java.io.FileFilter;
import java.io.FileInputStream;
import java.nio.ByteBuffer;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.content.DialogInterface;
import android.content.DialogInterface.OnClickListener;
import android.content.Intent;
import android.content.pm.ActivityInfo;
import android.os.Bundle;
import android.os.Environment;
import android.text.SpannableString;
import android.text.method.LinkMovementMethod;
import android.text.util.Linkify;
import android.view.KeyEvent;
import android.view.Menu;
import android.view.MenuItem;
import android.view.MotionEvent;
import android.view.SubMenu;
import android.view.View;
import android.view.WindowManager;
import android.widget.TextView;

public class MiniVMac extends Activity {
	private static MiniVMac instance;
	private final static int[] keycodeTranslationTable = {-1, -1, -1, -1, -1, -1, -1, 0x1D, 0x12, 0x13, 0x14, 0x15, 0x17, 0x16, 0x1A, 0x1C, 0x19, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 0x00, 0x0B, 0x08, 0x02, 0x0E, 0x03, 0x05, 0x04, 0x22, 0x26, 0x28, 0x25, 0x2E, 0x2D, 0x1F, 0x23, 0x0C, 0x0F, 0x01, 0x11, 0x20, 0x09, 0x0D, 0x07, 0x10, 0x06, 0x2B, 0x2F, 0x37, 0x37, 0x38, 0x38, 0x30, 0x31, 0x3A, -1, -1, 0x24, 0x33, 0x32, 0x1B, 0x18, 0x21, 0x1E, 0x2A, 0x29, 0x27, 0x2C, 0x37, 0x3A, -1, -1, 0x45, -1, -1, 0x3A, -1, -1, -1, -1, -1, -1, -1};
	private final static String[] diskExtensions = {"DSK", "dsk", "img", "IMG"};
	private final static int MENU_ABOUT = 1;
	private final static int MENU_INSERTDISK = 2;
	private final static int MENU_SETTINGS = 3;
	private final static int MENU_KEYBOARD = 4;
	private final static int MENU_RESET = 5;
	private final static int MENU_INTERRUPT = 6;
	private final static int MENU_SCALE = 7;
	private final static int MENU_CREATEDISK = 8;
	private final static int ACTIVITY_CREATE_DISK = 200;
	private static int TRACKBALL_SENSITIVITY = 8;
	private File dataDir;
	private ScreenView screenView;
	private Boolean onActivity = false;
	
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
    public void onCreate(Bundle savedInstanceState) {
    	super.onCreate(savedInstanceState);
    	
    	if (instance != null) throw new RuntimeException("There should be one instance to rule them all.");
    	
        instance = this;
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
        
        // set screen orientation
        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);

        setContentView(R.layout.screen);
        screenView = (ScreenView)findViewById(R.id.screen);
        
        onActivity = false;
        
        // find data directory
        dataDir = new File(Environment.getExternalStorageDirectory(), "minivmac");
        if (!(dataDir.isDirectory() && dataDir.canRead())) {
        	showAlert(getString(R.string.errNoDataDir) + " " + dataDir.getPath()
        			+ "\n\n" + getString(R.string.errNoDataDir2), true);
        	return;
        }

        // load ROM
        File romFile = new File(dataDir, "vMac.ROM");
        ByteBuffer rom = ByteBuffer.allocateDirect((int)romFile.length());
        try {
        	FileInputStream romReader = new FileInputStream(romFile);
        	romReader.getChannel().read(rom);
        } catch (Exception x) {
        	showAlert(getString(R.string.errNoROM) + " " + romFile.getPath()
        			+ "\n\n" + getString(R.string.errNoROM2), true);	
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
    
    public void onPause () {
    	Core.pauseEmulation();
    	super.onPause();
    	if (!Core.hasDisksInserted() && !onActivity) System.exit(0);
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
	public boolean onKeyDown (int keyCode, KeyEvent event) {
		int macKey = translateKeyCode(keyCode);
		if (macKey >= 0) {
			Core.setKeyDown(macKey);
			return true;
		}
		switch(keyCode) {
		case KeyEvent.KEYCODE_DPAD_CENTER:
			screenView.setScaled(!screenView.isScaled());
			return true;
		case KeyEvent.KEYCODE_DPAD_UP:
		case KeyEvent.KEYCODE_DPAD_DOWN:
		case KeyEvent.KEYCODE_DPAD_LEFT:
		case KeyEvent.KEYCODE_DPAD_RIGHT:
			screenView.scrollScreen(keyCode, 8);
			return true;
		case KeyEvent.KEYCODE_BACK:
			// letting this through will break on next launch
			// since it will create a new instance instead of resuming
			// this one. I thought singleInstance was for that.
			return true;
		}
		return super.onKeyDown(keyCode, event);
	}
	
	@Override
	public boolean onKeyUp (int keyCode, KeyEvent event) {
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
	
	public boolean onCreateOptionsMenu(Menu menu) {
		menu.addSubMenu(0, MENU_INSERTDISK, 0, R.string.menu_insert_disk).setIcon(R.drawable.disk_floppy_color);
		//menu.add(0, MENU_SETTINGS, 0, R.string.menu_settings);
		menu.add(0, MENU_KEYBOARD, 0, R.string.menu_keyboard).setIcon(R.drawable.keyboard);
		menu.add(0, MENU_SCALE, 0, R.string.menu_scale).setIcon(R.drawable.magnifier);
		menu.add(0, MENU_RESET, 0, R.string.menu_reset).setIcon(R.drawable.ps_reset);
		menu.add(0, MENU_INTERRUPT, 0, R.string.menu_interrupt).setIcon(R.drawable.ps_interrupt);
		menu.add(0, MENU_ABOUT, 0, R.string.menu_about).setIcon(R.drawable.icon_classic);
		return true;
	}
	
	public boolean onPrepareOptionsMenu(Menu menu) {
		SubMenu dm = menu.findItem(MENU_INSERTDISK).getSubMenu();
		dm.clear();
		dm.setHeaderIcon(R.drawable.disk_floppy_color);
		// add create disk
		dm.add(0, MENU_CREATEDISK, 0, R.string.menu_create_disk);
		// add disks
		File[] disks = getAvailableDisks();
		for(int i=0; i < disks.length; i++) {
			String diskName = disks[i].getName();
			MenuItem m = dm.add(MENU_INSERTDISK, diskName.hashCode(), 0, diskName.substring(0, diskName.lastIndexOf(".")));
			m.setEnabled(!Core.isDiskInserted(disks[i]));
		}
		return true;
	}
	
	public boolean onOptionsItemSelected (MenuItem item) {
		if (item.getGroupId() == MENU_INSERTDISK) {
			File[] disks = getAvailableDisks();
			for(int i=0; i < disks.length; i++) {
				if (disks[i].getName().hashCode() == item.getItemId()) {
					Core.insertDisk(disks[i]);
					return true;
				}
			}
			// disk not found
			return true;
		}
		switch(item.getItemId()) {
		case MENU_ABOUT:
			showAbout();
			break;
		case MENU_KEYBOARD:
			toggleKeyboard();
			break;
		case MENU_RESET:
			reset();
			break;
		case MENU_INTERRUPT:
			interrupt();
			break;
		case MENU_SCALE:
			screenView.setScaled(!screenView.isScaled());
			break;
		case MENU_CREATEDISK:
			showCreateDisk();
			break;
		case MENU_SETTINGS:
			
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
	
	public void toggleKeyboard() {
		View kbd = findViewById(R.id.keyboard);
		if (kbd.getVisibility() == View.VISIBLE) {
			kbd.setVisibility(View.INVISIBLE);
		} else {
			kbd.setVisibility(View.VISIBLE);
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
				for(int i=0; i < diskExtensions.length; i++) {
					if (diskExtensions[i].equals(ext)) return true;
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
	}
	
}

