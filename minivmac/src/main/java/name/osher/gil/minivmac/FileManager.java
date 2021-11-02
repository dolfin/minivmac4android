package name.osher.gil.minivmac;

import android.app.Activity;
import android.content.ContentResolver;
import android.content.Context;
import android.database.Cursor;
import android.net.Uri;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.provider.OpenableColumns;
import android.util.Log;
import android.webkit.MimeTypeMap;

import java.io.File;
import java.io.FileFilter;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

/**
 * Created by dolfin on 16/12/2015.
 */
public class FileManager {
    private static final String TAG = "minivmac.FileManager";
    private final static String[] diskExtensions = {"DSK", "dsk", "img", "IMG"};

    private static String DIRECTORY_ROM = "rom";
    private static String DIRECTORY_DISKS = "disks";
    private static String DIRECTORY_DOWNLOADS = "downloads";

    private static final int ZERO_BUFFER_SIZE = 2048;

    private static FileManager mInstance = new FileManager();
    private File mDataDir;
    private File mCacheDir;
    private File mRomDir;
    private File mDisksDir;
    private File mDownloadDir;
    private ContentResolver mContentResolver;

    private FileManager() { }

    public static FileManager getInstance() {
        return mInstance;
    }

    public Boolean init(Context context) {
        mContentResolver = context.getContentResolver();
        // find data directory
        mCacheDir = context.getExternalCacheDir();
        mDataDir = context.getExternalFilesDir(null);
        mRomDir = new File(mDataDir, DIRECTORY_ROM);
        mDisksDir = new File(mDataDir, DIRECTORY_DISKS);
        mDownloadDir = new File(mDataDir, DIRECTORY_DOWNLOADS);
        if (mDataDir.isDirectory() && mDataDir.canRead() &&
                mCacheDir.isDirectory() && mCacheDir.canRead()) {
            mRomDir.mkdirs();
            mDisksDir.mkdirs();
            mDownloadDir.mkdirs();
            return true;
        }
        return false;
    }

    public File getCacheFile(String name) {
        return new File(mCacheDir, name);
    }
    public File getCacheDir() {
        return mCacheDir;
    }

    public File getRomFile(String name) {
        return new File(mRomDir, name);
    }
    public File getRomDir() {
        return mRomDir;
    }
    public File getDisksFile(String name) {
        return new File(mDisksDir, name);
    }
    public File getDisksDir() {
        return mDisksDir;
    }
    public File getDownloadFile(String name) {
        return new File(mDownloadDir, name);
    }
    public File getDownloadDir() {
        return mDownloadDir;
    }

    public Boolean isInCache(String path) {
        return path.contains(mCacheDir.getAbsolutePath());
    }

    public File[] getAvailableDisks () {
        return mDisksDir.listFiles(new FileFilter() {
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

    public void copy(InputStream in, File dst) throws IOException {
        if (!dst.exists()) {
            dst.createNewFile();
        }

        try (OutputStream out = new FileOutputStream(dst)) {
            // Transfer bytes from in to out
            byte[] buf = new byte[ZERO_BUFFER_SIZE];
            int len;
            while ((len = in.read(buf)) > 0) {
                out.write(buf, 0, len);
            }
        }
    }

    public void delete(File file) {
        file.delete();
    }

    public String getFileName(Uri uri) {
        Cursor cursor = mContentResolver.query(uri, null, null, null, null);
        int nameIndex = cursor.getColumnIndex(OpenableColumns.DISPLAY_NAME);
        cursor.moveToFirst();
        String name = cursor.getString(nameIndex);
        cursor.close();
        return name;
    }

    public String getMimeType(Uri uri) {
        String mimeType = null;
        if (ContentResolver.SCHEME_CONTENT.equals(uri.getScheme())) {
            mimeType = mContentResolver.getType(uri);
        } else {
            String fileExtension = MimeTypeMap.getFileExtensionFromUrl(uri
                    .toString());
            mimeType = MimeTypeMap.getSingleton().getMimeTypeFromExtension(
                    fileExtension.toLowerCase());
        }
        if (mimeType != null) {
            return mimeType;
        } else {
            return "application/octet-stream";
        }
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
