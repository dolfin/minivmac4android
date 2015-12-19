package name.osher.gil.minivmac;

import java.io.File;
import java.io.RandomAccessFile;
import java.nio.ByteBuffer;

import android.content.Context;
import android.content.DialogInterface;
import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioTrack;
import android.os.Handler;
import android.util.Log;

public class Core {
	private static final String TAG = "name.osher.gil.minivmac.Core";
	
	private int numInsertedDisks = 0;
	private final int frameSkip = 4;
	private String[] diskPath;
	private RandomAccessFile[] diskFile;
	private Handler tickHandler = null;
	private Runnable runTickTask = null;
	private boolean initOk = false;

	private FileManager mFileManager;
	private Context mContext;
	private OnUpdateScreenListener mOnUpdateScreenListener;
	
	static {
		System.loadLibrary("mnvmcore");
	}
	
	public static void nativeCrashed()
	{
		// TODO: Add Error handeling here.
		Log.e(TAG, "Native crashed!");
	}

	public Core(Context context, FileManager fileManager) {
		mFileManager = fileManager;
		mContext = context;
	}

	public void setOnUpdateScreenListener(OnUpdateScreenListener listener) {
		mOnUpdateScreenListener = listener;
	}
	
	// initialization
	private native static boolean init(Core core, ByteBuffer rom);
	private native static boolean uninit();
	
	// emulation
	private native static void runTick();
	private native static void _resumeEmulation();
	private native static void _pauseEmulation();
	private native static boolean isPaused();
	private native static void setWantMacReset();
	private native static void setWantMacInterrupt();

	public Boolean initEmulation(Core core, ByteBuffer rom) {
		return init(core, rom);
	}

	public Boolean uninitEmulation() {
		return uninit();
	}

	public void wantMacReset() {
		setWantMacReset();
	}

	public void wantMacInterrupt() {
		setWantMacInterrupt();
	}
	
	public void startEmulation() {
		if (!initOk) return;
		// insert initial disks
		for(int i=0; i < getNumDrives(); i++) {
			File f = mFileManager.getDataFile("disk"+i+".dsk");
			if (!insertDisk(f)) break;
		}
		resumeEmulation();
	}
	
	public void resumeEmulation() {
		if (!initOk) return;
		if (!isPaused()) return;
		_resumeEmulation();
		// setup handler
		tickHandler = new Handler();
		runTickTask = new Runnable() {
			private int frame = 0;
			public void run() {
				int[] screenUpdate;
				runTick();
				if (++frame == frameSkip) {
					frame = 0;
					screenUpdate = getScreenUpdate();
					if (mOnUpdateScreenListener != null && screenUpdate != null) {
						mOnUpdateScreenListener.onUpdateScreen(screenUpdate);
					}
				}
				if (tickHandler == null) tickHandler = new Handler();
				tickHandler.post(runTickTask);
			}
		};
		tickHandler.post(runTickTask);
	}
	
	public void pauseEmulation() {
		if (!initOk) return;
		if (isPaused()) return;
		_pauseEmulation();
		tickHandler.removeCallbacks(runTickTask);
		tickHandler = null;
		runTickTask = null;
	}
	
	// mouse
	private native static void moveMouse(int dx, int dy);
	private native static void setMousePos(int x, int y);
	private native static void setMouseButton(boolean down);
	private native static int getMouseX();
	private native static int getMouseY();
	private native static boolean getMouseButton();

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
	private native static byte[] soundBuf();
	private native static void setPlayOffset(int newValue);
	
	private static AudioTrack mAudioTrack;

    private static final int SOUND_SAMPLERATE = 22255;

	public Boolean MySound_Init() {
        try {
            int minBufferSize = AudioTrack.getMinBufferSize(SOUND_SAMPLERATE, AudioFormat.CHANNEL_CONFIGURATION_MONO, AudioFormat.ENCODING_PCM_8BIT);
		    mAudioTrack = new AudioTrack(AudioManager.STREAM_MUSIC, SOUND_SAMPLERATE, AudioFormat.CHANNEL_CONFIGURATION_MONO, AudioFormat.ENCODING_PCM_8BIT, minBufferSize, AudioTrack.MODE_STREAM);

	    	mAudioTrack.pause();
	    	return true;
	    } catch (Exception e) {
	    	Log.e(TAG, "MySound_Init() can't init sound.", e);
	    	return false;
	    }
	}
	
	public void playSound() {
		for (int i = 0 ; i < 32 ; i++) {
			byte[] buf = soundBuf();
			if (buf == null) break;
			if (mAudioTrack.getPlayState() != AudioTrack.PLAYSTATE_PLAYING)
			{
				mAudioTrack.play();
			}
			int err = mAudioTrack.write(buf, 0, buf.length);
			if (err > 0) {
				setPlayOffset(err);
			} else {
				Log.w(TAG, "playSound() mAudioTrack.write() got error " + err);
				break;
			}
		}
		
		//mAudioTrack.flush();
		//mAudioTrack.pause();
	}

	public void MySound_Start () {
		if (mAudioTrack != null) {
			mAudioTrack.play();
		}
	}

	public void MySound_Stop () {
		if (mAudioTrack != null) {
		    mAudioTrack.stop();
		    mAudioTrack.release();
		}
	}
	
	// disks
	private native static void notifyDiskInserted(int driveNum, boolean locked);
	private native static void notifyDiskEjected(int driveNum);
	private native static int getFirstFreeDisk();
	private native static int getNumDrives();
	
	// disk driver callbacks
	public int sonyTransfer(boolean isWrite, ByteBuffer buf, int driveNum, int start, int length) {
		if (diskFile[driveNum] == null) return -1;
		try {
			if (isWrite)
			{
				byte[] bytes = new byte[length];
				buf.rewind();
				buf.get(bytes);
				diskFile[driveNum].seek(start);
				diskFile[driveNum].write(bytes);
				return length;
			}
			else
			{
				byte[] bytes = new byte[length];
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

	public int sonyEject(int driveNum) {
		if (diskFile[driveNum] == null) return -1;
		int ret;
		try {
			diskFile[driveNum].close();
			ret = 0;
		} catch (Exception x) {
			ret = -1;
		}
		
		diskFile[driveNum] = null;
		diskPath[driveNum] = null;
		numInsertedDisks--;
		
		notifyDiskEjected(driveNum);
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
			Utils.showAlert(mContext, mContext.getString(R.string.errTooManyDisks), false);
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
		return true;
	}
	
	public boolean insertDisk(String path) {
		return insertDisk(new File(path));
	}
	
	public boolean hasDisksInserted() {
		return numInsertedDisks > 0;
	}

	// warnings
	public void warnMsg(String shortMsg, String longMsg) {
		pauseEmulation();

		Utils.showWarnMessage(mContext, shortMsg, longMsg, false, new DialogInterface.OnClickListener() {
			public void onClick(DialogInterface di, int i) {
				resumeEmulation();
			}
		});
	}

	interface OnUpdateScreenListener {
		void onUpdateScreen(int[] update);
	}
}
