package name.osher.gil.minivmac;

import android.app.Activity;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.util.Log;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;

/**
 * Created by dolfin on 16/12/2015.
 */
public class FileManager {
    private static final String TAG = "minivmac.FileManager";

    private static final int ZERO_BUFFER_SIZE = 2048;

    private static FileManager mInstance = new FileManager();
    private File mDataDir;

    private FileManager() { }

    public static FileManager getInstance() {
        return mInstance;
    }

    public Boolean init() {
        // find data directory
        mDataDir = new File(Environment.getExternalStorageDirectory(), "minivmac");
        return mDataDir.isDirectory() && mDataDir.canRead();
    }

    public File getDataFile(String name) {
        return new File(mDataDir, name);
    }

    public File getDataDir() {
        return mDataDir;
    }

    public boolean makeNewDisk(int size, String fileName, String path, Handler progressHandler) {
        File disk = new File(path, fileName);
        try {
            if (!disk.createNewFile())
            {
                // Error show file exist warning
                handleError(null, R.string.errFileExist, progressHandler);
                return false;
            }
        } catch (IOException e) {
            e.printStackTrace();
            handleError(null, R.string.errGeneral, progressHandler);
            return false;
        }

        FileOutputStream writer;
        try {
            writer = new FileOutputStream(disk);
        } catch (FileNotFoundException e) {
            e.printStackTrace();
            handleError(disk, R.string.errGeneral, progressHandler);
            return false;
        }

        byte[] buffer = new byte[ZERO_BUFFER_SIZE];
        for (int i = 0 ; i < ZERO_BUFFER_SIZE ; i++)
            buffer[i] = 0;

        try {
            int sizeLeft = size;
            while (sizeLeft > 0) {
                int i = (sizeLeft > ZERO_BUFFER_SIZE) ? ZERO_BUFFER_SIZE : sizeLeft;
                writer.write(buffer, 0, i);
                sizeLeft -= i;

                if (progressHandler != null) {
                    Message msg = progressHandler.obtainMessage();
                    msg.arg1 = (int) (99.0 * (size - sizeLeft) / size);
                    progressHandler.sendMessage(msg);
                }
            }

            writer.close();
        } catch (IOException e) {
            handleError(disk, R.string.errCreateDisk, progressHandler);
            return false;
        }

        if (progressHandler != null) {
            Message msg = progressHandler.obtainMessage();
            msg.arg1 = 100;
            msg.arg2 = Activity.RESULT_OK;
            msg.obj = disk.getAbsolutePath();
            progressHandler.sendMessage(msg);
        }
        return true;
    }

    private void handleError(File disk, int error, Handler progressHandler) {

        if (disk != null)
        {
            // Delete what we got so far
            boolean res = disk.delete();
            if (!res) {
                Log.e(TAG, "Couldn't remove disk " + disk.getPath() + "!");
            }
        }

        if (progressHandler != null) {
            Message msg = progressHandler.obtainMessage();
            msg.arg1 = 100;
            msg.arg2 = Activity.RESULT_FIRST_USER;
            msg.obj = error;
            progressHandler.sendMessage(msg);
        }
    }
}
