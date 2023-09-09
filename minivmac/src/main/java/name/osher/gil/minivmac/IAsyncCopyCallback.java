package name.osher.gil.minivmac;

import java.io.File;

public interface IAsyncCopyCallback {
    void onSuccessfulCopy(File file);
}
