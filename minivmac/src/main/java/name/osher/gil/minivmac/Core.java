package name.osher.gil.minivmac;

import java.io.File;
import java.io.RandomAccessFile;
import java.nio.ByteBuffer;

import android.content.DialogInterface;
import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioTrack;
import android.util.Log;

import androidx.annotation.StringRes;

public class Core {
	private static final String TAG = "minivmac.Core";
	
	private int numInsertedDisks = 0;
	@SuppressWarnings("unused") private String[] diskPath;
	@SuppressWarnings("unused") private RandomAccessFile[] diskFile;
	private final boolean initOk = false;

	private OnInitScreenListener mOnInitScreenListener;
	private OnUpdateScreenListener mOnUpdateScreenListener;
	private OnDiskEventListener mOnDiskEventListener;
	private OnAlertListener mOnAlertListener;

	private static Boolean mIsInitialized = false;

	public Core() {
	}

	public void setOnInitScreenListener(OnInitScreenListener listener) {
		mOnInitScreenListener = listener;
	}

	public void setOnUpdateScreenListener(OnUpdateScreenListener listener) {
		mOnUpdateScreenListener = listener;
	}

	public void setOnDiskEventListener(OnDiskEventListener listener) {
		mOnDiskEventListener = listener;
	}

	public void setOnAlertListener(OnAlertListener listener) {
		mOnAlertListener = listener;
	}
	
	// initialization
	private native static boolean init(Core core, ByteBuffer rom);
	
	// emulation
	private native static void _resumeEmulation();
	private native static void _pauseEmulation();
	private native static boolean isPaused();
	private native static void _setSpeed(int value);
	private native static int _getSpeed();
	private native static void setWantMacReset();
	private native static void setWantMacInterrupt();
	private native static void setRequestMacOff();

	public static Boolean isInitialized() {
		return mIsInitialized;
	}

	public Boolean initEmulation(String moduleName, ByteBuffer rom) {
		System.loadLibrary(moduleName);
		mIsInitialized = true;
		return init(this, rom);
	}

	public void wantMacReset() {
		if (!initOk) return;
		setWantMacReset();
	}

	public void wantMacInterrupt() {
		if (!initOk) return;
		setWantMacInterrupt();
	}

	public void requestMacOff() {
		if (!initOk) return;
		setRequestMacOff();
	}
	
	public void resumeEmulation() {
		if (!initOk) return;
		if (!isPaused()) return;
		_resumeEmulation();
	}
	
	public void pauseEmulation() {
		if (!initOk) return;
		if (isPaused()) return;
		_pauseEmulation();
	}

	public static void setSpeed(int value) {
		if (!mIsInitialized) return;
		_setSpeed(value);
	}

	public static int getSpeed() {
		if (!mIsInitialized) return 0;
		return _getSpeed();
	}

	public boolean initScreen() {
		final int width = getScreenWidth();
		final int height = getScreenHeight();
		if (mOnInitScreenListener != null) {
			mOnInitScreenListener.onInitScreen(width, height);
		}
		return true;
	}

	public void updateScreen(final int top, final int left, final int bottom, final int right) {
		final int [] screenUpdate = getScreenUpdate();
		if (mOnUpdateScreenListener != null && screenUpdate != null) {
			mOnUpdateScreenListener.onUpdateScreen(screenUpdate, top, left, bottom, right);
		}
	}
	
	// mouse
	@SuppressWarnings("unused") private native static void moveMouse(int dx, int dy);
	private native static void setMousePos(int x, int y);
	private native static void setMouseButton(boolean down);
	@SuppressWarnings("unused") private native static int getMouseX();
	@SuppressWarnings("unused") private native static int getMouseY();
	@SuppressWarnings("unused") private native static boolean getMouseButton();

	public void setMousePosition(int x, int y) {
		setMousePos(x, y);
	}

	public void setMouseBtn(Boolean down) {
		setMouseButton(down);
	}
	
	// keyboard
	private native static void setKeyDown(int scancode);
	private native static void setKeyUp(int scancode);

	public void keyDown(int scancode) {
		setKeyDown(scancode);
	}

	public void keyUp(int scancode) {
		setKeyUp(scancode);
	}
	
	// screen
	private native static int screenWidth();
	private native static int screenHeight();
	private native static int[] getScreenUpdate();

	public int getScreenWidth() {
		return screenWidth();
	}

	public int getScreenHeight() {
		return screenHeight();
	}
	
	// sound
	private native static void MySound_Start0();
	
	private static AudioTrack mAudioTrack;

    private static final int SOUND_SAMPLERATE = 22255;
	private static final int kLn2SoundBuffers = 4;
	private static final int kLnOneBuffLen = 9;
	private static final int kLnAllBuffLen = (kLn2SoundBuffers + kLnOneBuffLen);
	private static final int kAllBuffLen = (1 << kLnAllBuffLen);

	public boolean MySound_Init() {
        try {
			mAudioTrack = new AudioTrack(AudioManager.STREAM_MUSIC, SOUND_SAMPLERATE, AudioFormat.CHANNEL_OUT_MONO, AudioFormat.ENCODING_PCM_8BIT, kAllBuffLen, AudioTrack.MODE_STREAM);

			mAudioTrack.pause();
			return true;
	    } catch (Throwable tr) {
	    	Log.e(TAG, "MySound_Init() can't init sound.", tr);
	    	return false;
	    }
	}
	
	public int playSound(byte[] buf) {
		if (mAudioTrack == null) return -1;
		return mAudioTrack.write(buf, 0, buf.length);
	}

	public void MySound_Start () {
		if (mAudioTrack != null) {
			MySound_Start0();
			mAudioTrack.play();
		}
	}

