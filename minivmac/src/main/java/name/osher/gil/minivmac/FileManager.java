package name.osher.gil.minivmac;

import android.os.Environment;
import java.io.File;

/**
 * Created by dolfin on 16/12/2015.
 */
public class FileManager {
    private File mDataDir;

    public FileManager() {

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


}