	public void MySound_Stop () {
		if (mAudioTrack != null) {
			try {
				mAudioTrack.stop();
			} catch (Throwable tr) {
				Log.e(TAG, "MySound_Stop() can't stop sound.", tr);
			}
		}
	}

	public void MySound_UnInit () {
		if (mAudioTrack != null) {
			AudioTrack deletedTrack = mAudioTrack;
			mAudioTrack = null;
			try {
				deletedTrack.pause();
			} catch (IllegalStateException e) {
				Log.w(TAG, "MySound_UnInit() can't pause sound.", e);
			}
			deletedTrack.flush();
			deletedTrack.release();
		}
	}
	
	// disks
	private native static void notifyDiskInserted(int driveNum, boolean locked);
	private native static void notifyDiskEjected(int driveNum);
	public native static void notifyDiskCreated();
	private native static int getFirstFreeDisk();
	@SuppressWarnings("unused") private native static int getNumDrives();
	
	// disk driver callbacks
	public int sonyTransfer(boolean isWrite, ByteBuffer buf, int driveNum, int start, int length) {
		if (diskFile[driveNum] == null) return -1;
		try {
			byte[] bytes = new byte[length];
			if (isWrite)
			{
				buf.rewind();
				buf.get(bytes);
				diskFile[driveNum].seek(start);
				diskFile[driveNum].write(bytes);
				return length;
			}
			else
			{
				diskFile[driveNum].seek(start);
				int actualLength = diskFile[driveNum].read(bytes);
				buf.rewind();
				buf.put(bytes);
				//Log.v(TAG, "Read " + actualLength + " bytes from drive number " + driveNum + ".");
				return actualLength;
			}
		} catch (Exception x) {
			Log.e(TAG, "Failed to read " + length + " bytes from drive number " + driveNum + ".");
			return -1;
		}
	}
	
	public int sonyGetSize(int driveNum) {
		if (diskFile[driveNum] == null) return 0;
		try {
			return (int)diskFile[driveNum].length();
		} catch (Exception x) {
			Log.e(TAG, "Failed to get disk size from drive number " + driveNum + ".");
			return -1;
		}
	}

	public int sonyEject(int driveNum, boolean deleteit) {
		if (diskFile[driveNum] == null) return -1;
		int ret;
		try {
			diskFile[driveNum].close();
			ret = 0;
		} catch (Exception x) {
			ret = -1;
		}

		String path = diskPath[driveNum];
		if (deleteit) {
			File file = new File(path);
			file.delete();
		}

		mOnDiskEventListener.onDiskEjected(diskPath[driveNum]);
		diskFile[driveNum] = null;
		diskPath[driveNum] = null;
		numInsertedDisks--;
		
		notifyDiskEjected(driveNum);
		return ret;
	}

	public String sonyGetName(int driveNum) {
		if (diskPath[driveNum] == null) return null;

		File file = new File(diskPath[driveNum]);
		return file.getName();
	}

	public int sonyMakeNewDisk(int size, String drivepath) {
		mOnDiskEventListener.onCreateDisk(size, drivepath);
		return 0;
	}

	public int makeNewDisk(int size, String path, String filename) {
		int ret = 0;

		if (!FileManager.getInstance().makeNewDisk(size, filename, path, null)) {
			ret = -1;
		} else {
			File disk = new File(path, filename);
			boolean isOk = insertDisk(disk);

			if (!isOk) {
				disk.delete();
			}
		}

		return ret;
	}
	
	public boolean isDiskInserted(File f) {
		if (numInsertedDisks == 0) return false;
		String path = f.getAbsolutePath();
		for(int i=0; i < diskFile.length; i++) {
			if (diskPath[i] == null) continue;
			if (path.equals(diskPath[i])) return true;
		}
		return false;
	}
	
	public boolean insertDisk(File f) {
		int driveNum = getFirstFreeDisk();
		// check for free drive
		if (driveNum == -1) {
			mOnAlertListener.onAlert(R.string.errTooManyDisks, false);
			return false;
		}
		
		// check for file
		if (!f.isFile()) return false;
		
		// check permissions
		String mode = "r";
		if (!f.canRead()) return false;
		if (f.canWrite()) mode = "rw";
		
		// open file
		try {
			diskFile[driveNum] = new RandomAccessFile(f, mode);
		} catch (Exception x) {
			diskFile[driveNum] = null;
			return false;
		}
		
		// insert disk
		notifyDiskInserted(driveNum, !f.canWrite());
		diskPath[driveNum] = f.getAbsolutePath();
		numInsertedDisks++;
		mOnDiskEventListener.onDiskInserted(f.getAbsolutePath());
		return true;
	}
	
	public boolean insertDisk(String path) {
		return insertDisk(new File(path));
	}

	public boolean sonyInsert2(String filename) {
        File f = FileManager.getInstance().getDisksFile(filename);
        return insertDisk(f);
    }
	
	public boolean hasDisksInserted() {
		return numInsertedDisks > 0;
	}

	// warnings
	public void warnMsg(final String shortMsg, final String longMsg) {
		pauseEmulation();

		mOnAlertListener.onAlert(shortMsg, longMsg, false, (di, i) -> resumeEmulation());
	}

	public interface OnInitScreenListener {
		void onInitScreen(int width, int height);
	}

	public interface OnUpdateScreenListener {
		void onUpdateScreen(int[] update, int top, int left, int bottom, int right);
	}

	public interface OnDiskEventListener {
		void onDiskInserted(String path);
		void onDiskEjected(String path);
		void onCreateDisk(int size, String filename);
	}

	public interface OnAlertListener {
		void onAlert(String title, String msg, boolean end, DialogInterface.OnClickListener listener);
		void onAlert(@StringRes int msgResId, boolean end);
	}
}
